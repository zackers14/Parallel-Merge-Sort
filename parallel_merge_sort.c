/*********************************************************************
* Programmers: Zachary Mericle, Erica Romero, Ryan Fay, Courtney Shi  *
* Last Edited: 10/29/18                                              *
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
#include <math.h>

/*Key number */
#define SHMKEY ((key_t) 9999)

/* Shared structure with buffer */
typedef struct
{
  int integer_array[];
} shared_mem;

pthread_attr_t  attr[1]; //Attribute pointer array
shared_mem *sh_mem;

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

    /*Recurive call for normalMergeSort of the lefthalf*/
    normalMergeSort(lefthalf, mid);

    /*Recurive call for normalMergeSort of the righthalf*/
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

void mergeSort(int minIndex, int maxIndex, int threadIndex) 
{ 
	// Designed as per our whiteboard discussion last week

    if (((maxIndex - minIndex) + 1) < 2){
        return;
    }
	// 1.) Determine midpoint
    int midpoint = floor((minIndex+maxIndex)/2); 
	
	// 2.) Find ranges for left half, right half:
	int leftThreadIndex = 2*threadIndex+1; 
    int rightThreadIndex = leftThreadIndex + 1;
            
	// 3.) Create a thread for each half in array
	
	pthread_create(&tids[leftThreadIndex], &attr[0], mergeSort(minIndex, midpoint, leftThreadIndex), NULL);
	pthread_create(&tids[rightThreadIndex], &attr[0], mergeSort(midpoint+1, maxIndex, rightThreadIndex), NULL);
  
    // 4.) Wait for return 
	pthread_join(tids[leftThreadIndex], NULL);
	pthread_join(tids[rightThreadIndex], NULL);
	
	// 5.) merge 
	
    merge(minIndex, midpoint, maxIndex); 

} 

void merge(int min, int mid, int max){
     //Create temporary arrays for left and right half, merge them, then store in shared memory

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



int main(int argc, char *argv[]){
  //Initialize necessary variables for shared memory
  int shmid;
  int array_size = atoi(argv[1]);
  char * shmadd;
  pthread_t tids[ceiling(array_size*(log10(array_size)/log10(2)))];

  shmadd = (char *) 0;
  
  //Create a shared memory section
  if ((shmid = shmget(SHMKEY, array_size*sizeof(int), IPC_CREAT | 0666)) < 0){
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

  pthread_create(&tids[0], &attr[0], mergeSort(0, array_size-1, 0), NULL);

  pthread_join(tids[0], NULL);

  //Detach and remove shared memory
  if (shmdt(sh_mem) == -1){
    perror("shmdt");
    exit(-1);
  }
  shmctl(shmid, IPC_RMID, NULL);
//Terminate threads
  pthread_exit(NULL);
}
