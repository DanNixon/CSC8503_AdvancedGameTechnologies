#include <CppUnitTest.h>

#include <ncltech/NetSyncStateMachine.h>
#include <ncltech/PubSubBroker.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// clang-format off
TEST_CLASS(NetSyncStateMachineTest)
{
public:
  TEST_METHOD(NetSyncStateMachine_activation)
  {
    PubSubBroker broker;

    // Machine 1
    NetSyncStateMachine m1(broker, "m2_to_m1", "m1_to_m2");

    State * m1s1 = new State("state1", m1.RootState(), &m1);
    State * m1s11 = new State("state1.1", m1s1, &m1);
    State * m1s12 = new State("state1.2", m1s1, &m1);
    State * m1s121 = new State("state1.2.1", m1s12, &m1);
    State * m1s2 = new State("state2", m1.RootState(), &m1);
    State * m1s21 = new State("state2.1", m1s2, &m1);

    // Machine 2
    NetSyncStateMachine m2(broker, "m1_to_m2", "m2_to_m1");

    State * m2s1 = new State("state1", m2.RootState(), &m2);
    State * m2s11 = new State("state1.1", m2s1, &m2);
    State * m2s12 = new State("state1.2", m2s1, &m2);
    State * m2s121 = new State("state1.2.1", m2s12, &m2);
    State * m2s2 = new State("state2", m2.RootState(), &m2);
    State * m2s21 = new State("state2.1", m2s2, &m2);
    
    // Test updating machine 1
    {
      // Activate a state on machine 1
      m1.ActivateState(m1s121);

      // Test active brance on machine 2
      StatePtrList branch = m2.ActiveStateBranch();
      Assert::AreEqual((size_t)3, branch.size());
      Assert::IsTrue(branch[0] == m2s1);
      Assert::IsTrue(branch[1] == m2s12);
      Assert::IsTrue(branch[2] == m2s121);
    }

    // Test updating machine 2
    {
      // Activate a state on machine 2
      m2.ActivateState(m2s21);

      // Test active brance on machine 1
      StatePtrList branch = m1.ActiveStateBranch();
      Assert::AreEqual((size_t)2, branch.size());
      Assert::IsTrue(branch[0] == m1s2);
      Assert::IsTrue(branch[1] == m1s21);
    }
  }
};