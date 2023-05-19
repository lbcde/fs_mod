//
//  res_model.hpp
//  downLoadTest
//
//  Created by hui gh on 2023/5/4.
//

#ifndef res_model_hpp
#define res_model_hpp

#include <stdio.h>

#include <stdio.h>
#include <string>
#include <vector>
#include "res_dir.hpp"




class ResModel
{
    
public:
    ResModel(ResDir* resDir);
    int file_state;
    int audio_state;
    int video_state;
    std::string file_id;
    std::string caller;
    std::string callee;
    std::string audio_text;
    std::string token_url;
    std::string file_url;
    std::string audio_url;
    std::string audio_name;
    std::string file_name;
    std::string video_name;
    std::string res_id;
    
    //原始图片素材路劲
    std::string orgImagePath();
    //原始音频素材路径
    std::string orgAudioPath();
    //原始视频素材路径
    std::string orgVideoPath();
    //视频路径
    std::string videoPath();
	std::string videoTempPath();
    
    //是否需要替换音频
    bool needReplaceAudio();
    
    std::string updateVideoStateSql();
    
private:
    ResDir* mResDir;
    
    
};




#endif /* res_model_hpp */
