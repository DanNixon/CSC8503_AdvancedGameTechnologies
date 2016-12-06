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

    IState * s1 = new IState("state1", m.RootState(), &m);
    IState * s11 = new IState("state1.1", s1, &m);
    IState * s12 = new IState("state1.2", s1, &m);
    IState * s121 = new IState("state1.2.1", s12, &m);
    IState * s2 = new IState("state2", m.RootState(), &m);
    IState * s21 = new IState("state2.1", s2, &m);

    // Test activation
    s12->SetActivation(true);
    Assert::IsTrue(s1 == m.RootState()->ActiveChild());
    Assert::IsTrue(s12 == s1->ActiveChild());
    Assert::IsNull(s12->ActiveChild());

    // Test active branch
    s121->SetActivation(true);
    std::vector<IState *> branch = m.ActiveStateBranch();
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
    IState * s1 = new IState("state1", nullptr, nullptr);
    IState * s12 = new IState("state1.2", s1, nullptr);
    IState * s121 = new IState("state1.2.1", s12, nullptr);

    std::vector<IState *> branch = { s1, s12, s121 };
    std::string branchStr = StateMachine::BranchToString(branch);

    Assert::AreEqual(std::string("state1/state1.2/state1.2.1"), branchStr);
  }
};