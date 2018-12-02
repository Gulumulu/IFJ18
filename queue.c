//
// Created by root on 11/23/18.
//

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void queueInit ( tQueue* q ) {
    int i;
    for(i = 0; i < QUEUE_SIZE; i++)
        q->arr[i] = 0;
    q->f_index = 0;
    q->b_index = 0;
}

int nextIndex ( int index ) {
    return (index + 1) % QUEUE_SIZE;
}

void queueFront ( const tQueue* q, int* c ) {
    if(queueEmpty(q))
        return;
    *c = q->arr[q->f_index];
}

void queuePreFront ( const tQueue* q, int* c ) {
    if(queueEmpty(q))
        return;
    *c = q->arr[q->f_index+1];
}

void queueRemove ( tQueue* q ) { // BUG DOEDELAT
    if(queueEmpty(q))
        return;
    q->f_index = nextIndex(q->f_index);
}

void queueGet ( tQueue* q, int* c) {
    if(queueEmpty(q))
        return;
    queueFront(q, c);
    queueRemove(q);
}

void queueUp ( tQueue* q, int c ) {
    if(queueFull(q))
        return;
    q->arr[q->b_index] = c;
    q->b_index = nextIndex(q->b_index);
}

int queueEmpty ( const tQueue* q ) {
    return (q->f_index == q->b_index)? 1 : 0;
}

int queueFull ( const tQueue* q ) {
    return (nextIndex(q->b_index) == q->f_index)? 1: 0;
}

