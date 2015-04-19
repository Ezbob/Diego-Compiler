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
