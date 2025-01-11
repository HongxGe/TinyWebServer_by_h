#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <queue>

#define BUFFER_SIZE 5

class ProducerConsumer {
public:
    ProducerConsumer() {
        pthread_mutex_init(&mutex, nullptr);
        pthread_cond_init(&cond_empty, nullptr);
        pthread_cond_init(&cond_full, nullptr);
    }

    ~ProducerConsumer() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond_empty);
        pthread_cond_destroy(&cond_full);
    }

    void produce(int item) {
        pthread_mutex_lock(&mutex);
        while (buffer.size() == BUFFER_SIZE) {
            pthread_cond_wait(&cond_full, &mutex);
        }
        buffer.push(item);
        std::cout << "Produced: " << item << std::endl;
        pthread_cond_signal(&cond_empty);
        pthread_mutex_unlock(&mutex);
    }

    int consume() {
        pthread_mutex_lock(&mutex);
        while (buffer.empty()) {
            pthread_cond_wait(&cond_empty, &mutex);
        }
        int item = buffer.front();
        buffer.pop();
        std::cout << "Consumed: " << item << std::endl;
        pthread_cond_signal(&cond_full);
        pthread_mutex_unlock(&mutex);
        return item;
    }

private:
    std::queue<int> buffer;
    pthread_mutex_t mutex;
    pthread_cond_t cond_empty;
    pthread_cond_t cond_full;
};

void* producer(void* arg) {
    ProducerConsumer* pc = static_cast<ProducerConsumer*>(arg);
    for (int i = 0; i < 10; ++i) {
        pc->produce(i);
        sleep(1); // 模拟生产时间
    }
    return nullptr;
}

void* consumer(void* arg) {
    ProducerConsumer* pc = static_cast<ProducerConsumer*>(arg);
    for (int i = 0; i < 10; ++i) {
        pc->consume();
        sleep(2); // 模拟消费时间
    }
    return nullptr;
}

int main() {
    pthread_t prod_thread, cons_thread;
    ProducerConsumer pc;

    // 创建生产者和消费者线程
    pthread_create(&prod_thread, nullptr, producer, &pc);
    pthread_create(&cons_thread, nullptr, consumer, &pc);

    // 等待线程结束
    pthread_join(prod_thread, nullptr);
    pthread_join(cons_thread, nullptr);

    return 0;
}