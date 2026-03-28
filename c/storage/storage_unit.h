#ifndef STORAGE_UNIT_H
# define STORAGE_UNIT_H

# include "../linkedList/linked_list.h"

typedef enum e_status
{
    FAILURE,
    SUCCESS

}   Status;

typedef struct s_storage_unit
{
    char        *root;
    linked_list *path;

}   StorageUnit;

Status    makeDirectory(StorageUnit *su, const char *dir_name);
Status    removeDirectory(StorageUnit *su, const char *dir_name);
Status    swapDirectory(StorageUnit *su, char *dir1, char *dir2);

#endif