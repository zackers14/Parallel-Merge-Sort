/*********************************************************************
* Programmer: Zachary Mericle, Erica Romero, Ryan Fay                                        *
* Last Edited: 10/24/18                                              *
*                                                                    *
**********************************************************************/

/* This program implements a parallel version of merge sort by
creating a thread to handle each recursive call
*/

#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>

/*Key number */
#define SHMKEY ((key_t) 9999)

/* Shared structure with buffer */
typedef struct
{
  int integer_array[];
} shared_mem;

pthread_t       tids[];
pthread_attr_t  attr[1]; //Attribute pointer array
shared_mem *sh_mem;
sem_t mutex;

void normalMergeSort(int arr[], int arr_size)
{   
  if(arr_size > 1)
  {  
    int mid = arr_size/2;

    int remainder = arr_size%2;

    if(remainder == 1)
    {
      mid = mid + 1;
    }

    int lefthalf[mid];
    int righthalf[arr_size-mid];
    int a;
    int b;  

    /*Stores the lowerhalf of the array into the lefthalf array*/  
    for(a = 0; a < mid; a++)
    {
      lefthalf[a] = arr[a];
    }

    int x = 0;

    /*Stores the upperhalf of the array into the righthalf array*/
    for(b = mid; b < arr_size; b++) 
    {
      righthalf[x] = arr[b];
      x++;
    }

    /*Recurive call for mergesort of the lefthalf*/
    normalMergeSort(lefthalf, mid);

    /*Recurive call for mergesort of the righthalf*/
    normalMergeSort(righthalf, arr_size/2);

    int i = 0;
    int j = 0;
    int k = 0;     

    /*Compares the elements from the lefthalf and righthalf array, and sorts them before placing them into the arr array */
    while (i < mid && j < arr_size-mid) 
    { 
        if (lefthalf[i] <= righthalf[j]) 
        { 
            arr[k] = lefthalf[i];
            i++; 
        } 
        else
        { 
            arr[k] = righthalf[j];
            j++; 
        } 
        k++; 
    } 

    /*Copies the remaining elements of the lefthand array, if any*/
    while (i < mid) 
    { 
        arr[k] = lefthalf[i]; 
        i++; 
        k++; 
    } 

    /*Copies the remaining elements of the righthand array, if any*/
    while (j < arr_size-mid) 
    { 
        arr[k] = righthalf[j]; 
        j++; 
        k++; 
    } 
  }
}

/*CONTAINS SAMPLE MAIN WITH MULTIPLE ARRAYS FOR CHECKING NORMALMERGESORT
int main() 
{
  //int arr[] = {12, 11, 13, 5, 6, 7}; 

  //int arr[] = {33, 12, 53, 4}; 

  //int arr[] = {100, 20, 12, 61, 89, 90, 40,71, 200}; 

  //int arr[] = {8, 403, 17, 15, 49, 10, 0, 15, 30};

  int arr[] = {60, 32, 1, 0, 3, 200, 1000, 4, 10, 19, 6000, 90, 2};

  int arr_size = sizeof(arr)/sizeof(arr[0]); 
  
  int i;
  
  printf("Original Array: ");

  for(i = 0; i < arr_size; i++)
    printf("%d ", arr[i]);

  normalMergeSort(arr, arr_size);

  printf("\nSorted Array: ");

  for(i = 0; i < arr_size; i++)
    printf("%d ", arr[i]);

  printf("\n");

  return 0; 
}*/


/*
int main(){
  //Initialize necessary variables for shared memory
  int shmid;
  char * shmadd;
  shmadd = (char *) 0;

  //Initialize the three semaphores
  sem_init(&mutex, 0, 1);
  sem_init(&full, 0, 0);
  sem_init(&empty, 0, 15);

  //Create a shared memory section
  if ((shmid = shmget(SHMKEY, 15*sizeof(char), IPC_CREAT | 0666)) < 0){
    perror("shmget");
    exit(1);
  }

  //Connect to shared memory section
  if ((sh_mem = (shared_mem *) shmat(shmid, shmadd, 0)) == (shared_mem *)-1){
    perror("shmat");
    exit(0);
  }

  //Schedule thread independently
  pthread_attr_init(&attr[0]);
  pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);
  //Schedule thread independently END

  //Create threads
  pthread_create(&tid1[0], &attr[0], producer, NULL);
  pthread_create(&tid2[0], &attr[0], consumer, NULL);

  //Wait for threads to finish
  pthread_join(tid1[0], NULL);
  pthread_join(tid2[0], NULL);

  //Terminate semaphores
  sem_destroy(&mutex);

  //Detach and remove shared memory
  if (shmdt(sh_mem) == -1){
    perror("shmdt");
    exit(-1);
  }
  shmctl(shmid, IPC_RMID, NULL);

//Terminate threads
  pthread_exit(NULL);
}

*/
