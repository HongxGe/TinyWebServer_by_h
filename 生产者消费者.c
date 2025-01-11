#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int count = 0;

pthread_mutex_t mutex;
pthread_cond_t cond_empty;
pthread_cond_t cond_full;

// 生产者线程函数
void* producer(void* arg) {
    for (int i = 0; i < 10; ++i) {
        pthread_mutex_lock(&mutex);
        
        // 如果缓冲区满，等待
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&cond_full, &mutex);
        }
        
        // 生产数据
        buffer[count++] = i;
        printf("Produced: %d\n", i);
        
        // 通知消费者
        pthread_cond_signal(&cond_empty);
        pthread_mutex_unlock(&mutex);
        
        sleep(1); // 模拟生产时间
    }
    return NULL;
}

// 消费者线程函数
void* consumer(void* arg) {
    for (int i = 0; i < 10; ++i) {
        pthread_mutex_lock(&mutex);
        
        // 如果缓冲区空，等待
        while (count == 0) {
            pthread_cond_wait(&cond_empty, &mutex);
        }
        
        // 消费数据
        int item = buffer[--count];
        printf("Consumed: %d\n", item);
        
        // 通知生产者
        pthread_cond_signal(&cond_full);
        pthread_mutex_unlock(&mutex);
        
        sleep(1); // 模拟消费时间
    }
    return NULL;
}

int main() {
    pthread_t prod, cons;

    // 初始化互斥锁和条件变量
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_empty, NULL);
    pthread_cond_init(&cond_full, NULL);

    // 创建生产者和消费者线程
    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    // 等待线程结束
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    // 清理资源
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_empty);
    pthread_cond_destroy(&cond_full);

    return 0;
}