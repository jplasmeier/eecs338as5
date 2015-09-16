#include "main.h"
#define NUM_WRITERS 3
#define NUM_READERS 6

char* messages[NUM_WRITERS];

struct writer_data writer_data_array[NUM_WRITERS];
struct reader_data reader_data_array[NUM_READERS];

pthread_mutex_t mutex; 			/* create mutex */
pthread_cond_t reader_cv;		/* create readCount condition variable to block writers */

int readCount = 0;

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
	

	/* Enter Critical Section */
	pthread_mutex_lock(&mutex);
	readCount++; 
	pthread_mutex_unlock(&mutex);

	printf("Reader with thread id %d and readCount is %d. Going to read from file: %s\n", thread_id, readCount, filename); 
	sleep(1);
	pthread_mutex_lock(&mutex);
	readCount--;
	printf("readCount is now %d.\n", readCount);
	if (readCount == 0) { 
		printf("readCount = %d. Signalling Writers.\n", readCount);
		pthread_cond_broadcast(&reader_cv);
	}
	pthread_mutex_unlock(&mutex);

	/* Exit the reader */
	printf("Reader is going to sleep. readCount = %d. \n", readCount);	
	sleep(2);
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
	

	/* Entering Critical Section */
	printf("Thread %d is going to attempt to write.\n", thread_id);
	pthread_mutex_lock(&mutex);
	printf("Thread %d has cleared the mutex. The readCount is %d\n", thread_id, readCount);
	while(readCount > 0) {
		printf("Thread %d waiting condition reached. Waiting for cv signal.\n", thread_id);
		pthread_cond_wait(&reader_cv, &mutex);
		printf("Writer with thread id %d. Going to write: %s to file %s\n", thread_id, data_to_write, filename); 
	}
	pthread_mutex_unlock(&mutex);
		
	/* Exit the writer */	
	pthread_exit(NULL);
		
}

int main() {
	printf("Readers - Writers Program w/ POSIX Threading\n");

	pthread_attr_t attribute;		/* create attribute for joining threads */
	pthread_mutex_init(&mutex, NULL);	/* initialize mutex */
	pthread_cond_init(&reader_cv, NULL);	/* initialize reader condition variable */
	pthread_t writer_threads[NUM_WRITERS];  /* create array of writer threads */
	int* writer_ids[NUM_WRITERS]; 		/* create array of writer thread IDs */
	pthread_t reader_threads[NUM_READERS];  /* create array of reader threads */
	int* reader_ids[NUM_READERS]; 		/* create array of writer thread IDs */
	pthread_attr_init(&attribute);
	pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);

	int rc,t;			
	srand(time(NULL));
	
	/* Initialize messages to write */
	messages[0] = "Circumambulation of the Transcendental Pillars of Reality.";
	messages[1] = "Whereof one cannot speak, thereof one must remain silent.";
	messages[2] = "Justice for Tamir Rice";
	messages[3] = "Any formal set of axioms which proves its own consistency is inconsistent";
	messages[4] = "Go Eagles";

	/* Non-deterministic process creation */
	int readers = 0, writers = 0;
	while (readers < NUM_READERS || writers < NUM_WRITERS) {
		int choice = rand() % 2;
		if (choice == 0 && readers < NUM_READERS) {
			printf("MAIN: Creating thread: %ld\n", readers);
		
			/* Populate parameters */
			reader_data_array[readers].thread_id = readers;
			reader_data_array[readers].filename = "log.out";
		
			/* Create thread */
			rc = pthread_create(&reader_threads[readers], &attribute, readFile, (void *) &reader_data_array[readers]);
			
			/* Error handling for thread creation */		
			if (rc) {
				perror("Error creating thead\n");
				exit(-1);
			}	
			readers++;
		}
		else if (writers < NUM_WRITERS) {
			printf("MAIN: Creating thread: %ld\n", writers);
		
			/* Populate parameters */
			writer_data_array[writers].thread_id = writers;
			writer_data_array[writers].filename = "log.out";
			writer_data_array[writers].data_to_write = messages[writers];	
		
			/* Create thread */
			rc = pthread_create(&writer_threads[writers], &attribute, writeFile, (void *) &writer_data_array[writers]);
		
			/* Error handling for thread creation */		
			if (rc) {
				perror("Error creating thead\n");
				exit(-1);
			}	
			writers++;
		}	
	}		
	
	/* Wait for readers to finish */
	for (t = 0; t < NUM_READERS; t++) {
		pthread_join(reader_threads[t], NULL);
	}
	/* Wait for writers to finish */
	for (t = 0; t < NUM_WRITERS; t++) {
		pthread_join(writer_threads[t], NULL);
	}

	pthread_mutex_destroy(&mutex); /* clean up mutex */	
	pthread_cond_destroy(&reader_cv); /* clean up condition variable */
	pthread_exit(NULL); /* Block main while threads exit */
}
