#ifndef UNBOUNDED_BUFFER_H
#define UNBOUNDED_BUFFER_H
#include <iostream>
#include <queue>
#include <string>
#include <mutex>
#include <semaphore.h>

class UnboundedBuffer {
private:
    std::queue<std::string> buffer;
    std::mutex lock;
    
    sem_t full;

public:
    UnboundedBuffer() {
        sem_init(&full, 0, 0);  // Initialize the semaphore to 0
    }

    ~UnboundedBuffer() {
        sem_destroy(&full);
    }

    void insert(const std::string& message); 

    std::string remove();
};
#endif