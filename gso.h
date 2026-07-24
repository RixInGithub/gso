#ifndef GSO_H
#include <stddef.h>
typedef unsigned int gso; // idk lol

gso gsoFromCStr(char*);
gso gsoFromBuf(void*,size_t);

void gsoAppend(gso,gso);
// gso gsoSlice(gso,int,int);

gso gsoCatBufs(gso);
void*gsoGetIndex(gso,int,void*);

void gsoFreeN(unsigned int,...);
void gsoFree(gso);

char*gsoSrz(gso,size_t*);
#endif