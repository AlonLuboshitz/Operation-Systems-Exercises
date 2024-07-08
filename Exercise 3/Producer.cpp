#include "Producer.h"
#include "Types_.h"
#include <cstring>


Producer::Producer(int num_tasks, int id,int que_size,Bounded_buffer* buffer) {
    this->num_tasks = num_tasks;
    this->num_tasks_completed = 0;
    this->id = id;
    this->que_size = que_size;
    this->buffer = buffer;
    //this->create_tasks();
}
std::string Producer::produce() {
    TYPE task_type = static_cast<TYPE>(rand() % 3);
    std::string task = "Producer " + std::to_string(id) + " " + typeToString(task_type) + " " + std::to_string(task_counts[task_type]);
    num_tasks_completed++;
    task_counts[task_type]++;
    return task;
}

void Producer::create_tasks() {
    std::cout<<"Producer Tid: "<<std::this_thread::get_id()<<std::endl;
    while (this->num_tasks_completed != this->num_tasks) {
        std::string task = produce();
        //std::cout << task << std::endl;
        char* cstr = new char[task.length() + 1];
        std::strcpy(cstr, task.c_str());
        while (this->buffer->insert(cstr) == -1); // Keep trying to insert until successful
        delete[] cstr;  
    }
    char* done = new char[5]; // "DONE" + '\0'
    std::strcpy(done, "DONE");
    while (this->buffer->insert(done) == -1);
    delete[] done;
}




