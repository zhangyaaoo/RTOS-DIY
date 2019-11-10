#ifndef __DLL_H_
#define __DLL_H_

#ifdef __DLL_C_
#define DLL_EXT
#else
#define DLL_EXT extern
#endif

#include <stdint.h>

/**
  *@Note    链表中的节点类型
  */
typedef struct _Node_t
{
    struct _Node_t *PrevNode;
    struct _Node_t *NextNode;
}Node_t;

/**
  *@Note    链表中的头节点类型
  */
typedef struct _List_t
{
    Node_t      HeadNode;
    uint32_t    NodeCount;
}List_t;

#define OFFSET(Type, Member) (uint32_t)&(((Type *)0)->Member)
#define CONTAINER_OF(Ptr, Type, Member) (Type *)((uint32_t)(Ptr) - OFFSET(Type, Member))

DLL_EXT void     NodeInit           (Node_t *Node);
DLL_EXT void     ListInit           (List_t *List);

DLL_EXT uint32_t ListCount          (List_t *List);
DLL_EXT Node_t  *ListFirst          (List_t *List);
DLL_EXT Node_t  *ListLast           (List_t *List);
DLL_EXT Node_t  *ListPrev           (List_t *list, Node_t *Node);
DLL_EXT Node_t  *ListNext           (List_t *List, Node_t *Node);


DLL_EXT void     ListInsertHead     (List_t *List, Node_t *Node);
DLL_EXT void     ListInsertTail     (List_t *List, Node_t *Node);
DLL_EXT void     ListInsertAfter    (List_t *List, Node_t *NodeAfter, Node_t *NodeToInsert);

DLL_EXT Node_t  *ListRemoveHead     (List_t *List);
DLL_EXT Node_t  *ListRemoveTail     (List_t *List);
DLL_EXT void     ListRemoveNode     (List_t *List, Node_t *Node);
DLL_EXT void     ListRemoveAll      (List_t *List);

#endif /* __DLL_H_ */

