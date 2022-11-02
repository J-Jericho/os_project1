#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

int main() {

	// Open/Create Shared Memory File
	int shared_table = shm_open("table", O_CREAT | O_RDWR, 0666);
	ftruncate(shared_table, sizeof(int));
	int* table = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shared_table, 0);

	// Create Buffers
    sem_t* fillBuffer      = sem_open("fillBuffer",      O_CREAT, 0666, 0);
	sem_t* emptyBuffer = sem_open("emptyBuffer", O_CREAT, 0666, 2);
	sem_t* taskBuffer     = sem_open("taskBuffer",     O_CREAT, 0666, 1);
	

	for (int i=0; i<10; ++i) {


		sem_wait(emptyBuffer);
		sleep(1);
		sem_wait(taskBuffer);


		++(*table);


		sem_post(taskBuffer);
		sem_post(fillBuffer);

        printf("Producing Item [#%d]\n", i);
	}

	printf("Producer has created 10 tasks.\n");
	
	// Close Semaphore Buffers
	sem_close(fillBuffer);
	sem_close(emptyBuffer);
	sem_close(taskBuffer);

	// Unlink Semaphore Buffers
	sem_unlink("fillBuffer");
	sem_unlink("emptyBuffer");
	sem_unlink("taskBuffer");
	munmap(table, sizeof(int));

	// Close Shared File
	close(shared_table);
	shm_unlink("table");

	// End Producer Program
	return 0;
}