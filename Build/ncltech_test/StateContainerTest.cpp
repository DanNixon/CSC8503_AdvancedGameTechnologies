#include <CppUnitTest.h>

#include <ncltech/State.h>
#include <ncltech/StateContainer.h>

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// clang-format off
TEST_CLASS(StateContainerTest)
{
public:
  TEST_METHOD(StateContainer_hasChildren)
  {
    StateContainer container;
    Assert::IsFalse(container.HasChildren());

    container.AddChild(new State("state1", nullptr, nullptr));
    Assert::IsTrue(container.HasChildren());
  }

  TEST_METHOD(StateContainer_activation)
  {
    StateContainer container;
    State * s1 = new State("state1", nullptr, nullptr);
    State * s11 = new State("state1.1", s1, nullptr);
    State * s12 = new State("state1.2", s1, nullptr);
    State * s121 = new State("state1.2.1", s12, nullptr);
    State * s2 = new State("state2", nullptr, nullptr);
    State * s21 = new State("state2.1", s2, nullptr);
    container.AddChild(s1);
    container.AddChild(s2);

    // Test activation
    s12->SetActivation(true);
    Assert::IsTrue(s12 == s1->ActiveChild());
    Assert::IsNull(s12->ActiveChild());

    // Test deactivation
    s12->SetActivation(false);
    Assert::IsNull(container.ActiveChild());
    Assert::IsNull(s1->ActiveChild());
    Assert::IsNull(s12->ActiveChild());
  }

  TEST_METHOD(StateContainer_findState)
  {
    StateContainer container;
    State * s1 = new State("state1", nullptr, nullptr);
    State * s11 = new State("state1.1", s1, nullptr);
    State * s12 = new State("state1.2", s1, nullptr);
    State * s121 = new State("state1.2.1", s12, nullptr);
    State * s2 = new State("state2", nullptr, nullptr);
    State * s21 = new State("state2.1", s2, nullptr);
    container.AddChild(s1);
    container.AddChild(s2);

    std::vector<State *> branch = container.FindState("state1/state1.2/state1.2.1");

    Assert::AreEqual((size_t) 3, branch.size());
    Assert::IsTrue(s1 == branch[0]);
    Assert::IsTrue(s12 == branch[1]);
    Assert::IsTrue(s121 == branch[2]);
  }
};