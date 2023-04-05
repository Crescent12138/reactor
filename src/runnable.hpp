///this is a thread
#ifndef MaoThread
#define MaoThread
#include<vector>
#include<memory>
#include<queue>
#include<mutex>
#include<csignal>
#include<future>
#include<pthread.h>
#include<exception>
#include"utils.hpp"
namespace Mao{
    class Runnable{

        pthread_t pth;
        std::promise<bool>pro;
        std::future<bool>fut;
        public:
        Runnable():fut(pro.get_future()){}
        Runnable(const Runnable&) = delete;
        Runnable(Runnable&&) = delete;
        Runnable &operator=(Runnable&&) = delete;
        virtual ~Runnable() = default;

        virtual void run_impl() = 0;

        void run (){
            auto thr_func = [](void *arg) -> void *{
                
            };
        }

    };
}
#endif