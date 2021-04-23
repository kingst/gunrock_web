#include "dthread.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <fcntl.h>
#include <unistd.h>

pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
std::vector<pthread_t> thread_list;
int logFd = -1;

void set_log_file(std::string file_name) {
  logFd = open(file_name.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
  if (logFd < 0) {
    std::cerr << "Could not open log file: " << file_name << std::endl;
    exit(1);
  }
}

void sync_print(std::string function, std::string payload) {
  int ret = pthread_mutex_lock(&print_lock);
  if (ret != 0) {
    std::cerr << "pthread lock mutex error!" << std::endl;
    exit(1);
  }

  // search through our thread list to find this thread
  pthread_t self = pthread_self();
  int tid = -1;
  for (size_t idx = 0; idx < thread_list.size(); idx++) {
    if (pthread_equal(self, thread_list[idx])) {
      tid = idx;
      break;
    }
  }

  // if we don't find self in our thread list, add it
  if (tid < 0) {
    tid = thread_list.size();
    thread_list.push_back(self);
  }

  std::stringstream write_stream;
  write_stream << function << " thread: " << tid << " " << payload << std::endl;
  std::string write_buffer = write_stream.str();
  ret = write(logFd, write_buffer.c_str(), write_buffer.length());
  if (ret >= 0 && (size_t) ret != write_buffer.length()) {
    std::cerr << "log file write error, ret = " << ret << " expected " << write_buffer.length() << std::endl;
    exit(1);
  }
  ret = pthread_mutex_unlock(&print_lock);
  if (ret != 0) {
    std::cerr << "pthread mutex unlock error!" << std::endl;
    exit(1);
  }
}

void sync_print_thread(std::string function, pthread_mutex_t *mutex, pthread_cond_t *cond) {
  std::stringstream payload;
  payload << " mutex: " << (void *) mutex << " cond: " << (void *) cond;
  sync_print(function, payload.str());
}

struct DthreadArgs {
  void *callerArg;
  void *(*start_routine)(void *);
};

void *my_start_routine(void *arg) {
  struct DthreadArgs *dthreadArgs = (struct DthreadArgs *) arg;

  sync_print_thread("my_start_routine_enter", NULL, NULL);
  void *ret = dthreadArgs->start_routine(dthreadArgs->callerArg);
  sync_print_thread("my_start_routine_return", NULL, NULL);

  delete dthreadArgs;
  return ret;
}

int dthread_create(pthread_t *thread, const pthread_attr_t *attr,
		   void *(*start_routine)(void *), void *arg) {
  struct DthreadArgs *dthreadArgs = new struct DthreadArgs;
  dthreadArgs->start_routine = start_routine;
  dthreadArgs->callerArg = arg;
  return pthread_create(thread, attr, my_start_routine, dthreadArgs);
}

int dthread_detach(pthread_t thread) {
  sync_print_thread("dthread_detach_enter", NULL, NULL);
  int ret = pthread_detach(thread);
  sync_print_thread("dthread_detach_return", NULL, NULL);

  return ret;
}

int dthread_mutex_lock(pthread_mutex_t *mutex) {
  sync_print_thread("dthread_mutex_lock_enter", mutex, NULL);
  int ret = pthread_mutex_lock(mutex);
  sync_print_thread("dthread_mutex_lock_return", mutex, NULL);

  return ret;
}

int dthread_mutex_unlock(pthread_mutex_t *mutex) {
  sync_print_thread("dthread_mutex_unlock_enter", mutex, NULL);
  int ret = pthread_mutex_unlock(mutex);
  sync_print_thread("dthread_mutex_unlock_return", mutex, NULL);

  return ret;
}

int dthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
  sync_print_thread("dthread_cond_wait_enter", mutex, cond);
  int ret = pthread_cond_wait(cond, mutex);
  sync_print_thread("dthread_cond_wait_return", mutex, cond);

  return ret;
}

int dthread_cond_signal(pthread_cond_t *cond) {
  sync_print_thread("dthread_cond_signal_enter", NULL, cond);
  int ret = pthread_cond_signal(cond);
  sync_print_thread("dthread_cond_signal_return", NULL, cond);

  return ret;
}

int dthread_cond_broadcast(pthread_cond_t *cond) {
  sync_print_thread("dthread_cond_broadcast_enter", NULL, cond);
  int ret = pthread_cond_broadcast(cond);
  sync_print_thread("dthread_cond_broadcast_return", NULL, cond);

  return ret;
}
