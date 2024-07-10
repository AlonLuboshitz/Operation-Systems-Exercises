#ifndef DISPATCHER_H
#define DISPATCHER_H
#include "Bounded_buffer.h"
#include "Unbounded_buffer.h"
#include <vector>

#include <string>
#include "Types_.h"

class Dispatcher{
    private:
        Bounded_buffer** buffers;
        int buffers_amount;
        int done_messages = 0;
        std::vector<int> active_buffers;
        // std::vector<std::string> news_que;
        // std::vector<std::string> sports_que;
        // std::vector<std::string> weather_que;
        UnboundedBuffer* news_que;
        UnboundedBuffer* sports_que;
        UnboundedBuffer* weather_que;
        void push_done();
        void add_to_que(std::string task, TYPE type, int buffer_index);
        //char * wait_for_message(Bounded_buffer* buffer);
        std::string wait_for_message(Bounded_buffer* buffer);
        
    public:
        Dispatcher(Bounded_buffer** buffers, int buffers_amount,UnboundedBuffer** unbounded_buffers);
        Dispatcher(const Dispatcher& other);
        void read();
        

        
    
};


/*
*/
#endif