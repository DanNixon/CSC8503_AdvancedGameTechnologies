#include "TestDataGenerator.h"

void GenerateTestDataSet1(std::vector<PathNode *> &nodes, std::vector<PathEdge *> &edges)
{
  nodes.push_back(new PathNode(Vector3(0.0f, 0.0f, 0.0f)));
  nodes.push_back(new PathNode(Vector3(1.0f, 0.0f, 0.0f)));
  nodes.push_back(new PathNode(Vector3(2.0f, 0.0f, 0.0f)));
  nodes.push_back(new PathNode(Vector3(0.0f, 1.0f, 0.0f)));
  nodes.push_back(new PathNode(Vector3(1.0f, 1.0f, 0.0f)));
  nodes.push_back(new PathNode(Vector3(2.0f, 1.0f, 0.0f)));
  nodes.push_back(new PathNode(Vector3(0.0f, 2.0f, 0.0f)));
  nodes.push_back(new PathNode(Vector3(1.0f, 2.0f, 0.0f)));
  nodes.push_back(new PathNode(Vector3(2.0f, 2.0f, 0.0f)));

  edges.push_back(new PathEdge(nodes[0], nodes[1]));
  edges.push_back(new PathEdge(nodes[1], nodes[2]));
  edges.push_back(new PathEdge(nodes[3], nodes[4]));
  edges.push_back(new PathEdge(nodes[4], nodes[5]));
  edges.push_back(new PathEdge(nodes[6], nodes[7]));
  edges.push_back(new PathEdge(nodes[7], nodes[8]));
  edges.push_back(new PathEdge(nodes[0], nodes[3]));
  edges.push_back(new PathEdge(nodes[3], nodes[6]));
  edges.push_back(new PathEdge(nodes[1], nodes[4]));
  edges.push_back(new PathEdge(nodes[4], nodes[7]));
  edges.push_back(new PathEdge(nodes[2], nodes[5]));
  edges.push_back(new PathEdge(nodes[5], nodes[8]));
  edges.push_back(new PathEdge(nodes[0], nodes[4]));
  edges.push_back(new PathEdge(nodes[4], nodes[2]));
  edges.push_back(new PathEdge(nodes[3], nodes[1]));
  edges.push_back(new PathEdge(nodes[1], nodes[5]));
  edges.push_back(new PathEdge(nodes[3], nodes[7]));
  edges.push_back(new PathEdge(nodes[7], nodes[5]));
  edges.push_back(new PathEdge(nodes[6], nodes[4]));
  edges.push_back(new PathEdge(nodes[4], nodes[8]));
}