#ifndef MaoThread_Pool
#define MaoThread_Pool
#include<vector>
#include<memory>
#include<queue>
#include<mutex>
#include<condition_variable>
#include<type_traits>
#include<functional>
#include"utils.hpp"
#include"runnable.hpp"
namespace Mao{

    
        
    template<class run,class ...Args>
    class thread_pool{
        static_assert(std::is_base_of<Runnable,run>::value,"Not runnable");
        static_assert(std::is_constructible<run,Args&&...>::value,"Not constructible");
        protected:
        std::vector<std::unique_ptr<run>>thrs;
        public:
        explict thread_pool(int thr_num,Args&&... args){
            for(int i = 0 ; i < thr_num ; ++ i){
                thrs.emplace_back(
                    std::make_unique<run>(std::forward<Args>(args)...)
                    );
            }
        }
       thread_pool(const thread_pool&) = delete;
       thread_pool& operator=(const thread_pool &) = delete;
       thread_pool(thread_pool&&other) = default;
       thread_pool& operator=(thread_pool &&other) = default;
       virtual ~thread_pool() = default;

    };

}


#endif