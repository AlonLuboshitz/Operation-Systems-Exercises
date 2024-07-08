#include "Screen_manager.h"
#include <iostream>
#include <thread>

Screen_manager::Screen_manager(Bounded_buffer* shared_que,std::vector<int>* done_que)
{
    this->shared_que=shared_que;
    this->done_que=done_que;
}

Screen_manager::~Screen_manager()
{
}

void Screen_manager::run_screen(){
    while (this->done_que->size()<3) // 3 DONES
    {
        char* message = this->shared_que->remove();
        std::cout<< message << " "<< std::this_thread::get_id()<<std::endl;
    }
    std::cout<<"DONE"<<std::endl;
}