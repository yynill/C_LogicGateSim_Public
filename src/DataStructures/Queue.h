#ifndef QUEUE_H
#define QUEUE_H
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct Node Node;

typedef struct QueueItem {
    Node *node;
    struct QueueItem *next;
} QueueItem;

typedef struct Queue {
    QueueItem *head;
    QueueItem *tail;
} Queue;

Queue *init_queue();
QueueItem *create_queue_item(Node *node);
void enqueue(Queue *q, Node *node);
Node *dequeue(Queue *q);
void print_queue(Queue *q);
void free_queue(Queue *q);
void queue_find_and_remove(Queue *q, Node *node);
int queue_size(Queue *q);
int queue_contains(Queue *q, Node *node);

#endif // QUEUE_H
