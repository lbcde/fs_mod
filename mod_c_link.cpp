#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "ai_media_engine.h"
#include "ai_mysql.h"
#include "ai_sql_res.h"
#include "ai_timer_single.h"
#include "ai_caculate.h"
#include "ai_http_client.h"
#include "cJSON.h"
#include "cJSON_Utils.h"
#include <libavutil/log.h>
#include <switch.h>
#include <libavutil/log.h>
#include <unistd.h>
#include "res_dir.hpp"
#include "res_model.hpp"
#include "res_server.hpp"
#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <sys/stat.h>




static char *MYSQL_HOST = NULL;
static char *MYSQL_USERNAME = NULL;
static char *MYSQL_PASSWORD = NULL;
static char *MYSQL_DATABASE = NULL;
static int TTS_TYPE = 0;
static char *ORG_IMG_PATH = NULL;
static char *ORG_AUD__PATH = NULL;
static char *OGR_VED_PATH = NULL;
static char *VIDEO_PATH = NULL;

bool isCommandline = false;

void create_directory_if_not_exists(char* path) {
	char* subpath;
	char* original = strdup(path);  

	subpath = strtok(original, "/");
	while (subpath != NULL) {
		if (access(subpath, F_OK) == -1) {
			if (mkdir(subpath, 0700) == -1) {
				perror("Error creating directory");
				free(original);  
				return;
			}
		}
		chdir(subpath);
		subpath = strtok(NULL, "/");
	}
	free(original);  
}

void assign_value(const char *name, const char *value) {
	if (strcmp(name, "host") == 0) {
		MYSQL_HOST = strdup(value);
	}
	else if (strcmp(name, "username") == 0) {
		MYSQL_USERNAME = strdup(value);
	}
	else if (strcmp(name, "password") == 0) {
		MYSQL_PASSWORD = strdup(value);
	}
	else if (strcmp(name, "database") == 0) {
		MYSQL_DATABASE = strdup(value);
	}
	else if (strcmp(name, "tts_type") == 0) {
		TTS_TYPE = atoi(value);
	}
	else if (strcmp(name, "org_path") == 0) {
		ORG_IMG_PATH = strdup(value);
		create_directory_if_not_exists(ORG_IMG_PATH);
	}
	else if (strcmp(name, "org_aud_path") == 0) {
		ORG_AUD__PATH = strdup(value);
		create_directory_if_not_exists(ORG_AUD__PATH);
	}
	else if (strcmp(name, "org_ved_path") == 0) {
		OGR_VED_PATH = strdup(value);
		create_directory_if_not_exists(OGR_VED_PATH);
	}
	else if (strcmp(name, "video_path") == 0) {
		VIDEO_PATH = strdup(value);
		create_directory_if_not_exists(VIDEO_PATH);
	}
	else if (strcmp(name, "isCommandline") == 0) {
		isCommandline = strcmp(value, "true") == 0;
		
	}
}

void process_elements(xmlNode *node) {
	for (xmlNode *cur_node = node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			xmlChar *content = xmlNodeGetContent(cur_node);
			assign_value((const char *)cur_node->name, (const char *)content);
			xmlFree(content);
		}
		process_elements(cur_node->children);
	}
}

void parse_config(const char *filename) {
	xmlDoc *doc = xmlReadFile(filename, NULL, 0);
	if (doc == NULL) {
		std::cerr << "Failed to parse XML file." << std::endl;
		return;
	}

	xmlNode *root_element = xmlDocGetRootElement(doc);
	process_elements(root_element);

	xmlFreeDoc(doc);
	xmlCleanupParser();
}









int main() {


	parse_config("c_link.config.xml");

	ResServer* res_server = new ResServer(MYSQL_DATABASE,
		MYSQL_USERNAME,
		MYSQL_PASSWORD,
		MYSQL_HOST,
		ORG_IMG_PATH,
		ORG_AUD__PATH,
		OGR_VED_PATH,
		VIDEO_PATH);

	res_server->start(500000£¬false);
	while (1)
	{
		sleep(1);
	}


}


