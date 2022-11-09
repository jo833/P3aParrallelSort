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

pthread_mutex_t lock;
struct key_value *input;
int numberOfEntries;
int threadTotal;
int threadCount = 0;


void combineAllMerge(int lowIndex, int middleIndex, int highIndex)
{
    
    // printf("Merging from inclusive i=%d to j=%d. with middle index: %d\n", lowIndex, highIndex, middleIndex);

    struct key_value *leftIndex;
    int leftIndexSize = middleIndex - lowIndex + 1;
   // printf()
    leftIndex = malloc(sizeof(struct key_value) * (leftIndexSize));

    struct key_value *rightIndex;
    int rightIndexSize = highIndex - middleIndex;
    rightIndex = malloc(sizeof(struct key_value) * (rightIndexSize));

    for (int i = 0; i < leftIndexSize; i++)
    {
        leftIndex[i]= input[lowIndex + i];
     
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
    int middleIndex = (highIndex + lowIndex) / 2;
    if (highIndex > lowIndex)
    {
        merge(lowIndex, middleIndex);
        // printf("%s\n", "i hate os2");
        merge(middleIndex + 1, highIndex);
        // printf("%s\n", "i hate os1");
        combineAllMerge(lowIndex, middleIndex, highIndex);
    }
}

void *merge_sort(void *args)
{

    // This part of your code has a bug!
    // THere's some bug even beyond this.

    // race condition: 
    pthread_mutex_lock(&lock);
    int threadIndex = threadCount;
    threadCount++;
    pthread_mutex_unlock(&lock);
    
    int lowIndex = (threadIndex) * (numberOfEntries / threadTotal);
    int highIndex = (threadIndex + 1) * (numberOfEntries / threadTotal) - 1;
    if (threadIndex == threadTotal - 1)
        highIndex = numberOfEntries - 1;
   // printf("%d\n", lowIndex);
   // printf("%d\n", middleIndex);
    //printf("%d\n", highIndex);

    // printf("part: %d\n", threadIndex);
        
    merge(lowIndex, highIndex);
}

int main(int argc, char *argv[])
{
    FILE *inputFile;
    inputFile = fopen(argv[1], "r");
    if(inputFile == NULL){
       fprintf(stderr, "An error has occurred\n");
        exit(0);
    }
    pthread_mutex_init(&lock, NULL);
    fseek(inputFile, 0L, SEEK_END);
    size_t sizeOfFile = ftell(inputFile);
    numberOfEntries = sizeOfFile / 100;
    if(sizeOfFile ==0){
        fprintf(stderr, "An error has occurred\n");
        exit(0);
    }
    input = malloc(sizeof(struct key_value) * numberOfEntries);
    fseek(inputFile, 0L, SEEK_SET);
    char *address = mmap(NULL, sizeOfFile, PROT_READ, MAP_PRIVATE, fileno(inputFile), 0);

    if (address == NULL){
        fprintf(stderr, "An error has occurred\n");
        exit(0);    
    }
 
    for (int i = 0; i < sizeOfFile; i += 100)
    {
      // printf("%d\n", i);
     
        char element[4];
        char nintySixByte[96];
        memcpy((void*) element, address + i, 4);
  
        memcpy((void*) nintySixByte, address + i + 4, 96);

        int temp = i/100;

        input[temp].key = *(unsigned int*)element;
        input[temp].value = malloc(96);
         
        memcpy(input[temp].value, nintySixByte, 96);
    
    }
     
  
    threadTotal = get_nprocs();
     if (threadTotal> numberOfEntries){
        threadTotal = numberOfEntries;
     }

  
    pthread_t threads[threadTotal];
    for (int i = 0; i < threadTotal; i++)
    { 
        pthread_create(&threads[i], NULL, merge_sort, (void *)NULL);
    }
    for (int i = 0; i < threadTotal; i++)
    {
       
        pthread_join(threads[i], NULL);
    }

    // for (int i = 0; i < numberOfEntries; i++){
    //     printf("key: %d\n", input[i].key);
    // }

    for (int i = 0; i < threadTotal; i++)
    {
        int partitionSize = numberOfEntries / threadTotal;
        int startIndex = 0;        
        int splitIndex = (i+1) * partitionSize - 1;
        int endIndexInclusive = (i+2) * partitionSize - 1;
        if (endIndexInclusive >= numberOfEntries) {
            endIndexInclusive = numberOfEntries - 1;
        }
        combineAllMerge(startIndex, splitIndex, endIndexInclusive);
        // combineAllMerge(0, ((i + 1) * (numberOfEntries / threadTotal)) / 2, (i + 1) * (numberOfEntries / threadTotal) - 1);
    }

   

    FILE *outputFile;
    outputFile = fopen(argv[2], "w");
    if(outputFile == NULL){
       fprintf(stderr, "An error has occurred\n");
        exit(0);
    }
    for (int i = 0; i < numberOfEntries; i++){
        // printf("key: %d\n", input[i].key);
        fwrite(&input[i].key, 4, 1, outputFile);
        fwrite(input[i].value, 96, 1, outputFile);
    }
    fclose(inputFile);
    fclose(outputFile);
    free(input);
}