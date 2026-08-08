#include <gso.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <unistd.h>

int main() {
	#ifndef TEST
		#define TEST 0
	#endif
	#if TEST == 0
		gso str, second, res;
		size_t size;
		char**wow;
		uint16_t count = -1;
		while (count>0) {
			gso str = gsoFromCStr("heyo ");
			gso second = gsoFromCStr("world");
			gsoAppend(str,second);
			gso res = gsoCatBufs(str);
			gsoFree(str);
			wow = gsoGetIndex(res,0,&size);
			assert(strncmp(*wow,"heyo world",size)==0);
			free(*wow);
			free(wow);
			gsoFree(res);
			count--;
		}
		return 0;
	#endif
	#if TEST == 1
		char pStr[] = "oneTwo\three\x00" "123!!!!!!!!!!!!!!!!!";
		gso user, pass, uCp;
		char*buf;
		size_t size;
		user = gsoFromCStr("grad");
		pass = gsoFromBuf(pStr,sizeof(pStr)-1);
		gsoAppend(user, pass);
		buf = gsoSrz(user,&size);
		write(1, buf, size);
		uCp = gsoParse(buf,size);
		free(buf);
		gsoFreeN(2, user, uCp);
	#endif
	#if TEST == 2
		uint64_t thing = 1786195387835;
		uint64_t*thingPtr;
		char*buf;
		size_t size;
		gso t, tCp;
		t = gsoFromU64(thing);
		buf = gsoSrz(t, &size);
		tCp = gsoParse(buf, size);
		thingPtr = gsoGetIndex(tCp,0,NULL);
		printf("before srz: %lu\n", thing);
		printf("after srz: %lu\n", *thingPtr);
		gsoFreeN(2, t, tCp);
		free(thingPtr);
		free(buf);
	#endif
}