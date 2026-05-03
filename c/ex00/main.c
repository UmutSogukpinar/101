#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum e_status
{
    FAILURE,
    SUCCESS

}   Status;

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

} LinkedList;


typedef struct s_storage_unit
{
    char        *root;
    LinkedList  *path;
    int (*cmp)(void *, void *);

}   StorageUnit;

typedef struct s_node_pair 
{
    node *prev;
    node *curr;

} NodePair;


//  ============================ Utils ============================

/*
 * Compares two string values.
 *
 * This function casts the given void pointers to char pointers
 * and compares them using strcmp.
 *
 * @param a  First string to compare.
 * @param b  Second string to compare.
 *
 * @return
 * - 0 if the strings are equal
 * - A negative value if the first  string is less than the second
 * - A positive value if the first string is greater than the second
 * 
 */
static int  cmp_str(void *a, void *b)
{
    return (strcmp((char *)a, (char *)b));
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
*
*/
static node* find_prev_tail(LinkedList* list)
{
    node* cur = NULL;

    if (!list || list->size < 2) return (NULL);

    cur = list->head;
    while (cur->next != list->tail) cur = cur->next;

    return (cur);
}


/*
* find_pair
*
* Searches the linked list for the first node whose stored value matches
* the given value according to the provided comparison function.
*
* Parameters:
* - list: the linked list to search
* - value: the target value to compare against node contents
* - cmp: comparison function used to test equality
*
* Returns:
* - A NodePair where:
*   - prev points to the node before the match
*   - curr points to the matching node
*
* - If the match is at the head:
*   - prev is NULL
*   - curr points to the head node
*
* - If no match is found:
*   - curr is NULL
*   - prev points to the last visited node
*
* Complexity:
* - O(n), where n is the number of nodes in the list.
*
*/
static NodePair find_pair(LinkedList* list, void* value, int (*cmp)(void *, void *))
{
    NodePair pair = {NULL, list->head};
    
    while (pair.curr != NULL && cmp(pair.curr->value, value) != 0)
    {
        pair.prev = pair.curr;
        pair.curr = pair.curr->next;
    }
    
    return (pair);
}


/*
* detach_node
*
* Detaches the given node from the linked list and returns it
* without freeing its memory.
*
* Parameters:
* - list: the linked list that owns the node
* - prev: the node immediately before cur
* - cur: the node to detach
*
* Returns:
* - The detached node.
*
* Complexity:
* - O(1)
*
*/
static node* detach_node(LinkedList *list, node *prev, node *cur)
{
    prev->next = cur->next;

    if (list->tail == cur) list->tail = prev;

    cur->next = NULL;
    list->size--;

    return (cur);
}


/*
* find_and_extract
*
* Searches the linked list for the first node whose value matches
* the given value and detaches it from the list.
*
* Parameters:
* - list: the linked list to search
* - value: the target value to match
* - cmp: comparison function used to compare node values
*
* Returns:
* - The detached node if a match is found.
* - NULL if no matching node exists.
*
* Complexity:
* - O(n), where n is the number of nodes in the list.
*
*/
static node* find_and_extract(
    LinkedList *list,
    void *value,
    int (*cmp)(void *, void *)
)
{
    node* prev = list->head;
    node* cur = list->head->next;

    while (cur)
    {
        if (cmp(cur->value, value) == 0)
            return (detach_node(list, prev, cur));

        prev = cur;
        cur = cur->next;
    }

    return (NULL);
}

/*
* dup_string
*
* Allocates and returns a heap-allocated copy of the given string.
*
* Returns:
* - A pointer to the duplicated string on success.
* - NULL if src is NULL or memory allocation fails.
*
*/
static char* dup_string(const char *src)
{
    if (!src) return (NULL);

    char* dup = strdup(src);
    if (!dup) return (NULL);

    return (dup);
}

// ================ cleaning utils ================

/*
* del_node
*
* Frees a single node and its stored value.
*
* Parameters:
* - deleted: the node to destroy
* - free_val: function used to free the stored value
*
*/
static void del_node(node* deleted, void (*free_val)(void *))
{
    if (!deleted || !free_val) return ;

    free_val(deleted->value);
    free(deleted);
}

/*
* del_linked_list
*
* Frees all nodes in the linked list and then frees the list structure.
*
* Parameters:
* - list: the linked list to destroy
* - free_val: function used to free each node value
*
*/
static void del_linked_list(LinkedList* list, void (*free_val)(void *))
{
    if (!list || !free_val) return;

    node* current = list->head;
    while (current)
    {
        node* next = current->next;

        del_node(current, free_val);

        current = next;
    }

    free(list);
}

/*
* del_storage_unit
*
* Recursively frees a StorageUnit and all nested child directories.
*
* Parameters:
* - ptr: pointer to the StorageUnit to destroy
*
* Behavior:
* - Frees the root string.
* - Frees the child directory list recursively.
* - Frees the StorageUnit structure itself.
*
*/
static void del_storage_unit(void* ptr)
{
    if (!ptr) return;

    StorageUnit* su = (StorageUnit*) ptr;

    free(su->root);
    del_linked_list(su->path, del_storage_unit);
    free(su);
}

// ============== makeDirectory utils ==============

/*
* create_node
*
* Allocates a new linked-list node and stores the given value in it.
*
* Parameters:
* - value: the value pointer to store inside the node
*
* Returns:
* - A pointer to the newly allocated node on success.
* - NULL on allocation failure.
*
*/
static node *create_node(void *value)
{
    node* new_node = calloc(1, sizeof(node));
    if (!new_node)  return (NULL);

    new_node->value = value;
    new_node->next = NULL;

    return (new_node);
}

/*
* create_storage_unit
*
* Allocates and initializes a new StorageUnit.
*
* Parameters:
* - value: the root name to store in the new storage unit
*
* Returns:
* - A pointer to the newly created StorageUnit on success.
* - NULL on allocation failure.
*
*/
static StorageUnit* create_storage_unit(void* value)
{
    StorageUnit* new_unit = calloc(1, sizeof(StorageUnit));
    if (!new_unit) return (NULL);

    LinkedList* new_list = calloc(1, sizeof(LinkedList));
    if (!new_list)
    {
        free(new_unit);
        return (NULL);
    }

    new_unit->root = value;
    new_unit->path = new_list;
    new_unit->cmp = cmp_str;
    
    return (new_unit);
}

// ============== swapDirectory utils ==============

/*
* cmp_su_and_str
*
* Compares a StorageUnit root name with a raw string.
*
* Parameters:
* - a: pointer to a StorageUnit
* - b: pointer to a null-terminated string
*
* Returns:
* - 0 if the names are equal.
* - A non-zero value otherwise, following strcmp() semantics.
*/
static int cmp_su_and_str(void *a, void *b)
{
    StorageUnit *su = (StorageUnit *)a;
    char *str = (char *)b;

    return (strcmp(su->root, str));
}

/*
* update_tail
*
* Updates the tail pointer after swapping two nodes.
*
* Parameters:
* - list: the linked list whose tail may need to change
* - cur1: the first swapped node
* - cur2: the second swapped node
*
*/
static void update_tail(LinkedList* list, node* cur1, node* cur2) 
{
    if (list->tail == cur1) list->tail = cur2;
    else if (list->tail == cur2) list->tail = cur1;
}

/*
* connect_to_prev
*
* Connects a previous node to a target node.
*
* Parameters:
* - list: the linked list being updated
* - prev: the node before the target position
* - target: the node that should follow prev
*
*/
static void connect_to_prev(LinkedList* list, node *prev, node *target)
{
    if (prev) prev->next = target;
    else list->head = target;
}

/*
* execute_swap
*
* Swaps the positions of two nodes in a singly linked list.
*
* Parameters:
* - list: the linked list containing the nodes
* - p1: pair containing the first target node and its previous node
* - p2: pair containing the second target node and its previous node
*
* Complexity:
* - O(1)
*/
static void execute_swap(LinkedList* list, NodePair p1, NodePair p2) 
{
    // case: p1->next == p2
    if (p1.curr->next == p2.curr)
    {
        connect_to_prev(list, p1.prev, p2.curr);
        p1.curr->next = p2.curr->next;
        p2.curr->next = p1.curr;
    } 
    // case: p2->next == p1
    else if (p2.curr->next == p1.curr) 
    {
        connect_to_prev(list, p2.prev, p1.curr);
        p2.curr->next = p1.curr->next;
        p1.curr->next = p2.curr;
    } 
    // case: default
    else {
        connect_to_prev(list, p1.prev, p2.curr);
        connect_to_prev(list, p2.prev, p1.curr);
        node *temp = p1.curr->next;
        p1.curr->next = p2.curr->next;
        p2.curr->next = temp;
    }
}

// ===================== Display utils =====================

/*
* display_storage_unit
*
* Recursively prints the directory tree rooted at the given storage unit.
*
* Parameters:
* - su: the storage unit to display
* - depth: current nesting depth used for indentation
*
*/
void display_storage_unit(StorageUnit *su, int depth)
{
    if (!su) return ;

    for (int i = 0; i < depth; i++)
    {
        if (i == depth - 1) printf("|-- ");
        else printf("    ");
    }

    if (su->root) printf("%s\n", su->root);
    else printf("NULL\n");
    

    if (su->path && su->path->size > 0)
    {
        node *current = su->path->head;
        while (current != NULL)
        {
            StorageUnit *child_su = (StorageUnit *)current->value;
            display_storage_unit(child_su, depth + 1);
            current = current->next;
        }
    }
}

//  ======================== Add Methods for linkedlist ========================

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
void add_front(LinkedList* l, node* new_node)
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
void add_back(LinkedList* l, node* new_node)
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

//  ======================== Remove Methods for linkedlist ========================


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
node* remove_front(LinkedList* l)
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

node* remove_back(LinkedList* list)
{
    node* removed = NULL;
    node* new_tail = NULL;

    if (!list || list->size == 0) return (NULL);

    removed = list->tail;

    if (list->size == 1)
    {
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        new_tail = find_prev_tail(list);
        new_tail->next = NULL;
        list->tail = new_tail;
    }
    removed->next = NULL;
    --(list->size);

    return (removed);
}


/*
* remove_the_node
*
* Removes and returns the first node whose stored value matches
* the given target value.
*
* Parameters:
* - list: the linked list to modify
* - value: the target value to search for
* - cmp: comparison function used to match node contents
*
* Returns:
* - The removed node on success.
* - NULL if the list is invalid, empty, or no match is found.
*
* Complexity:
* - O(n), where n is the number of nodes in the list.
*/
node* remove_the_node(
    LinkedList *list,
    void *value,
    int (*cmp)(void *, void *)
)
{
    if (list == NULL || cmp == NULL || list->head == NULL) return (NULL);

    if (cmp(list->head->value, value) == 0) return (remove_front(list));

    return (find_and_extract(list, value, cmp));
}

// ====================== Storage Unit Functions ======================

/*
* makeDirectory
*
* Creates a new child directory under the given parent storage unit.
*
* Parameters:
* - su: the parent storage unit
* - dir_name: the name of the child directory to create
*
* Returns:
* - A pointer to the newly created child StorageUnit on success.
* - NULL on failure.
*
*/
StorageUnit* makeDirectory(StorageUnit *su, const char* dir_name)
{
    char*           dir_copy;
    node*           new_node;
    StorageUnit*    new_su;

    if (!su || !dir_name) return (NULL);

    dir_copy = dup_string(dir_name);
    if (!dir_copy) return (NULL);

    new_su = create_storage_unit(dir_copy);
    if (!new_su)
    {
        free(dir_copy);
        return (NULL);
    }
    
    new_node = create_node(new_su);
    if (!new_node)
    {
        del_storage_unit(new_su);
        return (NULL);
    }

    add_back(su->path, new_node);
    return (new_su);
}

/*
* removeDirectory
*
* Removes the first child directory whose name matches dir_name.
*
* Parameters:
* - su: the parent storage unit
* - dir_name: the child directory name to remove
*
* Behavior:
* - Searches su->path for the first matching child directory.
* - Detaches the matching node from the list.
* - Frees the removed StorageUnit recursively.
*
* Note:
* - If the directory is not found, the function does nothing.
* - Removing a directory also removes all of its nested children.
*/
void removeDirectory(StorageUnit *su, const char* dir_name)
{
    if (!su || !su->path || !dir_name || su->path->size == 0) return ;

    node* removed = remove_the_node(su->path, (void *)dir_name, cmp_su_and_str);

    if (!removed) return ;

    del_node(removed, del_storage_unit);
}

/*
* swapDirectory
*
* Swaps the positions of two child directories in the parent's path list.
*
* Parameters:
* - su: the parent storage unit whose child list will be modified
* - dir1: the name of the first child directory
* - dir2: the name of the second child directory
*
* Returns:
* - SUCCESS if the swap succeeds or both names are equal.
* - FAILURE if the parent is invalid or either directory is not found.
*
*/
Status swapDirectory(StorageUnit *su, void *dir1, void *dir2) 
{
    if (!su || !su->path || !su->path->head) return (FAILURE);
    
    if (su->cmp(dir1, dir2) == 0) return (SUCCESS);

    LinkedList* list = su->path;

    NodePair pair1 = find_pair(list, dir1, cmp_su_and_str);
    NodePair pair2 = find_pair(list, dir2, cmp_su_and_str);

    if (!pair1.curr || !pair2.curr) return (FAILURE);

    execute_swap(list, pair1, pair2);
    update_tail(list, pair1.curr, pair2.curr);

    return (SUCCESS);
}

int main(void)
{
    StorageUnit *root;
    StorageUnit *users_su;
    StorageUnit *ahmet_su;

    printf("========== FILE SYSTEM TEST STARTED ==========\n\n");

    root = create_storage_unit(dup_string("C:"));
    if (!root) return (1);

    printf("[1] Creating main directories...\n");

    users_su = makeDirectory(root, "Users");
    if (!users_su)
    {
        del_storage_unit(root);
        return (1);
    }

    if (!makeDirectory(root, "Windows")
        || !makeDirectory(root, "Program Files"))
    {
        del_storage_unit(root);
        return (1);
    }

    ahmet_su = makeDirectory(users_su, "Ahmet");
    if (!ahmet_su)
    {
        del_storage_unit(root);
        return (1);
    }

    if (!makeDirectory(users_su, "Mehmet")
        || !makeDirectory(ahmet_su, "Documents")
        || !makeDirectory(ahmet_su, "Pictures"))
    {
        del_storage_unit(root);
        return (1);
    }

    printf("\n--- Current Directory Structure ---\n");
    display_storage_unit(root, 0);

    printf("\n[2] Swap Test: swapping 'Windows' and 'Users'...\n");
    if (swapDirectory(root, "Windows", "Users") == FAILURE)
        printf("Swap failed.\n");
    display_storage_unit(root, 0);

    printf("\n[3] Remove Test: removing 'Program Files'...\n");
    removeDirectory(root, "Program Files");
    display_storage_unit(root, 0);

    printf("\n[4] Deep Remove Test: removing 'Users' and all nested content...\n");
    removeDirectory(root, "Users");
    display_storage_unit(root, 0);

    printf("\n[5] Cleaning up the entire system from memory...\n");
    del_storage_unit(root);

    printf("\n========== TEST COMPLETED SUCCESSFULLY ==========\n");
    return (0);
}
