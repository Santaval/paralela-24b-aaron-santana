// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4

#include <assert.h>
#include <stdlib.h>

#include "queue.h"   // Header file with declarations for queue operations

// Forward declaration of the function to remove the first node from the queue
void queue_remove_first(queue_t* queue);

// Initialize the queue
int queue_init(queue_t* queue) {
  assert(queue);  // Ensure the queue pointer is not NULL
  queue->head = NULL;  // Set the head of the queue to NULL
  queue->tail = NULL;  // Set the tail of the queue to NULL
  return EXIT_SUCCESS; // Return success status
}

// Destroy the queue and free all resources
int queue_destroy(queue_t* queue) {
  queue_clear(queue);  // Clear all nodes in the queue
  return EXIT_SUCCESS; // Return success status
}

// Check if the queue is empty
bool queue_is_empty(queue_t* queue) {
  assert(queue);  // Ensure the queue pointer is not NULL
  return queue->head == NULL;  // Return true if head is NULL, indicating the queue is empty
}

// Enqueue (add) an element to the end of the queue
int queue_enqueue(queue_t* queue, const size_t data) {
  assert(queue);  // Ensure the queue pointer is not NULL
  int error = EXIT_SUCCESS;

  // Allocate memory for a new node
  queue_node_t* new_node = (queue_node_t*) calloc(1, sizeof(queue_node_t));
  if (new_node) {
    new_node->data = data;  // Set the data for the new node

    // If the queue has nodes, append the new node to the end
    if (queue->tail) {
      queue->tail = queue->tail->next = new_node;
    } else {
      // If the queue is empty, set both head and tail to the new node
      queue->head = queue->tail = new_node;
    }
  } else {
    error = EXIT_FAILURE;  // Allocation failed
  }

  return error;  // Return status
}

// Dequeue (remove) an element from the front of the queue
int queue_dequeue(queue_t* queue, size_t* data) {
  assert(queue);  // Ensure the queue pointer is not NULL
  int error = 0;

  // Check if the queue is not empty
  if (!queue_is_empty(queue)) {
    if (data) {
      *data = queue->head->data;  // Retrieve the data from the head node
    }
    queue_remove_first(queue);  // Remove the head node
  } else {
    error = EXIT_FAILURE;  // Queue is empty
  }

  return error;  // Return status
}

// Remove the first node from the queue
void queue_remove_first(queue_t* queue) {
  assert(queue);  // Ensure the queue pointer is not NULL
  assert(!queue_is_empty(queue));  // Ensure the queue is not empty
  queue_node_t* node = queue->head;  // Get the head node
  queue->head = queue->head->next;  // Move hea
