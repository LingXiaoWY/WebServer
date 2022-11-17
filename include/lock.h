#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

class sem
{
private:
    sem_t m_sem;

public:
    sem()
    {
        if (sem_init(&m_sem, 0, 0) != 0)
            throw std::exception();
    }
    sem(int num)
    {
        if (sem_init(&m_sem, 0, num) != 0)
            throw std::exception();
    }
    ~sem()
    {
        sem_destroy(&m_sem);
    }

    bool wait()
    {
        return sem_wait(&m_sem) == 0;
    }
    bool post()
    {
        return sem_post(&m_sem) == 0;
    }
};

class locker
{
private:
    //声明一个互斥锁
    pthread_mutex_t m_mutex;

public:
    locker()
    {
        if (pthread_mutex_init(&m_mutex, nullptr) != 0) //初始化锁属性
            throw std::exception();
    }
    ~locker()
    {
        pthread_mutex_destroy(&m_mutex); //销毁互斥锁
    }
    bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0; //加锁
    }
    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0; //解锁
    }
    pthread_mutex_t *get()
    {
        return &m_mutex; //获得锁
    }
};

class cond
{
private:
    pthread_cond_t m_cond; //声明条件变量
public:
    cond()
    {
        //初始化条件变量
        if (pthread_cond_init(&m_cond, nullptr) != 0)
            throw std::exception();
    }
    ~cond()
    {
        //销毁条件变量
        pthread_cond_destroy(&m_cond);
    }
    bool wait(pthread_mutex_t *m_mutex)
    {
        //等待条件变量
        return pthread_cond_wait(&m_cond, m_mutex) == 0;
    }
    bool timewait(pthread_mutex_t *m_mutex, timespec t)
    {
        //定时等待条件变量
        return pthread_cond_timedwait(&m_cond, m_mutex, &t) == 0;
    }
    bool signal()
    {
        //唤醒一个等待条件变量的线程
        return pthread_cond_signal(&m_cond) == 0;
    }
    bool broadcast()
    {
        //唤醒所有等待条件变量的线程
        return pthread_cond_broadcast(&m_cond) == 0;
    }
};

#endif
