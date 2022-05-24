#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FILE_NAME "file2"
#define FILE_SIZE 200<<20
#define SEQ_READ_SIZE 4<<10
#define SEQ_WRITE_SIZE 2<<10
#define RAN_READ_SIZE 3<<10
#define RAN_WRITE_SIZE 3<<10
#define READ_TIME 50000

int main() {
	printf("*****HW1_2*****\n");

	// Create a 200MB file
	int fp = open(FILE_NAME, O_WRONLY | O_TRUNC | O_CREAT);
	if(!fp) {
		printf("Create a 200MB file fail!\n");
		return -1;
	}
	
	long file_size = FILE_SIZE;
	printf("file_size is %ld bytes\n", file_size);
	char buffer[1<<20] = {0};
	for(int i = 0; i < 200; i++) {
		write(fp, buffer, 1<<20);
	}
	
	close(fp);
	
	// 1. Sequential read
	fp = open(FILE_NAME, O_RDONLY);
	if(fp == -1) {
		printf("Sequential read open file fail!\n");
		return -1;
	}
	// printf("fp=%d\n", fp);
	
	struct timeval start;
	struct timeval end;
	unsigned long diff;
	
	int count = (FILE_SIZE)/(SEQ_READ_SIZE);
	char seq_read_buffer[SEQ_READ_SIZE];
	
	gettimeofday(&start, NULL);
	
	for(int i = 0; i < count; i++) {
		int f = read(fp, seq_read_buffer, (SEQ_READ_SIZE));
		if(f == -1) {
			printf("Sequential read error!\n");
			return -1;
		}
	}
	
	gettimeofday(&end, NULL);
	
	diff = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	printf("Sequential read time = %lf(s)\n", (double)diff/1000000);
	
	close(fp);
	
	// 2. Sequential write
	fp = open(FILE_NAME, O_WRONLY);
	if(fp == -1) {
		printf("Sequential write fail!\n");
		return -1;
	}
	
	count = (FILE_SIZE)/(SEQ_WRITE_SIZE);
	char seq_write_buffer[SEQ_WRITE_SIZE] = {0};
	// memset(seq_write_buffer, 0, SEQ_WRITE_SIZE);
	
	gettimeofday(&start, NULL);
	
	for(int i = 0; i < count; i++) {
		int f = write(fp, seq_write_buffer, SEQ_WRITE_SIZE);
		if(f == -1) {
			printf("Sequential write error!\n");
			return -1;
		}
		fsync(fp);
	}
	
	gettimeofday(&end, NULL);
	
	diff = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	printf("Sequential write time = %lf(s)\n", (double)diff/1000000);
	
	close(fp);
	
	// 3. Random read
	fp = open(FILE_NAME, O_RDONLY);
	if(fp == -1) {
		printf("Random read fail!\n");
		return -1;
	}
	
	int max_range = (FILE_SIZE) / (RAN_READ_SIZE);
	srand(time(NULL));
	char ran_read_buffer[RAN_READ_SIZE];
	
	gettimeofday(&start, NULL);
	
	for(int i = 0; i < READ_TIME; i++) {
		lseek(fp, (rand()%max_range)<<10, SEEK_SET);
		read(fp, ran_read_buffer, RAN_READ_SIZE);
	}
	
	gettimeofday(&end, NULL);
	
	diff = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	printf("Random read time = %lf(s)\n", (double)diff/1000000);
	
	close(fp);
	
	// 4. Random write 2KB in 4KB
	fp = open(FILE_NAME, O_WRONLY);
	if(fp == -1) {
		printf("Random write 2KB in 4KB fail!\n");
		return -1;
	}
	
	char ran_write_buffer[RAN_WRITE_SIZE] = {0};
	// memset(seq_write_buffer, 0, SEQ_WRITE_SIZE);
	
	gettimeofday(&start, NULL);
	
	for(int i = 0; i < READ_TIME; i++) {
		lseek(fp, (rand()%max_range)<<10, SEEK_SET);
		write(fp, ran_write_buffer, RAN_WRITE_SIZE);
	}
	
	gettimeofday(&end, NULL);
	
	diff = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	printf("Random write 2KB in 4KB time = %lf(s)\n", (double)diff/1000000);
	
	close(fp);
	
	// 5. Random write 2KB in 4KB with fsync();
	fp = open(FILE_NAME, O_WRONLY);
	if(fp == -1) {
		printf("Random write 2KB in 4KB with fsync() fail!\n");
		return -1;
	}
	
	// memset(seq_write_buffer, 0, SEQ_WRITE_SIZE);
	
	gettimeofday(&start, NULL);
	
	for(int i = 0; i < READ_TIME; i++) {
		lseek(fp, (rand()%max_range)<<10, SEEK_SET);
		write(fp, ran_write_buffer, RAN_WRITE_SIZE);
		fsync(fp);
	}
	
	gettimeofday(&end, NULL);
	
	diff = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	printf("Random write 2KB in 4KB with fsync() time = %lf(s)\n", (double)diff/1000000);
	
	close(fp);
	
	return 0;
}

