#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
char input[100];
int index; //global value
/*
current question:

    purpose of fsync
    how to determine threads needed
    using create and join 
    sort of an idea on how to implement gloabl varibale inuse of odd or even but more info needed
    creating individual test cases
    
*/
void check_and_swap(void* args)
{

    if ((int)input[index] > (int)input[index + 1])
    {
        char temp = input[index + 1];
        input[index + 1] = input[index];
        input[index] = temp;
    }
}

void odd_even_sort(char input[], int n)
{
    pthread_t odd, even;

    for (int i = 1; i < n; i++)
    {
        if (i % 2 == 1){
        // odd case
        //for loop
            //call create
        //for loop
            //call join 
        } else {
        // even case
        //call create
        //call join 
        }
        pthread_create(&even, NULL, check_and_swap, NULL);
    }
}

int main(int argc, char *argv[])
{
    FILE *inputFile;
    //char input[100];
    inputFile = fopen(argv[1], "w+");
    fread(input, 100, 1, inputFile);

    FILE *outputFile;
    char output[100];
    outputFile = fopen(argv[2], "w+");

    odd_even_sort(input, 100);

    fwrite(input, 1, 100, outputFile);
    //what exactly is the purpose of fsync
    fsync();
    fclose(input);
    fclose(output);
}