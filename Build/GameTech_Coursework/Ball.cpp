#include "Ball.h"

#include <ncltech/CommonMeshes.h>
#include <ncltech/NCLDebug.h>
#include <ncltech/SphereCollisionShape.h>

Ball::Ball(float radius, float inverseMass, float lifetime)
    : ObjectMesh("ball")
    , m_lifetime(lifetime)
    , m_dead(false)
    , m_hitTarget(false)
{
  // Setup mesh and physics
  {
    SetMesh(CommonMeshes::Sphere(), false);
    SetTexture(CommonMeshes::CheckerboardTex(), false);
    SetLocalTransform(Matrix4::Scale(Vector3(radius, radius, radius)));
    SetColour(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    SetBoundingRadius(radius);

    CreatePhysicsNode();
    m_pPhysicsObject->SetInverseMass(inverseMass);

    ICollisionShape *shape = new SphereCollisionShape(radius);
    m_pPhysicsObject->AddCollisionShape(shape);
    m_pPhysicsObject->SetInverseInertia(shape->BuildInverseInertia(inverseMass));

    m_pPhysicsObject->AutoResizeBoundingBox();
  }

  // Collision handler
  {
    m_pPhysicsObject->SetOnCollisionCallback([this](PhysicsObject *a, PhysicsObject *b) {
      // Check if the ball hit the target
      if (b->GetAssociatedObject()->GetName() == "target")
        this->m_hitTarget = true;

      return true;
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

    // Die state
    State *die = new State("die", shot, &m_stateMachine);
    die->AddTransferToTest([]() { return true; });
    die->AddOnEntryBehaviour([this](State *) {
      // Set dead flag
      this->m_dead = true;
    });

    // Target hit state
    State *targetHit = new State("target_hit", shot, &m_stateMachine);
    targetHit->AddOnEntryBehaviour([](State *) {
      // TODO
      NCLDebug::Log("Hit Target!");
    });

    // State change from shot to hit target when hit target flag is set
    shot->AddTransferFromTest([this, shot, targetHit]() { return this->m_hitTarget ? targetHit : nullptr; });

    // State change from shot to die after lifetime has elapsed
    shot->AddTransferFromTest([this, shot, die]() { return (shot->TimeInState() > this->m_lifetime) ? die : nullptr; });

    // Set default state
    m_stateMachine.SetDefaultState(idle);
    m_stateMachine.ActivateState(idle);
  }
}

Ball::~Ball()
{
}