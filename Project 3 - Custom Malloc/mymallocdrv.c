#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define TIMES 10

int main (void){
	int i =0;
	int *array[TIMES];
	for(i=0; i<TIMES; i++){
		array[i] = (int *)my_nextfit_malloc(sizeof(int));
		printf("Malloc'd: %p\n",array[i]);
	}
	for(i=0; i<TIMES;i++){
		printf("freeing: %p\n",array[i]);
		my_free(array[i]);
	}
	void *test  = (void *)my_nextfit_malloc(50);
	void *test2 = (void *)my_nextfit_malloc(50);
	my_free(test);
	test = (void *)my_nextfit_malloc(4);
	my_free(test);
	my_free(NULL);
}