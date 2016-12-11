#include <CppUnitTest.h>

#include <ncltech/State.h>
#include <ncltech/StateContainer.h>
#include <ncltech/StateMachine.h>

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// clang-format off
TEST_CLASS(StateTest)
{
public:
  TEST_METHOD(State_Branch)
  {
    State * s1 = new State("state1", nullptr, nullptr);
    State * s12 = new State("state1.2", s1, nullptr);
    State * s121 = new State("state1.2.1", s12, nullptr);

    Assert::IsTrue(StatePtrList{ s1, s12, s121 } == s121->Branch());
    Assert::IsTrue(StatePtrList{ s1, s12, s121 } == s121->Branch(false));
    Assert::IsTrue(StatePtrList{ s121, s12, s1 } == s121->Branch(true));
  }

  TEST_METHOD(State_ClosestCommonAncestor)
  {
    State * s1 = new State("state1", nullptr, nullptr);
    State * s11 = new State("state1.1", s1, nullptr);
    State * s12 = new State("state1.2", s1, nullptr);
    State * s121 = new State("state1.2.1", s12, nullptr);
    State * s122 = new State("state1.2.2", s12, nullptr);

    Assert::IsTrue(s12 == State::ClosestCommonAncestor(s122, s121));
    Assert::IsTrue(s1 == State::ClosestCommonAncestor(s122, s11));
    Assert::IsTrue(s1 == State::ClosestCommonAncestor(s121, s11));
    Assert::IsTrue(s1 == State::ClosestCommonAncestor(s121, s1));
    Assert::IsTrue(s1 == State::ClosestCommonAncestor(s1, s1));
  }

  TEST_METHOD(State_ClosestCommonAncestor_noCommonAncestory)
  {
    State * s1 = new State("state1", nullptr, nullptr);
    State * s11 = new State("state1.1", s1, nullptr);

    State * s2 = new State("state2", nullptr, nullptr);
    State * s22 = new State("state2.2", s2, nullptr);
    State * s221 = new State("state2.2.1", s22, nullptr);
    State * s222 = new State("state2.2.2", s22, nullptr);

    Assert::IsTrue(nullptr == State::ClosestCommonAncestor(s11, s221));
    Assert::IsTrue(nullptr == State::ClosestCommonAncestor(s11, s22));
    Assert::IsTrue(nullptr == State::ClosestCommonAncestor(s22, s1));
  }

  TEST_METHOD(State_Behaviours)
  {
    std::vector<int> data;

    StateMachine m;
    State * state = new State("test_state", m.RootState(), &m);

    // Test behaviours
    state->AddOnEntryBehaviour([&data](State *) {data.push_back(2); });
    state->AddOnEntryBehaviour([&data](State *) {data.push_back(7); });
    state->AddOnExitBehaviour([&data](State *) {data.push_back(9); });
    state->AddOnExitBehaviour([&data](State *) {data.push_back(12); });
    state->AddOnOperateBehaviour([&data]() {data.push_back(99); });
    state->AddOnOperateBehaviour([&data]() {data.push_back(42); });

    // Execute behaviours
    m.ActivateState(state);
    m.Operate(20.0f);
    m.ActivateState(m.RootState());
    
    // Test data modifications
    std::vector<int> expected = {2, 7, 99, 42, 9, 12};
    Assert::IsTrue(expected == data);

    // Test time in state
    Assert::AreEqual(20.0f, state->TimeInState());
  }

  TEST_METHOD(State_TransferToTests)
  {
    StateMachine m;

    State * state1 = new State("test_state_1", m.RootState(), &m);
    State * state2 = new State("test_state_2", m.RootState(), &m);

    m.ActivateState(state1);

    state1->AddTransferFromTest([]() { return nullptr; });
    m.Update(10.0f);
    Assert::IsTrue(m.ActiveStateBranch().back() == state1);

    state1->AddTransferFromTest([state2]() { return state2; });
    m.Update(17.0f);
    Assert::IsTrue(m.ActiveStateBranch().back() == state2);

    state2->AddTransferFromTest([state1]() { return state1; });
    m.Update(42.0f);
    Assert::IsTrue(m.ActiveStateBranch().back() == state1);

    Assert::AreEqual(42.0f, state1->TimeInState());
    Assert::AreEqual(17.0f, state2->TimeInState());
  }

  TEST_METHOD(State_TransferFromTests)
  {
    StateMachine m;

    State * state1 = new State("test_state_1", m.RootState(), &m);
    State * state2 = new State("test_state_2", m.RootState(), &m);

    m.ActivateState(state1);

    m.Update(100.0f);
    Assert::IsTrue(m.ActiveStateBranch().back() == state1);

    state2->AddTransferToTest([]() { return true; });
    m.Update(170.0f);
    Assert::IsTrue(m.ActiveStateBranch().back() == state2);

    state1->AddTransferToTest([]() { return true; });
    m.Update(420.0f);
    Assert::IsTrue(m.ActiveStateBranch().back() == state1);

    Assert::AreEqual(420.0f, state1->TimeInState());
    Assert::AreEqual(170.0f, state2->TimeInState());
  }
};