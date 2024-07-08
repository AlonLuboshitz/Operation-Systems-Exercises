#ifndef COEDITOR_H
#define COEDITOR_H
#include "Unbounded_buffer.h"
#include "Bounded_buffer.h"

class Coeditor
{
private:
    std::vector<int>* done_que;
    UnboundedBuffer* dispatcher_que;
    Bounded_buffer* screen_que;
public:
    Coeditor(UnboundedBuffer* UnboundedBuffer, Bounded_buffer* Screen_buffer,std::vector<int>* done_que);
    ~Coeditor();
    void read_dispatcher_que();
};



#endif