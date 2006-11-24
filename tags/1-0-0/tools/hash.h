/* if a hash is too dense (for example, more than 70% filled)
 * regrow it
 */

/* When an element's deletion causes the table to be too sparse
 * (for example, less than 20% filled), we will cut its
 * size by half (making it about 40% filled).
 */

/* Create an empty hash table, with "ncell" cells.
 * Return a pointer to a structure with the
 * table's information, of NULL in case of
 * failure (e.g. out of free memory).
 */
hash_t* hmalloc(int ncell);

/* Insert the given element, with the given key,
 * to the given hash table. Return '1' on success,
 * '0' if an element with the same key is already
 * found in the table, or '-1' if the table is full.
 */
int hinsert( hash_t* htable, char* key, void* data );

void* hash_remove( hash_t* htable, char* key );

void hfree( hash_t* htable );

hiterator();
hbegin();
hend();

/*
void hset_resize_high_density(struct hash_table* htable, int fill_factor);

Set the fill density of the table after which it will be expanded. The fill factor is a number between 1 and 100.

void hash_set_resize_low_density(struct hash_table* htable, int fill_factor);

Set the fill density of the table below which it will be shrank. The fill factor is a number between 1 and 100.

int hash_get_resize_high_density(struct hash_table* htable);

Get the "high" fill density of the table. The fill factor is a number between 1 and 100.

void hash_get_resize_low_density(struct hash_table* htable);

Get the "low" fill density of the table. The fill factor is a number between 1 and 100. 
*/
