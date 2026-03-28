#include "linked_list.h"
#include "stdlib.h"

/*
* add_front
*
* Inserts the given node at the beginning of the linked list.
*
* Behavior:
* - If the list or node pointer is NULL, the function does nothing.
* - The given node becomes the new head of the list.
* - If the list was empty before insertion, the node also becomes the tail.
*
* Side effects:
* - Updates the head pointer.
* - May update the tail pointer if the list was empty.
* - Increments the list size on success.
*/
void add_front(linked_list* l, node* new_node)
{
    if (!l || !new_node) return ;

    new_node->next = l->head;
    l->head = new_node;

    if (l->size == 0) l->tail = new_node;
    ++(l->size);
}

/*
* add_back
*
* Appends the given node to the end of the linked list.
*
* Behavior:
* - If the list or node pointer is NULL, the function does nothing.
* - If the list is empty, the node becomes both head and tail.
* - Otherwise, the current tail is linked to the new node,
*   and the new node becomes the new tail.
*
* Side effects:
* - Updates the tail pointer.
* - May update the head pointer if the list was empty.
* - Increments the list size on success.
*
* Note:
* - The caller is expected to provide a valid node.
* - Setting new_node->next to NULL before insertion is recommended.
*/
void add_back(linked_list* l, node* new_node)
{
    if (!l || !new_node) return ;

    if (l->size == 0) 
    {
        l->head = new_node;
        l->tail = new_node;
    }
    else l->tail->next = new_node;

    l->tail = new_node;
    ++(l->size);
}

/*
* remove_front
*
* Removes and returns the first node of the linked list.
*
* Returns:
* - The removed head node on success.
* - NULL if the list pointer is NULL or the list is empty.
*
* Behavior:
* - If the list contains a single node, both head and tail are reset to NULL.
* - Otherwise, the head pointer is advanced to the next node.
* - The removed node is detached from the list before being returned.
*
* Side effects:
* - Updates the head pointer.
* - May update the tail pointer if the last node is removed.
* - Decrements the list size on success.
*
* Ownership:
* - The function does not free the removed node.
* - The caller becomes responsible for handling the returned node.
*/
node* remove_front(linked_list* l)
{
    if (!l || l->size == 0) return (NULL);

    node* removed = l->head;

    if (l->size == 1)
    {
        l->head = NULL;
        l->tail = NULL;
    }
    else l->head = removed->next;

    --(l->size);
    removed->next = NULL;

    return (removed);
}

// Forward declarations
static node* find_prev_tail(linked_list* l);

/*
* remove_back
*
* Removes and returns the last node of the linked list.
*
* Returns:
* - The removed tail node on success.
* - NULL if the list pointer is NULL or the list is empty.
*
* Behavior:
* - If the list contains a single node, both head and tail are reset to NULL.
* - Otherwise, the node before the tail is located, detached from the old tail,
*   and promoted to become the new tail.
* - The removed node is detached from the list before being returned.
*
* Side effects:
* - Updates the tail pointer.
* - May update the head pointer if the last node is removed.
* - Decrements the list size on success.
*
* Complexity:
* - O(n), because the previous node of the tail must be found by traversal
*   in a singly linked list.
*
* Ownership:
* - The function does not free the removed node.
* - The caller becomes responsible for handling the returned node.
*/

node* remove_back(linked_list* l)
{
    node* removed = NULL;
    node* new_tail = NULL;

    if (!l || l->size == 0) return (NULL);

    removed = l->tail;

    if (l->size == 1)
    {
        l->head = NULL;
        l->tail = NULL;
    }
    else
    {
        new_tail = find_prev_tail(l);
        new_tail->next = NULL;
        l->tail = new_tail;
    }
    removed->next = NULL;
    --(l->size);

    return (removed);
}

/*
* find_prev_tail
*
* Finds and returns the node immediately before the current tail.
*
* Returns:
* - A pointer to the node preceding the tail.
* - NULL if the list pointer is NULL or the list contains fewer than two nodes.
*
* Behavior:
* - Traverses the list from the head until it reaches the node
*   whose next pointer is the current tail.
*
* Complexity:
* - O(n), since it may traverse most of the list.
*
* Usage:
* - Primarily used by remove_back in a singly linked list,
*   where the previous node of the tail cannot be accessed directly.
*/
static node* find_prev_tail(linked_list* l)
{
    node* cur = NULL;

    if (!l || l->size < 2) return (NULL);

    cur = l->head;
    while (cur->next != l->tail) cur = cur->next;

    return (cur);
}
