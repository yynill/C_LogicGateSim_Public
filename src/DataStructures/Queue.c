#include "Queue.h"
#include "../node.h"

Queue *init_queue() {
    Queue *queue = malloc(sizeof(Queue));
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

QueueItem *create_queue_item(Node *node) {
    QueueItem *item = malloc(sizeof(QueueItem));
    item->node = node;
    item->next = NULL;
    return item;
}

void enqueue(Queue *q, Node *node) {
    QueueItem *item = create_queue_item(node);
    assert(item != NULL);
    if (q->tail != NULL) {
        q->tail->next = item;
    }
    q->tail = item;

    if (q->head == NULL) {
        q->head = item;
    }

    return;
}

Node *dequeue(Queue *q) {
    assert(q != NULL);
    if (q->head == NULL) {
        return NULL;
    }

    QueueItem *item_to_remove = q->head;
    Node *result = item_to_remove->node;
    q->head = q->head->next;

    if (q->head == NULL) {
        q->tail = NULL;
    }

    free(item_to_remove);
    return result;
}

void print_queue(Queue *q) {
    assert(q != NULL);
    QueueItem *item = q->head;
    while (item != NULL) {
        Node *node = item->node;
        const char *name = (node && node->name) ? node->name : "(null)";
        printf("🔵node %p name: %s\n", (void *)node, name);
        item = item->next;
    }
}

void free_queue(Queue *q) {
    assert(q != NULL);
    QueueItem *item = q->head;
    while (item != NULL) {
        free(item);
        item = item->next;
    }
}

void queue_find_and_remove(Queue *q, Node *node) {
    assert(q != NULL);

    QueueItem *prev = NULL;
    QueueItem *curr = q->head;

    while (curr != NULL) {
        if (curr->node == node) {
            QueueItem *to_delete = curr;

            if (prev == NULL) {
                q->head = curr->next;
                if (curr == q->tail) {
                    q->tail = NULL;
                }
                curr = curr->next;
            } else {
                prev->next = curr->next;
                if (curr == q->tail) {
                    q->tail = prev;
                }
                curr = curr->next;
            }

            free(to_delete);
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

int queue_size(Queue *q) {
    assert(q != NULL);
    
    int count = 0;
    QueueItem *item = q->head;
    while (item != NULL) {
        count++;
        item = item->next;
    }
    return count;
}

int queue_contains(Queue *q, Node *node) {
    assert(q != NULL);
    
    QueueItem *item = q->head;
    while (item != NULL) {
        if (item->node == node) {
            return 1;
        }
        item = item->next;
    }
    return 0;
}
