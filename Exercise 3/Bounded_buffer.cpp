#include "Bounded_buffer.h"
#include <cstring>
Bounded_buffer::Bounded_buffer(int size)
{
    pthread_mutex_init(&lock, NULL);
    sem_init(&empty, 0, size);
    sem_init(&full, 0, 0);  
    this->buffer = new char*[size];
    this->size = size;
}
Bounded_buffer::~Bounded_buffer()
{
    for (int i = 0; i < this->current_place; ++i) {
        delete[] buffer[i];
    }
    delete[] buffer;
    pthread_mutex_destroy(&lock);
    sem_destroy(&empty);
    sem_destroy(&full);
}
// Copy Constructor
Bounded_buffer::Bounded_buffer(const Bounded_buffer& other) : size(other.size) {
        buffer = new char*[size];
        for (int i = 0; i < size; ++i) {
            buffer[i] = other.buffer[i];  // Assuming a shallow copy of the individual char pointers is okay
        }
        current_place = other.current_place;
        pthread_mutex_init(&lock, NULL);
        sem_init(&empty, 0, size - current_place);
        sem_init(&full, 0, current_place);

    }
int Bounded_buffer::insert(char * s)
{   
    sem_wait(&empty);
    pthread_mutex_lock(&lock);
    // if (this->current_place == this->size) {
    //     pthread_mutex_unlock(&lock);
    //     sem_post(&full);
    //     return -1;
    // }
    // copy chr into buffer
    
    char* temp = new char[strlen(s) + 1];
    strcpy(temp, s);
    this->buffer[this->current_place] = temp;
    this->current_place++;
    pthread_mutex_unlock(&lock);
    sem_post(&full);
    return 0;
}
char * Bounded_buffer::remove()
{
    sem_wait(&full);
     pthread_mutex_lock(&lock);
    //    if (this->current_place == 0) {
    //     pthread_mutex_unlock(&lock);
    //     sem_post(&empty);
    //     return nullptr;
    // }
    char* temp = this->buffer[0]; // Get the first element
    
    
    pthread_mutex_unlock(&lock);
    sem_post(&empty);
    return temp;
}
void Bounded_buffer::update_que() {
    pthread_mutex_lock(&lock);
    for (int i = 0; i < this->current_place-1; i++) {
        strcpy(this->buffer[i], this->buffer[i + 1]);
    }
    this->current_place--;
    pthread_mutex_unlock(&lock);
}