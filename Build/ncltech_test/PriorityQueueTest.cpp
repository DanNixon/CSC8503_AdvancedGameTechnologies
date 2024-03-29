#include "CppUnitTest.h"

#include <ncltech/PathNodePriorityQueue.h>
#include <ncltech/QueueablePathNode.h>

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// clang-format off
TEST_CLASS(PriorityQueueTest)
{
public:
  TEST_METHOD(PriorityQueue_Pred)
  {
    PathNode * n1 = new PathNode();
    PathNode * n2 = new PathNode();

    QueueablePathNode * qn1 = new QueueablePathNode(n1);
    QueueablePathNode * qn2 = new QueueablePathNode(n2);

    qn1->fScore = 10;
    qn2->fScore = 5;

    auto pred = greater_ptr<QueueablePathNode>();

    Assert::IsTrue(pred(qn1, qn2));
    Assert::IsFalse(pred(qn2, qn1));

    delete n1;
    delete n2;

    delete qn1;
    delete qn2;
  }

  TEST_METHOD(PriorityQueue_PriorityOrder)
  {
    PathNode * n1 = new PathNode();
    PathNode * n2 = new PathNode();
    PathNode * n3 = new PathNode();

    QueueablePathNode * qn1 = new QueueablePathNode(n1);
    QueueablePathNode * qn2 = new QueueablePathNode(n2);
    QueueablePathNode * qn3 = new QueueablePathNode(n3);

    qn1->fScore = 10;
    qn2->fScore = 5;
    qn3->fScore = 20;

    PathNodePriorityQueue q;
    q.Push(qn1);
    q.Push(qn2);
    q.Push(qn3);

    Assert::IsTrue(q.Top() == qn2);
    q.Pop();
    Assert::IsTrue(q.Top() == qn1);
    q.Pop();
    Assert::IsTrue(q.Top() == qn3);

    delete n1;
    delete n2;
    delete n3;

    delete qn1;
    delete qn2;
    delete qn3;
  }

  TEST_METHOD(PriorityQueue_ModifyFScore)
  {
    PathNode * n1 = new PathNode();
    PathNode * n2 = new PathNode();
    PathNode * n3 = new PathNode();

    QueueablePathNode * qn1 = new QueueablePathNode(n1);
    QueueablePathNode * qn2 = new QueueablePathNode(n2);
    QueueablePathNode * qn3 = new QueueablePathNode(n3);

    qn1->fScore = 10.0f;
    qn2->fScore = 5.0f;
    qn3->fScore = 20.0f;

    PathNodePriorityQueue q;
    q.Push(qn1);
    q.Push(qn2);
    q.Push(qn3);

    qn3->fScore = 2.0f;
    q.Update();

    Assert::IsTrue(q.Top() == qn3);
    q.Pop();
    Assert::IsTrue(q.Top() == qn2);
    q.Pop();
    Assert::IsTrue(q.Top() == qn1);

    delete n1;
    delete n2;
    delete n3;

    delete qn1;
    delete qn2;
    delete qn3;
  }
};