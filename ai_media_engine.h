#ifndef VEDIO_COMBINE_H
#define VEDIO_COMBINE_H

#include <stdio.h>
#include <stdlib.h>


typedef struct ai_media_engine ai_media_engine;

/**
 * 生成对象
 **/
ai_media_engine* media_engine_new();


/**
 * 图片音频合成视频
 * param engine 媒体引擎对象
 * param image_path 图片绝对路径
 * param audio_path 音频文件绝对路径
 * param de_path 生成视频问文件绝对路径
 * param return 失败返回错误信息，成功返回null
 **/
char* image_audio_to_video(ai_media_engine * engine,
									char* image_path, 
									char* audio_path, 
									char* de_path);




int convert_wav_to_m4a_engine(const char* input_file, const char* output_file);



/**
 * 图片引擎对象销毁
 * param return null
 **/
void media_engine_dealloc(ai_media_engine* engine);





#endif // VEDIO_COMBINE_H