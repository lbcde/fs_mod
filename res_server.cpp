//
//  res_server.cpp
//  downLoadTest
//
//  Created by hui gh on 2023/5/5.
//

#include "res_server.hpp"
#include "ai_http_client.h"



void timer_callback(void *userData) {
    ResServer* res_server = (ResServer*)userData;
    res_server->downloadResAndCreateVideo();
}

void image_download_completed(ai_http_client *client, http_response* response){
    ResServer *res_server = (ResServer *)response->userData;
    ResModel *res_model = (ResModel *)response->ext;
    if (response->code == 200) {
        printf("image_res_id  downloaded successfully.\n");
        //检测并合成视频
        res_server->removeFromDownloadingQueue(res_model->file_id);
        res_server->checkAndCreeateVideo(res_model);
    
    }else {
        printf("image Download failed. res_id: \n");
    }
    
    ai_http_dealloc(client);
}

void audio_download_completed(ai_http_client *client, http_response* response){
    ResServer *res_server = (ResServer *)response->userData;
    ResModel *res_model = (ResModel *)response->ext;
    if (response->code == 200) {
        printf("image_res_id  downloaded successfully.\n");
        //检测并合成视频
        res_server->removeFromDownloadingQueue(res_model->audio_name);
        res_server->checkAndCreeateVideo(res_model);
    }else {
        printf("image Download failed. res_id: \n");
    }
    ai_http_dealloc(client);
    
}

void video_download_completed(ai_http_client *client, http_response* response){
    
    ResServer *res_server = (ResServer *)response->userData;
    ResModel *res_model = (ResModel *)response->ext;
    if (response->code == 200) {
        printf("image_res_id  downloaded successfully.\n");
        //检测并合成视频
        res_server->removeFromDownloadingQueue(res_model->file_id);
        res_server->checkAndCreeateVideo(res_model);
    
    }else {
        printf("image Download failed. res_id: \n");
    }
    ai_http_dealloc(client);
}


ResServer::ResServer(const std::string& db_name,
          const std::string& user_name,
          const std::string& pwd,
          const std::string& db_host,
          const std::string& org_image_path,
          const std::string& org_audio_path,
          const std::string& org_video_path,
           const std::string& video_path){
    this->db_name = db_name;
    this->user_name = user_name;
	this->pwd = pwd;
    this->db_host = db_host;
    this->res_dir = new ResDir(org_image_path,org_audio_path,org_video_path,video_path);
}

void ResServer::start(int interval_sec,bool isUserStartThread){

    this->my_sql = ai_mysql_conn_new((char*)this->db_host.c_str(), (char*)this->user_name.c_str(), (char*)this->pwd.c_str(), (char*)this->db_name.c_str());
    this->timer = ai_timer_new();
    if (timer == NULL) {
        printf("Failed to create timer.\n");
        return;
    }
    start_timer(timer, 0, interval_sec,timer_callback,this, isUserStartThread);
}

void ResServer::downloadResAndCreateVideo(){
	printf("schedule check... .\n");
    std::vector<ResModel*> down_list = getDownloadList();
	int count = down_list.size();
    for (int i=0; i< count; i++) {
        ResModel* res_model = down_list[i];
        
        if (isMp4(res_model->file_id)) {
            
            //是否在下载队列
            if (!inDownloadingQueue(res_model->file_id)) {
                //判断原始视频是否已经下载
                if (!isResExist(res_model->orgVideoPath())) {
                    //视频下载
					
                    videoDownload(res_model);
                
                }else{
                    
                    //是否需要替换音频
                    if (res_model->needReplaceAudio()) {
                        //是否在下载队列
                        if(!inDownloadingQueue(res_model->audio_name)){
                            //判断音频原始素材是否已下载
                            if (!isResExist(res_model->orgAudioPath())) {
                                //下载音频资源

								
                                audioDownload(res_model);
                                
                            }else{
                                //资源已经存在，查看是否已经生成视频
                                if (!inSynthQueue(res_model->videoPath())){
                                    if (!isResExist(res_model->videoPath())) {
                                        //音频替换
										
                                        audioReplace(res_model);
                                        
                                    }
                                }
                                
                            }
                        }
					}
					else
					{
						updateVideoState(res_model);
					}
                    
                }
            }
            
        }else{
            
            //是否在下载队列
			
            if (!inDownloadingQueue(res_model->file_id)) {
                //判断原始图片资源是否已经下载
                if (!isResExist(res_model->orgImagePath())) {
                    //下载图片 资源
					
                    imageDownload(res_model);
                }else{
                    //是否需要替换音频
					
                    if (res_model->needReplaceAudio()) {
                        //是否在下载队列
						
                        if(!inDownloadingQueue(res_model->audio_name)){
                            //判断音频原始素材是否已下载
							
                            if (!isResExist(res_model->orgAudioPath())) {
                                //下载音频资源
								
                                audioDownload(res_model);
                                
                            }else{
                                //是否在合成队列
								
                                if (!inSynthQueue(res_model->video_name)) {
									
                                    if (!isResExist(res_model->videoPath())) {
                                        //视频合成
										if(isCommandline){
											videoCreate(res_model);
										}
										else {
											videoCreateSourceCode(res_model);
										}
										
                                    }
                                }
                            }
                        }
                    }
                    
                }
            }
        }
    }
}


void ResServer::checkAndCreeateVideo(ResModel*resmodel){
    
    if (isMp4(resmodel->file_id)) {
        
        //是否在下载队列
        if (!inDownloadingQueue(resmodel->file_id)) {
            //判断原始视频是否已经下载
            if (isResExist(resmodel->orgVideoPath())) {
                //是否需要替换音频
                if (resmodel->needReplaceAudio()) {
                    //是否在下载队列
                    if(!inDownloadingQueue(resmodel->audio_name)){
                        //判断音频原始素材是否已下载
                        if (isResExist(resmodel->orgAudioPath())) {
                            //资源已经存在，查看是在视频生成队列
                            if (!inSynthQueue(resmodel->videoPath())){
                                if (!isResExist(resmodel->videoPath())) {
                                    //音频替换
                                    audioReplace(resmodel);
                                }
                            }
                        }
                    }
                }
            
            }
        }
        
    }else{
        
        //是否在下载队列
        if (!inDownloadingQueue(resmodel->file_id)) {
            //判断原始图片资源是否已经下载
            if (isResExist(resmodel->orgImagePath())) {
                //是否需要替换音频
                if (resmodel->needReplaceAudio()) {
                    //是否在下载队列
                    if(!inDownloadingQueue(resmodel->audio_name)){
                        //判断音频原始素材是否已下载
                        if (isResExist(resmodel->orgAudioPath())) {
                            //是否在合成队列
                            if (!inSynthQueue(resmodel->videoPath())) {
                                if (!isResExist(resmodel->videoPath())) {
                                    //视频合成
									if (isCommandline) {
										videoCreate(resmodel);
									}
									else {
										videoCreateSourceCode(resmodel);
									}
									
									
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

std::vector<ResModel*> ResServer::getDownloadList(){
    
    std::vector<ResModel*> rst;
    std::string  sql = "SELECT * FROM ai_ivvr_res WHERE video_state = 0;";
    ai_sql_res *sql_res = ai_mysql_query(this->my_sql,(char*)sql.c_str());
    if (sql_res==NULL)
    {
        return rst;
    }
    
    if (sql_res == NULL)
    {
        return rst;
    }
    int i;
    int rowcount = row_count(sql_res);
    //数据解析
    for (i = 0; i < rowcount; ++i) {
        ResModel* res = new ResModel(this->res_dir);
        res->file_state = int_res(sql_res, i, 1);
        //数据解析
        res->file_id = strdup(char_res(sql_res, i, 0));
        res->caller = strdup(char_res(sql_res, i, 2));
        res->callee = strdup(char_res(sql_res, i, 3));
        res->audio_text = strdup(char_res(sql_res, i, 4));
        res->token_url = strdup(char_res(sql_res, i, 5));
        res->audio_url = strdup(char_res(sql_res, i, 6));
        res->file_url = strdup(char_res(sql_res, i, 7));
        res->audio_name = strdup(char_res(sql_res, i, 8));
        res->file_name = strdup(char_res(sql_res, i, 9));
        res->video_name = strdup(char_res(sql_res, i, 10));
        res->res_id = strdup(char_res(sql_res, i, 11));
        res->audio_state = int_res(sql_res, i, 12);;
        res->video_state = int_res(sql_res, i, 13);;
        rst.push_back(res);
    }
    return rst;
}

bool ResServer::inDownloadingQueue(const std::string key){
    std::map<std::string,ResModel*>::iterator it;
    it = this->mDownloadQueue.find(key);
    if (it!=this->mDownloadQueue.end()) {
        return true;
    }
    return false;
}

bool ResServer::inSynthQueue(const std::string key){
    std::map<std::string,ResModel*>::iterator it;
    it = this->mSynthQueue.find(key);
    if (it!=this->mSynthQueue.end()) {
        return true;
    }
    return false;
}

bool ResServer::isMp4(const std::string fileName){
    int pos = fileName.find(".mp4");
    if(pos != std::string::npos)
    {
       return true;
   }
    return false;
}

bool ResServer::isResExist(const std::string fullPath){

	return (access(fullPath.c_str(), F_OK) != -1);
	
}

void ResServer::removeFromDownloadingQueue(const std::string key){
    this->mDownloadQueue.erase(key);
}

void ResServer::removeFromSynthQueue(const std::string key){
    this->mSynthQueue.erase(key);
}


void ResServer::imageDownload(ResModel*resmodel){
    int res;
	this->mDownloadQueue.insert(std::pair<std::string, ResModel*>(resmodel->file_id, resmodel));
    ai_http_client *client = ai_http_client_new();
    ai_http_on_request_complete(client, image_download_completed);
    res = ai_download(client, (char*)resmodel->file_url.c_str(), NULL, NULL, this,resmodel, (char*)resmodel->orgImagePath().c_str());
}

void ResServer::audioDownload(ResModel*resmodel){
    int res;
	this->mDownloadQueue.insert(std::pair<std::string, ResModel*>(resmodel->audio_name, resmodel));
    ai_http_client *client = ai_http_client_new();
    ai_http_on_request_complete(client, audio_download_completed);
    res = ai_download(client, (char*)resmodel->audio_url.c_str(), NULL, NULL, this,resmodel, (char*)resmodel->orgAudioPath().c_str());
}

void ResServer::videoDownload(ResModel*resmodel){
	int res;
    this->mDownloadQueue.insert(std::pair<std::string, ResModel*>(resmodel->file_id, resmodel));
    
    ai_http_client *client = ai_http_client_new();
    ai_http_on_request_complete(client, video_download_completed);
    res = ai_download(client, (char*)resmodel->file_url.c_str(), NULL, NULL, this,resmodel, (char*)resmodel->orgVideoPath().c_str());
    
}


void ResServer::audioReplace(ResModel*resmodel){
    int res;
    const std::string video_path = resmodel->videoPath();
    const std::string audio_path = resmodel->orgAudioPath();
    const std::string out_video_path = resmodel->videoPath();
	std::string  command = "ffmpeg -y -i " + video_path + "  -i " + audio_path + " -c:v copy -c:a aac -strict experimental -map 0:v:0 -map 1:a:0 " + out_video_path;
	printf("command:   %s\n",command.c_str());
	this->mDownloadQueue.insert(std::pair<std::string, ResModel*>(resmodel->video_name, resmodel));
	res=system(command.c_str());
	if (res) {
		updateVideoState_fail(resmodel);
	}
	else {
		updateVideoState(resmodel);
	}
    this->mSynthQueue.erase(resmodel->video_name);
}


void ResServer::videoCreate(ResModel*resmodel){
	int res_1;
	int res_2;
    const std::string image_path = resmodel->orgImagePath();
    const std::string audio_path = resmodel->orgAudioPath();
    const std::string out_video_path = resmodel->videoPath();
	const std::string output_path_temp = resmodel->videoTempPath();
	std::string command_img = "ffmpeg -y -r 1 -loop 1 -i " + image_path + " -pix_fmt yuv420p -vcodec mpeg4 -r:v 10 -preset medium -crf 20  -t 2 " + output_path_temp;
	std::string command_vedio = "ffmpeg -y -i " + output_path_temp + "  -i " + audio_path + " -c:v copy -c:a aac -strict experimental -map 0:v:0 -map 1:a:0 " + out_video_path;

	this->mDownloadQueue.insert(std::pair<std::string, ResModel*>(resmodel->video_name, resmodel));
	printf("command:   %s\n", command_img.c_str());
	res_1=system(command_img.c_str());
	if (res_1) {
		updateVideoState_fail(resmodel);
		this->mSynthQueue.erase(resmodel->video_name);
		return;
	}
	printf("command:   %s\n", command_vedio.c_str());
	res_2=system(command_vedio.c_str());
    
	if (res_2) {
		updateVideoState_fail(resmodel);
	}
	else {
		updateVideoState(resmodel);
	}
    this->mSynthQueue.erase(resmodel->video_name);
    
}


void ResServer::videoCreateSourceCode(ResModel*resmodel) {

	const std::string image_path_str = resmodel->orgImagePath();
	const std::string audio_path_str = resmodel->orgAudioPath();
	const std::string out_video_path_str = resmodel->videoPath();
	const std::string output_path_temp = resmodel->videoTempPath();
	char* image_path = new char[image_path_str.length() + 1];
	strcpy(image_path, image_path_str.c_str());

	char* audio_path = new char[audio_path_str.length() + 1];
	strcpy(audio_path, audio_path_str.c_str());

	char* out_video_path = new char[out_video_path_str.length() + 1];
	strcpy(out_video_path, out_video_path_str.c_str());
	ai_media_engine *engine = media_engine_new();

	
	image_audio_to_video(engine, image_path, audio_path, out_video_path);

	media_engine_dealloc(engine);
	delete[] image_path;
	delete[] audio_path;
	delete[] out_video_path;
}







void ResServer::updateVideoState(ResModel*resmodel) {
	int res;
	resmodel->video_state = 1;
	std::string sql = resmodel->updateVideoStateSql();
	printf("sql =  %s  \n", (char*)sql.c_str());
	res = ai_mysql_exe(this->my_sql, (char*)sql.c_str());
	if (res) {
		printf("Error: Failed to update row with res_id: %s\n", (char*)resmodel->res_id.c_str());
	}
	else {
		printf("Successfully updated row with res_id: %s\n", (char*)resmodel->res_id.c_str());
	}

}


void ResServer::updateVideoState_fail(ResModel*resmodel) {
	int res;
	resmodel->video_state = 0;
	std::string sql = resmodel->updateVideoStateSql();
	printf("sql =  %s  \n", (char*)sql.c_str());
	res = ai_mysql_exe(this->my_sql, (char*)sql.c_str());
	if (res) {
		printf("Error: Failed to update row with res_id: %s\n", (char*)resmodel->res_id.c_str());
	}
	else {
		printf("Successfully updated row with res_id: %s\n", (char*)resmodel->res_id.c_str());
	}

}

