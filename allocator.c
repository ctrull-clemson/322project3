/****************************************************************************************************************
*
* Author name: Christian Trull
*
* File Name:   allocator.c
*
* Summary:
*  This file is the library that is used to shim malloc() and free() so that the memory leaks can be traced.
*     A linked list is also created to keep track of all the memory allocated with malloc() and then freed with 
*     free(). The calls to the normal malloc() and free() are used within the new calls to perform the functions;
*     the new calls are just for keeping track of the memory. At termination, the library will print out the
*     number of leaks and how much memory was leaked.
*
****************************************************************************************************************/
#define _GNU_SOURCE
 
#include <stdio.h>
#include <stdlib.h> 
#include <dlfcn.h>
#include <stddef.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

void __attribute__ ((constructor)) init_methods(void);
void __attribute__ ((destructor)) cleanup(void);

// Shimmed malloc. Uses mmap to get pages which are used similar to how malloc normally works.
void *malloc (size_t size)
{  

}

// 
void free (void *ptr)
{
   
}

// 
void *calloc(size_t nmemb, size_t size)
{  

}

// 
void *realloc(void *ptr, size_t size)
{
   
}

// Initializes original functions for use in the rest of the library
void init_methods()
{
   original_malloc = dlsym(RTLD_NEXT, "malloc");
   original_free = dlsym(RTLD_NEXT, "free");

   // Creating dummy header node for easier list management
   head = (node_t *)original_malloc(sizeof(node_t));
   head->ptr = NULL;
   head->next = NULL;
   head->size = 0;
}

// Called when the library is unloaded and prints out the number of leaks, size of the leaks, and the totals for both.
void cleanup(void)
{
   node_t * iter = head->next;
   size_t sum = 0;
   int i = 0;

   // Walk through all nodes with memory that hasnt been freed and print out.
   while(iter != NULL)
   {
      fprintf(stderr, "LEAK\t%zu\n", iter->size);
      sum += iter->size;
      iter = iter->next;
      i += 1;
   }  
   fprintf(stderr, "TOTAL\t%d\t%zu\n", i, sum);
}
