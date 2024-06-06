#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_FLOWERS 5
#define NUM_GARDENERS 2

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int flower_flags[NUM_FLOWERS] = {0};

void* gardener(void* arg) 
{
    int gardener_id = *((int*)arg);
    
    while (1) 
    {
        pthread_mutex_lock(&mutex);
        
        for (int i = 0; i < NUM_FLOWERS; i++) 
        {
            if (flower_flags[i]) 
            {
                printf("Gardener %d is watering flower %d\n", gardener_id + 1, i);
                flower_flags[i] = 0;
            }
        }
        
        pthread_mutex_unlock(&mutex);
    }
    
    return NULL;
}

void* flower(void* arg) 
{
    int flower_id = *((int*)arg);
    
    srand(time(NULL) + flower_id);
    
    while (1) 
    {
        int random_time = (rand() % 5000000) * 100 + 2000000;
        usleep(random_time);
        pthread_mutex_lock(&mutex);
        printf("Flower %d has wilted\n", flower_id);
        flower_flags[flower_id] = 1;
        pthread_mutex_unlock(&mutex);
    }
    
    return NULL;
}

int main() 
{
    pthread_t gardeners[NUM_GARDENERS];
    pthread_t flowers[NUM_FLOWERS];
    
    int i;
    int gardener_indices[NUM_GARDENERS];
    
    for (i = 0; i < NUM_GARDENERS; i++) 
    {
        gardener_indices[i] = i;
        pthread_create(&gardeners[i], NULL, gardener, &gardener_indices[i]);
    }
    
    for (i = 0; i < NUM_FLOWERS; i++) 
    {
        int* flower_id = (int*)malloc(sizeof(int));
        *flower_id = i;
        pthread_create(&flowers[i], NULL, flower, flower_id);
    }
    
    for (i = 0; i < NUM_GARDENERS; i++) 
    {
        pthread_join(gardeners[i], NULL);
    }
    for (i = 0; i < NUM_FLOWERS; i++) 
    {
        pthread_join(flowers[i], NULL);
    }
    
    return 0;
}
