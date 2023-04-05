#ifndef MaoUtils
#define MaoUtils
#include<cstring>
#include<cstdint>
#include<functional>
#include<chrono>
#include<string>
#include<thread>
#include<vector>
#include<ctime>
#include<csignal>
#include<cassert>
#include<system_error>
#include<algorithm>
namespace Mao{
    using string = std::string;
    enum priority{
        p6 = 14,
        p5,p4,p3,p2,p1,p0
    };


    /// @brief hash class
    class enum_hash{
        public:
        template<class T> 
        std::size_t operator ()(const T&t) const {
            return std::hash<int>()(int(t));
        }
    };


    /// @brief tuple class used to fast compare a < b
    /// @tparam T tuple
    /// @tparam sz tuple.No
    template<class T ,size_t sz>
    class tuple_less{
        public:
        bool operator()(const T& lhs,const T& rhs) const noexcept{
            return std::get<sz>(lhs) < std::get<sz>(rhs);
        }
    };


    /// @brief tuple class used to fast compare a > b
    /// @tparam T tuple
    /// @tparam sz tuple.No
    template<class T ,size_t sz>
    class tuple_greater{
        public:
        bool operator()(const T& lhs,const T& rhs) const noexcept{
            return std::get<sz>(lhs) > std::get<sz>(rhs);
        }
    };
    /*
        time
    */

    time_t time(){
        time_t t = ::time(nullptr);
        if (t == -1) {
            throw_system_error("time error");
        }
        return t;
    }


    /// @brief time template util
    /// @param t time()
    /// @param format  template default "%F %T %Z"
    /// @return 
    string timestamp(time_t t = time(),const char *format = "%F %T %Z"){
        static_assert(std::is_signed<time_t>::value,"time_t is not signed!");
        tm s_tm;
        localtime_r(&t,&s_tm);
        char buff [1024];
        ::memset(buff,0,1024);
        if(0 == strftime(buff,1024,format,&s_tm)){
            throw_system_error("strftime error");
        }
        return buff;
    }

    template <class Clock = std::chrono::system_clock>
    void sleep_until(){
        
    }
    /*
        algorithm
    */
   
   /// @brief least_common_multiple
   /// @tparam T integral
   /// @param nums vector
   /// @return 
   template<class T>
   T lcm(std::vector<T>&nums){
        if(nums.empty()){
            std::logic_error("lcm could not support an empty set");
        }
        T ans = 1;
        for(auto const& k : nums){
            //both zero are not allowed
            ans =lcm(k,ans);
        }
        return ans;
   }

   template<class T>
   T lcm(T a,T b){
        assert(a != 0 && b != 0);
        return (a / gcd(a,b) * b);
   }
   
   template<class T>
   T gcd(std::vector<T>&nums){
        if(nums.empty()){
            std::logic_error("lcm could not support an empty set");
        }
        T ans = 0;
        for(auto const& k : nums){
            //both zero are not allowed
            ans = gcd(ans,k);
        }
        return ans;
   }
    /*
        error
    */
   
   /// @brief system error throw with str and code
   /// @param str text
   /// @param err error code
    void throw_system_error(const string&str,int err = 0){
        if (err == 0) err = errno;
        throw std::system_error(
            std::error_code(err,std::system_category()),
            string(str).append(std::to_string(err))
        );
    }
    void handle_signal(int sig, sig_t handler){
        struct sigaction sigact;
        ::memset(&sigact, 0, sizeof(sigact));
        sigact.sa_handler = handler;
        if (sigaction(sig, &sigact, nullptr) == -1){
            throw_system_error("sigaction error");
        }
    }
    void ignore_signal(int sig){
        handle_signal(sig, SIG_IGN);
    }

    void reset_signal(int sig){
        handle_signal(sig, SIG_DFL);
    }

};

#endif