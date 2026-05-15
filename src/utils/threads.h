#pragma once

#include "filters.h"
#include "parser_args.h"
#include "queue.h"

#define READERS_NUMBER 3
#define WORKERS_NUMBER 3
#define WRITERS_NUMBER 3

typedef struct {
  queue_t *load_queue;
  queue_t *save_queue;
  filter_t *filter;
  args_t *args; // for mode, number of files and file list
} params_t;

void *reader_thread(void *arg);
void *worker_thread(void *arg);
void *writer_thread(void *arg);
