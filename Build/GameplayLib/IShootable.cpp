#include "IShootable.h"

#include <ncltech/CommonMeshes.h>
#include <ncltech/Manifold.h>
#include <ncltech/NCLDebug.h>

#include "Player.h"

/**
 * @brief Creates a new shootable object.
 * @param owner Player that shot the object
 * @param lifetime TIme in seconds the object is expected to stay alive for
 */
IShootable::IShootable(Player *owner, float lifetime)
    : ObjectMesh("shootable")
    , m_owner(owner)
    , m_lifetime(lifetime)
    , m_deltaPoints(0.0f)
    , m_hitTarget(false)
    , m_expired(false)
{
  CreatePhysicsNode();

  // Collision handler
  {
    m_pPhysicsObject->SetOnCollisionManifoldCallback([this](PhysicsObject *a, PhysicsObject *b, Manifold *m) {
      // Check if the ball hit the target
      if (b->GetAssociatedObject() != nullptr && b->GetAssociatedObject()->GetName() == "target")
      {
        Vector3 pointOnTarget;
        auto cps = m->ContactPoints();
        for (auto it = cps.begin(); it != cps.end(); ++it)
          pointOnTarget += it->relPosB;
        pointOnTarget = pointOnTarget / (float)cps.size();

        // TODO
        m_deltaPoints = 100.0f * pointOnTarget.LengthSquared();

        m_hitTarget = true;
      }
    });
  }

  // State machine
  {
    // Idle state
    State *idle = new State("idle", m_stateMachine.RootState(), &m_stateMachine);

    // Shot state
    State *shot = new State("shot", m_stateMachine.RootState(), &m_stateMachine);

    // State change from idel to shot when ball is awake
    shot->AddTransferToTest([this]() { return this->Physics()->IsAwake(); });

    // Expired state
    State *expired = new State("expired", shot, &m_stateMachine);
    expired->AddTransferToTest([]() { return true; });
    expired->AddOnEntryBehaviour([this](State *) {
      // Set dead flag
      m_expired = true;
    });

    // Target hit state
    State *targetHit = new State("target_hit", shot, &m_stateMachine);
    targetHit->AddOnEntryBehaviour([this](State *) {
      NCLDebug::Log("Hit Target!");
      m_owner->GetScoreCounter().DeltaPoints(m_deltaPoints);
    });

    // State change from shot to hit target when hit target flag is set
    shot->AddTransferFromTest([this, shot, targetHit]() { return this->m_hitTarget ? targetHit : nullptr; });

    // State change from shot to expired after lifetime has elapsed
    shot->AddTransferFromTest([this, shot, expired]() -> State * {
      if (shot->TimeInState() > m_lifetime)
      {
        m_owner->GetScoreCounter().DeltaPoints(-20.0f);
        return expired;
      }

      return nullptr;
    });

    // Set default state
    m_stateMachine.SetDefaultState(idle);
    m_stateMachine.ActivateState(idle);
  }
}

IShootable::~IShootable()
{
}

/**
 * @brief Performs final creation steps that must be done after the collision shape has been added.
 * @param inverseMass Inverse mass
 */
void IShootable::PostCreate(float inverseMass)
{
  SetTexture(CommonMeshes::CheckerboardTex(), false);
  SetColour(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

  m_pPhysicsObject->SetInverseMass(inverseMass);
  m_pPhysicsObject->SetElasticity(0.5f);
  m_pPhysicsObject->AutoResizeBoundingBox();
}
