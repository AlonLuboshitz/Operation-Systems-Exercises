#include "Unbounded_buffer.h"
void UnboundedBuffer::insert(const std::string& message)
{
        // Lock the buffer for insertion
        std::unique_lock<std::mutex> locker(lock);
        buffer.push(message);

        // Unlock and signal that there is data available
        locker.unlock();
        sem_post(&full);
    }
std::string UnboundedBuffer::remove(){
     
        // Wait until there is data available
        sem_wait(&full);

        // Lock the buffer for removal
        std::unique_lock<std::mutex> locker(lock);

        // Get the first element from the buffer
        std::string temp = buffer.front();
        buffer.pop();
        locker.unlock();

        // Unlock the buffer and return the removed element
        return temp;
    
}