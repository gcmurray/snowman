#include <string.h>
#include <stdlib.h>
#include "macro.h"
#include "slist.h"


NODE* del_node(LIST* lst, NODE* prev, NODE* removed)
{ /*cases: deleting items after the firstNode, deleting the lastNode. */
    if (removed != lst->firstNode) /*this is NOT how to delete the first Node.*/
    {
        if (removed->next == NULL) //if removed is the last...
        {
            lst->lastNode = prev;
            lst->lastNode->next = NULL; //the prev is the last node now.
            free(removed->data);        //it points to nothing.
            free(removed);              //clean out the removed.
        }
        else
        {
        prev->next = removed->next; //else, prev points to the removed next.
        free(removed->data);        //clean out the removed.
        free(removed);
        }
    }
    return prev;
}

NODE* ins_node(LIST* lst, NODE* prev, NODE* added)
{
    added->next = prev->next;
    prev->next = added;
    if (added->next == NULL)
        lst->lastNode = added;
    return added;
}

void clear_list(LIST* lst)
{
    NODE* tmp = lst->firstNode;
    while (tmp->next != NULL)
    {
        del_node(lst, tmp, tmp->next);
    }
    
    free(lst->firstNode->data); //just incase something was stored here.
}

int foreach_list(LIST* lst, int (*func)(void*))
{
	int count = 0;
    NODE* tmp = lst->firstNode->next;
        while (tmp != NULL)
        {
			if (func(tmp->data))
			{
				count++;
			}
			
			tmp=tmp->next;
        }
        
    return count;
}

NODE* find_list(LIST* lst, int (*func)(void*,void*), void* data)
{
	NODE* tmp = lst->firstNode->next;
	while (tmp)
	{
		if (func(tmp->data, data) > 0)
		    {
				return tmp;
			}
		tmp = tmp->next;
	}
	
	return NULL;
}

LIST* new_list()
{
    LIST* lst;
    if(!(lst = malloc(sizeof(LIST))))
    {
        return NULL;
    }

    lst->firstNode = new_node();
    if (!(lst->firstNode))
    {
		free(lst);
		return NULL;
	}
	
    lst->lastNode = lst->firstNode; /*there is only one node, the first and the last.*/
                                           /* ~"THERE CAN BE ONLY ONE!"~ */
    return lst;
}

NODE* new_node()
{
    NODE* tmp = NULL; /*what we are going to allocate and return.*/
    
	if (!(tmp = malloc(sizeof(NODE))))
	{
		return NULL;
	}
	tmp->data = NULL;
	tmp->next = NULL;
    
    return tmp;
}//end of function new_node().


