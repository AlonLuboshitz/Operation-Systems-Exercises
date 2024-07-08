#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H
#include "Bounded_buffer.h"
#include <vector>
class Screen_manager
{
private:
    Bounded_buffer* shared_que;
    std::vector<int>* done_que; 
public:
    Screen_manager(Bounded_buffer* shared_que, std::vector<int>* done_que);
    ~Screen_manager();
    void run_screen();
};


#endif