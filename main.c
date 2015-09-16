#include "main.h"
#define NUM_WRITERS 5
#define NUM_READERS 3

char* messages[NUM_WRITERS];

struct writer_data writer_data_array[NUM_WRITERS];
struct reader_data reader_data_array[NUM_READERS];

void *readFile (void *thread_arg) {
	/* Initialize local variables */
	int thread_id;
	char* filename;
	struct reader_data* data;
	sleep(1);

	/* Populate local variables with parameters */
	data = (struct reader_data*) thread_arg;
	thread_id = data->thread_id;
	filename = data->filename;
	
	printf("Reader Created with thread id %d. Going to read from file: %s\n", thread_id, filename); 

	/* Exit the reader */	
	pthread_exit(NULL);
}

void *writeFile(void *thread_arg) {
	/* Initialize local variables */
	int thread_id;
	char* filename;
	char* data_to_write;
	struct writer_data* data;
	sleep(1);

	/* Populate local variables with parameters */
	data = (struct writer_data*) thread_arg;
	thread_id = data->thread_id;
	filename = data->filename;
	data_to_write = data->data_to_write;
	
	printf("Writer Created with thread id %d. Going to write: %s to file %s\n", thread_id, data_to_write, filename); 

	/* Exit the writer */	
	pthread_exit(NULL);
		
}

int main() {
	printf("Readers - Writers Program w/ POSIX Threading\n");
	pthread_t writer_threads[NUM_WRITERS];
	int* writer_ids[NUM_WRITERS];
	pthread_t reader_threads[NUM_READERS];
	int* reader_ids[NUM_READERS];

	int rc,t;

	/* Initialize messages to write */
	messages[0] = "This is a message";
	messages[1] = "This is another message";
	messages[2] = "Justice for Tamir Rice";
	messages[3] = "Legalize Gay Marijuana";

	/* Create Writer Threads */
	for(t = 0; t < NUM_WRITERS; t++) {
		printf("MAIN: Creating thread: %ld\n", t);
		
		/* Populate parameters */
		writer_data_array[t].thread_id = t;
		writer_data_array[t].filename = "log.out";
		writer_data_array[t].data_to_write = messages[t];	
		
		/* Create thread */
		rc = pthread_create(&writer_threads[t], NULL, writeFile, (void *) &writer_data_array[t]);
		
		/* Error handling for thread creation */		
		if (rc) {
			perror("Error creating thead\n");
			exit(-1);
		}	
	}


	/* Create Reader Threads */
	for(t = 0; t < NUM_READERS; t++) {
		printf("MAIN: Creating thread: %ld\n", t);
		
		/* Populate parameters */
		reader_data_array[t].thread_id = t;
		reader_data_array[t].filename = "log.out";
		
		/* Create thread */
		rc = pthread_create(&reader_threads[t], NULL, readFile, (void *) &reader_data_array[t]);
		
		/* Error handling for thread creation */		
		if (rc) {
			perror("Error creating thead\n");
			exit(-1);
		}	
	}
	
	/* Block main while threads exit */	
	pthread_exit(NULL);
}
