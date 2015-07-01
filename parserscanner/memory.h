#ifndef __MEMORY_H
#define __MEMORY_H

void *Malloc(unsigned n);
void *Calloc(unsigned n);

#define NEW(type) (type *)Calloc(sizeof(type));

#endif /* __MEMORY_H */
