#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define page 5000

int main()
{
    uint8_t *array[100];
    for(int i = 0; i < 100; i++)
    	array[i] = NULL;
    
    srand((unsigned) 76);
    int final = 0;
    int ndx;
    
    //printf("Command\tNdx\tSize\tCount\n");
    while(final < 500000) {
    	ndx = rand() % 100;
    	int instruct = rand() % 2;
    	
    	if(final == 268)
    	   printf("\n");
    	
    	if(array[ndx] == NULL) 
    	{
    		int si = rand() % page;
    		printf("malloc\t%d\t%d\t%d\n", ndx, si, final);
    		fflush(stdout);
    		uint8_t *p = malloc(si);

    		memset(p, final, si);
    		//printf("\t%p\n", p);
    		array[ndx] = p;
    		final++;
    	}
    	
    	else 
    	{
    		if(instruct == 1) 
    		{
    		   printf("free\t%d\t\t%d\n", ndx, final);
    		   fflush(stdout);
    			free(array[ndx]);
    			array[ndx] = NULL;
    			final++;
    		}
    		else 
    		{
    			int si = rand() % page;
    			printf("realloc\t%d\t%d\t%d\n", ndx, si, final);
    			fflush(stdout);
    			
    			uint8_t *p = realloc(array[ndx], si);
            memset(p, final, si);
    			array[ndx] = p;
    			final++;
    		}
    	}
    }
}
