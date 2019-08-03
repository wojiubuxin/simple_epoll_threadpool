#include "thread.h"
#include <sys/sysinfo.h>
ThreadPool::ThreadPool()
{
  shutdown_flag = false;
  thread_num = 1;
  m_pthreadnutex = PTHREAD_MUTEX_INITALIZER;
  m_pthreadcond = PTHREAD_COND_INITALIZER;
  Create_ThreadPool();
}

ThreadPool::~ThreadPool()
{
  StopAll_Thread();
}

void ThreadPool::Create_ThreadPool()
{
  thread_num = get_nprocs() * 2;
  pthread_id = (pthread_t)malloc(sizeof(pthread_t)*thread_num);
  for(int i = 0; i < thread_num; i++)
  {
    pthread_create(&pthread_id[i], NULL, ThreadFunc, NULL);
  }
}

void ThreadPool::StopAll_Thread()
{
  if(shutdown_flag)
  {
    return;
  }
  
  shutdonw_flag = true;
  pthread_cond_broadcast(&m_pthreadcond);
  
  for(int i=0; i < thread_num; i++)
  {
    pthread_join(pthread_id[i], NULL);
  }
  
  free(pthread_id);
  pthread_id = NULL;
  
  pthread_mutex_destroy(&m_pthreadmutex);
  pthread_cond_destroy(&m_pthreadcond);
}

void* ThreadPool::ThreadFunc()
{
  while(1)
  {
    pthread_mutex_lock(&m_pthreadmutex);
    while(task_list.size() == 0 && shutdown_flag == flase)
    {
      pthread_cond_wait(&m_pthreadcond, &m_pthreadmutex);
    }
    
    if(shutdown_flag)
    {
      pthread_mutex_unlock(&m_pthreadmutex);
      pthread_exit(NULL);
    }
    
    task ctask = task_list.front();
    task_list.pop();
    pthread_mutex_unlock(&m_pthreadmutex);
    ctask.run(ctask.arg);
    catch(...)
    {}
  }
}

int ThreadPool::GetTaskSize()
{
  return task_list.size();
}

void ThreadPool::AddTask(task ctask)
{
  pthread_mutex_lock(&m_pthreadmutex);
  task_list.push(ctask);
  pthread_cond_signal(&m_pthreadcond);
  pthread_mutex_unlock(&m_pthreadmutex);
}
