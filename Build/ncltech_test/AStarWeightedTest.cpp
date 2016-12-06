#include "CppUnitTest.h"

#include <ncltech/AStar.h>
#include <ncltech/PathEdge.h>
#include <ncltech/PathNode.h>

#include "TestDataGenerator.h"

/**
 * @def FP_ACC
 * @brief Accuracy for floating point comparison.
 */
#define FP_ACC 0.001f

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// clang-format off
TEST_CLASS(AStarWeightedTest)
{
public:
  TEST_METHOD(AStarWeighted_StartAndEndNodeIdentical)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Set weights
    nodes[3]->Edge(2)->SetWeight(12.7f);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.FindPath(nodes[3], nodes[3]));

    // Assert path cost
    Assert::AreEqual(0.0f, pathFinder.PathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.Path();
    Assert::AreEqual((size_t)1, path.size());
    Assert::IsTrue(nodes[3] == path[0]);
  }

  TEST_METHOD(AStarWeighted_TestPath_1_fwd)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Set weights
    edges[2]->SetWeight(15.0f);
    edges[15]->SetWeight(6.0f);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.FindPath(nodes[3], nodes[5]));

    // Assert path cost
    Assert::AreEqual(2.82842f, pathFinder.PathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.Path();
    Assert::AreEqual((size_t)3, path.size());
    Assert::IsTrue(nodes[3] == path[0]);
    Assert::IsTrue(nodes[7] == path[1]);
    Assert::IsTrue(nodes[5] == path[2]);
  }

  TEST_METHOD(AStarWeighted_TestPath_1_rev)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Set weights
    edges[2]->SetWeight(15.0f);
    edges[15]->SetWeight(6.0f);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.FindPath(nodes[5], nodes[3]));

    // Assert path cost
    Assert::AreEqual(2.82842f, pathFinder.PathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.Path();
    Assert::AreEqual((size_t)3, path.size());
    Assert::IsTrue(nodes[5] == path[0]);
    Assert::IsTrue(nodes[7] == path[1]);
    Assert::IsTrue(nodes[3] == path[2]);
  }

  TEST_METHOD(AStarWeighted_TestPath_2_fwd)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Set weights
    edges[7]->SetWeight(10.0f);
    edges[8]->SetWeight(10.0f);
    edges[14]->SetWeight(10.0f);
    edges[18]->SetWeight(10.0f);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.FindPath(nodes[6], nodes[1]));

    // Assert path cost
    Assert::AreEqual(3.82842f, pathFinder.PathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.Path();
    Assert::AreEqual((size_t)4, path.size());
    Assert::IsTrue(nodes[6] == path[0]);
    Assert::IsTrue(nodes[7] == path[1]);
    Assert::IsTrue(nodes[5] == path[2]);
    Assert::IsTrue(nodes[1] == path[3]);
  }

  TEST_METHOD(AStarWeighted_TestPath_2_rev)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Set weights
    edges[7]->SetWeight(10.0f);
    edges[8]->SetWeight(10.0f);
    edges[14]->SetWeight(10.0f);
    edges[18]->SetWeight(10.0f);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.FindPath(nodes[1], nodes[6]));

    // Assert path cost
    Assert::AreEqual(3.82842f, pathFinder.PathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.Path();
    Assert::AreEqual((size_t)4, path.size());
    Assert::IsTrue(nodes[1] == path[0]);
    Assert::IsTrue(nodes[5] == path[1]);
    Assert::IsTrue(nodes[7] == path[2]);
    Assert::IsTrue(nodes[6] == path[3]);
  }

  TEST_METHOD(AStarWeighted_TestPath_3_fwd)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Set weights
    edges[12]->SetWeight(10.0f);
    edges[16]->SetWeight(10.0f);
    edges[19]->SetWeight(10.0f);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.FindPath(nodes[8], nodes[0]));

    // Assert path cost
    Assert::AreEqual(3.41421f, pathFinder.PathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.Path();
    Assert::AreEqual((size_t)4, path.size());
    Assert::IsTrue(nodes[8] == path[0]);
    Assert::IsTrue(nodes[5] == path[1]);
    Assert::IsTrue(nodes[1] == path[2]);
    Assert::IsTrue(nodes[0] == path[3]);
  }

  TEST_METHOD(AStarWeighted_TestPath_3_rev)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Set weights
    edges[12]->SetWeight(10.0f);
    edges[16]->SetWeight(10.0f);
    edges[19]->SetWeight(10.0f);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.FindPath(nodes[0], nodes[8]));

    // Assert path cost
    Assert::AreEqual(3.41421f, pathFinder.PathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.Path();
    Assert::AreEqual((size_t)4, path.size());
    Assert::IsTrue(nodes[0] == path[0]);
    Assert::IsTrue(nodes[1] == path[1]);
    Assert::IsTrue(nodes[5] == path[2]);
    Assert::IsTrue(nodes[8] == path[3]);
  }

  TEST_METHOD(AStarWeighted_TestPath_4_fwd)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Set weights
    edges[9]->SetWeight(10.0f);
    edges[2]->SetWeight(0.1f);
    edges[7]->SetWeight(0.1f);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.FindPath(nodes[7], nodes[4]));

    // Assert path cost
    Assert::AreEqual(1.2f, pathFinder.PathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.Path();
    Assert::AreEqual((size_t)4, path.size());
    Assert::IsTrue(nodes[7] == path[0]);
    Assert::IsTrue(nodes[6] == path[1]);
    Assert::IsTrue(nodes[3] == path[2]);
    Assert::IsTrue(nodes[4] == path[3]);
  }

  TEST_METHOD(AStarWeighted_TestPath_4_rev)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Set weights
    edges[9]->SetWeight(10.0f);
    edges[2]->SetWeight(0.1f);
    edges[7]->SetWeight(0.1f);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.FindPath(nodes[4], nodes[7]));

    // Assert path cost
    Assert::AreEqual(1.2f, pathFinder.PathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.Path();
    Assert::AreEqual((size_t)4, path.size());
    Assert::IsTrue(nodes[4] == path[0]);
    Assert::IsTrue(nodes[3] == path[1]);
    Assert::IsTrue(nodes[6] == path[2]);
    Assert::IsTrue(nodes[7] == path[3]);
  }
};
