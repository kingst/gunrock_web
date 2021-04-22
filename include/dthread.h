#ifndef _DTHREAD_H_
#define _DTHREAD_H_

#include <pthread.h>
#include <string>

int dthread_create(pthread_t *thread, const pthread_attr_t *attr,
		   void *(*start_routine)(void *), void *arg);
int dthread_detach(pthread_t thread);

int dthread_mutex_lock(pthread_mutex_t *mutex);
int dthread_mutex_unlock(pthread_mutex_t *mutex);

int dthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int dthread_cond_signal(pthread_cond_t *cond);
int dthread_cond_broadcast(pthread_cond_t *cond);


// don't use these, they're used by the autograder
void sync_print(std::string function, std::string payload);
void set_log_file(std::string file_name);

#endif
