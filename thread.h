#ifdef _THREAD_H
#define _THREAD_H

#include <queue>
#include <pthread.h>
using namespace std;

struct task
{
  void (*run)(void* args);
  void *arg;
};

class ThreadPool
{
private:
  pthread_t *pthread_id;
  queue<task> task_list;         //任务队列
  bool shutdown_flag;            //线程退出标志
  int thread_num;                //线程数
  pthread_mutex_t m_pthreadmutex;//线程锁
  pthread_cond_t m_pthreadcond;  //线程同步条件变量
  
public:
  ThreadPool();                  
  virtual ~ThreadPool();        
  void *ThreadFunc();             //线程操作函数
  void Create_ThreadPool();       //创建池
  void AddTask(task ctask);       //添加任务
  void StopAll_Thread();          //线程池退出
  int GetTaskSize();              //获取任务数
}

#endif
