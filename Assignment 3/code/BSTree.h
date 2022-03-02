/************************************************************************************//*!
 @file    BSTree.h
 @author  Diren D Bharwani, 2002216, diren.dbharwani, diren.dbharwani@digipen.edu
 @date    Mar 3, 2022
 @brief   Contains the interface for the BSTException, and BSTree objects.
 
 Copyright (C) 2022 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the prior written 
 consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#ifndef BSTREE_H
#define BSTREE_H

// Standard Libraries
#include <string>
#include <stdexcept>
// Project Headers
#include "ObjectAllocator.h"

/*-------------------------------------------------------------------------------------*/
/* Type  Definitions                                                                   */
/*-------------------------------------------------------------------------------------*/

/************************************************************************************//*!
 @brief     Exception Class for the AVL/BST classes.
*//*************************************************************************************/
class BSTException : public std::exception
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Type  Definitions                                                               */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      The exception type.
    *//*********************************************************************************/
    enum BST_EXCEPTION
    {
        E_NO_MEMORY
    };

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Constructor for BSTException

    @param      ErrCode
        The error code for the exception.
    @param      Message
        THe description of the exception.
    *//*********************************************************************************/
    BSTException(int ErrCode, const std::string& Message) 
    : error_code_(ErrCode)
    , message_(Message) 
    {};
    /********************************************************************************//*!
    @brief      Destructor for BSTException
    *//*********************************************************************************/
    virtual ~BSTException() {}

    /*---------------------------------------------------------------------------------*/
    /* Getter Functions                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Getter for the error code.

    @return     The error code.
    *//*********************************************************************************/
    virtual inline int          code()  const           { return error_code_; }
    /********************************************************************************//*!
    @brief      Getter for the error message.

    @return     The description of the exception.
    *//*********************************************************************************/
    virtual inline const char*  what()  const throw()   { return message_.c_str(); }

private:
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    int         error_code_;    //!< The code of the exception
    std::string message_;       //!< Readable message text
};

/************************************************************************************//*!
 @brief     Encapsulates a Binary Search Tree.

 @tparam    T
    The type of an element in the Binary Search Tree.
*//*************************************************************************************/
template <typename T>
class BSTree
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Type  Definitions                                                               */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Encapsulates a node in the binary tree.
    *//*********************************************************************************/
    struct BinTreeNode
    {
    public:
        BinTreeNode*    left;               //!< The left child
        BinTreeNode*    right;              //!< The right child
        T               data;               //!< The data
        int             balance_factor;     //!< optional for efficient balancing
        unsigned        count;              //!< nodes in this subtree for efficient indexing

        /****************************************************************************//*!
        @brief      Default Constructor for BinTreeNode.
        *//*****************************************************************************/
        BinTreeNode() 
        : left          (0)
        , right         (0)
        , data          (0)
        , balance_factor(0)
        , count         (1)
        {};
        /****************************************************************************//*!
        @brief      Default Constructor for BinTreeNode.

        @param      value
            The value to construct as the BinTreeNode.
        *//*****************************************************************************/
        BinTreeNode(const T& value) 
        : left          (0)
        , right         (0)
        , data          (value)
        , balance_factor(0)
        , count         (1)
        {};
    };

    /*---------------------------------------------------------------------------------*/
    /* Type  Aliases                                                                   */
    /*---------------------------------------------------------------------------------*/
    using BinTree = BinTreeNode*;

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Default Constructor for a BSTree.

    @param      oa
        A pointer to the object allocator to use for memory management.
    @param      ShareOA
        I don't know what this is for rn.
    *//*********************************************************************************/
    BSTree(ObjectAllocator* oa = nullptr, bool ShareOA = false);
    /********************************************************************************//*!
    @brief      Copy Constructor for a BSTree.

    @param      rhs
        A BSTree to copy data from.
    *//*********************************************************************************/
    BSTree(const BSTree& rhs);
    /********************************************************************************//*!
    @brief      Destructor for a BSTree.
    *//*********************************************************************************/
    virtual ~BSTree();

    /*---------------------------------------------------------------------------------*/
    /* Operator Overloads                                                              */
    /*---------------------------------------------------------------------------------*/
    BSTree&             operator=   (const BSTree& rhs);
    const BinTreeNode*  operator[]  (int index)             const;

    /*---------------------------------------------------------------------------------*/
    /* Getter Functions                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Gets the size of the tree.

    @returns    The number of nodes in the tree starting from the root.
    *//*********************************************************************************/
    unsigned int    size    ()  const;
    /********************************************************************************//*!
    @brief      Gets the height of the tree.

    @returns    The height of the tree starting from the root.
    *//*********************************************************************************/
    int             height  ()  const;
    /********************************************************************************//*!
    @brief      Gets the root node.

    @returns    A pointer to the root node.
    *//*********************************************************************************/
    BinTree         root    ()  const;
    /********************************************************************************//*!
    @brief      Checks if the tree is empty.

    @returns    True if the tree is empty.
    *//*********************************************************************************/
    bool            empty   ()  const;

    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief      Inserts an element into the tree.

    @param      value
        The value of the element to insert into the tree.
    *//*********************************************************************************/
    virtual void    insert  (const T& value);
    /********************************************************************************//*!
    @brief      

    @param      
        
    *//*********************************************************************************/
    virtual void    remove  (const T& value);
    /********************************************************************************//*!
    @brief      
    *//*********************************************************************************/
    void            clear   ();
    /********************************************************************************//*!
    @brief      

    @param      
        
    @param      
        

    @returns    
    *//*********************************************************************************/
    bool            find    (const T& value, unsigned &compares)    const;

  protected:
    /********************************************************************************//*!
    @brief      

    @returns    
    *//*********************************************************************************/
    BinTree&        get_root            ();
    /********************************************************************************//*!
    @brief      

    @param      
        

    @returns    
    *//*********************************************************************************/
    BinTree         make_node           (const T& value)                        const;
    /********************************************************************************//*!
    @brief      

    @param      
        
    *//*********************************************************************************/
    void            free_node           (BinTree node);
    /********************************************************************************//*!
    @brief      

    @param      
        

    @returns
    *//*********************************************************************************/
    int             tree_height         (BinTree tree)                          const;
    /********************************************************************************//*!
    @brief      

    @param      
        
    @param      
        
    *//*********************************************************************************/
    void            find_predecessor    (BinTree tree, BinTree &predecessor)    const;

  private:
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    ObjectAllocator*    oa;
    BinTree             root;
};

// #include "BSTree.cpp"

#endif
//---------------------------------------------------------------------------
