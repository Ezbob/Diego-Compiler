#ifndef __MEMORY_H
#define __MEMORY_H

void *Malloc(unsigned n);

#define NEW(type) (type *)Malloc(sizeof(type));

#endif /* __MEMORY_H */
