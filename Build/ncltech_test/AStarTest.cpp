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
TEST_CLASS(AStarTest)
{
public:
  TEST_METHOD(AStar_StartAndEndNodeIdentical)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.findPath(nodes[3], nodes[3]));

    // Assert path cost
    Assert::AreEqual(0.0f, pathFinder.pathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.path();
    Assert::AreEqual((size_t)1, path.size());
    Assert::IsTrue(nodes[3] == path[0]);
  }

  TEST_METHOD(AStar_TestPath_1_fwd)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.findPath(nodes[3], nodes[5]));

    // Assert path cost
    Assert::AreEqual(2.0f, pathFinder.pathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.path();
    Assert::AreEqual((size_t)3, path.size());
    Assert::IsTrue(nodes[3] == path[0]);
    Assert::IsTrue(nodes[4] == path[1]);
    Assert::IsTrue(nodes[5] == path[2]);
  }
  
  TEST_METHOD(AStar_TestPath_1_rev)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.findPath(nodes[5], nodes[3]));

    // Assert path cost
    Assert::AreEqual(2.0f, pathFinder.pathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.path();
    Assert::AreEqual((size_t)3, path.size());
    Assert::IsTrue(nodes[5] == path[0]);
    Assert::IsTrue(nodes[4] == path[1]);
    Assert::IsTrue(nodes[3] == path[2]);
  }

  TEST_METHOD(AStar_TestPath_2_fwd)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.findPath(nodes[6], nodes[1]));

    // Assert path cost
    Assert::AreEqual(2.41421f, pathFinder.pathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.path();
    Assert::AreEqual((size_t)3, path.size());
    Assert::IsTrue(nodes[6] == path[0]);
    Assert::IsTrue(nodes[3] == path[1]);
    Assert::IsTrue(nodes[1] == path[2]);
  }
  
  TEST_METHOD(AStar_TestPath_2_rev)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.findPath(nodes[1], nodes[6]));

    // Assert path cost
    Assert::AreEqual(2.41421f, pathFinder.pathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.path();
    Assert::AreEqual((size_t)3, path.size());
    Assert::IsTrue(nodes[1] == path[0]);
    Assert::IsTrue(nodes[4] == path[1]);
    Assert::IsTrue(nodes[6] == path[2]);
  }

  TEST_METHOD(AStar_TestPath_3_fwd)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.findPath(nodes[8], nodes[0]));

    // Assert path cost
    Assert::AreEqual(2.82843f, pathFinder.pathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.path();
    Assert::AreEqual((size_t)3, path.size());
    Assert::IsTrue(nodes[8] == path[0]);
    Assert::IsTrue(nodes[4] == path[1]);
    Assert::IsTrue(nodes[0] == path[2]);
  }
  
  TEST_METHOD(AStar_TestPath_3_rev)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.findPath(nodes[0], nodes[8]));

    // Assert path cost
    Assert::AreEqual(2.82843f, pathFinder.pathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.path();
    Assert::AreEqual((size_t)3, path.size());
    Assert::IsTrue(nodes[0] == path[0]);
    Assert::IsTrue(nodes[4] == path[1]);
    Assert::IsTrue(nodes[8] == path[2]);
  }

  TEST_METHOD(AStar_TestPath_4_fwd)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.findPath(nodes[7], nodes[4]));

    // Assert path cost
    Assert::AreEqual(1.0f, pathFinder.pathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.path();
    Assert::AreEqual((size_t)2, path.size());
    Assert::IsTrue(nodes[7] == path[0]);
    Assert::IsTrue(nodes[4] == path[1]);
  }
  
  TEST_METHOD(AStar_TestPath_4_rev)
  {
    // Load test graph
    std::vector<PathNode *> nodes;
    std::vector<PathEdge *> edges;
    GenerateTestDataSet1(nodes, edges);

    // Run path finding
    AStar pathFinder(nodes);
    Assert::IsTrue(pathFinder.findPath(nodes[4], nodes[7]));

    // Assert path cost
    Assert::AreEqual(1.0f, pathFinder.pathCost(), FP_ACC);

    // Assert path
    std::vector<PathNode *> &path = pathFinder.path();
    Assert::AreEqual((size_t)2, path.size());
    Assert::IsTrue(nodes[4] == path[0]);
    Assert::IsTrue(nodes[7] == path[1]);
  }
};