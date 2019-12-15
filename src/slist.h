
#ifndef SLIST_H
#define SLIST_H

typedef struct Node NODE;

struct Node
{
    void* data;
    NODE* next;
};

typedef struct List
{
    NODE* firstNode;
    NODE* lastNode;

}LIST;

/******************LISTS*******************/
LIST* new_list(); /*sets the firstNode pointer, and lastNode pointer, like a cleared list.*/

void clear_list(LIST* lst); /*deletes all items, sets first and last to same.. NULL. like new.*/

int foreach_list(LIST* lst, int(*func)(void*)); /* performs some function with node data.  */
                                                      /* iterates through list. */
NODE* find_list(LIST* lst, int (*func)(void*, void*), void* data);

void del_list(LIST* lst); /*clears the list, then frees the data inside, then frees the list.*/

/******************NODES******************/
NODE* new_node(); /*malloc's a new Node, checks for failed allocation, returns pointer.*/

NODE* del_node(LIST* lst, NODE* prev, NODE* removed); /*delete the node after prev.
                                             prev will be firstNode for the first item.*/
NODE* ins_node(LIST* lst, NODE* prev, NODE* added); /*inserts after previous.*/

#endif
