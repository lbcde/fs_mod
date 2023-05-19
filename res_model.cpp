//
//  res_model.cpp
//  downLoadTest
//
//  Created by hui gh on 2023/5/4.
//

#include "res_model.hpp"

ResModel::ResModel(ResDir* resDir){
    this->mResDir = resDir;
}
std::string ResModel::orgImagePath(){
    return this->mResDir->orgImagePath(this->file_name);
}


std::string ResModel::orgAudioPath(){
    return this->mResDir->orgAudioPath(this->audio_name);
}

std::string ResModel::orgVideoPath(){
    return this->mResDir->orgVideoPath(this->file_name);
}


std::string ResModel::videoPath(){
    return this->mResDir->videoPath(this->video_name);
}

std::string ResModel::videoTempPath() {
	return this->mResDir->videoTempPath("temp_"+ this->video_name);
}

std::string ResModel::updateVideoStateSql(){
    char sql[256];
    snprintf(sql, sizeof(sql),
        "UPDATE ai_ivvr_res SET "
        "video_state = %d "
        "WHERE res_id = '%s';",
        this->video_state,
             this->res_id.c_str());
    return sql;
}
bool ResModel::needReplaceAudio(){
    if (this->audio_text=="-1") {
        return false;
    }
    return true;
}
