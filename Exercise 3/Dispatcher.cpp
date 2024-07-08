#include "Dispatcher.h"
#include <iostream>
#include <thread>
Dispatcher::Dispatcher(Bounded_buffer** buffers, int buffers_amount) {
    this->buffers = buffers;
    this->buffers_amount = buffers_amount;
    for (int i = 0; i < buffers_amount; i++) {
        this->active_buffers.push_back(i);
    }
}
Dispatcher::Dispatcher(const Dispatcher& other) {
    this->buffers = other.buffers;
    this->buffers_amount = other.buffers_amount;
}
void Dispatcher::read() {
    //Thread wait for message
    std::cout<<"Dispatcher Tid: "<<std::this_thread::get_id()<<std::endl;
    while (this->buffers_amount > 0) {
        for (int i =0;i<active_buffers.size();i++) {
            int buffer_index = active_buffers[i];
            char* message = wait_for_message(buffers[buffer_index]);
            std::string message_str(message);
            
            TYPE type = stringToType(message_str);
            add_to_que(message_str, type,i);
            buffers[i]->update_que();
            
        }
    }
    for (long unsigned int i = 0; i < news_que.size(); i++) {
        std::cout << news_que[i] << std::endl;
    }
    for (long unsigned int i = 0; i < sports_que.size(); i++) {
        std::cout << sports_que[i] << std::endl;
    }
    for (long unsigned int i = 0; i < weather_que.size(); i++) {
        std::cout << weather_que[i] << std::endl;
    }
    std::cout << "Dispatcher done" << std::endl;

}
void Dispatcher::add_to_que(std::string task, TYPE type, int buffer_index) {
    
    switch (type) {
        case SPORTS:
            this->sports_que.push_back(task);
            break;
        case NEWS:
            this->news_que.push_back(task);
            break;
        case WEATHER:
            this->weather_que.push_back(task);
            break;
        case DONE:
            this->active_buffers.erase(active_buffers.begin() + buffer_index);
            buffers_amount--; // Decrement the count since we removed one buffer
    }
}
char * Dispatcher::wait_for_message(Bounded_buffer* buffer) {
    while (true) {
        char* message = buffer->remove();
        if (message != nullptr) {
            return message;
        }
    }
}
