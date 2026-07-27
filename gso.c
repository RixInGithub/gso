#include <gso.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#define __(a) __gso__##a
#define _(a) __(a)
#define DECLCAREFUL(type,name) _(carefulData) _(name) = {sizeof(type),0,0,NULL}
#define EXTENDBY 30
#define THINGTHATIUSETWICE {_(extend)(&out);dat=out.data;} // im not tracking the count of times im using this macro in its name

typedef unsigned char _(c);

// type byte structure:
// xxxxxttt
// x - extra
// t - type

// type = 000, buffer/string type, extra is used for length. if extra==0, contents are "escaped"
// type = 001, blank buffer type, extra is unused here.
// type = 010, bool type, if extra is nonzero, the bool represents a truthy value, else, it represents a falsy value.

// escaped => meaning:
// \t\t => \t
// \t\x00 => \x00
// \x00 => end of buffer

typedef struct {
	size_t unit;
	size_t available;
	size_t len;
	void*data;
} _(carefulData);

typedef struct {
	size_t size;
	void*raw;
	size_t escSz;
	void*escaped;
} _(buf);

typedef struct _(handle) {
	bool used;
	_(c) typeByte;
	union {
		_(buf) buffer;
	};
	struct _(handle)*next;
} _(handle);

DECLCAREFUL(_(handle),handles);

bool _(didInit) = false;
bool _(ignoreRest) = false;

static void _(init)() {
	if (_(didInit)) return;
	// ...
	_(didInit) = true;
}

// the dose is the poison.
void _(extend)(_(carefulData)*dat) {
	size_t old = dat->available;
	dat->available += EXTENDBY;
	dat->data = realloc(dat->data,dat->available*dat->unit);
	memset((void*)((uintptr_t)(dat->data)+(old*dat->unit)),0,(EXTENDBY*dat->unit));
}

_(handle)*_(toHandle)(gso ref) {
	_(init)();
	return (_(handle)*)_(handles).data + ref;
}

char*_(escapeBuf)(_(buf)*b,size_t*size) {
	if (!(b->escaped)) {
		_(carefulData) out = {sizeof(char),0,0,NULL};
		char*dat = NULL;
		size_t count = 0;
		while (count<b->size) {
			if (out.len+1>=out.available) THINGTHATIUSETWICE;
			char ch = ((char*)(b->raw))[count];
			switch (ch) {
				case 0:
				case 9:
					dat[out.len++] = 9;
				default:
					dat[out.len++] = ch;
					break;
			}
			count++;
		}
		if (out.len+1>=out.available) THINGTHATIUSETWICE;
		dat[out.len++] = 0;
		b->escaped = dat;
		b->escSz = out.len;
	}
	*size = b->escSz;
	return b->escaped;
}

_(handle)*_(findH)(gso*idx) {
	static gso placeholder;
	_(init)();
	if (_(handles).len==_(handles).available) _(extend)(&_(handles));
	_(handle)*h = _(handles).data;
	if (idx==NULL) idx=&placeholder;
	*idx = 0;
	_(handle)*o = NULL;
	while (*idx<_(handles).len) {
		if (!(h[*idx].used)) {
			o = &h[*idx];
			break;
		}
		(*idx)++;
	}
	if (o==NULL) {
		*idx = _(handles).len++;
		o=&h[*idx];
	}
	memset(o, 0, sizeof(_(handle)));
	return o;
}

gso gsoFromBuf(void*inp, size_t size) {
	_(c) typeByte = (size<32)?(size<<3):0;
	if (size==0) typeByte=1;
	gso res;
	_(handle)*where = _(findH)(&res);
	where->used=true;
	where->typeByte=typeByte;
	if ((typeByte&7)!=0) return res;
	where->buffer.size=size;
	where->buffer.raw=malloc(size);
	memcpy(where->buffer.raw,inp,size);
	return res;
}

gso gsoFromCStr(char*inp) {
	return gsoFromBuf(inp,strlen(inp));
}

gso gsoFromBool(bool a) {
	gso res;
	_(handle)*where = _(findH)(&res);
	where->used = true;
	where->typeByte = (a?8:0)|2;
	return res;
}

void gsoAppend(gso _a, gso _b) {
	_(handle)*a = _(toHandle)(_a);
	_(handle)*b = _(toHandle)(_b);
	while (a->next) a=a->next; // we only need the last one, lol
	// TODO: copy b
	a->next=b;
}

gso gsoCatBufs(gso _bufs) {
	_(handle)*bufs = _(toHandle)(_bufs);
	size_t needed = 0;
	size_t fulfilled = 0;
	_(handle)*a = bufs;
	while (a) {
		if ((a->typeByte&7)==0) needed+=a->buffer.size;
		// printf("%d\n",a->next==a);
		a=a->next;
	}
	_(c)*new = malloc(needed);
	while ((bufs)&&(fulfilled<needed)) {
		if ((bufs->typeByte&7)==0) {
			memcpy(new+fulfilled,bufs->buffer.raw,bufs->buffer.size);
			fulfilled+=bufs->buffer.size;
		}
		bufs=bufs->next;
	}
	gso ref = gsoFromBuf(new,fulfilled);
	free(new); // new got remalloced
	return ref;
}

void gsoFree(gso _a) {
	_(handle)*a = _(toHandle)(_a);
	while (a) {
		switch (a->typeByte&7) {
			case 0:
				free(a->buffer.raw);
				if (a->buffer.escaped) free(a->buffer.escaped);
				break;
			default:
				break; // idfk
		}
		a->used=false;
		a=a->next;
	}
}

void gsoFreeN(unsigned int len, ...) {
	va_list a;
	va_start(a,len);
	unsigned int count = 0;
	while (count<len) {
		gsoFree(va_arg(a,gso));
		count++;
	}
	va_end(a);
}

void*gsoGetIndex(gso _a, int idx, void*extra) {
	_(handle)*a = _(toHandle)(_a);
	while ((a)&&(idx)) {
		a=a->next;
		idx--;
	}
	if (idx>0) return NULL;
	unsigned char*aa;
	switch (a->typeByte&7) {
		case 0:
			if (extra) *(size_t*)extra=a->buffer.size;
			unsigned char*copy = calloc(a->buffer.size+1,sizeof(unsigned char));
			memcpy(copy,a->buffer.raw,a->buffer.size);
			unsigned char**yay = malloc(sizeof(unsigned char*));
			*yay = copy;
			return yay;
		case 1:
			aa=calloc(1,sizeof(unsigned char));
			unsigned char**bb=calloc(1,sizeof(unsigned char*));
			*bb=aa;
			return bb;
		default:
			return NULL;
	}
	__builtin_unreachable();
}

char*gsoSrz(gso _a, size_t*size) {
	if (size==NULL) return NULL;
	_(handle)*a = _(toHandle)(_a);
	_(carefulData) out = {sizeof(char),0,0,NULL};
	char*dat = NULL;
	while (a) {
		if (out.len+1>=out.available) THINGTHATIUSETWICE;
		dat[out.len++] = a->typeByte;
		char*escaped;
		size_t escSz;
		switch (a->typeByte&7) {
			case 0:
				escaped = a->buffer.raw;
				escSz = a->buffer.size;
				if ((a->typeByte>>3)==0) escaped = _(escapeBuf)(&(a->buffer),&escSz);
				while (out.len+escSz>=out.available) THINGTHATIUSETWICE;
				memcpy(dat+out.len,escaped,escSz);
				out.len+=escSz;
				break;
			default: break; // 1 and 2 go here
		}
		a=a->next;
	}
	*size = out.len;
	return dat;
}

gso gsoParse(char*dat, size_t sz) {
	struct {
		bool iWantMore; // the result of malformed gso? segfault!
		bool inTypeByte;
		_(c) tBType;
		_(c) tBMeta;
		bool big;
	} _ = {
		sz==0,
		true,
		0, 0,
		false
	};
	size_t count = 0;
	gso ret;
	_(handle)*first = NULL;
	while ((count<sz)||(_.iWantMore)) {
		char ch = dat[count];
		if (_.inTypeByte) {
			_.tBType = ch&7;
			_.tBMeta = ch>>3;
			_.inTypeByte = true;
			if (!(first)) first = _(findH)(&ret);
			switch (_.tBType) {
				case 0:
					_.inTypeByte = false;
					_.big = _.tBMeta==0;
					_.iWantMore = true;
					break;
				default: break;
			}
			count++;
			continue;
		}
		switch (_.tBType) {
			case 0:
				
				break;
			default: break;
		}
		count++;
	}
	return ret;
}

__attribute__((destructor)) void _(freeInternals)() {
	free(_(handles).data);
}