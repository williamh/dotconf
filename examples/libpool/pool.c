/* libpool - simple memory management library
 * Copyright (C) 2000,2001 Lukas Schroeder <lukas@azzit.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 *
 */

#define LIBPOOL_VERSION  "0.1.1 - 20 Jun 2001"
static const char libpool_version[] = LIBPOOL_VERSION;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef WIN32
#define vsnprintf _vsnprintf
#else
#include <unistd.h>
#endif

#include "libpool.h"

/*
	Things in struct pool at ->pointers get allocated in a manner
	similar to allocating and updating a singly list list of this type

	struct
	{
		void *next;
		void *data;
	};

	In fact, pool->pointers is nothing else :-)
	This is also true for sub_pools, although i created their own
	type for readability of the code.

	On malloc, the ptr is prepended to the list for performance.

	The first pointer added to the list (say, the last item) is the
	pointer returned when malloc()ing memory for the struct pool itself.

	That's why we dont need to bother free()ing the structure pointer
	separately b/c this is done automagically upon list destruction.

*/

/* memory allocation code */
#undef malloc
#undef free

typedef struct sub_pool sub_pool;
struct pool {
	/* num of bytes allocated with reference to this pool
	   note: this element is not maintained during free's !
	 */
	unsigned long size;

	/* list of allocated pointers */
	void *pointers;

	sub_pool *sub_pools;	/* sub pools */
	struct pool *parent;
};

struct sub_pool {
	void *base;
	struct pool *pool;
};

  /* the size needed at the beginning of ->pointers to
     store the pointer to the next element of the list
   */
#define ALLOC_HDR_SIZE    (sizeof(void *))

/* acquire a new pool */
struct pool *pool_new(struct pool *p)
{
	void *acquired;
	struct pool *pool;
	int size = sizeof(struct pool) + ALLOC_HDR_SIZE;

	acquired = malloc(size);
	memset(acquired, 0, size);

	/* a pool is 'acquired' at the beginning of it's own ->pointers */
	*(void **)acquired = 0;
	pool = (struct pool *)((unsigned long)acquired + ALLOC_HDR_SIZE);
	memset(pool, 0, sizeof(struct pool));

	pool->parent = p;
	pool->sub_pools = 0;
	pool->pointers = acquired;

	if (pool->parent) {
		sub_pool *subpool = pool_calloc(pool->parent, sizeof(sub_pool));
		subpool->pool = pool;
		*(sub_pool **) subpool = pool->parent->sub_pools;
		pool->parent->sub_pools = subpool;
	}

	return pool;
}

/* free a pool and all subpools */
void pool_free(struct pool *pool)
{
	void *n, *c;		/* next and current */

	if (!pool)
		return;		/* somebody fucks around */

	if (pool->parent && pool->parent->sub_pools) {
		sub_pool *s_pool, *ps_pool;	/* previous and current */

		/* find the pointer to pool in the parent's subpool list and it's ancestor */
		for (ps_pool = s_pool = pool->parent->sub_pools;
		     s_pool && s_pool->pool != pool;
		     ps_pool = s_pool, s_pool = s_pool->base) ;

		if (ps_pool == s_pool && !s_pool->base)	/* remove last sub_pool */
			pool->parent->sub_pools = 0;
		else if (ps_pool == s_pool && s_pool->base)	/* replace root node */
			pool->parent->sub_pools = s_pool->base;
		else if (ps_pool && s_pool)	/* remove man-in-the-middle */
			ps_pool->base = s_pool->base;

	}

	/* free up the sub_pools recursively */
	for (c = pool->sub_pools; c; c = n) {
		n = *(void **)c;
		pool_free(((sub_pool *) c)->pool);
	}

	/* free all pointers listed in ->pointers;
	 * the last pointer free'd is the pool itself
	 */
	for (c = pool->pointers; c; c = n) {
		n = *(void **)c;
		free(c);
	}

}

void *pool_alloc(struct pool *pool, int size)
{
	void *ptr;

	if (!pool) {
		fprintf(stderr,
			"Hey, fuckhead, gimme a pool if you want memory!\n");
		exit(1);
	}

	size += ALLOC_HDR_SIZE;
	pool->size += size;
	ptr = malloc(size);
	if (!ptr) {
		fprintf(stderr, "Ouch...out of memory\n");
		exit(1);
	}

	memset(ptr, 0, size);
	*(void **)ptr = pool->pointers;
	pool->pointers = ptr;

	return (void *)((unsigned long)ptr + ALLOC_HDR_SIZE);
}

void *pool_calloc(struct pool *pool, int size)
{
	void *acquired;

	acquired = pool_alloc(pool, size);
	memset(acquired, 0, size);

	return acquired;
}

   /* all '...' MUST be char* */
char *pool_strcat(struct pool *pool, ...)
{
	va_list args;
	int size;
	char *str, *result, *cp;

	va_start(args, pool);
	/* get the length of all string to know how much memory to allocate */
	for (size = 0; (str = va_arg(args, char *));)
		size += strlen(str);
	va_end(args);

	result = pool_alloc(pool, size + 1);

	cp = result;
	*cp = 0;

	/* now start copying every string onto the end of result */
	va_start(args, pool);
	while ((str = va_arg(args, char *))) {
		strcpy(cp, str);
		cp += strlen(str);
	}
	va_end(args);

	return result;
}

char *pool_strdup(struct pool *pool, const char *str)
{
	char *res;

	res = pool_alloc(pool, strlen(str) + 1);
	strcpy(res, str);

	return res;
}

char *pool_strndup(struct pool *pool, const char *str, int n)
{
	char *res;

	res = pool_alloc(pool, n + 1);
	strncpy(res, str, n);
	*(res + n) = 0;

	return res;
}

char *pool_sprintf(struct pool *pool, const char *fmt, ...)
{
	/* FIXME: pool_sprintf is limited to 8192 max length */
	char buffer[8192];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buffer, 8192, fmt, args);
	va_end(args);

	return pool_strdup(pool, buffer);
}
