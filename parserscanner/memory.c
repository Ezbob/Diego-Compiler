#include <stdio.h>
#include <stdlib.h>

void *Malloc(unsigned n){ 
	void *p;

	if (!(p = malloc(n))) {
		fprintf(stderr,"Malloc(%d) failed.\n",n);
		fflush(stderr);
		exit(1);
	}
		return p;
}

void *Calloc(unsigned n){
	void *p;

	if (!(p = calloc(4, n))) {
		fprintf(stderr,"Calloc(%d) failed.\n",n);
		fflush(stderr);
		exit(1);
	}
	return p;
}
