#ifndef MaoLog
#define MaoLog
#include<iostream>
#include<thread>
#include<cassert>
#include<sstream>
#include<ctime>
#include<condition_variable>
#include<chrono>
#include<string>
#include<mutex>
#include<cstring>
#include<memory>
#include<iomanip>
#include<unistd.h>
class MaoLoger{
    using string = std::string;
private:
    int m_level;
    static const int m_buffer = 5 * 1024 * 1024;
    std::shared_ptr<char[]> m_buffer_ptr;
    std::shared_ptr<std::thread>m_thread_ptr;
    string m_buffer_string;
    std::condition_variable_any _the_cv;
    std::recursive_mutex _m;
    bool _flush = true;
    bool _stop = false;

public:
    bool _enable_time = 1,_enable_id = 1;
    size_t _recur_level = 0;
    MaoLoger(){
        MaoLoger(1);
    }
    MaoLoger(int level):m_level(level){
        // assert(m_level == 1 || m_level == 2);
        if(m_level != 1 && m_level != 2) return ;
        m_buffer_string = "";
        m_thread_ptr = std::make_shared<std::thread>(&MaoLoger::run,this);
    }
    MaoLoger &operator<<(const string &str){
        _m.lock();
        _recur_level ++;
        
        if(this->_flush) _write_debug();
        this->_flush = false;
        m_buffer_string += str;
      
        return *this;
    }
    MaoLoger &operator<<(const long long num) {
        return (*this << std::to_string(num));

    }
    MaoLoger &operator<<(const char* str){
        string tmp = str;
        return *this << tmp;
    }
    MaoLoger &operator <<(const MaoLoger& ){
        if(_recur_level == 0){
            _the_cv.notify_all();
            return *this;
        }
        m_buffer_string += "\n";
        for(int i = 0 ;  i< _recur_level -1 ; i ++){
            _m.unlock();
        }
        _recur_level = 0;
        _flush = true;
        _m.unlock();
        _the_cv.notify_all();
        return *this;
    }
private:
    void write_head(){
        assert(m_level == 1 || m_level == 2);
        if(m_level == 1) m_buffer_string += "[info]->";
        else m_buffer_string += "[error]->";
    }
    void write_id(){
        std::stringstream st;
        st << std::this_thread::get_id();
        m_buffer_string += "[id = " + st.str() +"]->";
    }
    void write_time(){
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&t);
        std::stringstream ss;
        ss << std::put_time(&tm,"%Y-%m-%d %H:%M:%S");
        m_buffer_string += "[time = " + ss.str() + "]->";
    }
    void _write_debug(){
        write_head();
        if(_enable_time)write_time();
        if(_enable_id)write_id();
    }
    void run(){
        while(true){
            std::unique_lock<std::recursive_mutex>log(this->_m);

            _the_cv.wait(log,
            [this](){
                return this->_stop || m_buffer_string.size() >0;
            });

            ::write(this->m_level,
            m_buffer_string.c_str(),
            m_buffer_string.size());
            
            if(this->_stop)break;
            m_buffer_string.clear();
            

        }
    }
public:
    ~MaoLoger() {
        if (m_level != 1 && m_level != 2) return;

            {
                std::unique_lock<std::recursive_mutex> lg(_m);
                *this<<*this;
                _stop = true;
                _the_cv.notify_one();
            }
            

        m_thread_ptr->join();
    }
};


#endif
