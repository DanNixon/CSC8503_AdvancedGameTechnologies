#include "CppUnitTest.h"

#include <ncltech/PathEdge.h>
#include <ncltech/PathNode.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// clang-format off
TEST_CLASS(PathEdgeTest)
{
public:
  TEST_METHOD(Edge_Test_CostWeight)
  {
    PathNode n1(Vector3(10.0f, 0.0f, 0.0f));
    PathNode n2(Vector3(0.0f, 5.0f, 0.0f));
    PathEdge e(&n1, &n2);

    // With default weight = 1
    Assert::AreEqual(1.0f, e.Weight());
    Assert::AreEqual(11.18f, e.StaticCost(), 0.01f);
    Assert::AreEqual(11.18f, e.Cost(), 0.01f);

    e.SetWeight(0.1f);

    // With weight = 0.1
    Assert::AreEqual(0.1f, e.Weight());
    Assert::AreEqual(11.18f, e.StaticCost(), 0.01f);
    Assert::AreEqual(1.118f, e.Cost(), 0.01f);

    e.SetWeight(5.0f);

    // With weight = 5
    Assert::AreEqual(5.0f, e.Weight());
    Assert::AreEqual(11.18f, e.StaticCost(), 0.01f);
    Assert::AreEqual(55.90f, e.Cost(), 0.01f);
  }

  TEST_METHOD(Edge_Equality)
  {
    PathNode n1;
    PathNode n2;
    PathNode n3;

    PathEdge e1(&n1, &n2);
    PathEdge e2(&n1, &n2);
    PathEdge e3(&n2, &n1);
    PathEdge e4(&n1, &n3);

    Assert::IsTrue(e1 == e2);
    Assert::IsTrue(e1 == e3);
    Assert::IsTrue(e2 == e3);
    Assert::IsFalse(e1 != e2);
    Assert::IsFalse(e1 != e3);
    Assert::IsFalse(e2 != e3);

    Assert::IsFalse(e1 == e4);
    Assert::IsTrue(e1 != e4);
  }
};