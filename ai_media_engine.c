extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/pixfmt.h>
#include <libavutil/samplefmt.h>
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
#include <libswresample/swresample.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
}

#define AUDIO_STREAM_INDEX 0
#define VIDEO_STREAM_INDEX 1

const char* input_image = "/usr/local/lbc/vedio_combine/Resources/input_image.jpg";
const char* input_audio = "/usr/local/lbc/vedio_combine/Resources/input_audio.mp3";
const char* output_video = "/usr/local/lbc/vedio_combine/Resources/output.mp4";


typedef struct {
    AVFormatContext* audio_fmt_ctx;
    AVCodecContext* audio_codec_ctx;
	const AVCodec* audio_codec;
    AVFormatContext* image_fmt_ctx;
    AVCodecContext* image_codec_ctx;
	const AVCodec* image_codec;
    AVFormatContext* out_fmt_ctx;
    AVCodecContext* audio_out_codec_ctx;
    AVCodecContext* video_out_codec_ctx;
	const AVCodec* audio_out_codec;
	const AVCodec* video_out_codec;
    AVStream* audio_out_stream;
    AVStream* video_out_stream;
    AVPacket audio_packet;
    AVPacket image_packet;
    AVFrame* audio_frame;
    AVFrame* image_frame;
    AVFrame* video_frame;
    AVPacket* out_packet;
    int audio_stream_index;
    int image_stream_index;
	char *m4a_audio_path;
} ai_media_engine;

int open_input_audio(ai_media_engine* engine, const char* input_audio);
int open_input_image(ai_media_engine* engine, const char* input_image);
int setup_codecs(ai_media_engine* engine);
int create_output_context(ai_media_engine* engine, const char* output_video);
int convert_image_to_YUV(ai_media_engine* engine);
int process_streams(ai_media_engine* engine);
void media_engine_dealloc(ai_media_engine* engine);
int open_input_audio(ai_media_engine* engine, const char* input_audio);
int convert_wav_to_m4a(ai_media_engine* engine, const char* input_audio_path);


ai_media_engine* media_engine_new() {
	ai_media_engine* engine = (ai_media_engine*)malloc(sizeof(ai_media_engine));
	if (engine == NULL) {
		// Handle memory allocation failure
		return NULL;
	}
	memset(engine, 0, sizeof(ai_media_engine));
	return engine;
}


//返回报错还没写
char* image_audio_to_video(ai_media_engine * engine,
	char* image_path,
	char* audio_path,
	char* dest_path) {
	
	// Open input audio and image files

	

	if (convert_wav_to_m4a(engine, audio_path) < 0) {
	
		printf("Error converting wav to m4a.\n");
		media_engine_dealloc(engine);
		return NULL;
	
	}

	

	if (open_input_audio(engine, engine->m4a_audio_path) < 0) {
		printf("Error opening input audio file.\n");
		media_engine_dealloc(engine);
		return NULL;
	}

	

	if (open_input_image(engine, image_path) < 0) {
		printf("Error opening input image file.\n");
		media_engine_dealloc(engine);
		return NULL;
	}

	
	// Setup codecs
	if (setup_codecs(engine) < 0) {
		printf("Error setting up codecs.\n");
		media_engine_dealloc(engine);
		return NULL;
	}

	
	// Create output context
	if (create_output_context(engine, dest_path) < 0) {
		printf("Error creating output context.\n");
		media_engine_dealloc(engine);
		return NULL;
	}

	
	// Convert image to YUV format
	if (convert_image_to_YUV(engine) < 0) {
		printf("Error converting image to YUV format.\n");
		media_engine_dealloc(engine);
		return NULL;
	}

	

	// Process streams
	if (process_streams(engine) < 0) {
		printf("Error processing streams.\n");
		media_engine_dealloc(engine);
		return NULL;
	}


}




void media_engine_dealloc(ai_media_engine* engine) {
	// Close codecs
	avcodec_close(engine->audio_codec_ctx);
	avcodec_close(engine->image_codec_ctx);
	avcodec_close(engine->audio_out_codec_ctx);
	avcodec_close(engine->video_out_codec_ctx);
	// Free codec contexts
	avcodec_free_context(&engine->audio_codec_ctx);
	avcodec_free_context(&engine->image_codec_ctx);
	avcodec_free_context(&engine->audio_out_codec_ctx);
	avcodec_free_context(&engine->video_out_codec_ctx);
	// Free frames
	av_frame_free(&engine->image_frame);
	av_frame_free(&engine->video_frame);
	// Close input format contexts
	avformat_close_input(&engine->audio_fmt_ctx);
	avformat_close_input(&engine->image_fmt_ctx);
	// Close output format context
	if (!(engine->out_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
		avio_closep(&engine->out_fmt_ctx->pb);
	}
	avformat_free_context(engine->out_fmt_ctx);
	// Free the engine
	free(engine);
}

/*
int convert_wav_to_m4a(ai_media_engine* engine,const char *input_audio_path) {
	char output_audio_path[1024];

	// Replace the .wav extension with .m4a for the output file
	strncpy(output_audio_path, input_audio_path, sizeof(output_audio_path));
	char *ext = strrchr(output_audio_path, '.');
	if (ext != NULL && strcmp(ext, ".wav") == 0) {
		strcpy(ext, ".m4a");
	}
	else {
		fprintf(stderr, "Invalid file extension\n");
		return -1;
	}

	char command[2048];
	snprintf(command, sizeof(command), "ffmpeg -y -i \"%s\" -vn -acodec aac -b:a 256k \"%s\"", input_audio_path, output_audio_path);

	int ret = system(command);
	if (ret != 0) {
		fprintf(stderr, "Error executing ffmpeg command\n");
		return ret;
	}
	engine->m4a_audio_path = strdup(output_audio_path);
	return 0;
}

*/




int convert_wav_to_m4a_engine(const char* input_file, const char* output_file) {
	int ret;
	int i;
	AVPacket input_packet;
	AVPacket* output_packet = av_packet_alloc();
	if (!output_packet) {
		printf("Cannot allocate output packet\n");
		return -1;
	}


	printf("image_audio_to_video 11111 44444\n");
	// Open input file
	AVFormatContext* input_fmt_ctx = NULL;//
	if (avformat_open_input(&input_fmt_ctx, input_file, NULL, NULL) < 0) {
		printf("Cannot open input file\n");
		return -1;
	}

	if (avformat_find_stream_info(input_fmt_ctx, NULL) < 0) {
		printf("Cannot find input stream information\n");
		return -1;
	}

	int audio_stream_index = -1;
	for (i = 0; i < input_fmt_ctx->nb_streams; i++) {
		if (input_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			audio_stream_index = i;
			break;
		}
	}

	if (audio_stream_index == -1) {
		printf("Cannot find audio stream\n");
		return -1;
	}

	AVCodec* input_codec = (AVCodec *)avcodec_find_decoder(input_fmt_ctx->streams[audio_stream_index]->codecpar->codec_id);
	if (!input_codec) {
		printf("Cannot find input codec\n");
		return -1;
	}

	AVCodecContext* input_codec_ctx = avcodec_alloc_context3(input_codec);
	if (!input_codec_ctx) {
		printf("Cannot allocate input codec context\n");
		return -1;
	}

	printf("image_audio_to_video 11111 555555\n");

	if (avcodec_parameters_to_context(input_codec_ctx, input_fmt_ctx->streams[audio_stream_index]->codecpar) < 0) {
		printf("Cannot copy input codec parameters to context\n");
		return -1;
	}

	if (avcodec_open2(input_codec_ctx, input_codec, NULL) < 0) {
		printf("Cannot open input codec\n");
		return -1;
	}

	// Prepare output file
	AVFormatContext* output_fmt_ctx = NULL;//
	if (avformat_alloc_output_context2(&output_fmt_ctx, NULL, NULL, output_file) < 0) {
		printf("Cannot allocate output format context\n");
		return -1;
	}

	AVStream* output_stream = avformat_new_stream(output_fmt_ctx, NULL);
	if (!output_stream) {
		printf("Cannot create output stream\n");
		return -1;
	}

	AVCodec* output_codec = (AVCodec *)avcodec_find_encoder(AV_CODEC_ID_AAC);
	if (!output_codec) {
		printf("Cannot find output codec\n");
		return -1;
	}

	AVCodecContext* output_codec_ctx = avcodec_alloc_context3(output_codec);
	if (!output_codec_ctx) {
		printf("Cannot allocate output codec context\n");
		return -1;
	}

	printf("image_audio_to_video 11111 66666\n");

	output_codec_ctx->channels = input_codec_ctx->channels;
	if (input_codec_ctx->channel_layout == 0) {
		input_codec_ctx->channel_layout = av_get_default_channel_layout(input_codec_ctx->channels);

	}
	output_codec_ctx->channel_layout = input_codec_ctx->channel_layout;

	output_codec_ctx->sample_rate = input_codec_ctx->sample_rate;
	output_codec_ctx->sample_fmt = output_codec->sample_fmts[0];
	output_codec_ctx->bit_rate = 96000;
	output_codec_ctx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
	if (avcodec_open2(output_codec_ctx, output_codec, NULL) < 0) {
		printf("Cannot open output codec\n");
		return -1;
	}

	if (avcodec_parameters_from_context(output_stream->codecpar, output_codec_ctx) < 0) {
		printf("Cannot copy output codec parameters to context\n");
		return -1;
	}

	if (!(output_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
		if (avio_open(&output_fmt_ctx->pb, output_file, AVIO_FLAG_WRITE) < 0) {
			printf("Cannot open output file\n");
			return -1;
		}
	}

	if (avformat_write_header(output_fmt_ctx, NULL) < 0) {
		printf("Cannot write output file header\n");
		return -1;
	}

	// Initialize resampler
	SwrContext* swr_ctx = swr_alloc_set_opts(NULL, output_codec_ctx->channel_layout, output_codec_ctx->sample_fmt, output_codec_ctx->sample_rate, input_codec_ctx->channel_layout, input_codec_ctx->sample_fmt, input_codec_ctx->sample_rate, 0, NULL);
	if (!swr_ctx) {
		printf("Cannot allocate resampler context\n");
		return -1;
	}

	if (swr_init(swr_ctx) < 0) {
		printf("Cannot initialize resampler context\n");
		return -1;
	}

	// Resample and encode
	AVFrame* input_frame = av_frame_alloc();
	if (!input_frame) {
		printf("Failed to allocate input frame\n");
		// Perform cleanup and return an error code, e.g. -1
	}
	AVFrame* output_frame = av_frame_alloc();
	if (!output_frame) {
		printf("Failed to allocate output frame\n");
		// Perform cleanup and return an error code, e.g. -1
	}

	printf("image_audio_to_video 11111 777777\n");

	int read_frame_result;
	int64_t processed_samples = 0;

	while ((read_frame_result = av_read_frame(input_fmt_ctx, &input_packet)) >= 0) {
		printf("image_audio_to_video 11111 777777  11\n");

		if (input_packet.stream_index == audio_stream_index) {

			printf("image_audio_to_video 11111 777777 222\n");

			if (avcodec_send_packet(input_codec_ctx, &input_packet) >= 0) {
				printf("image_audio_to_video 11111 777777 333\n");

				while (avcodec_receive_frame(input_codec_ctx, input_frame) == 0) {

					printf("image_audio_to_video 11111 777777 444\n");

					output_frame->nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, input_codec_ctx->sample_rate) + input_frame->nb_samples, output_codec_ctx->sample_rate, input_codec_ctx->sample_rate, AV_ROUND_UP);

					output_frame->format = output_codec_ctx->sample_fmt;

					output_frame->channel_layout = output_codec_ctx->channel_layout;

					if (av_frame_get_buffer(output_frame, 0) < 0) {
						printf("Failed to allocate output frame buffer\n");
						// Perform cleanup and return an error code, e.g. -1
					}
					swr_convert(swr_ctx, output_frame->data, output_frame->nb_samples, (const uint8_t**)input_frame->data, input_frame->nb_samples);

					output_frame->pts = av_rescale_q(input_frame->pts, input_codec_ctx->time_base, output_codec_ctx->time_base);

					int remaining_samples = output_frame->nb_samples;
					int offset = 0;
					int frame_size = output_codec_ctx->frame_size;

					printf("image_audio_to_video 11111 777777 55\n");

					while (remaining_samples > 0) {
						printf("image_audio_to_video 11111 777777 55 1\n");
						int samples_to_process = FFMIN(remaining_samples, frame_size);
						printf("image_audio_to_video 11111 777777 55 2\n");
						AVFrame *subframe = av_frame_clone(output_frame);
						printf("image_audio_to_video 11111 777777 55 3\n");
						if (!subframe) {
							printf("Failed to clone output frame\n");
							// Perform cleanup and return an error code, e.g. -1
						}

						printf("image_audio_to_video 11111 777777 55 4\n");
						for (i = 0; i < output_codec_ctx->channels; ++i) {
							subframe->data[i] = output_frame->data[i] + offset * av_get_bytes_per_sample(output_codec_ctx->sample_fmt);
						}

						printf("image_audio_to_video 11111 777777 55 5\n");

						subframe->nb_samples = samples_to_process;

						// Set proper timestamp for the subframe
						printf("image_audio_to_video 11111 777777 55  6\n");
						subframe->pts = av_rescale_q(processed_samples, (AVRational) { 1, output_codec_ctx->sample_rate }, output_codec_ctx->time_base);

						printf("image_audio_to_video 11111 777777 55 7\n");

						if (avcodec_send_frame(output_codec_ctx, subframe) >= 0) {
							printf("image_audio_to_video 11111 777777 55 7 1\n");

							if (output_codec_ctx==NULL)
							{
								printf("output_codec_ctx error\n");
								return 0;
							}
							printf("image_audio_to_video 11111 777777 55 7 1 1\n");
							if (output_packet == NULL)
							{
								printf("output_packet error\n");
								return 0;
							}
							printf("image_audio_to_video 11111 777777 55 7 1 2\n");

							printf("output_codec_ctx: %p\n", output_codec_ctx);
							printf("output_codec_ctx->codec_id: %d\n", output_codec_ctx->codec_id);
							printf("output_codec_ctx->width: %d\n", output_codec_ctx->width);
							printf("output_codec_ctx->height: %d\n", output_codec_ctx->height);

							printf("output_packet: %p\n", output_packet);
							printf("output_packet->size: %d\n", output_packet->size);
							printf("output_packet->stream_index: %d\n", output_packet->stream_index);
							printf("output_packet->pts: %" PRId64 "\n", output_packet->pts);
							printf("output_packet->dts: %" PRId64 "\n", output_packet->dts);



							while (avcodec_receive_packet(output_codec_ctx, output_packet) == 0) {
								printf("image_audio_to_video 11111 777777 55 7 2\n");
								output_packet->stream_index = output_stream->index;
								av_packet_rescale_ts(output_packet, output_codec_ctx->time_base, output_stream->time_base);
								av_interleaved_write_frame(output_fmt_ctx, output_packet);
								printf("image_audio_to_video 11111 777777 55 7 3\n");
								av_packet_unref(output_packet);
								printf("image_audio_to_video 11111 777777 55 7 4\n");
							}

							printf("image_audio_to_video 11111 777777 55 7 5\n");
						}
						printf("image_audio_to_video 11111 777777 55 8\n");

						av_frame_unref(subframe);

						remaining_samples -= samples_to_process;
						offset += samples_to_process;
						processed_samples += samples_to_process;
					}
					printf("image_audio_to_video 11111 777777 7777\n");

					av_frame_unref(output_frame);

					printf("image_audio_to_video 11111 777777 7777 --111\n");
				}
				printf("image_audio_to_video 11111 777777 888\n");
			}
			printf("image_audio_to_video 11111 777777 999\n");
		}

		printf("image_audio_to_video 11111 8888\n");

		av_packet_unref(&input_packet);

		printf("image_audio_to_video 11111 99999\n");
	}




	// Flush and write trailer
	avcodec_send_frame(output_codec_ctx, NULL);
	while (avcodec_receive_packet(output_codec_ctx, output_packet) == 0) {
		output_packet->stream_index = output_stream->index;
		av_packet_rescale_ts(output_packet, output_codec_ctx->time_base, output_stream->time_base);
		av_interleaved_write_frame(output_fmt_ctx, output_packet);
		av_packet_unref(output_packet);
	}
	av_write_trailer(output_fmt_ctx);

	// Clean up
	av_frame_free(&input_frame);
	av_frame_free(&output_frame);
	av_packet_unref(&input_packet);
	av_packet_unref(output_packet);

	avcodec_close(input_codec_ctx);
	avcodec_close(output_codec_ctx);
	avcodec_free_context(&input_codec_ctx);
	avcodec_free_context(&output_codec_ctx);

	swr_free(&swr_ctx);

	avformat_close_input(&input_fmt_ctx);
	if (!(output_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
		avio_closep(&output_fmt_ctx->pb);
	}
	avformat_free_context(output_fmt_ctx);

	avformat_network_deinit();

	return 0;
}

int convert_wav_to_m4a(ai_media_engine* engine, const char* input_audio_path) {
	char output_audio_path[1024];

	// Replace the .wav extension with .m4a for the output file
	printf("image_audio_to_video 11111   1111\n");

	strncpy(output_audio_path, input_audio_path, sizeof(output_audio_path));
	char *ext = strrchr(output_audio_path, '.');
	if (ext != NULL && strcmp(ext, ".wav") == 0) {
		strcpy(ext, ".m4a");
	}
	else {
		fprintf(stderr, "Invalid file extension\n");
		return -1;
	}

	// Call the original convert_wav_to_m4a function with the input and output paths
	printf("image_audio_to_video 11111  22222\n");
	int ret = convert_wav_to_m4a_engine(input_audio_path, output_audio_path);
	if (ret != 0) {
		fprintf(stderr, "Error converting WAV to M4A\n");
		return ret;
	}

	// Update the ai_media_engine with the new M4A audio path
	engine->m4a_audio_path = strdup(output_audio_path);
	printf("image_audio_to_video 11111 333333\n");

	return 0;
}



int open_input_audio(ai_media_engine* engine, const char* input_audio) {
    // Open audio file and find audio stream
    int i;
    if (avformat_open_input(&engine->audio_fmt_ctx, input_audio, NULL, NULL) < 0) {
        printf("Could not open audio file '%s'\n", input_audio);
        return -1;
    }
    if (avformat_find_stream_info(engine->audio_fmt_ctx, NULL) < 0) {
        printf("Could not find audio stream information\n");
        avformat_close_input(&engine->audio_fmt_ctx);
        return -1;
    }
    for (i = 0; i < engine->audio_fmt_ctx->nb_streams; i++) {
        if (engine->audio_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            engine->audio_stream_index = i;
            break;
        }
    }
    if (engine->audio_stream_index == -1) {
        printf("Could not find audio stream\n");
        avformat_close_input(&engine->audio_fmt_ctx);
        return -1;
    }
    return 0;
}

int open_input_image(ai_media_engine* engine, const char* input_image) {
    int i;
    // Open image file and find image stream
    if (avformat_open_input(&engine->image_fmt_ctx, input_image, NULL, NULL) < 0) {
        printf("Could not open image file '%s'\n", input_image);
        avformat_close_input(&engine->audio_fmt_ctx);
        return -1;
    }
    if (avformat_find_stream_info(engine->image_fmt_ctx, NULL) < 0) {
        printf("Could not find image stream information\n");
        avformat_close_input(&engine->audio_fmt_ctx);
        avformat_close_input(&engine->image_fmt_ctx);
        return -1;
    }
    for (i = 0; i < engine->image_fmt_ctx->nb_streams; i++) {
        if (engine->image_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            engine->image_stream_index = i;
            break;
        }
    }
    if (engine->image_stream_index == -1) {
        printf("Could not find image stream\n");
        avformat_close_input(&engine->audio_fmt_ctx);
        avformat_close_input(&engine->image_fmt_ctx);
        return -1;
    }
    return 0;
}


int setup_codecs(ai_media_engine* engine) {
    // Set up audio codec and codec context
    engine->audio_codec = avcodec_find_decoder(engine->audio_fmt_ctx->streams[engine->audio_stream_index]->codecpar->codec_id);
    engine->audio_codec_ctx = avcodec_alloc_context3(engine->audio_codec);
    avcodec_parameters_to_context(engine->audio_codec_ctx, engine->audio_fmt_ctx->streams[engine->audio_stream_index]->codecpar);
    if (avcodec_open2(engine->audio_codec_ctx, engine->audio_codec, NULL) < 0)
    {
        printf("Could not open audio codec\n");
        avcodec_free_context(&engine->audio_codec_ctx);
        avformat_close_input(&engine->audio_fmt_ctx);
        avformat_close_input(&engine->image_fmt_ctx);
        return -1;
    }
    // Set up image codec and codec context
    engine->image_codec = avcodec_find_decoder(engine->image_fmt_ctx->streams[engine->image_stream_index]->codecpar->codec_id);
    engine->image_codec_ctx = avcodec_alloc_context3(engine->image_codec);
    avcodec_parameters_to_context(engine->image_codec_ctx, engine->image_fmt_ctx->streams[engine->image_stream_index]->codecpar);
    if (avcodec_open2(engine->image_codec_ctx, engine->image_codec, NULL) < 0) {
        printf("Could not open image codec\n");
        avcodec_free_context(&engine->audio_codec_ctx);
        avcodec_free_context(&engine->image_codec_ctx);
        avformat_close_input(&engine->audio_fmt_ctx);
        avformat_close_input(&engine->image_fmt_ctx);
        return -1;
    }
    return 0;
}


int create_output_context(ai_media_engine* engine, const char* output_video) {
    // Create output format context
    if (avformat_alloc_output_context2(&engine->out_fmt_ctx, NULL, "mp4", output_video) < 0) {
        printf("Could not create output context\n");
        avcodec_free_context(&engine->audio_codec_ctx);
        avcodec_free_context(&engine->image_codec_ctx);
        avformat_close_input(&engine->audio_fmt_ctx);
        avformat_close_input(&engine->image_fmt_ctx);
        return -1;
    }

    // Create output audio stream
    engine->audio_out_codec = avcodec_find_encoder(engine->audio_codec_ctx->codec_id);
    engine->audio_out_stream = avformat_new_stream(engine->out_fmt_ctx, engine->audio_out_codec);
    engine->audio_out_codec_ctx = avcodec_alloc_context3(engine->audio_out_codec);
    avcodec_parameters_to_context(engine->audio_out_codec_ctx, engine->audio_fmt_ctx->streams[engine->audio_stream_index]->codecpar);
    engine->audio_out_codec_ctx->time_base = (AVRational){ 1, engine->audio_codec_ctx->sample_rate };
    if (avcodec_open2(engine->audio_out_codec_ctx, engine->audio_out_codec, NULL) < 0) {
        printf("Could not open output audio codec\n");
        avcodec_free_context(&engine->audio_codec_ctx);
        avcodec_free_context(&engine->image_codec_ctx);
        avcodec_free_context(&engine->audio_out_codec_ctx);
        avformat_close_input(&engine->audio_fmt_ctx);
        avformat_close_input(&engine->image_fmt_ctx);
        avformat_free_context(engine->out_fmt_ctx);
        return -1;
    }
    avcodec_parameters_from_context(engine->audio_out_stream->codecpar, engine->audio_out_codec_ctx);
    engine->audio_out_stream->time_base = engine->audio_out_codec_ctx->time_base;

    // Create output video stream
    engine->video_out_codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    engine->video_out_stream = avformat_new_stream(engine->out_fmt_ctx, engine->video_out_codec);
    engine->video_out_codec_ctx = avcodec_alloc_context3(engine->video_out_codec);
    engine->video_out_codec_ctx->width = engine->image_codec_ctx->width;
    engine->video_out_codec_ctx->height = engine->image_codec_ctx->height;
    engine->video_out_codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    engine->video_out_codec_ctx->time_base = (AVRational){ 1, 30 };
    engine->video_out_codec_ctx->framerate = (AVRational){ 30, 1};
    engine->video_out_codec_ctx->bit_rate = 100000;
    engine->video_out_codec_ctx->gop_size = 120;
    engine->video_out_codec_ctx->max_b_frames = 2;
    engine->video_out_codec_ctx->qmin = 23;
    engine->video_out_codec_ctx->qmax = 25;
    if (engine->out_fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
        engine->video_out_codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    if (avcodec_open2(engine->video_out_codec_ctx, engine->video_out_codec, NULL) < 0) {
        printf("Could not open output video codec\n");
        avcodec_free_context(&engine->audio_codec_ctx);
        avcodec_free_context(&engine->image_codec_ctx);
        avcodec_free_context(&engine->audio_out_codec_ctx);
        avcodec_free_context(&engine->video_out_codec_ctx);
        avformat_close_input(&engine->audio_fmt_ctx);
        avformat_close_input(&engine->image_fmt_ctx);
        avformat_free_context(engine->out_fmt_ctx);
        return -1;
    }
    avcodec_parameters_from_context(engine->video_out_stream->codecpar, engine->video_out_codec_ctx);
    engine->video_out_stream->time_base = engine->video_out_codec_ctx->time_base;

    // Create output file
    if (!(engine->out_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&engine->out_fmt_ctx->pb, output_video, AVIO_FLAG_WRITE) < 0) {
            printf("Could not create output file '%s'\n", output_video);
            avcodec_free_context(&engine->audio_codec_ctx);
            avcodec_free_context(&engine->image_codec_ctx);
            avcodec_free_context(&engine->audio_out_codec_ctx);
            avcodec_free_context(&engine->video_out_codec_ctx);
            avformat_close_input(&engine->audio_fmt_ctx);
            avformat_close_input(&engine->image_fmt_ctx);
            avformat_free_context(engine->out_fmt_ctx);
            return -1;
        }
    }

    // Write output file header
    if (avformat_write_header(engine->out_fmt_ctx, NULL) < 0) {
        printf("Could not write output file header\n");
        avcodec_free_context(&engine->audio_codec_ctx);
        avcodec_free_context(&engine->image_codec_ctx);
        avcodec_free_context(&engine->audio_out_codec_ctx);
        avcodec_free_context(&engine->video_out_codec_ctx);
        avformat_close_input(&engine->audio_fmt_ctx);
        avformat_close_input(&engine->image_fmt_ctx);
        avformat_free_context(engine->out_fmt_ctx);
        return -1;
    }
    return 0;
}

int convert_image_to_YUV(ai_media_engine* engine) {
    // Read image and convert to YUV420P format
    int ret;
    struct SwsContext* sws_ctx;
    engine->image_frame = av_frame_alloc();
    if (av_read_frame(engine->image_fmt_ctx, &engine->image_packet) >= 0) {
        if (engine->image_packet.stream_index == engine->image_stream_index) {
            ret = avcodec_send_packet(engine->image_codec_ctx, &engine->image_packet);
            if (ret >= 0) {
                ret = avcodec_receive_frame(engine->image_codec_ctx, engine->image_frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    // Do nothing
                }
                else if (ret < 0) {
                    printf("Error while decoding image frame\n");
                    media_engine_dealloc(engine);
                    return -1;
                }
            }
        }
    }

    engine->video_frame = av_frame_alloc();
    engine->video_frame->format = AV_PIX_FMT_YUV420P;
    engine->video_frame->width = engine->image_codec_ctx->width;
    engine->video_frame->height = engine->image_codec_ctx->height;
    av_frame_get_buffer(engine->video_frame, 32);

    sws_ctx = sws_getContext(
        engine->image_codec_ctx->width, engine->image_codec_ctx->height, engine->image_codec_ctx->pix_fmt,
        engine->video_out_codec_ctx->width, engine->video_out_codec_ctx->height, engine->video_out_codec_ctx->pix_fmt,
        SWS_BICUBIC, NULL, NULL, NULL);

    sws_scale(sws_ctx, (const uint8_t* const*)engine->image_frame->data, engine->image_frame->linesize,
        0, engine->image_codec_ctx->height, engine->video_frame->data, engine->video_frame->linesize);

    return 0;
}

int process_streams(ai_media_engine* engine) {
    // Process audio and video streams
    int video_pts = 0;
    int audio_pts = 0;
    int finished = 0;
    int ret;
    engine->out_packet = av_packet_alloc();  // 分配一个AVPacket结构体变量的内存空间

    while (!finished) {
        if (av_compare_ts(video_pts, engine->video_out_codec_ctx->time_base, audio_pts, engine->audio_out_codec_ctx->time_base) <= 0) {
            // Write video frame
            ret = avcodec_send_frame(engine->video_out_codec_ctx, engine->video_frame);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
                printf("Error while sending video frame\n");
                break;
            }

            ret = avcodec_receive_packet(engine->video_out_codec_ctx, engine->out_packet);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
                printf("Error while receiving video packet\n");
                break;
            }

            engine->out_packet->stream_index = VIDEO_STREAM_INDEX;
            engine->out_packet->pts = video_pts;
            engine->out_packet->dts = video_pts;
            video_pts++;

            if (av_interleaved_write_frame(engine->out_fmt_ctx, engine->out_packet) < 0) {
                printf("Error while writing video packet\n");
                break;
            }
            av_packet_unref(engine->out_packet);
        }
        else {
            // Read audio packet
            ret = av_read_frame(engine->audio_fmt_ctx, &engine->audio_packet);
            if (ret < 0) {
                finished = 1;
                continue;
            }
            // Write audio packet
            engine->out_packet->data = engine->audio_packet.data;
            engine->out_packet->size = engine->audio_packet.size;
            engine->out_packet->stream_index = AUDIO_STREAM_INDEX;
            engine->out_packet->pts = engine->audio_packet.pts;
            engine->out_packet->dts = engine->audio_packet.dts;
            audio_pts = engine->audio_packet.pts;

            if (av_interleaved_write_frame(engine->out_fmt_ctx, engine->out_packet) < 0) {
                printf("Error while writing audio packet\n");
                break;
            }
            av_packet_unref(&engine->audio_packet);
        }
    }

    // Write output file trailer
    av_write_trailer(engine->out_fmt_ctx);

    // Cleanup
   // cleanup(media_ctx);

    return 0;

}

