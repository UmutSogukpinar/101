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

void add_front(linked_list* head, node* new_node);
void add_back(linked_list* head, node* new_node);

linked_list* remove(linked_list* head);

#endif