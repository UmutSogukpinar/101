#ifndef LINKED_LIST_H
# define LINKED_LIST_H

typedef struct s_node
{
    void* value;
    struct s_node* next;

}  node;

typedef struct s_linked_list
{
    node*   head;
    node*   tail;
    int     size;

} linked_list;

void del_node(node* deleted);

void add_back(linked_list* head, node* new_node);
void add_front(linked_list* head, node* new_node);

node* remove_back(linked_list* head);
node* remove_front(linked_list* head);
node* remove_the_node(
    linked_list *list,
    void *value,
    int (*cmp)(void *, void *)
);

#endif