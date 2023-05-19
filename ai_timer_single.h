#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>
#include <stdbool.h>
#include "list.h"






typedef struct ai_timer_single ai_timer_single;


//生成对象
ai_timer_single* ai_timer_new_single();


/**
 * 启动计时器
 */
void start_timer_single(ai_timer_single* timer,int interval_sec,int interval_usec ,void(*callback)(void * userData), void * userData,bool isUserStartThread);


/**
 * 销毁计时器
 */

int timer_del_single(ai_timer_single* timer);


#endif // TIMER_H
