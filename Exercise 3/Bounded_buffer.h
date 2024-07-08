#ifndef BOUDNED_BUFFER_H
#define BOUDNED_BUFFER_H
#include <pthread.h>
#include <semaphore.h>

class Bounded_buffer
{
private:
    pthread_mutex_t lock;
    sem_t empty;
    sem_t full;
    char** buffer;
    int size;
    int current_place = 0;
public:
    Bounded_buffer(int size);
    Bounded_buffer(const Bounded_buffer& other);
    ~Bounded_buffer();
    int insert (char * s);
    char * remove ();
    void update_que();
};


#endif