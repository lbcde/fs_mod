//
//  res_dir.hpp
//  downLoadTest
//
//  Created by hui gh on 2023/5/4.
//

#ifndef res_dir_hpp
#define res_dir_hpp

#include <stdio.h>

#include <string>
#include <vector>
#include <stdlib.h>

class ResDir
{
    
public:
    ResDir(const std::string& org_image_path,
           const std::string& org_audio_path,
           const std::string& org_video_path,
           const std::string& video_path);
    
    //原始图片素材路径
    std::string orgImagePath(const std::string image_name);
    //原始音频素材路径
    std::string orgAudioPath(const std::string audio_name);
    //原始视频素材路径
    std::string orgVideoPath(const std::string video_name);
    //视频路径
    std::string videoPath(const std::string video_name);
	std::string videoTempPath(const std::string video_name);
    
    
private:
    //原始图片素材存储路径，可能多个流程共用
    std::string org_image_path;
    //原始音频素材存储路径，可能多个流程共用
    std::string org_audio_path;
    //原始视频素材存储路径，可能多个流程共用
    std::string org_video_path;
    //视频路径
    std::string video_path;
    
};

#endif /* res_dir_hpp */
