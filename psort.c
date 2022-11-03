#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
int *input;
int index; // global value

/*
 using merge/quick sort instead of odd even
 use the stack instead heap
*/
/*
current question:

    purpose of fsync
    how to determine threads needed
    using create and join
    sort of an idea on how to implement gloabl varibale inuse of odd or even but more info needed
    creating individual test cases

*/
void check_and_swap(void *args)
{

    if ((int)input[index] > (int)input[index + 1])
    {
        char temp = input[index + 1];
        input[index + 1] = input[index];
        input[index] = temp;
    }
}

void merge_sort(int *input, int n)
{
    pthread_t first, second;
    // get threads from anywhere dependendent on processor
    /*  for (int i = 1; i < n; i++)
      {
          if (i % 2 == 1)
          {
              // odd case
              // for loop
              // call create
              // for loop
              // call join
          }
          else
          {
              // even case
              // call create
              // call join
          }
          pthread_create(&even, NULL, check_and_swap, NULL);
      }
      */
}

int main(int argc, char *argv[])
{
    FILE *inputFile;
    inputFile = fopen(argv[1], "w+");
    fseek(inputFile, 0L, SEEK_END);
    int sizeOfFile = (int)ftell(inputFile);
    index = sizeOfFile / 4;
    input = malloc(sizeof *input * sizeOfFile); // probably not right but you get the idea
    fseek(inputFile, 0, SEEK_SET);
    fread(input, sizeof(input), 1, inputFile);

    merge_sort(input, index);
    FILE *outputFile;
    outputFile = fopen(argv[2], "w+");
    fwrite(input, sizeof input, 1, outputFile);
    fclose(inputFile);
    fclose(outputFile);
    free(input);
    // fsync(outputFile);
    /*   FILE *inputFile;
       int sizeOfFile;
       char *fileOnHeap;
       // char input[100];
       inputFile = fopen(argv[1], "w+");

       // pull file into heap --> get size -- move to stack
       fseek(inputFile, 0L, SEEK_END);

       fread(input, &input, 1, inputFile);

       FILE *outputFile;
       char output[100];
       outputFile = fopen(argv[2], "w+");

       odd_even_sort(input, 100);

       fwrite(input, 1, 100, outputFile);
       // what exactly is the purpose of fsync
       fsync();
       fclose(inputFile);
       fclose(outputFile);*/
}