#include "Coeditor.h"
#include <iostream>
#include <thread>      
#include <chrono>
#include <string>
#include <cstring>
Coeditor::Coeditor(UnboundedBuffer* UnboundedBuffer, Bounded_buffer* Screen_buffer, std::vector<int>* done_que)
{
    this->done_que = done_que;
    this->dispatcher_que = UnboundedBuffer;
    this->screen_que = Screen_buffer;
}
Coeditor::~Coeditor(){
    //
}
void Coeditor::read_dispatcher_que(){
    
    std::string s = this->dispatcher_que->remove();
    while (s != "DONE") {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        const char* c_str = s.c_str();
        char* modifiable_c_str = new char[s.length() + 1];
        std::strcpy(modifiable_c_str, c_str);
        this->screen_que->insert(modifiable_c_str);
        //std::cout<< s<<" in coeditors : "<<std::this_thread::get_id()<<std::endl;
        s = this->dispatcher_que->remove();
        delete[] modifiable_c_str;
    }
    this->done_que->push_back(1);
}
