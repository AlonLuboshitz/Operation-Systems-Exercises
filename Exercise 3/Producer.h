#ifndef PRODUCER_H
#define PRODUCER_H

#include <iostream>
#include <string>
#include <cstdlib>
#include "Bounded_buffer.h"
#include  <thread>
class Producer  {
    private:
        int num_tasks;
        int num_tasks_completed;
        int task_counts[3] = {0, 0, 0};
        int id;
        int que_size;
        std::string produce();
        Bounded_buffer* buffer;
    public:
        Producer(int num_tasks, int id, int que_size, Bounded_buffer* buffer);
        void create_tasks();

};


#endif