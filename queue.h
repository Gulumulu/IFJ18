#ifndef IFJ18_QUEUE_H
#define IFJ18_QUEUE_H

#define MAX_QUEUE 50

#define QUEUE_SIZE 50

/*typedef struct { // struktura pro pole struktur ve fronte
    int name; // id nazev podle static counter
    char* type; // typ promenne je potreba nekde ukladat, proc ne tady
} tQItem;
*/

typedef struct {
    int arr[MAX_QUEUE]; // pole struktur ve fronte
    int f_index;
    int b_index;
} tQueue;

void queueInit(tQueue* q);
int nextIndex(int index);
int queueEmpty(const tQueue* q);
int queueFull(const tQueue* q);
void queueFront(const tQueue* q, int* c);
void queuePreFront(const tQueue* q, int* c);
void queueRemove(tQueue* q);
void queueGet(tQueue* q, int* c);
void queueUp(tQueue* q,int c);


#endif //IFJ18_QUEUE_H