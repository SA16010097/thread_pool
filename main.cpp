#include "thread_pool.h"

void* my_fun(int num, void* args)
{
    cout << "run my fun:" << num << " thread_id:" << std::this_thread::get_id() << endl;
    sleep(100);
}

int main(){
    for(int i = 0; i < 30; ++i)
    {
        ThreadPool::GetInstance()->push(std::function<void*(void*)>(std::bind(my_fun, i, std::placeholders::_1))); 
    }
    while(true)
    {
        sleep(10); 
    }
    return 0;  
}
