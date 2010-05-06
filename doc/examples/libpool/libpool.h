#ifndef ST_HAVE_POOL_H
#define ST_HAVE_POOL_H

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct pool pool_t;

/* ------ pool_new - create and return a new pool; make a sub-pool of p if p is non-NULL -------- */
	struct pool *pool_new(struct pool *p);

/* ------ pool_free - destroy a pool and free all memory acquired in this pool and subpools - */
	void pool_free(struct pool *pool);

/* ------ pool_alloc - allocate memory inside of pool ------------------------------------------- */
	void *pool_alloc(struct pool *pool, int size);

/* ------ pool_calloc - allocate memory inside of pool and set to 0 ----------------------------- */
	void *pool_calloc(struct pool *pool, int size);

/* ------ pool_strcat - concatenate the strings in ...; all parts must be char*, last be NULL --- */
	char *pool_strcat(struct pool *pool, ...);

/* ------ pool_strdup - duplicate the string within pool ---------------------------------------- */
	char *pool_strdup(struct pool *pool, const char *str);

/* ------ pool_sprintf - sprintf fmt and ... in pool; currently limited to max length of 8192 --- */
	char *pool_sprintf(struct pool *pool, const char *fmt, ...);

	/* duplicate the first n non-null characters beginning at str */
	char *pool_strndup(struct pool *pool, const char *str, int n);

#ifdef __cplusplus
}				/* extern "C" */
#endif
#endif
