#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/mman.h>
#include <string.h>

struct key_value
{
    unsigned int key;
    char *value;
};

struct key_value *input;
int numberOfEntries;
int threadTotal;
int threadCount = 0;

void combineAllMerge(int lowIndex, int middleIndex, int highIndex)
{
    struct key_value *leftIndex;
    int leftIndexSize = middleIndex - lowIndex + 1;
    leftIndex = malloc(sizeof(struct key_value *) * (leftIndexSize));

    struct key_value *rightIndex;
    int rightIndexSize = highIndex - middleIndex;
    rightIndex = malloc(sizeof(struct key_value *) * (rightIndexSize));

    for (int i = 0; i < leftIndexSize; i++)
    {
        leftIndex[i]= input[i];
     
    }
    for (int j = 0; j < rightIndexSize; j++)
    {
        rightIndex[j] = input[middleIndex + j + 1];
        
    }

    int indexer = lowIndex;
    int leftCounter = 0;
    int rightCounter = 0;

    while (leftCounter < leftIndexSize && rightCounter < rightIndexSize)
    {
        if (rightIndex[rightCounter].key >= leftIndex[leftCounter].key)
        {
            input[indexer] = leftIndex[leftCounter];
       
            leftCounter++;
        }
        else
        {
            input[indexer]= rightIndex[rightCounter];
           
            rightCounter++;
        }
        indexer++;
    }

    while (leftCounter < leftIndexSize)
    {
        input[indexer] = leftIndex[leftCounter];
        leftCounter++;
        indexer++;
    }

    while (rightCounter < rightIndexSize)
    {
        input[indexer] = rightIndex[rightCounter];
        rightCounter++;
        indexer++;
    }

    free(rightIndex);
    free(leftIndex);
}
void merge(int lowIndex, int highIndex)
{
    int middleIndex = lowIndex + (highIndex + lowIndex) / 2;
    if (highIndex > lowIndex)
    {
        merge(lowIndex, highIndex);
        merge(middleIndex + 1, highIndex);
        combineAllMerge(lowIndex, middleIndex, highIndex);
    }
}

void *merge_sort(void *args)
{
    int threadIndex = threadCount;
    threadCount++;
    int lowIndex = (threadIndex) * (numberOfEntries / threadTotal);
    int highIndex = (threadIndex + 1) * (numberOfEntries / threadTotal) - 1;
    int middleIndex = lowIndex + (highIndex - lowIndex) / 2;
    if (highIndex > lowIndex)
    {
        merge(lowIndex, middleIndex);
        merge(middleIndex + 1, highIndex);
        combineAllMerge(lowIndex, middleIndex, highIndex);
    }
}

int main(int argc, char *argv[])
{
    FILE *inputFile;
    inputFile = fopen(argv[1], "r");
    if(inputFile == NULL){
       fprintf(stderr, "An error has occurred\n");
        exit(0);
    }
    fseek(inputFile, 0L, SEEK_END);
    size_t sizeOfFile = ftell(inputFile);
    numberOfEntries = sizeOfFile / 100;
    if(sizeOfFile ==0){
        fprintf(stderr, "An error has occurred\n");
        exit(0);
    }
   
    input = malloc(sizeof(struct key_value *) * numberOfEntries);
    fseek(inputFile, 0L, SEEK_SET);
    char *address = mmap(NULL, sizeOfFile, PROT_READ, MAP_PRIVATE, inputFile, 0);
 
    for (int i = 0; i < sizeOfFile; i += 100)
    {
        char element[4];
        char nintySixByte[96];
     
        memcpy((void*) element, &address + i, 4);
   
        memcpy((void*) nintySixByte, &address + i + 4, 96);

        int temp = i/100;

        input[temp].key = *(int*)element;
        input[temp].value = strdup(nintySixByte);
      
    }
  /*   if(sizeOfFile ==100){
        FILE *outputFile;
    outputFile = fopen(argv[2], "w+");
    fwrite(input, sizeof input, 1, outputFile);
    exit(0);
    }*/

    threadTotal = get_nprocs();
  
    pthread_t threads[threadTotal];
    for (int i = 0; i < threadTotal; i++)
    {
        pthread_create(&threads[i], NULL, merge_sort, (void *)NULL);
    }
    for (int i = 0; i < threadTotal; i++)
    {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < threadTotal; i++)
    {
        combineAllMerge(0, ((i + 1) * (numberOfEntries / threadTotal)) / 2, (i + 1) * (numberOfEntries / threadTotal) - 1);
    }

    FILE *outputFile;
    outputFile = fopen(argv[2], "w+");
    fwrite(input, sizeof input, 1, outputFile);
    fclose(inputFile);
    fclose(outputFile);
    free(input);
}