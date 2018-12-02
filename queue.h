#ifndef IFJ18_QUEUE_H
#define IFJ18_QUEUE_H

#define MAX_QUEUE 50

#define QUEUE_SIZE 50

typedef struct {
    int arr[MAX_QUEUE]; // pole struktur ve fronte
    int f_index;
    int b_index;
} tQueue;

void queueInit(tQueue* q);
void queueRemove(tQueue* q);
void queueGet(tQueue* q, int* c);
void queueUp(tQueue* q,int c);
void queueFront(const tQueue* q, int* c);
void queuePreFront(const tQueue* q, int* c);
int queueEmpty(const tQueue* q);
int queueFull(const tQueue* q);
int nextIndex(int index);


#endif //IFJ18_QUEUE_H