#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>
#include <stdbool.h>
#include "list.h"






typedef struct ai_timer_single ai_timer_single;


//���ɶ���
ai_timer_single* ai_timer_new_single();


/**
 * ������ʱ��
 */
void start_timer_single(ai_timer_single* timer,int interval_sec,int interval_usec ,void(*callback)(void * userData), void * userData,bool isUserStartThread);


/**
 * ���ټ�ʱ��
 */

int timer_del_single(ai_timer_single* timer);


#endif // TIMER_H
