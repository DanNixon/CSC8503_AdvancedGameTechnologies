#include <CppUnitTest.h>

#include <ncltech/StateMachine.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// clang-format off
TEST_CLASS(StateMachineTest)
{
public:
  TEST_METHOD(StateMachine_activation_activeStateBranch)
  {
    StateMachine m;

    State * s1 = new State("state1", m.RootState(), &m);
    State * s11 = new State("state1.1", s1, &m);
    State * s12 = new State("state1.2", s1, &m);
    State * s121 = new State("state1.2.1", s12, &m);
    State * s2 = new State("state2", m.RootState(), &m);
    State * s21 = new State("state2.1", s2, &m);

    // Test activation
    s12->SetActivation(true);
    Assert::IsTrue(s1 == m.RootState()->ActiveChild());
    Assert::IsTrue(s12 == s1->ActiveChild());
    Assert::IsNull(s12->ActiveChild());

    // Test active branch
    s121->SetActivation(true);
    std::vector<State *> branch = m.ActiveStateBranch();
    Assert::AreEqual((size_t) 3, branch.size());
    Assert::IsTrue(s1 == branch[0]);
    Assert::IsTrue(s12 == branch[1]);
    Assert::IsTrue(s121 == branch[2]);

    // Test deactivation
    s121->SetActivation(false);
    Assert::IsNull(m.RootState()->ActiveChild());
    Assert::IsNull(s1->ActiveChild());
    Assert::IsNull(s12->ActiveChild());
  }

  TEST_METHOD(StateMachine_BranchToString)
  {
    State * s1 = new State("state1", nullptr, nullptr);
    State * s12 = new State("state1.2", s1, nullptr);
    State * s121 = new State("state1.2.1", s12, nullptr);

    std::vector<State *> branch = { s1, s12, s121 };
    std::string branchStr = StateMachine::BranchToString(branch);

    Assert::AreEqual(std::string("state1/state1.2/state1.2.1"), branchStr);
  }
};