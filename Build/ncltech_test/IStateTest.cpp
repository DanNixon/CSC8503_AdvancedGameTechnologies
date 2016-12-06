#include <CppUnitTest.h>

#include <ncltech/IState.h>
#include <ncltech/StateContainer.h>
#include <ncltech/StateMachine.h>

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// clang-format off
TEST_CLASS(IStateTest)
{
public:
  TEST_METHOD(IState_Branch)
  {
    IState * s1 = new IState("state1", nullptr, nullptr);
    IState * s12 = new IState("state1.2", s1, nullptr);
    IState * s121 = new IState("state1.2.1", s12, nullptr);

    Assert::IsTrue(IStatePtrList{ s1, s12, s121 } == s121->branch());
    Assert::IsTrue(IStatePtrList{ s1, s12, s121 } == s121->branch(false));
    Assert::IsTrue(IStatePtrList{ s121, s12, s1 } == s121->branch(true));
  }

  TEST_METHOD(IState_ClosestCommonAncestor)
  {
    IState * s1 = new IState("state1", nullptr, nullptr);
    IState * s11 = new IState("state1.1", s1, nullptr);
    IState * s12 = new IState("state1.2", s1, nullptr);
    IState * s121 = new IState("state1.2.1", s12, nullptr);
    IState * s122 = new IState("state1.2.2", s12, nullptr);

    Assert::IsTrue(s12 == IState::ClosestCommonAncestor(s122, s121));
    Assert::IsTrue(s1 == IState::ClosestCommonAncestor(s122, s11));
    Assert::IsTrue(s1 == IState::ClosestCommonAncestor(s121, s11));
    Assert::IsTrue(s1 == IState::ClosestCommonAncestor(s121, s1));
    Assert::IsTrue(s1 == IState::ClosestCommonAncestor(s1, s1));
  }

  TEST_METHOD(IState_ClosestCommonAncestor_noCommonAncestory)
  {
    IState * s1 = new IState("state1", nullptr, nullptr);
    IState * s11 = new IState("state1.1", s1, nullptr);

    IState * s2 = new IState("state2", nullptr, nullptr);
    IState * s22 = new IState("state2.2", s2, nullptr);
    IState * s221 = new IState("state2.2.1", s22, nullptr);
    IState * s222 = new IState("state2.2.2", s22, nullptr);

    Assert::IsTrue(nullptr == IState::ClosestCommonAncestor(s11, s221));
    Assert::IsTrue(nullptr == IState::ClosestCommonAncestor(s11, s22));
    Assert::IsTrue(nullptr == IState::ClosestCommonAncestor(s22, s1));
  }

  TEST_METHOD(IState_Behaviours)
  {
    std::vector<int> data;

    StateMachine m;
    IState * state = new IState("test_state", m.rootState(), &m);

    // Test behaviours
    state->AddOnEntryBehaviour([&data](IState *) {data.push_back(2); });
    state->AddOnEntryBehaviour([&data](IState *) {data.push_back(7); });
    state->AddOnExitBehaviour([&data](IState *) {data.push_back(9); });
    state->AddOnExitBehaviour([&data](IState *) {data.push_back(12); });
    state->AddOnOperateBehaviour([&data]() {data.push_back(99); });
    state->AddOnOperateBehaviour([&data]() {data.push_back(42); });

    // Execute behaviours
    state->setActivation(true);
    m.operate();
    state->setActivation(false);
    
    // Test data modifications
    std::vector<int> expected = {2, 7, 99, 42, 9, 12};
    Assert::IsTrue(expected == data);
  }

  TEST_METHOD(IState_TransferToTests)
  {
    StateMachine m;

    IState * state1 = new IState("test_state_1", m.rootState(), &m);
    IState * state2 = new IState("test_state_2", m.rootState(), &m);

    state1->setActivation(true);

    state1->AddTransferFromTest([]() { return nullptr; });
    m.update();
    Assert::IsTrue(m.activeStateBranch().back() == state1);

    state1->AddTransferFromTest([state2]() { return state2; });
    m.update();
    Assert::IsTrue(m.activeStateBranch().back() == state2);

    state2->AddTransferFromTest([state1]() { return state1; });
    m.update();
    Assert::IsTrue(m.activeStateBranch().back() == state1);
  }

  TEST_METHOD(IState_TransferFromTests)
  {
    StateMachine m;

    IState * state1 = new IState("test_state_1", m.rootState(), &m);
    IState * state2 = new IState("test_state_2", m.rootState(), &m);

    state1->setActivation(true);

    m.update();
    Assert::IsTrue(m.activeStateBranch().back() == state1);

    state2->AddTransferToTest([]() { return true; });
    m.update();
    Assert::IsTrue(m.activeStateBranch().back() == state2);

    state1->AddTransferToTest([]() { return true; });
    m.update();
    Assert::IsTrue(m.activeStateBranch().back() == state1);
  }
};