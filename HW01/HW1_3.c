#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define FILE_NAME "file3"
#define FILE_SIZE 200 // MB
#define SEQ_READ_SIZE 4   // KB
#define SEQ_WRITE_SIZE 2  // KB
#define RUNNING_TIMES 50000
#define RAN_READ_SIZE 3    // KB
#define RAN_WRITE_SIZE_1 4 // KB
#define RAN_WRITE_SIZE_2 3 // KB

int main() {
	printf("*****HW1_3*****\n");
	
    // Creating a 200 MB file
    void *fpmap = NULL;
    int fp = open(FILE_NAME, O_WRONLY | O_TRUNC | O_CREAT);
    if (fp == -1)
        return -1;

    size_t filesize_in_byte = FILE_SIZE << 20;
    printf("file_size is %ld bytes\n", filesize_in_byte);

    char write_buffer[1 << 20] = {0};
    memset(write_buffer, 0, 1 << 20);

    // write 1 MB in each iteration, 200 times in total
    for (int i = 0; i < FILE_SIZE << 10; i++)
        write(fp, write_buffer, 1 << 10);
    // get filesize
    struct stat st;
    stat(FILE_NAME, &st);
    long int filesize = st.st_size;

    close(fp);

    // 1. Sequential Read (4 KB)
    fp = open(FILE_NAME, O_RDONLY);
    fpmap = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fp, 0);
    if (fp == -1)
        return -2;

    struct timeval start;
    struct timeval end;
    unsigned long diff;

    int count = filesize_in_byte / (SEQ_READ_SIZE << 10);
    char seq_read_buffer[SEQ_READ_SIZE << 10];
    // start read
    gettimeofday(&start, NULL);
    // Sequential Read
    for (int i = 0; i < count; i++) {
        memcpy(seq_read_buffer, fpmap, SEQ_READ_SIZE << 10);
        fpmap += (SEQ_READ_SIZE << 10); // Sequential Read: +4KB
    }
    // record time
    gettimeofday(&end, NULL);
    diff = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    // show the outcome
    printf("Sequential Read(4 KB) = %lf(s)\n", (double)diff/1000000);
    fpmap -= (count) * (SEQ_READ_SIZE << 10);
    munmap(fpmap, filesize);
    close(fp);

    // 2. Sequential Write 2 KB
    fp = open(FILE_NAME, O_RDWR);
    fpmap = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fp, 0);
    if (!fp)
        return -3;

    count = filesize_in_byte / (SEQ_WRITE_SIZE << 10);
    char seq_write_buffer[SEQ_WRITE_SIZE << 10];
    memset(seq_write_buffer, 1, SEQ_WRITE_SIZE << 10);

    // start write
    gettimeofday(&start, NULL);
    // Sequential Write
    for (int i = 0; i < count; i++)
    {
        memcpy(fpmap, seq_write_buffer, SEQ_WRITE_SIZE << 10);
        fsync(fp);
        fpmap += (SEQ_WRITE_SIZE << 10); // Sequential Write: +2KB
    }
    // record time
    gettimeofday(&end, NULL);
    diff = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    // show the outcome
    printf("Sequential Write(2 KB) = %lf(s)\n", (double)diff/1000000);
    fpmap -= (count) * (SEQ_WRITE_SIZE << 10);
    munmap(fpmap, filesize);
    close(fp);

    // 3. Random Read 3 KB
    fp = open(FILE_NAME, O_RDONLY);
    fpmap = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fp, 0);
    if (!fp)
        return -4;
    int MaxRange = (filesize_in_byte >> 10) / (RAN_READ_SIZE + 1); // 4 KB
    srand(time(NULL));
    count = RUNNING_TIMES;
    char ran_read_buffer[RAN_READ_SIZE << 10];
    // start read
    gettimeofday(&start, NULL);
    // Random Read
    for (int i = 0; i < count; i++) {
        memcpy(ran_read_buffer, fpmap + 4 * ((rand() % MaxRange) << 10), RAN_READ_SIZE << 10);
    }
    // record time
    gettimeofday(&end, NULL);
    diff = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    // show the outcome
    printf("Random Read(3 KB) = %lf(s)\n", (double)diff/1000000);
    munmap(fpmap, filesize);
    close(fp);

    // Random Write 3 KB in "4 KB"
    fp = open(FILE_NAME, O_RDWR);
    fpmap = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fp, 0);
    if (!fp)
        return -5;

    char ran_write_buffer[RAN_WRITE_SIZE_1 << 10];
    memset(ran_write_buffer, 2, RAN_WRITE_SIZE_1 << 10);
    // start write
    gettimeofday(&start, NULL);
    
    // 4. Random write without fsync()
    for (int i = 0; i < count; i++) {
        // Choose 4KB-aligned offset
        memcpy(fpmap + 4 * ((rand() % MaxRange) << 10), ran_write_buffer, RAN_WRITE_SIZE_1 << 10);
    }
    // record time
    gettimeofday(&end, NULL);
    diff = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    // show the outcome
    printf("Random Write(3 KB to 4 KB) = %lf(s)\n", (double)diff/1000000);
    munmap(fpmap, filesize);
    close(fp);

    // Random Write 4 KB in "4 KB"
    fp = open(FILE_NAME, O_RDWR);
    fpmap = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fp, 0);
    if (!fp)
        return -5;

    char ran_write_buffer2[RAN_WRITE_SIZE_2 << 10];
    memset(ran_write_buffer2, 3, RAN_WRITE_SIZE_2 << 10);
    // start write
    gettimeofday(&start, NULL);
    
    // 5. Random write without fsync()
    for (int i = 0; i < count; i++) {
        // Choose 4KB-aligned offset
        memcpy(fpmap + 4 * ((rand() % MaxRange) << 10), ran_write_buffer2, RAN_WRITE_SIZE_2 << 10);
        fsync(fp);
    }
    // record time
    gettimeofday(&end, NULL);
    diff = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    // show the outcome
    printf("Random Write with fsync() = %lf(s)\n", (double)diff/1000000);
    munmap(fpmap, filesize);
    close(fp);
    
    return 0;
}

