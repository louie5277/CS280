/************************************************************************************//*!
\file           BList.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           Jan 19, 2022
\brief          Contains the implementation of the BList class.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written 
consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Primary Header
#include "BList.h"
// Standard Libraries
#include <iostream>     // For debugging only
#include <exception>
#include <cstring>

/*-------------------------------------------------------------------------------------*/
/* Constructors & Destructors                                                          */
/*-------------------------------------------------------------------------------------*/
template <typename T, unsigned int Size>
BList<T, Size>::BList()
: head  { nullptr }
, tail  { nullptr }
, stats {}
{
    try
    {
        // Allocate a single node.
        head = tail = new BNode;
        head->count = 0;
        ++stats.NodeCount;
        
        stats.NodeSize    = sizeof(BNode);
        stats.ArraySize   = static_cast<int>(Size);
    }
    catch(const std::bad_alloc&)
    {
        throw BListException{BListException::E_NO_MEMORY, "No physical memory left for allocation!"};
    }
}
template <typename T, unsigned int Size>
BList<T, Size>::BList(const BList<T, Size>& rhs)
: head  { nullptr }
, tail  { nullptr }
, stats {}
{
    try
    {
        int toAlloc = rhs.stats.NodeCount;

        // Allocate the first node
        head = tail = new BNode;
        head->count = rhs.head->count;
        memcpy(head->values, rhs.head->values, static_cast<size_t>(Size));
        ++stats.NodeCount;
        --toAlloc;

        // Allocate remaining nodes and reassign
        while (toAlloc != 0)
        {
            allocateNodeAtBack();
        }

        stats.NodeSize  = rhs.nodesize();
        stats.ItemCount = rhs.stats.ItemCount;
        stats.ArraySize = Size;
    }
    catch(const std::bad_alloc&)
    {
        throw BListException{BListException::E_NO_MEMORY, "No physical memory left for allocation!"};
    }
}
template <typename T, unsigned int Size>
BList<T, Size>::~BList()
{
    clear();
}
/*-------------------------------------------------------------------------------------*/
/* Operator Overloads                                                                  */
/*-------------------------------------------------------------------------------------*/
template <typename T, unsigned int Size>
BList<T, Size>& BList<T, Size>::operator=(const BList<T, Size>& rhs)
{
    // Clear the BList and reallocate to copy data from rhs
    clear();

    // Copy stats over
    try
    {
        int toAlloc = rhs.stats.NodeCount;

        // Allocate the first node
        head = tail = new BNode;
        head->count = rhs.head->count;
        memcpy(head->values, rhs.head->values, static_cast<size_t>(Size));
        ++stats.NodeCount;
        --toAlloc;

        // Allocate remaining nodes and reassign
        while (toAlloc != 0)
        {
            allocateNodeAtBack();
        }

        stats.NodeSize  = rhs.nodesize();
        stats.ItemCount = rhs.stats.ItemCount;
        stats.ArraySize = Size;
    }
    catch(const std::bad_alloc&)
    {
        throw BListException{BListException::E_NO_MEMORY, "No physical memory left for allocation!"};
    }

    return *this;
}
template <typename T, unsigned int Size>
T& BList<T, Size>::operator[](int)
{
    static T t;
    return t;
}
template <typename T, unsigned int Size>
const T& BList<T, Size>::operator[](int) const
{
    static T t;
    return t;
}
/*-------------------------------------------------------------------------------------*/
/* Getter Functions                                                                    */
/*-------------------------------------------------------------------------------------*/
template <typename T, unsigned int Size>
const typename BList<T, Size>::BNode* BList<T, Size>::GetHead() const
{
    return head;
}
template <typename T, unsigned int Size>
BListStats BList<T,Size>::GetStats() const
{
    return stats;
}
template <typename T, unsigned int Size>
size_t BList<T, Size>::size() const
{
    return stats.ItemCount;
}
template <typename T, unsigned int Size>
size_t BList<T, Size>::nodesize(void)
{
    return sizeof(BNode);
}
/*-------------------------------------------------------------------------------------*/
/* Public Function Members                                                             */
/*-------------------------------------------------------------------------------------*/
template <typename T, unsigned int Size>
void BList<T, Size>::push_back(const T& value)
{
    if (tail->count == Size)
    {
        allocateNodeAtBack();
    }

    // Back deals with tail node
    tail->values[tail->count] = value;

    // Increment stats
    ++tail->count;
    ++stats.ItemCount;
}
template <typename T, unsigned int Size>
void BList<T, Size>::push_front(const T& value)
{
    if (head->count == Size)
    {
        allocateNodeInFront();
    }

    // Front deals with head node
    for (int i = head->count; i > 0; --i)
    {
        head->values[i] = head->values[i - 1];
    }
    head->values[0] = value;

    // Increment stats
    ++head->count;
    ++stats.ItemCount;
}
template <typename T, unsigned int Size>
void BList<T, Size>::insert(const T& value)
{
    // If no values initialised, just insert.
    if (stats.ItemCount == 0)
    {
        head->values[0] = value;

        ++head->count;
        ++stats.ItemCount;

        return;
    }

    BNode* current = head;

    while (current)
    {
        BNode* left = current;
        BNode* right = current->next;

        // If head
        if (left == head && value < head->values[0])
        {
            insertAtHead(value);
            return;
        }
        
        // If tail
        if (left == tail)
        {
            insertAtTail(value);
            return;
        }

        // Look for range where value belongs
        if (!inRange(value, left, right))
        {
            current = current->next;
            continue;
        }

        const bool IsLeftFull = isNodeFull(left);
        const bool IsRightFull = isNodeFull(right);
        const State LRState = IsLeftFull ? (IsRightFull ? State::L_FULL_R_FULL : State::L_FULL_R_NFULL)
                                         : (IsRightFull ? State::L_NFULL_R_FULL : State::L_NFULL_R_NFULL);

        switch (LRState)
        {
            // Both left and right are not full
            case State::L_NFULL_R_NFULL:
            {
                insertIntoNode(value, left);
                return;
            }
            // Both left and right are full
            case State::L_FULL_R_FULL:
            {
                splitNode(left);
                right = left->next;
                insertAfterSplit(value, left, right);
                return;
            }
            // Left not full but right is full
            case State::L_NFULL_R_FULL:
            {
                if (inRange(value, right))
                {
                    current = current->next;
                }
                else
                {
                    insertIntoNode(value, left);
                }
                break;
            }
            // Left is full but right is not full
            case State::L_FULL_R_NFULL:
            {
                insertIntoNode(value, right);
                return;
            }
            default: break;
        }

        current = current->next;
    }
}
template <typename T, unsigned int Size>
void BList<T, Size>::remove(int)
{

}
template <typename T, unsigned int Size>
void BList<T, Size>::remove_by_value(const T&)
{

}
template <typename T, unsigned int Size>
int BList<T, Size>::find(const T&) const
{
    return 0;
}
template <typename T, unsigned int Size>
void BList<T, Size>::clear()
{
    while (head)
    {
        BNode* next = head->next;
        delete head;
        head = next;
    }
} 


/*-------------------------------------------------------------------------------------*/
/* Private Function Members                                                            */
/*-------------------------------------------------------------------------------------*/
template <typename T, unsigned int Size>
void BList<T, Size>::allocateNodeInFront()
{
    try
    {
        BNode* temp = head;
        head = new BNode;
        temp->prev = head;
        head->next = temp;

        head->count = 0;
        ++stats.NodeCount;
    }
    catch(const std::bad_alloc&)
    {
        throw BListException{BListException::E_NO_MEMORY, "No physical memory left for allocation!"};
    }
}
template <typename T, unsigned int Size>
void BList<T, Size>::allocateNodeAtBack()
{
    try
    {
        BNode* temp = tail;
        tail = new BNode;
        temp->next = tail;
        tail->prev = temp;

        tail->count = 0;
        ++stats.NodeCount;
    }
    catch(const std::bad_alloc&)
    {
        throw BListException{BListException::E_NO_MEMORY, "No physical memory left for allocation!"};
    }
}
template <typename T, unsigned int Size>
void BList<T, Size>::splitNode(BNode* node)
{
    const unsigned int HalfSize = (Size > 1) ? Size << 1 : 1;

    BNode* newNode = new BNode;

    // Split Data
    node->count = static_cast<int>(HalfSize);

    // Edge case where size is 1, so data cannot be split. 
    // An empty node is created instead.
    if (Size > 1)
    {
        const T* secondHalf = node->values + HalfSize;
        memcpy(newNode->values, secondHalf, static_cast<size_t>(HalfSize));
        newNode->count = static_cast<int>(HalfSize);
    }

    if (node != tail)
    {
        BNode* temp = node->next;
        temp->prev = newNode;
    }
    else
    {
        tail = newNode;
    }

    newNode->next = node->next;
    node->next = newNode;
    newNode->prev = node;

    ++stats.NodeCount;
}
template <typename T, unsigned int Size>
void BList<T, Size>::insertIntoNode(const T& value, BNode* node)
{
    int insertIdx = 0;
    for (; insertIdx < node->count; ++insertIdx)
    {
        if (value < node->values[insertIdx])
            break;
    }

    if (Size > 1)
    {
        node->values[insertIdx + 1] = node->values[insertIdx];
    }
    node->values[insertIdx] = value;
    
    ++node->count;
    ++stats.ItemCount;
}
template <typename T, unsigned int Size>
void BList<T, Size>::insertAtHead(const T& value)
{
    splitNode(head);
    insertAfterSplit(value, head, head->next);
}
template <typename T, unsigned int Size>
void BList<T, Size>::insertAtTail(const T& value)
{
    // Check if tail has space.
    if (!isNodeFull(tail))
    {
        insertIntoNode(value, tail);
        return;
    }

    // Otherwise, split tail node.
    splitNode(tail);
    insertAfterSplit(value, tail->prev, tail);
}
template <typename T, unsigned int Size>
void BList<T, Size>::insertAfterSplit(const T& value, BNode* left, BNode* right)
{
    // Edge case for size of 1
    if (Size > 1)
    {
        insertIntoNode(value, (value < right->values[0]) ? left : right);
    }
    else
    {
        if (value < left->values[0])
        {
            // Swap values
            right->values[0] = left->values[0];
            left->values[0] = value;
        }
        else
        {
            right->values[0] = value;
        }

        ++right->count;
        ++stats.ItemCount;
    }
}
template <typename T, unsigned int Size>
bool BList<T, Size>::isNodeFull(const BNode* node) const
{
    return node->count == static_cast<int>(Size);
}
template <typename T, unsigned int Size>
bool BList<T, Size>::inRange(const T& value, const BNode* node) const
{
    const T& Min = node->values[0];
    const T& Max = node->values[node->count - 1];

    return (!(value < Min) && value < Max) || value == Max;
}
template <typename T, unsigned int Size>
bool BList<T, Size>::inRange(const T& value, const BNode* left, const BNode* right) const
{
    const T& Min = left->values[0];
    const T& Max = right->values[right->count - 1];

    return (!(value < Min) && value < Max) || value == Max;
}