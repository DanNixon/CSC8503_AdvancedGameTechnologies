#pragma once

#include <algorithm>
#include <functional>

#include "QueueablePathNode.h"

/**
 * @struct greater_ptr
 * @brief Used to perform comparison between pointers to objects.
 * @see http://forums.codeguru.com/showthread.php?337599&p=1137606#post1137606
 */
template <typename T> struct greater_ptr : std::binary_function<bool, const T *, const T *>
{
  /**
  * @brief Checks if an item is greater than another.
  * @param a Pointer to first item
  * @param b Pointer to second item
  * @return True if a > b
  */
  const bool operator()(const T *a, const T *b) const
  {
    if (a == nullptr)
      // If b is also 0, then they are equal, hence a is not < than b
      return b != nullptr;
    else if (b == nullptr)
      return false;
    else
      return (*a) > (*b);
  }
};

  /**
   * @class PathNodePriorityQueue
   * @brief Wrapper around a vector that provides a priority queue.
   * @author Dan Nixon
   */
  class PathNodePriorityQueue : public std::vector<QueueablePathNode *>
  {
  public:
    /**
     * @brief Creates a new queue.
     */
    PathNodePriorityQueue()
        : std::vector<QueueablePathNode *>()
        , m_comp()
    {
      std::make_heap(begin(), end(), m_comp);
    }

    /**
     * @brief Adds a new node to the queue.
     * @param item Node to add
     */
    void push(QueueablePathNode *item)
    {
      push_back(item);
      std::push_heap(begin(), end(), m_comp);
    }

    /**
     * @brief Removes the node at the top of the list.
     */
    void pop()
    {
      std::pop_heap(begin(), end(), m_comp);
      pop_back();
    }

    /**
     * @brief Returns the node at the top of the list.
     * @return Highest node
     */
    QueueablePathNode *top() const
    {
      return front();
    }

    /**
     * @brief Searches for a node in the queue.
     * @param item Node to find
     * @return Const iterator to item
     */
    std::vector<QueueablePathNode *>::const_iterator find(QueueablePathNode *item) const
    {
      return std::find(cbegin(), cend(), item);
    }

    /**
     * @brief Updates positions of nodes in the queue to preserve the queue
     *        priority.
     */
    void update()
    {
      std::make_heap(begin(), end(), m_comp);
    }

  private:
    greater_ptr<QueueablePathNode> m_comp; //!< Sorting comparator
  };