#ifndef GSO_H
#include <stddef.h>
#include <stdbool.h>
typedef unsigned int gso; // idk lol

gso gsoFromCStr(char*);
gso gsoFromBuf(void*,size_t);
gso gsoFromBool(bool);

void gsoAppend(gso,gso);
// gso gsoSlice(gso,int,int);

gso gsoCatBufs(gso);
void*gsoGetIndex(gso,int,void*);

void gsoFreeN(unsigned int,...);
void gsoFree(gso);

char*gsoSrz(gso,size_t*);
gso gsoParse(char*,size_t);
#endif