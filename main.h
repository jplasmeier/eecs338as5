#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define NUM_WRITERS 8 
#define NUM_READERS 12

struct reader_data {
	int thread_id;
	char* filename;
};

struct writer_data {
	int thread_id;
	char* filename;
	char* data_to_write;
};
