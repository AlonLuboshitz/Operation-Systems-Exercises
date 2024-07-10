#ifndef BOUDNED_BUFFER_H
#define BOUDNED_BUFFER_H
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <string>

class Bounded_buffer
{
private:
    pthread_mutex_t lock;
    sem_t empty;
    sem_t full;
    char** buffer;
    std::queue<std::string> buffer_strings;
    int size;
    int current_place = 0;
    void update_que();

public:
    Bounded_buffer(int size);
    Bounded_buffer(const Bounded_buffer& other);
    ~Bounded_buffer();
    int insert (std::string s);
    std::string remove();
    
};


#endif