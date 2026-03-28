#include <stdlib.h>
#include <string.h>

#include "storage_unit.h"

/*
* Allocates and returns a copy of the given string.
* Returns NULL if allocation fails or if src is NULL.
*/
static char *dup_string(const char *src)
{
    char* dup;

    if (!src) return (NULL);

    dup = strdup(src);
    if (!dup) return (NULL);

    return (dup);
}

/*
* create_node
* 
* Allocates a new node and stores the given value pointer in it.
* Returns NULL on allocation failure.
*/
static node *create_node(void *value)
{
    node    *new_node;

    new_node = calloc(1, sizeof(node));
    if (!new_node)  return (NULL);

    new_node->value = value;
    new_node->next = NULL;

    return (new_node);
}

/*
* makeDirectory
*
* Appends a new directory name to the end of the path list.
*
* Returns:
* - 1 on success
* - 0 on failure
*
* Behavior:
* - Duplicates the given directory string
* - Wraps it into a new node
* - Adds the node to the end of su->path
*/
Status makeDirectory(StorageUnit *su, const char *dir_name)
{
    char    *dir_copy;
    node    *new_node;

    if (!su || !dir_name) return (FAILURE);

    dir_copy = dup_string(dir_name);
    if (!dir_copy) return (FAILURE);
    new_node = create_node(dir_copy);

    if (!new_node)
    {
        free(dir_copy);
        return (FAILURE);
    }

    add_back(su->path, new_node);
    return (SUCCESS);
}

/*
* Removes the first node whose directory name matches dir_name.
*
* Returns:
* - 1 if a matching directory was found and removed
* - 0 if no match was found or on invalid input
*
* Behavior:
* - Updates head and tail when needed
* - Frees both the stored string and the removed node
*/
Status removeDirectory(StorageUnit *su, const char *dir_name)
{
    node    *removed;

    if (!su || !su->path || !dir_name || su->path->size == 0)
        return (FAILURE);

    removed = remove_the_node(su->path, dir_name, cmp_str);

    if (!removed) return (FAILURE);

    del_node(removed);

    return (SUCCESS);
}

// Forward Declaration
static void swap(void** v1, void** v2);

/*
* Swaps the positions of two directories by swapping their stored value pointers.
*
* Returns:
* - 1 on success
* - 0 if either directory is not found or on invalid input
*
* Notes:
* - If dir1 and dir2 are the same string, the function succeeds immediately.
* - This swaps directory contents, not node links.
*/
Status swapDirectory(StorageUnit *su, const char *dir1, const char *dir2)
{
    if (!su || !dir1 || !dir2) return (FAILURE);
    if (strcmp(dir1, dir2) == 0) return (SUCCESS);

    node* cur = su->path->head;
    node* first = NULL;
    node* sec = NULL;

    while (cur)
    {
        if (!first && strcmp((char *)cur->value, dir1) == 0) first = cur;
        else if (!sec && strcmp((char *)cur->value, dir2) == 0) sec = cur;

        cur = cur->next;
    }

    if (!first || !sec) return (FAILURE);

    swap(&(first->value), &(sec->value));

    return (SUCCESS);
}

static void swap(void** v1, void** v2)
{
    void *tmp;

    tmp = *v1;
    *v1 = v2;
    *v2 = tmp;
}

static int  cmp_str(void *a, void *b)
{
    return (strcmp((char *)a, (char *)b));
}
