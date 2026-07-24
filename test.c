#include <gso.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

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
}