#ifndef VEDIO_COMBINE_H
#define VEDIO_COMBINE_H

#include <stdio.h>
#include <stdlib.h>


typedef struct ai_media_engine ai_media_engine;

/**
 * ���ɶ���
 **/
ai_media_engine* media_engine_new();


/**
 * ͼƬ��Ƶ�ϳ���Ƶ
 * param engine ý���������
 * param image_path ͼƬ����·��
 * param audio_path ��Ƶ�ļ�����·��
 * param de_path ������Ƶ���ļ�����·��
 * param return ʧ�ܷ��ش�����Ϣ���ɹ�����null
 **/
char* image_audio_to_video(ai_media_engine * engine,
									char* image_path, 
									char* audio_path, 
									char* de_path);




int convert_wav_to_m4a_engine(const char* input_file, const char* output_file);



/**
 * ͼƬ�����������
 * param return null
 **/
void media_engine_dealloc(ai_media_engine* engine);





#endif // VEDIO_COMBINE_H