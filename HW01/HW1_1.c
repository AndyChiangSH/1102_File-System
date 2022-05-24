#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<string.h>
#include<sys/time.h>

#define FILE_NAME "file1"
#define FILE_SIZE 200<<20
#define SEQ_READ_SIZE 4<<10
#define SEQ_WRITE_SIZE 2<<10
#define RAN_READ_SIZE 4<<10
#define RAN_WRITE_SIZE 2<<10
#define READ_TIME 50000

int main() {
	printf("*****HW1_1*****\n");

	// Create a 200MB file
	FILE* fp = fopen(FILE_NAME, "w");
	if(!fp) {
		printf("Create a 200MB file fail!\n");
		return -1;
	}
	
	long file_size = FILE_SIZE;
	printf("file_size is %ld bytes\n", file_size);
	char buffer[1<<20] = {0};
	for(int i = 0; i < 200; i++) {
		fwrite(buffer, 1<<20, 1, fp);
	}
	
	fclose(fp);
	
	// 1. Sequential read
	fp = fopen(FILE_NAME, "r");
	if(!fp) {
		printf("Sequential read fail!\n");
		return -1;
	}
	
	struct timeval start;
	struct timeval end;
	unsigned long diff;
	int count = (FILE_SIZE)/(SEQ_READ_SIZE);
	char seq_read_buffer[SEQ_READ_SIZE];
	
	gettimeofday(&start, NULL);
	
	for(int i = 0; i < count; i++) {
		int f = fread(seq_read_buffer, 1, SEQ_READ_SIZE, fp);
		if(f == -1) {
			printf("Sequential read error!\n");
			return -1;
		}
	}
	
	gettimeofday(&end, NULL);
	
	diff = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	printf("Sequential read time = %lf(s)\n", (double)diff/1000000);
	
	fclose(fp);
	
	// 2. Sequential write
	fp = fopen(FILE_NAME, "w+");
	if(!fp) {
		printf("Sequential write fail!\n");
		return -1;
	}
	
	count = (FILE_SIZE)/(SEQ_WRITE_SIZE);
	char seq_write_buffer[SEQ_WRITE_SIZE] = {0};
	// memset(seq_write_buffer, 0, SEQ_WRITE_SIZE);
	
	gettimeofday(&start, NULL);
	
	for(int i = 0; i < count; i++) {
		int f = fwrite(seq_write_buffer, 1, SEQ_WRITE_SIZE, fp);
		if(f == -1) {
			printf("Sequential write error!\n");
			return -1;
		}
		fsync(fileno(fp));
	}
	
	gettimeofday(&end, NULL);
	
	diff = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	printf("Sequential write time = %lf(s)\n", (double)diff/1000000);
	
	fclose(fp);
	
	// 3. Random read
	fp = fopen(FILE_NAME, "r");
	if(!fp) {
		printf("Random read fail!\n");
		return -1;
	}
	
	int max_range = (FILE_SIZE) / (RAN_READ_SIZE);
	srand(time(NULL));
	char ran_read_buffer[RAN_READ_SIZE];
	
	gettimeofday(&start, NULL);
	
	for(int i = 0; i < READ_TIME; i++) {
		fseek(fp, (rand()%max_range)<<10, SEEK_SET);
		int f = fread(ran_read_buffer, 1, RAN_READ_SIZE, fp);
		if(f == -1) {
			printf("Random read error!\n");
			return -1;
		}
	}
	
	gettimeofday(&end, NULL);
	
	diff = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	printf("Random read time = %lf(s)\n", (double)diff/1000000);
	
	fclose(fp);
	
	// 4. Random write 2KB in 4KB
	fp = fopen(FILE_NAME, "w");
	if(!fp) {
		printf("Random write 2KB in 4KB fail!\n");
		return -1;
	}
	
	char ran_write_buffer[RAN_WRITE_SIZE] = {0};
	// memset(seq_write_buffer, 0, SEQ_WRITE_SIZE);
	
	gettimeofday(&start, NULL);
	
	for(int i = 0; i < READ_TIME; i++) {
		fseek(fp, (rand()%max_range)<<10, SEEK_SET);
		int f = fwrite(ran_write_buffer, 1, RAN_WRITE_SIZE, fp);
		if(f == -1) {
			printf("Random write 2KB in 4KB error!\n");
			return -1;
		}
	}
	
	gettimeofday(&end, NULL);
	
	diff = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	printf("Random write 2KB in 4KB time = %lf(s)\n", (double)diff/1000000);
	
	fclose(fp);
	
	// 5. Random write 2KB in 4KB with fsync()
	fp = fopen(FILE_NAME, "w");
	if(!fp) {
		printf("Random write 2KB in 4KB with fsync() fail!\n");
		return -1;
	}
	
	// memset(seq_write_buffer, 0, SEQ_WRITE_SIZE);
	
	gettimeofday(&start, NULL);
	
	for(int i = 0; i < READ_TIME; i++) {
		fseek(fp, (rand()%max_range)<<10, SEEK_SET);
		int f = fwrite(ran_write_buffer, 1, RAN_WRITE_SIZE, fp);
		if(f == -1) {
			printf("Random write 2KB in 4KB with fsync() error!\n");
			return -1;
		}
		fsync(fileno(fp));
	}
	
	gettimeofday(&end, NULL);
	
	diff = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	printf("Random write 2KB in 4KB with fsync() time = %lf(s)\n", (double)diff/1000000);
	
	fclose(fp);

	return 0;
}
