#include <CppUnitTest.h>

#include <ncltech/NetSyncStateMachine.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// clang-format off
TEST_CLASS(NetSyncStateMachineTest)
{
public:
  TEST_METHOD(NetSyncStateMachine_activation)
  {
    // Machine 1
    NetSyncStateMachine m1;

    State * m1s1 = new State("state1", m1.RootState(), &m1);
    State * m1s11 = new State("state1.1", m1s1, &m1);
    State * m1s12 = new State("state1.2", m1s1, &m1);
    State * m1s121 = new State("state1.2.1", m1s12, &m1);
    State * m1s2 = new State("state2", m1.RootState(), &m1);
    State * m1s21 = new State("state2.1", m1s2, &m1);

    // Machine 2
    NetSyncStateMachine m2;

    State * m2s1 = new State("state1", m2.RootState(), &m2);
    State * m2s11 = new State("state1.1", m2s1, &m2);
    State * m2s12 = new State("state1.2", m2s1, &m2);
    State * m2s121 = new State("state1.2.1", m2s12, &m2);
    State * m2s2 = new State("state2", m2.RootState(), &m2);
    State * m2s21 = new State("state2.1", m2s2, &m2);

    m1s121->SetActivation(true);
    m1s1->SetActivation(true);

    Assert::IsTrue(m1.NeedsNetworkSync());
    Assert::IsTrue(m2.NeedsNetworkSync());

    // Test build packet
    NetSyncStateMachine::StateMachineUpdatePacket pkt;
    m1.BuildNetworkSyncPacket(pkt);
    Assert::IsTrue(PACKET_TYPE_FSM == pkt.type);
    Assert::AreEqual("state1/state1.2/state1.2.1", pkt.nextStateName);
    Assert::IsFalse(m1.NeedsNetworkSync());

    // Test parse packet on second machine
    Assert::IsTrue(m2.ProcessNetworkSyncPacket(pkt));
    StatePtrList m2Branch = m2.ActiveStateBranch();
    Assert::AreEqual((size_t)3, m2Branch.size());
    Assert::IsTrue(m2Branch[0] == m2s1);
    Assert::IsTrue(m2Branch[1] == m2s12);
    Assert::IsTrue(m2Branch[2] == m2s121);
    Assert::IsFalse(m2.NeedsNetworkSync());
  }
};