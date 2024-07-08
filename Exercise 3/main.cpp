#include "Producer.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <pthread.h>
#include "Bounded_buffer.h"
#include "Producer.h"
#include "Dispatcher.h"
/*Function parse the config file into vectors of:
Ids, Number of tasks, Que sizes*/
void Parse_args(const std::string &filename, std::vector<int>& producer_ids,std::vector<int>& num_tasks, std::vector<int>& queue_sizes) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return;
    }
    producer_ids.clear();
    num_tasks.clear();
    queue_sizes.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("PRODUCER") != std::string::npos) {
            int id, tasks, queue_size;

            // Extract the producer ID
            std::istringstream iss_id(line);
            std::string producer_label;
            iss_id >> producer_label >> id;
            producer_ids.push_back(id);
            // Extract the number of tasks
            if (std::getline(file, line)) {
                std::istringstream iss(line);
                iss >> tasks;
            }
            num_tasks.push_back(tasks);
             // Extract the queue size
            if (std::getline(file, line)) {
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string size_str = line.substr(pos + 1);
                    queue_size = std::stoi(size_str);
                }
            }
            queue_sizes.push_back(queue_size);

            
        }
    }

    file.close();
    if (producer_ids.size() != num_tasks.size() || producer_ids.size() != queue_sizes.size()) {
        std::cerr << "Error: Number of producer IDs, number of tasks, and number of queue sizes do not match." << std::endl;
        producer_ids.clear();
        num_tasks.clear();
        queue_sizes.clear();
    }
}
/*Function creates |Producers| + 1 (co-editors) Mutexes.
Returns via vector of mutexes. */
void create_mutexes(int num_mutexes,std::vector<pthread_mutex_t>& mutexes) {
    for ( int i = 0; i < num_mutexes; i++) {
        pthread_mutex_init(&mutexes[i], NULL);
    }
}

void create_buffers(std::vector<int> queue_sizes,std::vector<Bounded_buffer>& buffers) {
    for (long unsigned int i = 0; i < queue_sizes.size(); i++) {
        buffers.push_back(Bounded_buffer(queue_sizes[i]));
    }
}

void create_producers(std::vector<int> num_tasks, std::vector<int> producer_ids, std::vector<int> queue_sizes, std::vector<Bounded_buffer>& buffers,std::vector<Producer>& producers) {
    for (long unsigned int i = 0; i < num_tasks.size(); i++) {
        producers.push_back(Producer(num_tasks[i], producer_ids[i], queue_sizes[i], &buffers[i]));
    }
    
}
int main(int argc, char* argv[]) {
    std::vector<int> producers_ids, num_tasks, que_sizes;
    Parse_args(argv[1],producers_ids,num_tasks,que_sizes); // Need to parse the co editor args
    long unsigned int num_producers = producers_ids.size();
    Bounded_buffer** buffers = new Bounded_buffer*[num_producers];
    Producer** producers = new Producer*[num_producers];    
    for (long unsigned int i = 0; i < num_producers; i++) {
        buffers[i] = new Bounded_buffer(que_sizes[i]);
        producers[i] = new Producer(num_tasks[i], producers_ids[i], que_sizes[i], buffers[i]);
    }
    std::vector<std::thread> threads;
    for (size_t i = 0; i < num_producers; i++) {
        threads.emplace_back([producers, i]() {
            producers[i]->create_tasks();
        });
    }   
    Dispatcher dispatcher(buffers, num_producers); 
    std::thread dispatcher_thread([&dispatcher]() {
        dispatcher.read();
    });
    for (auto& thread : threads) {
        thread.join();
    }
    dispatcher_thread.join();

   
   
return 0;}
    