#include <iostream>
#include <pthread.h>
#include <queue>
#include <unistd.h>

template <typename T>
class ProducerConsumer {
public:
    ProducerConsumer(size_t buffer_size):buffer_size(buffer_size) {
        pthread_mutex_init(&mutex, nullptr);
        pthread_cond_init(&cond_empty, nullptr);
        pthread_cond_init(&cond_full, nullptr);
    }
    ~ProducerConsumer() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond_empty);
        pthread_cond_destroy(&cond_full);
    }
    void produce(T item) {
        pthread_mutex_lock(&mutex);
        while(buffer.size() >= buffer_size) {
            std::cout << "buffer.size = " <<  buffer.size() << std::endl;
            pthread_cond_wait(&cond_full, &mutex);
        }
        buffer.push(item);
        std::cout << "Produce: "<< item << std::endl;

        pthread_cond_signal(&cond_empty);
        sleep(1);
        pthread_mutex_unlock(&mutex);
        sleep(1);

        

    }
    T consum() {
        std::cout << "consum " << std::endl;
        pthread_mutex_lock(&mutex);
        while(1 == buffer.empty()) {
            std::cout << "buffer.empty = " <<  buffer.empty() << std::endl;
            pthread_cond_wait(&cond_empty, &mutex);
        }

        T item = buffer.front();
        buffer.pop();

        std::cout << "Consume: " << item << std::endl;
        pthread_cond_signal(&cond_full);
        sleep(2);
        pthread_mutex_unlock(&mutex);
        sleep(4);
        
        return item;
    }
private:
    pthread_mutex_t mutex;
    pthread_cond_t cond_empty;
    pthread_cond_t cond_full;
    size_t buffer_size;
    std::queue<T> buffer;
};

void* produce(void *arg) {
    std::cout << "produce_pthread" << std::endl;
    ProducerConsumer<int>* pc = static_cast<ProducerConsumer<int>*>(arg);
    for(int it = 0; it < 10; ++it) {
        pc->produce(it);
    }
    return nullptr;
}

void* consume(void *arg) {
    std::cout << "consume_pthread" << std::endl;
    ProducerConsumer<int>* pc = static_cast<ProducerConsumer<int>*>(arg);
    for(int it = 0; it < 10; ++it) {
        pc->consum();
    }
}

int main(void) {
    pthread_t produce_pthread, consume_pthread;
    ProducerConsumer<int> pc(11);

    pthread_create(&produce_pthread, nullptr, &produce, &pc);
    pthread_create(&consume_pthread, nullptr, &consume, &pc);

    pthread_join(produce_pthread, nullptr);
    pthread_join(consume_pthread, nullptr);

}
