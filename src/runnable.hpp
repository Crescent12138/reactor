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
                Runnable *ps = static_cast<Runnable*>(arg);
                if(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,nullptr) != 0) {
                    std::logic_error("pthread_setcancelstate error");
                }
                if (pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,nullptr) != 0){
                    std::logic_error("pthread_setcanceltype error");
                }
                ps->run_impl();
                ps->pro.set_value(true);
                return nullptr;
            };
            int ret =0;
            if(ret != pthread_create(&pth,nullptr,thr_func,this)){
                throw_system_error("pthread create error",ret);
            }
            
        }
        //join thread
        void join(){
            int ret = pthread_join(pth,nullptr);
            if(ret != 0){
                throw_system_error("pthread_join error",ret);
            }
        }

        //detach thread
        void detach(){
            int ret = pthread_detach(pth);
            if(ret != 0){
                throw_system_error("pthread detach error",ret);
            }
        }

        //cancel thread and return ok
        virtual bool cancel() noexcept {
            return 0==pthread_cancel(pth);
        }

        /// @brief wait and make share whether ok or not
        /// @tparam rep type of tik
        /// @tparam period turn of tik
        /// @param span 
        /// @return 
        template<class rep,class period>
        bool wait_for(const std::chrono::duration<rep,period>&span){
            std::future_status stat = fut.wait_for(span);
            bool ret = false;
            switch(stat){
                case std::future_status::ready:
                    ret = true;
                    break;
                case std::future_status::timeout:
                    break;
                default:
                    throw_system_error("future::wait_for error");
            }
            return ret;
        }

        /// @brief send signal to thread
        /// @param sig 
        void send_signal(int sig)noexcept{
            pthread_kill(pth,sig);
        }
    };
}
#endif