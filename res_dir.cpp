//
//  res_dir.cpp
//  downLoadTest
//
//  Created by hui gh on 2023/5/4.
//

#include "res_dir.hpp"


ResDir::ResDir(const std::string& org_image_path,
       const std::string& org_audio_path,
       const std::string& org_video_path,
               const std::string& video_path){
    this->org_image_path = org_image_path;
    this->org_audio_path = org_audio_path;
    this->org_video_path = org_video_path;
    this->video_path = video_path;
}

std::string ResDir::orgImagePath(const std::string image_name){
    return this->org_image_path + image_name ;
}


std::string ResDir::orgAudioPath(const std::string audio_name){
    return this->org_audio_path + audio_name ;
}


std::string ResDir::orgVideoPath(const std::string video_name){
    return this->org_video_path + video_name ;
}


std::string ResDir::videoPath(const std::string video_name){
    return this->video_path + video_name ;
}
std::string ResDir::videoTempPath(const std::string video_name) {
	return this->video_path + video_name;
}

