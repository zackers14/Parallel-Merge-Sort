#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

/*Key number */
#define SHMKEY ((key_t) 9999)

/* Shared structure with buffer */
typedef struct
{
  int *integer_array;
} shared_mem;

pthread_attr_t  attr[1]; //Attribute pointer array
shared_mem *sh_mem;

struct timezone Idunno;
struct timeval startTime, endTime;

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
  //Local array to store sorted array
  int local_arr[max+1]; // <-- Size should be max - min + 1 because only the left and right halves are necessary
  // Local_Arr is fine for the left + right halves but you're going to need a left and right array

  //Array to hold copy of array in shared memory
  int copy_arr[max+1]; // Unnecessary

  for (int i = 0; i < max+1; i++){ // <-- Unnecessary loop, should put left half in left array and right half in right array
    copy_arr[i] = sh_mem->integer_array[i];
  }

  //Iterators for each half of the given array
  int leftIter = min;
  int rightIter = mid;

  //Go through whole array
  for (i = min; i < max; i++){
    if (leftIter < mid && (rightIter >= max || copy_arr[leftIter] <= copy_arr[rightIter])) {
      local_arr[i] = copy_arr[leftIter];
      leftIter += 1;
    }
    else {
      local_arr[i] = copy_arr[rightIter];
      rightIter += 1;
    }
  }

  //Transfer sorted array into array in shared memory
  for (int i = 0; i < max+1; i++){ // <-- Should be i = min
    sh_mem->integer_array[i] = local_arr[i];
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

double report_running_time(char[] type, int size) {
	long sec_diff, usec_diff;
	gettimeofday(&endTime, &Idunno);
	sec_diff = endTime.tv_sec - startTime.tv_sec;
	usec_diff= endTime.tv_usec-startTime.tv_usec;
	if(usec_diff < 0) {
		sec_diff --;
		usec_diff += 1000000;
	}
	printf("Running time for %s Mergesort of size %d: %ld.%06ld\n", type, size, sec_diff, usec_diff);
	return (double)(sec_diff*1.0 + usec_diff/1000000.0);
}



int main(int argc, char *argv[]){
  //Initialize necessary variables for shared memory
  int shmid;
  int array_size = atoi(argv[1]);
  char * shmadd;

  pthread_t tids[ceiling(array_size*(log10(array_size)/log10(2)))];
  int int_array[array_size];
  sh_mem->integer_array = malloc(sizeof(int) * array_size);
  time_t t;

  srand((unsigned) time(&t));

  for (int i = 0; i < array_size){
    int random_integer = rand()%100;
    int_array[i] = random_integer;
    sh_mem->integer_array[i] = random_integer;
  }
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

  gettimeofday(&startTime, &Idunno);

  normalMergeSort(int_array, array_size);
  report_running_time("Normal", array_size);
  printf("\nSorted Array: ");
  for(i = 0; i < array_size; i++)
    printf("%d ", int_array[i]);
  printf("\n");

  gettimeofday(&startTime, &Idunno);

  pthread_create(&tids[0], &attr[0], mergeSort(0, array_size-1, 0), NULL);

  pthread_join(tids[0], NULL);

  repot_running_time("parallel", array_size);

  for(i = 0; i < array_size; i++)
    printf("%d ", sh_mem->integer_array[i]);
  printf("\n");

  //Detach and remove shared memory
  if (shmdt(sh_mem) == -1){
    perror("shmdt");
    exit(-1);
  }
  shmctl(shmid, IPC_RMID, NULL);
//Terminate threads
  pthread_exit(NULL);
}
