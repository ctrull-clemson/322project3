/****************************************************************************************************************
*
* Author name: Christian Trull
*
* File Name:   allocator.c
*
* Summary:
*  Type here
*
****************************************************************************************************************/
#define _GNU_SOURCE
#define PAGESIZE 4096
 
#include <stdio.h>
#include <stdlib.h> 
#include <dlfcn.h>
#include <stddef.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>

void __attribute__ ((constructor)) init_methods(void);
void __attribute__ ((destructor)) cleanup(void);

int fd;
void *head, *tail;

typedef struct pageheader{
    int psize;
    void *prev;
    void *next;
    void *data;
} pageheader;

// Shimmed malloc. Uses mmap to get pages which are used similar to how malloc normally works.
void *malloc (size_t size)
{
   // No reason to get memory if none was requested
   if(size <= 0)
   {
      return NULL;
   }
   size += sizeof(pageheader);
   int pageCount = 1;
   
   if((size) > PAGESIZE)
   {
      pageCount = ceil(size/PAGESIZE);
   }
   
   void *page = mmap(NULL, (pageCount * PAGESIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
   pageheader *header = page;
   header->psize = size;
   header->prev = tail;
   header->next = NULL;
   
   return &(header->data);
}

// 
void free (void *ptr)
{
   ptr -= sizeof(ptr);
   munmap(ptr, (int)ptr);
}

// 
void *calloc(size_t nmemb, size_t size)
{  
   
   
   return NULL;
}

// 
void *realloc(void *ptr, size_t size)
{
   
   
   return NULL;   
}

// Initializes original functions for use in the rest of the library
void init_methods()
{
   fd = open("/dev/zero", O_RDWR);
   
}

// Called when the library is unloaded and prints out the number of leaks, size of the leaks, and the totals for both.
void cleanup(void)
{
   
}
