#pragma once

#include <iostream>
#include <thread>
#include <string>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <list>
#include <vector>
#include <unistd.h>
#include <time.h>
#include <memory>
#include <queue>
#include <memory>
using namespace std;


class ThreadPool{
public:
    ThreadPool() = delete;
    static ThreadPool* GetInstance()
    {
        static ThreadPool* ptr = new ThreadPool(10);
        return ptr; 
    }

    ~ThreadPool(){
        stop();
        for(auto& item:m_threads)
            item->join();
    }

    void push(std::function<void*(void*)> func){
        std::unique_lock<std::mutex> tmp_lock(m_mutex);
        m_fun_queue.push(std::function<void*(void*)>(func));
        tmp_lock.unlock();
        m_cv.notify_one();
    }

    void run(){
        for(size_t i = 0; i < m_thread_num; ++i)
        {
            m_threads.push_back(std::make_shared<std::thread>([this]{
                    while(true){
                    std::unique_lock<std::mutex> tmp_lock(this->m_mutex); 

                    while(this->m_fun_queue.empty() && !this->m_stop){
                    this->m_cv.wait(tmp_lock); 
                    }
                    if(this->m_stop)
                    {
                        cout << "queue stop." << endl; 
                        return; 
                    }

                    auto fun = std::move(this->m_fun_queue.front());
                    this->m_fun_queue.pop(); 
                    tmp_lock.unlock();
                  
                    void* arg;
                    (fun)(arg);
                    
                    }
                    }));
        }
    }

    void stop()
    {
        std::unique_lock<std::mutex> tmp_lock(m_mutex);
        m_stop = true; 
        tmp_lock.unlock();
        m_cv.notify_all(); 
    }

private:
    ThreadPool(int n):m_stop(false), m_thread_num(n){run();}

private:
    std::vector<std::shared_ptr<std::thread> > m_threads;
    size_t m_thread_num;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::queue<std::function<void*(void*)> > m_fun_queue; 
    bool m_stop;
};


