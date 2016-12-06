#include "AStar.h"

#include "PathEdge.h"

/**
 * @brief Creates a new A* path planner.
 * @param nodes Vector of nodes in graph
 */
AStar::AStar(const std::vector<PathNode *> &nodes)
{
  // Create node data
  for (auto it = nodes.begin(); it != nodes.end(); ++it)
    m_nodeData[*it] = new QueueablePathNode(*it);
}

AStar::~AStar()
{
}

/**
 * @brief Clears open and closed lists and computed path and resets node
 *        data.
 */
void AStar::Reset()
{
  // Clear caches
  m_openList = PathNodePriorityQueue();
  m_closedList.clear();
  m_path.clear();

  // Reset node data
  for (auto it = m_nodeData.begin(); it != m_nodeData.end(); ++it)
  {
    it->second->Parent = nullptr;
    it->second->fScore = std::numeric_limits<float>::max();
    it->second->gScore = std::numeric_limits<float>::max();
  }
}

/**
 * @brief Finds the shortest path between two nodes.
 * @param start Starting node
 * @param end Target node
 * @return True if path finding was successful
 */
bool AStar::FindPath(PathNode *start, PathNode *end)
{
  // Clear caches
  Reset();

  // Add start node to open list
  m_nodeData[start]->gScore = 0.0f;
  m_nodeData[start]->fScore = m_nodeData[start]->node->HeuristicValue(*end);
  m_openList.Push(m_nodeData[start]);

  bool success = false;
  while (!m_openList.empty())
  {
    QueueablePathNode *p = m_openList.Top();

    // Move this node to the closed list
    m_openList.Pop();
    m_closedList.push_back(p);

    // Check if this is the end node
    if (p->node == end)
    {
      success = true;
      break;
    }

    // For each node connected to the next node
    for (size_t i = 0; i < p->node->NumConnections(); i++)
    {
      PathEdge *pq = p->node->Edge(i);

      // Skip an edge that cannot be traversed
      if (!pq->Traversable())
        continue;

      QueueablePathNode *q = m_nodeData[pq->OtherNode(p->node)];

      // Calculate new scores
      float gScore = p->gScore + pq->Cost();
      float fScore = gScore + q->node->HeuristicValue(*end);

      // Search for this node on open and closed lists
      auto closedIt = std::find(m_closedList.begin(), m_closedList.end(), q);
      auto openIt = m_openList.Find(q);

      if (closedIt != m_closedList.end() || openIt != m_openList.end())
      {
        // Check if this path is more efficient that the previous best
        if (q->gScore > gScore)
        {
          q->Parent = p;
          q->gScore = gScore;
          q->fScore = fScore;
          m_openList.Update();
        }
      }
      else
      {
        // Add this path to the open list if it has yet to be considered
        q->Parent = p;
        q->gScore = gScore;
        q->fScore = fScore;
        m_openList.Push(q);
      }
    }
  }

  // If successful then reconstruct the best path
  if (success)
  {
    // Add nodes to path
    QueueablePathNode *n = m_closedList.back();
    while (n)
    {
      m_path.push_back(n->node);
      n = n->Parent;
    }

    // Reverse path to be ordered start to end
    std::reverse(m_path.begin(), m_path.end());
  }

  return success;
}
