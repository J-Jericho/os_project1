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
	
	// Open Shared Memory File
	int shared_file  = shm_open("table", O_RDWR, 0666);
	int* table       = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shared_file, 0);

	// Create Buffers
	sem_t* fillBuffer      = sem_open("fillBuffer",      O_CREAT, 0666, 0);
	sem_t* emptyBuffer = sem_open("emptyBuffer", O_CREAT, 0666, 2);
	sem_t* taskBuffer     = sem_open("taskBuffer",     O_CREAT, 0666, 1);
	
	for (int i=0; i<10; ++i) {


		sem_wait(fillBuffer);
		sleep(3);
		sem_wait(taskBuffer);


		--(*table);

        
		sem_post(taskBuffer);
		printf("Consuming Item [#%d]\n", i);
		sem_post(emptyBuffer);
	}
	
	printf("Consumer has consumed all 10 tasks.\n");

	// Close Semaphore Buffers
	sem_close(fillBuffer);
	sem_close(emptyBuffer);
	sem_close(taskBuffer);

	// Unlink Semaphore Buffers
	sem_unlink("fillBuffer");
	sem_unlink("emptyBuffer");
	sem_unlink("taskBuffer");

	// Close Shared File
	munmap(table, sizeof(int));
	close(shared_file);
	shm_unlink("table");

	// End Consumer Program
	return 0;
}