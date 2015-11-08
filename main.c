/*********************************
*	Assignment 5
*	Concurrent Readers and Writers via POSIX Threads
*	EECS 338: Introduction to Operating Systems and Concurrent Programming
*	Instructor: Gultekin Ozsoyoglu, Case Western Reserve University
*	Author: Justin Plasmeier | jgp45@case.edu	
*
*********************************/

#include "main.h"

/*********************************
*	SHARED VARIABLE DECLARATION
*
*********************************/

/* Reader and Writer Data */
char* messages[NUM_WRITERS];				/* array to store messages to write */
struct writer_data writer_data_array[NUM_WRITERS];	/* array to store writer data */
struct reader_data reader_data_array[NUM_READERS];	/* array to store reader data */

/* Thread Semaphores and Condition Variables */
pthread_mutex_t mutex; 			/* create mutex */
pthread_cond_t reader_cv;		/* create readCount condition variable to block writers */

/* Program Control Variables */
int readCount = 0;			/* count to keep track of active readers */


/********************************
*	METHODS
*
********************************/

/* Reader Method */
void *readFile (void *thread_arg) {
	/* Initialize local variables */
	int thread_id;
	char* filename;
	struct reader_data* data;
	sleep(2);

	/* Populate local variables with parameters */
	data = (struct reader_data*) thread_arg;
	thread_id = data->thread_id;
	filename = data->filename;
	

	/* Enter Critical Section */
	pthread_mutex_lock(&mutex);
	readCount++; 			/* increment reader count */
	pthread_mutex_unlock(&mutex);
	
	/* Effectively Read from the file */	
	printf("READER [%d]: Now reading from file: %s. [readCount = %d]\n", thread_id, filename, readCount);
	/*Sleep to give duration to read operation */
	sleep(1);

	/* Reader finished reading*/
	printf("READER [%d]: Finished reading. [readCount = %d]\n", thread_id, readCount);
	pthread_mutex_lock(&mutex);
	readCount--;			/* decrement reader count */
	printf("READER [%d]: Exited Critical Section. [readCount = %d]\n", thread_id, readCount);
	
	/* Check if readers are done reading */
	if (readCount == 0) { 							/* if no readers are reading */
		printf("READER [%d]: No readers reading. Signalling Writers. [readCount = %d]\n", thread_id, readCount);
		pthread_cond_broadcast(&reader_cv);				/* signal the waiting writers */
	}
	pthread_mutex_unlock(&mutex);

	/* Exit the reader */
	printf("READER [%d]: Going to sleep then exiting. [readCount = %d]. \n", thread_id, readCount);	
	sleep(2);
	pthread_exit(NULL);
}

/* Writer Method */
void *writeFile(void *thread_arg) {
	/* Initialize local variables */
	int thread_id;
	char* filename;
	char* data_to_write;
	struct writer_data* data;
	sleep(2);

	/* Populate local variables with parameters */
	data = (struct writer_data*) thread_arg;
	thread_id = data->thread_id;
	filename = data->filename;
	data_to_write = data->data_to_write;
	

	/* Entering Critical Section */
	printf("WRITER [%d]: Attempting to write.\n", thread_id);
	pthread_mutex_lock(&mutex);
	printf("WRITER [%d]: Cleared the mutex. [readCount = %d]\n", thread_id, readCount);
	if(readCount > 0) {				/* If there are readers reading */
		printf("WRITER [%d]: Readers are reading, writer will wait. [readCount = %d].\n", thread_id, readCount);
		pthread_cond_wait(&reader_cv, &mutex);	/* Wait on the reader condition variable */
		printf("WRITER [%d]: Writing: \"%s\" to file: %s\n", thread_id, data_to_write, filename); 
		sleep(3);
		printf("WRITER [%d]: Finished writing.\n", thread_id);
	}
	else {						/* No readers reading, proceed with writing */
		printf("WRITER [%d]: No readers reading. Writing: \"%s\" to file: %s\n", thread_id, data_to_write, filename); 
		sleep(3);
	}
	pthread_mutex_unlock(&mutex);
		
	/* Exit the writer */	
	printf("WRITER [%d]: Writing is exiting. [readCount = %d]\n", thread_id, readCount);
	pthread_exit(NULL);
		
}

/* Main Method */
int main() {
	printf("Readers - Writers Program w/ POSIX Threading\n");

	/* VARIABLE DECLARATIONS */
	pthread_attr_t attribute;		/* create attribute for joining threads */
	pthread_mutex_init(&mutex, NULL);	/* initialize mutex */
	pthread_cond_init(&reader_cv, NULL);	/* initialize reader condition variable */
	pthread_t writer_threads[NUM_WRITERS];  /* create array of writer threads */
	int* writer_ids[NUM_WRITERS]; 		/* create array of writer thread IDs */
	pthread_t reader_threads[NUM_READERS];  /* create array of reader threads */
	int* reader_ids[NUM_READERS]; 		/* create array of writer thread IDs */
	pthread_attr_init(&attribute);		/* create join attrbiute */
	pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);

	int rc,t;				/* return code and thread index variables */
	srand(time(NULL));			/* seed random number generator */
	
	/* Initialize messages to write */
	messages[0] = "Circumambulation of the Transcendental Pillars of Reality.";
	messages[1] = "Whereof one cannot speak, thereof one must remain silent.";
	messages[2] = "Justice for Tamir Rice";
	messages[3] = "Any formal set of axioms which proves its own consistency is inconsistent";
	messages[4] = "Go Eagles";

	int readers = 0, writers = 0;		/* counts of readers and writers */

	
	/************************************
	*   Hack to force writer to go first when no readers are reading.*/

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
	
	/*************************************
	* Non-deterministic process creation */

	while (readers < NUM_READERS || writers < NUM_WRITERS) {
		int choice = rand() % 2;				/* returns 0 or 1 */
		if (choice  == 1 && writers < NUM_WRITERS) {
			printf("MAIN: Creating WRITER: [%d]\n", writers);
		
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
		else if (choice == 0 && readers < NUM_READERS) {
			printf("MAIN: Creating READER: [%d]\n", readers);
		
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
	}		
	
	/* Wait for readers to finish */
	for (t = 0; t < NUM_READERS; t++) {
		pthread_join(reader_threads[t], NULL);
	}
	/* Wait for writers to finish */
	for (t = 0; t < NUM_WRITERS; t++) {
		pthread_join(writer_threads[t], NULL);
	}

	/* Exit Section of Main Program */
	printf("MAIN: Execution Finished.\n");
	pthread_mutex_destroy(&mutex); /* clean up mutex */	
	pthread_cond_destroy(&reader_cv); /* clean up condition variable */
	pthread_exit(NULL); /* Block main while threads exit */
}
