#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"

/*
* create_int
*
* Allocates an integer on the heap, stores the given value in it,
* and returns its address.
*
* Returns:
* - A pointer to the allocated integer on success.
* - NULL if memory allocation fails.
*
* Ownership:
* - The caller is responsible for freeing the returned pointer.
*/
static int *create_int(int value)
{
    int *data;

    data = malloc(sizeof(int));

    if (!data) return (NULL);

    *data = value;

    return (data);
}

/*
* create_node
*
* Allocates a new linked list node and stores the given generic value pointer in it.
*
* Behavior:
* - The node does not copy the data pointed to by value.
* - The node only stores the pointer as-is.
* - The next pointer is initialized to NULL.
*
* Returns:
* - A pointer to the newly allocated node on success.
* - NULL if memory allocation fails.
*
* Ownership:
* - The caller is responsible for freeing the returned node.
* - The lifetime of value must be managed separately by the caller.
*/
static node *create_node(void *value)
{
    node *new_node;

    new_node = malloc(sizeof(node));

    if (!new_node) return (NULL);

    new_node->value = value;
    new_node->next = NULL;

    return (new_node);
}

/*
* Prints the contents of the linked list assuming that each node stores
* a pointer to an integer as its value.
*
* Behavior:
* - Prints "List is NULL" if the list pointer itself is NULL.
* - Prints the current size of the list followed by all integer values.
* - Values are displayed in traversal order from head to tail.
*
* Warning:
* - This function assumes that every node->value is a valid int*.
* - Using it with another value type results in undefined behavior.
*/
static void print_int_list(linked_list *list)
{
    node *cur;

    if (!list)
    {
        printf("List is NULL\n");
        return ;
    }
    printf("List(size=%d): ", list->size);

    cur = list->head;
    while (cur)
    {
        printf("%d", *(int *)cur->value);
        if (cur->next) printf(" -> ");
        cur = cur->next;
    }
    printf("\n");
}

/*
* Frees a single node and the heap-allocated data stored in its value field.
*
* Behavior:
* - Does nothing if the given node pointer is NULL.
* - Frees node->value first, then frees the node itself.
*
* Warning:
* - This function assumes node->value points to dynamically allocated memory.
* - Do not use this function if value was not allocated on the heap,
*   or if value ownership belongs elsewhere.
*/
static void free_node_data(node *n)
{
    if (!n) return;

    free(n->value);
    free(n);
}

// Entrypoint of the program
int main(void)
{
    linked_list list;
    node *n1;
    node *n2;
    node *n3;
    node *removed;

    list.head = NULL;
    list.tail = NULL;
    list.size = 0;

    n1 = create_node(create_int(10));
    n2 = create_node(create_int(20));
    n3 = create_node(create_int(30));
    if (!n1 || !n2 || !n3)
    {
        printf("Allocation failed\n");
        free_node_data(n1);
        free_node_data(n2);
        free_node_data(n3);
        return (1);
    }

    printf("Initial state:\n");
    print_int_list(&list);

    add_front(&list, n1);
    printf("After add_front(10):\n");
    print_int_list(&list);

    add_back(&list, n2);
    printf("After add_back(20):\n");
    print_int_list(&list);

    add_front(&list, n3);
    printf("After add_front(30):\n");
    print_int_list(&list);

    removed = remove_front(&list);
    if (removed)
    {
        printf("Removed from front: %d\n", *(int *)removed->value);
        free_node_data(removed);
    }
    print_int_list(&list);

    removed = remove_back(&list);
    if (removed)
    {
        printf("Removed from back: %d\n", *(int *)removed->value);
        free_node_data(removed);
    }
    print_int_list(&list);

    removed = remove_back(&list);
    if (removed)
    {
        printf("Removed from back: %d\n", *(int *)removed->value);
        free_node_data(removed);
    }
    print_int_list(&list);

    return (0);
}