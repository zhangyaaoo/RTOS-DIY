#define __DLL_C_

#include "dll.h"

#define FirstNode   HeadNode.NextNode
#define LastNode    HeadNode.PrevNode

/** @funcname  NodeInit
  * @brief     Initialize the new node.
  * @param     Node: The pointer to the new node.
  * @retval    None
  */
void NodeInit (Node_t *Node)
{
    Node->NextNode = Node;
    Node->PrevNode = Node;
}

/** @funcname  ListInit
  * @brief     Initialize the double linked list.
  * @param     List: The pointer to the list.
  * @retval    None
  */
void ListInit (List_t * List)
{
    List->FirstNode = &(List->HeadNode);
    List->LastNode = &(List->HeadNode);
    List->NodeCount = 0;
}

/** @funcname  ListCount
  * @brief     Return the amount of node in the double linked list.
  * @param     List: The pointer to the list.
  * @retval    The amount of node.
  */
uint32_t ListCount (List_t * List)
{
    return List->NodeCount;
}


/** @funcname  ListFirst
  * @brief     Return the addr of first node in the double linked list.
  * @param     List: The pointer to the list.
  * @retval    The pointer to the first node.
  * @Note      If the list is empty, return NULL.
  */
Node_t *ListFirst (List_t * List)
{
    Node_t * Node = (Node_t *)0;

    if (List->NodeCount != 0)
    {
        Node = List->FirstNode;
    }
    return  Node;
}

/** @funcname  ListLast
  * @brief     Return the addr of first node in the double linked list.
  * @param     List: The pointer to the list.
  * @retval    The pointer to the last node.
  * @Note      If the list is empty, return NULL.
  */
Node_t *ListLast (List_t *List)
{
    Node_t *Node = (Node_t *)0;

    if (List->NodeCount != 0)
    {
        Node = List->LastNode;
    }
    return  Node;
}


/** @funcname  ListPrev
  * @brief     Return the addr of node ahead of the specified node.
  * @param     List: The pointer to the list.
  * @retval    The pointer to the node we want.
  * @Note      If the list is empty, return NULL.
  */
Node_t *ListPrev (List_t *list, Node_t *Node)
{
    if (Node->PrevNode == Node)
    {
        return (Node_t *)0;
    }
    else
    {
        return Node->PrevNode;
    }
}

/** @funcname  ListNext
  * @brief     Return the addr of node behind of the specified node.
  * @param     List: The pointer to the list.
  * @retval    The pointer to the node we want.
  * @Note      If the list is empty, return NULL.
  */
Node_t *ListNext (List_t *List, Node_t *Node)
{
    if (Node->NextNode == Node)
    {
        return (Node_t *)0;
    }
    else
    {
        return Node->NextNode;
    }
}


/** @funcname  ListRemoveAll
  * @brief     Remove all the node in the specified list.
  * @param     List: The pointer to the list.
  * @retval    None.
  * @Note      None.
  */
void ListRemoveAll (List_t *List)
{
    uint32_t count;
    Node_t *NextNode, *CurrentNode;

    NextNode = List->FirstNode;

    for (count = List->NodeCount; count != 0; count--)
    {

        CurrentNode = NextNode;
        NextNode = NextNode->NextNode;

        CurrentNode->NextNode = CurrentNode;
        CurrentNode->PrevNode = CurrentNode;
    }

    List->FirstNode = &(List->HeadNode);
    List->LastNode = &(List->HeadNode);
    List->NodeCount = 0;
}


/** @funcname  ListInsertHead
  * @brief     Insert a new node to the list head.
  * @param     List: The pointer to the list.
  *            Node: The pointer to the node.
  * @retval    None.
  * @Note      None.
  */
void ListInsertHead(List_t *List, Node_t *Node)
{
    Node->PrevNode = List->FirstNode->PrevNode;
    Node->NextNode = List->FirstNode;

    List->FirstNode->PrevNode = Node;
    List->FirstNode = Node;
    List->NodeCount++;
}

/** @funcname  ListInsertTail
  * @brief     Insert a new node to the list Tail.
  * @param     List: The pointer to the list.
  *            Node: The pointer to the node.
  * @retval    None.
  * @Note      None.
  */
void ListInsertTail(List_t *List, Node_t *Node)
{
    Node->NextNode = &(List->HeadNode);
    Node->PrevNode = List->LastNode;

    List->LastNode->NextNode = Node;
    List->LastNode = Node;
    List->NodeCount++;
}

/** @funcname  ListRemoveHead
  * @brief     Remove node in the list Head.
  * @param     List: The pointer to the list.
  * @retval    The addr of the first node in the list.
  * @Note      If the list is empty, return NULL.
  */
Node_t *ListRemoveHead(List_t *List)
{
    Node_t * Node = (Node_t *)0;

    if( List->NodeCount != 0 )
    {
        Node = List->FirstNode;

        Node->NextNode->PrevNode = &(List->HeadNode);
        List->FirstNode = Node->NextNode;
        List->NodeCount--;

        NodeInit(Node);
    }
    return  Node;
}


/** @funcname  ListRemoveTail
  * @brief     Remove node in the list Tail.
  * @param     List: The pointer to the list.
  * @retval    The addr of the last node in the list.
  * @Note      If the list is empty, return NULL.
  */
Node_t *ListRemoveTail(List_t *List)
{
    Node_t *Node = (Node_t *)0;

    if( List->NodeCount != 0 )
    {
        Node = List->LastNode  ;

        Node->PrevNode->NextNode = &(List->HeadNode);
        List->LastNode = Node->PrevNode;
        List->NodeCount--;

        NodeInit(Node);
    }
    return  Node;
}


/** @funcname  ListInsertAfter
  * @brief     Insert a new node after the specified node in the list.
  * @param     List: The pointer to the list.
  *            NodeAfter: The pointer to the specified node.
  *            NodeToInsert: The pointer to the node inserted.
  * @retval    None.
  * @Note      None.
  */
void ListInsertAfter (List_t *List, Node_t *NodeAfter, Node_t *NodeToInsert)
{
    NodeToInsert->PrevNode = NodeAfter;
    NodeToInsert->NextNode = NodeAfter->NextNode;

    NodeAfter->NextNode->PrevNode = NodeToInsert;
    NodeAfter->NextNode = NodeToInsert;

    List->NodeCount++;
}


/** @funcname  ListRemoveNode
  * @brief     Remove the specified node in the list.
  * @param     List: The pointer to the list.
  *            Node: The pointer to the node.
  * @retval    None.
  * @Note      None.
  */
void ListRemoveNode(List_t *List, Node_t *Node)
{
    Node->PrevNode->NextNode = Node->NextNode;
    Node->NextNode->PrevNode = Node->PrevNode;
    List->NodeCount--;

    //NodeInit(Node);   //不能添加这条语句，因为在  SysTick_Handler  函数中，使用了删除节点的后向指针。
                        //在V8.0版本中，函数会死在  SysTick_Handler  函数中的 for 循环中。就是因为有了这条语句
}

