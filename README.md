# Parllel-Merge-Sort

## Basic Idea

1. Main should receive an input of an array of integers from the command line
2. Merge Sort should be called on this input
    - Merge Sort should:
      1. Divide the array into a left and right half
      2. Create a thread to handle the left half and create a thread to handle the right half
      3. Once these threads return their sorted halves, the parent of these two threads should merge these sorted halves
      4. The merged halves should be returned
3. The merged and sorted array should be printed

## How is this faster than normal merge sort?
- Both left and right halves can be sorted simultaneously by two threads instead of one after the other

## TBD
- How to use shared memory to improve this process
- If used, mutex semaphores required
- Maybe a shared memory segment created per parent

## Anticipated Required Functions
 - MergeSort(int[] array)
 - Merge(int[] left, int[] right)

## How to compile:
 - gcc parallel_merge_sort.c -lm -lpthread -o OS4
 - ./OS4 n
 - Where n >= 2 and n<= 12000

## Analysis
 - The parallel version of merge sort runs slower than the normal one
 - This is likely because of how expensive it is to create threads
 - n can only go as high as about 12000, likely because that's either how big shared memory can get or there are too many threads
 - The thread array is n*log2(n), because there are roughly that many calls to merge sort. This is a massive space complexity issue
 - Overall, it's better to just run it the normal way. Maybe running on a GPU would be better with higher n sizes, because it's suited for this type of massive paralleization
