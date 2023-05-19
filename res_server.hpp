//
//  res_server.hpp
//  downLoadTest
//
//  Created by hui gh on 2023/5/5.
//

#ifndef res_server_hpp
#define res_server_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include "res_model.hpp"
#include "res_dir.hpp"
#include "ai_mysql.h"
#include "ai_timer_single.h"

#include "ai_media_engine.h"
extern bool isCommandline;


class ResServer
{
    
public:
    ResServer(const std::string& db_name,
               const std::string& user_name,
               const std::string& pwd,
               const std::string& db_host,
               const std::string& org_image_path,
               const std::string& org_audio_path,
               const std::string& org_video_path,
               const std::string& video_path);
    
    //启动
    void start(int interval_sec,bool isUserStartThread);
    
    
    
    //内部接口
    void downloadResAndCreateVideo();
    void checkAndCreeateVideo(ResModel*resmodel);
    void removeFromDownloadingQueue(const std::string key);
    void removeFromSynthQueue(const std::string key);
    
private:
    std::vector<ResModel*> getDownloadList();
    bool inDownloadingQueue(const std::string key);
    bool inSynthQueue(const std::string key);
    
    bool isMp4(const std::string fileName);
    bool isResExist(const std::string fullPath);
    //图片下载
    void imageDownload(ResModel*resmodel);
    //音频下载
    void audioDownload(ResModel*resmodel);
    //视频下载
    void videoDownload(ResModel*resmodel);
    //音频替换
    void audioReplace(ResModel*resmodel);
    //视频生成
    void videoCreate(ResModel*resmodel);
	void videoCreateSourceCode(ResModel*resmodel);
	void updateVideoState(ResModel*resmodel);
	void updateVideoState_fail(ResModel*resmodel);

    
private:
    std::string db_name;
    std::string user_name;
    std::string db_host;
	std::string pwd;
    ResDir* res_dir;
    
    ai_timer *timer;
    ai_mysql *my_sql;
    //资源下载队列
    std::map<std::string,ResModel*> mDownloadQueue;
    //视频合成队列
    std::map<std::string,ResModel*> mSynthQueue;
};


#endif /* res_server_hpp */
