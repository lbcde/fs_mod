#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "ai_timer_single.h"



struct ai_timer_single {
	timer_t timerid;
	struct timeval timeout;
	struct timeval interval;
	bool isloop;
	void *userData;
	void(*callback)(void *);
	struct list_head list;
};
// 全局链表，用于存储计时器节点
LIST_HEAD(timer_list);

void timer_initial() {
	INIT_LIST_HEAD(&timer_list);
}

static void timer_handler(int sig, siginfo_t *si, void *uc) {
	struct ai_timer *tn;

	tn = (struct ai_timer *)(si->si_value.sival_ptr);

	tn->callback(tn->userData);

	if (!tn->isloop) {
		timer_del(tn); 
	}
}


unsigned long int timer_start(ai_timer *tn,bool is_loop, void(*callback)(void * userData), void * userData, long sec, long usec) {
	struct sigevent sev;
	struct itimerspec its;


	tn->interval.tv_sec = sec;
	tn->interval.tv_usec = usec;
	tn->isloop = is_loop;
	tn->userData = userData;
	tn->callback = callback;

	// 使用 CLOCK_REALTIME 时钟
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGRTMIN;
	sev.sigev_value.sival_ptr = tn;

	// 创建定时器
	if (timer_create(CLOCK_REALTIME, &sev, &tn->timerid) == -1) {
		free(tn);
		return 0;
	}

	// 设置定时器超时时间和间隔
	its.it_value.tv_sec = sec;
	its.it_value.tv_nsec = usec * 1000;
	its.it_interval.tv_sec = is_loop ? sec : 0;
	its.it_interval.tv_nsec = is_loop ? usec * 1000 : 0;

	// 启动定时器
	if (timer_settime(tn->timerid, 0, &its, NULL) == -1) {
		timer_delete(tn->timerid); 
		free(tn);
		return 0;
	}

	// 将新定时器添加到链表中
	timer_list_addxxx(&tn->list,&timer_list);
	return (unsigned long int)tn->timerid; 
}

int timer_del_single(ai_timer* timer) {
	struct ai_timer *tn, *n;
	
	list_for_each_entry_safe(tn, n, &timer_list, list) {
		if (timer->timerid == tn->timerid) {
			list_del(&tn->list);
			timer_delete(tn->timerid);
			free(tn);
			printf("release succeed\n");

			return 0;
		}
	}

	return -1;
}
//释放计时器

//生成对象
ai_timer* ai_timer_new_single() {
	struct ai_timer *tn;
	tn = (struct ai_timer *)malloc(sizeof(struct ai_timer));
	if (tn == NULL) {
		return NULL;
	}
	return tn;

}

void start_timer_single(ai_timer* timer, int interval_sec,int interval_usec ,void(*callback)(void *), void *userData,bool isUserStartThread) {
	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = timer_handler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	timer_initial();
	timer_start(timer, true, callback,userData , interval_sec, interval_usec);
	
}






