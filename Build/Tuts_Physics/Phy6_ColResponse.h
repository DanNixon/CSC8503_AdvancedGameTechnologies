/******************************************************************************
Class: Phy5_ColManifolds
Implements:
Author: Pieran Marris <p.marris@newcastle.ac.uk> and YOU!
Description:

This demo scene demonstrating varying elasticity and frictional coefficients
and it's affect in the world. The boxes should hopefully all drift down the 20 
degree slope at different rates and the bouncing balls should all hopefully
bounce at different heights :)

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::: IF YOUR BORED! :::
::::::::::::::::::::::
	1. Using the same method of pre-compution force to apply to the constraint as
	   we used for elasticity_term, could you adapt the distance constraint to
	   form a spring constraint? Hookes law states spring force applied should be
	   equal to F = -kx, where x is the length error of the spring and k describes
	   the spring stiffness.

	2. Why not try exapanding frictional forces into both static and kinematic
	   friction components. This would involve using a higher frictional coefficient
	   when the two objects are at rest and lower frictional coefficient as the
	   relative velocity starts to diverge.

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

		(\_/)
		( '_')
	 /""""""""""""\=========     -----D
	/"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*/ /////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ncltech\BruteForceBroadphase.h>
#include <ncltech\CommonUtils.h>
#include <ncltech\NCLDebug.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\Scene.h>
#include <ncltech\SceneManager.h>

class Phy6_ColResponse : public Scene
{
public:
  Phy6_ColResponse(const std::string &friendly_name)
      : Scene(friendly_name)
  {
  }

  virtual void OnInitializeScene() override
  {
    PhysicsEngine::Instance()->SetPaused(true);

    //SceneManager::Instance()->GetCamera()->SetPosition(Vector3(-3.0f, 10.0f, 15.0f));
    //SceneManager::Instance()->GetCamera()->SetYaw(-10.f);
    //SceneManager::Instance()->GetCamera()->SetPitch(-30.f);

    PhysicsEngine::Instance()->SetBroadphase(new BruteForceBroadphase());

    // Create Ground
    {
      Object *ground = CommonUtils::BuildCuboidObject("Ground", Vector3(0.0f, -1.0f, 0.0f), Vector3(20.0f, 1.0f, 20.0f), true,
                                                      0.0f, false, false, Vector4(0.2f, 0.5f, 1.0f, 1.0f));

      // Plane collision shape
      ICollisionShape *shape = new PlaneCollisionShape(Vector2(20.0f, 20.0f));
      shape->SetLocalTransform(Matrix4::Translation(Vector3(0.0f, 1.0f, 0.0f)) *
                               Matrix4::Rotation(90.0f, Vector3(-1.0f, 0.0f, 0.0f)));
      ground->Physics()->AddCollisionShape(shape);

      ground->Physics()->AutoResizeBoundingBox();
      AddGameObject(ground);
    }

    // ELASTICITY EXAMPLE -> Balls bouncing on a pad
    {
      // Sphere Bounce-Pad
      Object *obj = CommonUtils::BuildCuboidObject("BouncePad", Vector3(-2.5f, 0.0f, 6.0f), Vector3(5.0f, 1.0f, 2.0f), true, 0.0f,
                                                   false, false, Vector4(0.2f, 0.5f, 1.0f, 1.0f));

      // Plane collision shape
      ICollisionShape *shape = new PlaneCollisionShape(Vector2(5.0f, 2.0f));
      shape->SetLocalTransform(Matrix4::Translation(Vector3(0.0f, 1.0f, 0.0f)) *
                               Matrix4::Rotation(90.0f, Vector3(-1.0f, 0.0f, 0.0f)));
      obj->Physics()->AddCollisionShape(shape);

      obj->Physics()->SetFriction(1.0f);
      obj->Physics()->SetElasticity(1.0f);

      obj->Physics()->AutoResizeBoundingBox();
      AddGameObject(obj);

      // Create Bouncing Spheres
      for (int i = 0; i < 5; ++i)
      {
        Vector4 colour = CommonUtils::GenColour(0.7f + i * 0.05f, 1.0f);
        Object *obj =
            CommonUtils::BuildSphereObject("", Vector3(-5.0f + i * 1.25f, 5.5f, 6.0f), 0.5f, true, 1.0f, true, true, colour);
        obj->Physics()->SetFriction(0.1f);
        obj->Physics()->SetElasticity(i * 0.1f + 0.5f);
        AddGameObject(obj);
      }
    }

    // FRICTION EXAMPLE -> Cubes sliding down a ramp
    {
      // Create Ramp
      Object *ramp = CommonUtils::BuildCuboidObject("Ramp", Vector3(4.0f, 3.5f, -5.0f), Vector3(5.0f, 0.5f, 4.0f), true, 0.0f,
                                                    true, false, Vector4(1.0f, 0.7f, 1.0f, 1.0f));
      ramp->Physics()->SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 0.0f, 1.0f), 20.0f));
      ramp->Physics()->SetFriction(1.0f);
      AddGameObject(ramp);

      // Create Cubes to slide down a ramp
      for (int i = 0; i < 5; ++i)
      {
        Vector4 colour = Vector4(i * 0.25f, 0.7f, (2 - i) * 0.25f, 1.0f);
        Object *cube = CommonUtils::BuildCuboidObject("", Vector3(8.0f, 6.0f, -7.0f + i * 1.1f), Vector3(0.5f, 0.5f, 0.5f), true,
                                                      1.f, true, true, colour);
        cube->Physics()->SetFriction((i + 1) * 0.05f); // Ranging from 0.05 - 0.25
        cube->Physics()->SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 0.0f, 1.0f), 200.0f));
        AddGameObject(cube);
      }
    }
  }

  virtual void OnUpdateScene(float dt) override
  {
    Scene::OnUpdateScene(dt);

    uint64_t drawFlags = PhysicsEngine::Instance()->GetDebugDrawFlags();

    NCLDebug::AddStatusEntry(Vector4(1.0f, 0.9f, 0.8f, 1.0f), "Physics:");
    NCLDebug::AddStatusEntry(Vector4(1.0f, 0.9f, 0.8f, 1.0f), "     At rest: %s",
                             PhysicsEngine::Instance()->SimulationIsAtRest() ? "Yes" : "Nope");
    NCLDebug::AddStatusEntry(Vector4(1.0f, 0.9f, 0.8f, 1.0f), "     Draw Collision Volumes : %s (Press C to toggle)",
                             (drawFlags & DEBUGDRAW_FLAGS_COLLISIONVOLUMES) ? "Enabled" : "Disabled");
    NCLDebug::AddStatusEntry(Vector4(1.0f, 0.9f, 0.8f, 1.0f), "     Draw Collision Normals : %s (Press N to toggle)",
                             (drawFlags & DEBUGDRAW_FLAGS_COLLISIONNORMALS) ? "Enabled" : "Disabled");
    NCLDebug::AddStatusEntry(Vector4(1.0f, 0.9f, 0.8f, 1.0f), "     Draw Manifolds : %s (Press M to toggle)",
                             (drawFlags & DEBUGDRAW_FLAGS_MANIFOLD) ? "Enabled" : "Disabled");
    NCLDebug::AddStatusEntry(Vector4(1.0f, 0.9f, 0.8f, 1.0f), "     Draw AABB : %s (Press B to toggle)",
                             (drawFlags & DEBUGDRAW_FLAGS_AABB) ? "Enabled" : "Disabled");

    if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_C))
      drawFlags ^= DEBUGDRAW_FLAGS_COLLISIONVOLUMES;

    if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_N))
      drawFlags ^= DEBUGDRAW_FLAGS_COLLISIONNORMALS;

    if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_M))
      drawFlags ^= DEBUGDRAW_FLAGS_MANIFOLD;

    if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_B))
      drawFlags ^= DEBUGDRAW_FLAGS_AABB;

    PhysicsEngine::Instance()->SetDebugDrawFlags(drawFlags);
  }
};