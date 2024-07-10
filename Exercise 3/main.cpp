#include "Producer.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <pthread.h>
#include "Bounded_buffer.h"
#include "Producer.h"
#include "Dispatcher.h"
#include "Unbounded_buffer.h"
#include "Coeditor.h"
#include "Screen_manager.h"
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

            
        }else if (line.find("Co-Editor queue size") != std::string::npos) {
            int co_editor_queue_size;
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string size_str = line.substr(pos + 1);
                co_editor_queue_size = std::stoi(size_str);
                queue_sizes.push_back(co_editor_queue_size);
            }
        }
    }

    file.close();
    if (producer_ids.size() != num_tasks.size() || producer_ids.size() != (queue_sizes.size()-1)) {
        std::cerr << "Error: Number of producer IDs, number of tasks, and number of queue sizes do not match." << std::endl;
        producer_ids.clear();
        num_tasks.clear();
        queue_sizes.clear();
    }
}


int main(int argc, char* argv[]) {
    std::vector<int> producers_ids, num_tasks, que_sizes;
    Parse_args(argv[1],producers_ids,num_tasks,que_sizes); 
    long unsigned int num_producers = producers_ids.size();
    Bounded_buffer** buffers = new Bounded_buffer*[num_producers]; // producer-dispatcher bound buffers array
    Producer** producers = new Producer*[num_producers]; // producers array
    UnboundedBuffer** co_editor_un_bounded_buffers = new UnboundedBuffer*[3]; // 3 unbounded buffers for the co editors
    Coeditor** coeditors = new Coeditor*[3]; // 3 co-editors
    Bounded_buffer* shared_buffer = new Bounded_buffer(que_sizes[num_producers]); // bound buffer co-editors-screen
    std::vector<int>* done_que = new std::vector<int>();
    for (int i = 0; i < 3; i++) { // init unbounded buffers and co editors
        co_editor_un_bounded_buffers[i] = new UnboundedBuffer();
        coeditors[i] = new Coeditor(co_editor_un_bounded_buffers[i],shared_buffer,done_que);
    }
    
    Screen_manager* screen_manager = new Screen_manager(shared_buffer,done_que);
    Dispatcher dispatcher(buffers, num_producers, co_editor_un_bounded_buffers); // init dispatcher
    for (long unsigned int i = 0; i < num_producers; i++) { // init bounded buffers (pro-dis) and producers
        buffers[i] = new Bounded_buffer(que_sizes[i]);
        producers[i] = new Producer(num_tasks[i], producers_ids[i], que_sizes[i], buffers[i]);
    }
     std::thread screen_thread([&screen_manager]() {
        screen_manager->run_screen();
    });
    std::vector<std::thread> coeditors_threads;
    for (int i = 0; i < 3; i++) {
        coeditors_threads.emplace_back([coeditors,i]() {
            coeditors[i]->read_dispatcher_que();
        });
    }
    std::thread dispatcher_thread([&dispatcher]() {
        dispatcher.read();
    });
    
    std::vector<std::thread> producers_threads;
    for (size_t i = 0; i < num_producers; i++) {
        producers_threads.emplace_back([producers, i]() {
            producers[i]->create_tasks();
        });
    }   
    
   
    for (auto& thread : producers_threads) {
        thread.join();
    }
    dispatcher_thread.join();
    for (auto& thread : coeditors_threads) {
        thread.join();
    }
    
    screen_thread.join();
   
   
   
return 0;}
    