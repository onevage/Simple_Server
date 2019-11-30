//构造一个线程池
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include<list>
#include<cstdio>
#include<exception>
#include<pthread.h>
#include"locker.h"

//实现一个线程池类，为了能够复用，将其设置为模板类
template<typename T>
class threadpool{
    public:
    threadpool(int thread_number,int max_requests=10000);
    ~threadpool();
    //往队列里面添加任务
    bool append(T* request);

    private:
         static void* worker(void* arg);//工作线程运行的函数，不断从队列当中取出任务，并执行该任务
         void run();
    private:
        int m_thread_number;//线程池里面的线程数目
        int m_max_requests;//请求队列当中允许的最大请求数
        pthread_t* m_threads;//描述线程池的数组
        std::list<T*>   m_workqueue;//请求队列
        locker m_queuelocker;//保护请求队列的互斥锁
        sem m_queuestat;//当前是否有任务需要处理
        bool m_stop;//是否结束当前的线程
};

//进程池的构造函数
template<typename T>
threadpool<T>::threadpool(int thread_number,int max_requests):m_thread_number(thread_number),m_max_requests(max_requests),
                                                                                                                                        m_threads(NULL),m_stop(false)
{
    if(thread_number)<1||max_requests<1)
        throw std::exception();
    //创建存储线程号的数组
    m_threads = new pthread_t[m_thread_number];
    if(!m_threads)
        throw std::exception();
    //创建规定个数的线程，并将线程设置为detach()
    for(int i=0;i<thread_number;i++)
    {
        printf("create the %dth thread\n",i);
        if(pthread_create(m_threads+i,NULL,worker,this)!=0)
        {
            delete []m_threads;
            throw std::exception();
        }
        if(pthread_detach(m_threads[i]))
        {
        //分离成功的话，则返回值为0
            delete []m_threads;
            throw std::exception;
        }
    }
} 

//线程池的析构函数
template<typename T>
threadpool<T>::~threadpool()
{
    delete []m_threads;
    //将是否结束进程池当中的所有进程设置为true
    m_stop=true;
}

//向工作队列里面添加任务，此时一定要进行加锁
template<typename T>
bool threadpool<T>::append(T* request)
{
    m_queuelocker.lock();
    if(m_workqueue.size()>m_max_requests)
    {
        m_workqueue.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    //下面的信号量来增加任务队列的长度
    m_queuestat.post();
    return true;
}

//线程池里面的工作线程,传入的参数为当前线程池的地址
template<typename T>
void* threadpool<T>::worker(void* arg){
    //工作线程的参数为空指针的类型
    threadpool* pool=(threadpool*)arg;
    pool->run();
    return pool;
}

//工作线程执行的函数
template<typename T>
void threadpool<T>::run()
{
    //m_stop全局对象用来保证，当线程执行完一个任务队列当中的任务之后仍然能够继续从
    while(!m_stop)
    {   
        //等待当前的对立面有任务
        m_queuestat.wait();
        //加锁后读取并执行队列里面的任务
        m_queuelocker.lock();
        if(m_workqueue.empty()){
            m_queuelocker.unlock();
            continue;
        }
        T* request=m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();
        if(!request)
            continue;
        request->process();
    }
}
#endif 