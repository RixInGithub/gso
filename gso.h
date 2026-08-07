#ifndef GSO_H
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
typedef unsigned int gso; // idk lol

gso gsoFromCStr(char*);
gso gsoFromBuf(void*,size_t);
gso gsoFromBool(bool);
gso gsoFromU8(uint8_t);
gso gsoFromU16(uint16_t);
gso gsoFromU32(uint32_t);
gso gsoFromU64(uint64_t);

void gsoAppend(gso,gso);
// gso gsoSlice(gso,int,int);

gso gsoCatBufs(gso);
void*gsoGetIndex(gso,int,void*);
int gsoGetType(gso,int);
size_t gsoLen(gso);

void gsoFreeN(size_t,...);
void gsoFree(gso);

char*gsoSrz(gso,size_t*);
gso gsoParse(char*,size_t);
#endif
