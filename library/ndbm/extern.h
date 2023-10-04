BUFHEAD	*__add_ovflpage(HTAB *, BUFHEAD *);
int	     __addel(HTAB *, BUFHEAD *, const DBT *, const DBT *);
int	     __big_delete(HTAB *, BUFHEAD *);
int	     __big_insert(HTAB *, BUFHEAD *, const DBT *, const DBT *);
int	     __big_keydata(HTAB *, BUFHEAD *, DBT *, DBT *, int);
int	     __big_return(HTAB *, BUFHEAD *, int, DBT *, int);
int	     __big_split(HTAB *, BUFHEAD *, BUFHEAD *, BUFHEAD *, int, uint32_t, SPLIT_RETURN *);
int	     __buf_free(HTAB *, int, int);
void	 __buf_init(HTAB *, int);
uint32_t __call_hash(HTAB *, char *, int);
int	     __delpair(HTAB *, BUFHEAD *, int);
int	     __expand_table(HTAB *);
int	     __find_bigpair(HTAB *, BUFHEAD *, int, char *, int);
uint16_t __find_last_page(HTAB *, BUFHEAD **);
void	 __free_ovflpage(HTAB *, BUFHEAD *);
BUFHEAD	*__get_buf(HTAB *, uint32_t, BUFHEAD *, int);
int	     __get_page(HTAB *, char *, uint32_t, int, int, int);
int	     __ibitmap(HTAB *, int, int, int);
uint32_t __log2(uint32_t);
int	     __put_page(HTAB *, char *, uint32_t, int, int);
void	 __reclaim_buf(HTAB *, BUFHEAD *);
int	     __split_page(HTAB *, uint32_t, uint32_t);

/* Default hash routine. */
extern uint32_t (*__default_hash)(const void *, size_t);

#ifdef HASH_STATISTICS
extern int hash_accesses, hash_collisions, hash_expansions, hash_overflows;
#endif