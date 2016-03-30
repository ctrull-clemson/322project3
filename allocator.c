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

struct pageheader{
    int psize;
    struct pageheader *prev;
    struct pageheader *next;
};

struct queueheader{
    struct pageheader *head;
    struct pageheader *tail;
};

int fd;
struct queueheader *queue;

// Removes the page from the linked list of pages
void takeFromQueue(struct pageheader *page)
{
   return;

   if(page == queue->tail)
   {
      queue->tail = page->prev;
   }
   
   // Remove page from queue
   page->prev->next = page->next;
   if(page->next != NULL)
      page->next->prev = page->prev;   
}

// Adds the page to the linked list of pages
void addToQueue(struct pageheader *addingPage)
{
   return;
   
   queue->tail->next = addingPage;
   addingPage->prev = queue->tail;
   queue->tail = addingPage;
   addingPage->next = NULL;
}

// Gets a number of pages to hold the desired size of memory that the user requested and returns 
// a pointer to where data can be stored on it.
void *malloc (size_t size)
{
   // No reason to get memory if none was requested
   if(size <= 0)
   {
      return NULL;
   }
   
   size += sizeof(struct pageheader);
   int pageCount = 1;
   
   // Determines number of pages needed
   if(size > PAGESIZE)
   {
      pageCount = ceil(size/PAGESIZE);
   }
   
   void *page = mmap(NULL, (pageCount * PAGESIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
   struct pageheader *header = page;
   header->psize = (pageCount * PAGESIZE);
   addToQueue(header);
   
   return (page + sizeof(struct pageheader));
}

// Releases page when the user calls to free the memory.
void free (void *ptr)
{
   // Corner case check. Cant free NULL.
   if(ptr == NULL)
   {
      return;
   }

   struct pageheader *page = ptr - sizeof(struct pageheader);
   takeFromQueue(page);
   
   // Unmap the page that was used
   munmap(page, page->psize);
}

// Allocate a block of memory which has a block for data the size of nmemb * size
void *calloc(size_t nmemb, size_t size)
{  
   // No reason to get memory if none was requested
   if((nmemb * size) <= 0)
   {
      return NULL;
   }
   
   size = (nmemb * size);
   size += sizeof(struct pageheader);
   int pageCount = 1;
   
   if(size > PAGESIZE)
   {
      pageCount = ceil(size/PAGESIZE);
   }
   
   // Get the page, and set the relevant information before returning a pointer to the data section
   void *page = mmap(NULL, (pageCount * PAGESIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
   struct pageheader *header = page;
   header->psize = (pageCount * PAGESIZE);
   addToQueue(header);
   
   return (page + sizeof(struct pageheader));
}

// If the size parameter is larger than the block pointed to by ptr, a new page
// is created, the memory is copied over, and the original block is freed.
void *realloc(void *ptr, size_t size)
{
   // If realloc is called on a null pointer, malloc space and return it.
   if(ptr == NULL)
   {
      return malloc(size);
   }
   
   struct pageheader *info = ptr - sizeof(struct pageheader);
   
   // Determine if the original block is smaller than the current memory
   if(info->psize >= (size + sizeof(struct pageheader)))
   {
      return ptr;
   }
   
   // The memory block is smaller than what is asked for, so allocating a new block of memory.
   else
   {  
      // Create new page
      int pageCount = ceil((size + sizeof(struct pageheader))/PAGESIZE);      
      void *page = mmap(NULL, (pageCount * PAGESIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
      struct pageheader *header = page;
      header->psize = (pageCount * PAGESIZE);
      addToQueue(header);
      void *dataSection = page + sizeof(struct pageheader);
      
      // Copy over data from the old page to the new page
      memmove(dataSection, ptr, (info->psize - sizeof(struct pageheader))); 
      
      // Free up the original page
      free(ptr);
      
      return (page + sizeof(struct pageheader));
   }
}

// Initializes original functions for use in the rest of the library
void init_methods()
{
   fd = open("/dev/zero", O_RDWR);
   
   // Create header page   
   queue = mmap(NULL, sizeof(struct queueheader), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
   struct pageheader *headPage = mmap(NULL, sizeof(struct pageheader), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
   headPage->psize = sizeof(struct pageheader);
   headPage->prev = NULL;
   headPage->next = NULL;
   queue->head = headPage;
   queue->tail = queue->head;
}

void cleanup(void)
{
   
}









