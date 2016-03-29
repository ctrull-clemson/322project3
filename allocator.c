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

void takeFromQueue(struct pageheader *page)
{
   if(page == queue->tail)
   {
      queue->tail = page->prev;
   }
   
   // Remove page from queue
   page->prev->next = page->next;
   if(page->next != NULL)
      page->next->prev = page->prev;   
}

void addToQueue(struct pageheader *addingPage)
{
   queue->tail->next = addingPage;
   addingPage->prev = queue->tail;
   queue->tail = addingPage;
   addingPage->next = NULL;
}

// Shimmed malloc. Uses mmap to get pages which are used similar to how malloc normally works.
void *malloc (size_t size)
{
   // No reason to get memory if none was requested
   if(size <= 0)
   {
      return NULL;
   }
   
   size += sizeof(struct pageheader);
   int pageCount = 1;
   
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
   if(ptr == NULL)
   {
      return;
   }

   struct pageheader *page = ptr - sizeof(struct pageheader);
   takeFromQueue(page);
   
   // Unmap the page that was used
   munmap(page, page->psize);
}

// 
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
   
   void *page = mmap(NULL, (pageCount * PAGESIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
   struct pageheader *header = page;
   header->psize = (pageCount * PAGESIZE);
   addToQueue(header);
   
   return (page + sizeof(struct pageheader));
}

// 
void *realloc(void *ptr, size_t size)
{
   struct pageheader *info = ptr - sizeof(struct pageheader);
   if(info->psize >= (size + sizeof(struct pageheader)))
   {
      return ptr;
   }
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

// Called when the library is unloaded and prints out the number of leaks, size of the leaks, and the totals for both.
void cleanup(void)
{
   struct pageheader *previous, *current;
   current = queue->tail;
   previous = current;
   
   while(queue->head != queue->tail)
   {
      previous = current->prev;
      takeFromQueue(current);
      free(current);
      current = previous;
   }
   
   munmap(queue, sizeof(struct queueheader));
}









