//实现线程同步机制的包装类，包装了信号量、条件变量和互斥锁用于线程见的同步
#ifndef LOCKER_H
#define LOCKER_H

#include<exception>
#include<pthread.h>
#include<semaphore.h>

class  sem{
    public:
        //创建并初始化信号量
        sem(){
            if(sem_init(&m_sem,0,0)!=0)
                throw std::exception();
        }
        //销毁信号量
        ~sem()
        {
            sem_destroy(&m_sem);
        }
        //等待信号量
        bool wait()
        {
            return sem_wait(&m_sem)==0; //该等待函数成功返回值为0
        }
        //增加信号量
        bool post()
        {
            return sem_post(&m_sem)==0;
        }
    private:
        sem_t m_sem;
};

//封装互斥锁类
class locker
{
    public:
    //初始化互斥锁
        locker(){
            pthread_mutex_init(&m_mutex,NULL);
        }
    //销毁互斥锁
    ~locker()
    {
        pthread_mutex_destroy(&m_mutex);
    }
    //获取互斥锁
    bool lock()
    {
        return pthread_mutex_lock(&m_mutex)==0;
    }
    //释放互斥锁
    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex);
    }
    private:
        pthread_mutex_t  m_mutex;
};

//封装条件变量类
class locker
{
private:
    pthread_cond_t  m_cond;
    pthread_mutex_t m_mutex;
public:
    //创建并初始化条件变量
    locker(){
    //此时的构造函数当中，初始化互斥锁和条件变量时可能会出现失败的情况，要及时抛出异常
        pthread_cond_init(&m_cond,NULL);
        pthread_mutex_init(&m_mutex,NULL);
    }
    //销毁条件变量
    ~locker(){
        pthread_cond_destroy(&m_cond);
        pthread_mutex_destroy(&m_mutex);  
    }
    //等待条件变量
     bool wait()
    {
        int ret=0;
        //在调用pthread_cond_wait()之前就要锁住条件变量防止更改
        pthread_mutex_lock(&m_mutex);
        //mutex作为参数来保护
        ret=pthread_cond_wait(&m_cond,&m_mutex);
        pthread_mutex_unlock(&m_mutex);
        return ret==0;
    }
    //唤醒等待改条件变量的线程
    bool signal()
    {
        return pthread_cond_signal(&m_cond);
    }
};




#endif