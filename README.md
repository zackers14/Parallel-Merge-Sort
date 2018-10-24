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
