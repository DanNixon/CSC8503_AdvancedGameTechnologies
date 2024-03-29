/******************************************************************************
Class: Phy4_ColDetection
Implements:
Author: Pieran Marris <p.marris@newcastle.ac.uk> and YOU!
Description:

This demo scene creates a series of collision tests for the physics
engine to solve. The Sphere-Sphere, Sphere-Cuboid and Cuboid-Cuboid
tests should automatically be performed by the physics engine. 

Hopefully the visual collision data, showing penetration depth and collision
normal are correct for all collisions. =]

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::: IF YOUR BORED! :::
::::::::::::::::::::::
	1. Why not try making your own collision shapes?
	   Note: Useful list of moments of inertia:  https://en.wikipedia.org/wiki/List_of_moments_of_inertia

			1. Ellipsoid
			   Based off the sphereCollisionShape, can you adapt it to take in
			   a half-width, half-height and half-depth instead to describe an ellipsoid
			   instead of a perfect sphere? You will need to use the world transform matrix
			   to convert axis-aligned w,h,d into current orientation of the object.

			2. Regular Tetrahedron
			   Based off the cuboidCollisionShape, can you adapt it to form a 
			   regular tetrahedron - where all 4 vertices are 'r' radius away from the 
			   centre of mass.

			3. Why stop?
			   Based off these techniques, you should be able to form any convex collision
			   shape. Why not have a look at what collision shapes are available on other
			   popular physics engines?
	
	2. We can only handle convex collision shapes. This unfortunately is a curse in 
	   all physics engines, it is possible to make a generic concave collision algorithm
	   however just our O(n^3) seperating axis theorem is slow enough so we need to get creative.
	   Why not try having a physics object that can have multiple convex collision shapes
	   that contain a relative offset. This way we could string them together to roughly approximate
	   the concave shape we are trying to display; for instance a tree with an ellipsoid for the leaves
	   and a cylinder for the trunk.
	  
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

		(\_/)
		( '_')
	 /""""""""""""\=========     -----D
	/"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*/ /////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ncltech\AABBCollisionShape.h>
#include <ncltech\BruteForceBroadphase.h>
#include <ncltech\CommonUtils.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\NCLDebug.h>
#include <ncltech\OctreeBroadphase.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\PlaneCollisionShape.h>
#include <ncltech\Scene.h>
#include <ncltech\SceneManager.h>
#include <ncltech\SortAndSweepBroadphase.h>
#include <ncltech\SphereCollisionShape.h>

class Phy4_ColDetection : public Scene
{
public:
  Phy4_ColDetection(const std::string &friendly_name)
      : Scene(friendly_name)
      , m_AccumTime(0.0f)
      , m_Rotating(true)
  {
  }

  float m_AccumTime;
  bool m_Rotating;

  const Vector3 ss_pos = Vector3(-5.5f, 1.5f, -5.0f);
  const Vector3 sc_pos = Vector3(4.5f, 1.5f, -5.0f);
  const Vector3 cc_pos = Vector3(-0.5f, 1.5f, 5.0f);

  virtual void OnInitializeScene() override
  {
    // SceneManager::Instance()->GetCamera()->SetPosition(Vector3(-3.0f, 4.0f, 10.0f));
    // SceneManager::Instance()->GetCamera()->SetPitch(-20.f);

    // PhysicsEngine::Instance()->SetBroadphase(new SortAndSweepBroadphase(Vector3(1.0f, 0.0f, 0.0f)));
    PhysicsEngine::Instance()->SetBroadphase(new OctreeBroadphase(2, 2, new BruteForceBroadphase()));

    PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_COLLISIONNORMALS | DEBUGDRAW_FLAGS_COLLISIONVOLUMES |
                                                 DEBUGDRAW_FLAGS_AABB); // | DEBUGDRAW_FLAGS_BROADPHASE);

    m_AccumTime = 0.0f;
    m_Rotating = true;

    // Create Ground
    {
      Object *ground = CommonUtils::BuildCuboidObject("Ground", Vector3(0.0f, 0.0f, 0.0f), Vector3(20.0f, 1.0f, 20.0f), true,
                                                      0.0f, false, false, Vector4(0.2f, 0.5f, 1.0f, 1.0f));

      // Plane collision shape
      ICollisionShape *shape = new PlaneCollisionShape(Vector2(2.0f, 2.0f));
      shape->SetLocalTransform(Matrix4::Translation(Vector3(0.0f, 1.0f, 0.0f)) *
                               Matrix4::Rotation(90.0f, Vector3(-1.0f, 0.0f, 0.0f)));
      ground->Physics()->AddCollisionShape(shape);

      ground->Physics()->AutoResizeBoundingBox();

      AddGameObject(ground);
    }

    // Create Sphere-Sphere Manifold Test
    {
      Object *sphere = CommonUtils::BuildSphereObject("orbiting_sphere1", ss_pos + Vector3(0.75f, 0.0f, 0.0f), 0.5f, true, 0.0f,
                                                      false, false, CommonUtils::GenColour(0.3f, 0.5f));

      // Testing
      ICollisionShape *shape = new AABBCollisionShape();
      sphere->Physics()->AddCollisionShape(shape);

      AddGameObject(sphere);

      AddGameObject(CommonUtils::BuildSphereObject("orbiting_sphere1.1", ss_pos, 0.5f, true, 0.0f, true, true,
                                                   CommonUtils::GenColour(0.55f, 1.0f)));
    }

    // Create Sphere-Cuboid Manifold Test
    {
      Object *sphere = CommonUtils::BuildSphereObject("orbiting_sphere2", sc_pos + Vector3(0.9f, 0.0f, 0.0f), 0.5f, true, 0.0f,
                                                      true, false, CommonUtils::GenColour(0.3f, 0.5f));
      AddGameObject(sphere);

      // Testing
      (*(sphere->Physics()->CollisionShapesBegin()))->SetLocalTransform(Matrix4::Translation(Vector3(0.5f, 0.0f, 0.0f)));

      // Second collision sphere shape (for testing)
      ICollisionShape *secondSphereCollShape = new SphereCollisionShape(0.5f);
      secondSphereCollShape->SetLocalTransform(Matrix4::Translation(Vector3(-0.25f, 0.0f, 0.0f)));
      sphere->Physics()->AddCollisionShape(secondSphereCollShape);

      // Auto set bounding box
      sphere->Physics()->AutoResizeBoundingBox();

      AddGameObject(CommonUtils::BuildCuboidObject("static_cuboid2.2", sc_pos, Vector3(0.5f, 0.5f, 0.5f), true, 0.0f, true, true,
                                                   CommonUtils::GenColour(0.55f, 1.0f)));
    }

    // Create Cuboid-Cuboid Manifold Test
    {
      Object *cuboid =
          CommonUtils::BuildCuboidObject("rotating_cuboid3", cc_pos + Vector3(0.75f, 0.0f, 0.0f), Vector3(0.5f, 0.5f, 0.5f), true,
                                         0.0f, true, false, CommonUtils::GenColour(0.3f, 0.5f));
      AddGameObject(cuboid);

      cuboid->Physics()->SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 1.0f, 0.0f), 30.0f));

      Object *staticCuboid = CommonUtils::BuildCuboidObject("static_cuboid3.3", cc_pos, Vector3(0.5f, 0.5f, 0.5f), true, 0.0f,
                                                            true, true, CommonUtils::GenColour(0.55f, 1.0f));
      staticCuboid->Physics()->SetRestVelocityThreshold(-1.0f);
      AddGameObject(staticCuboid);
    }
  }

  virtual void OnUpdateScene(float dt) override
  {
    Scene::OnUpdateScene(dt);

    // Update Rotating Objects!
    if (m_Rotating)
    {
      m_AccumTime += dt;

      // Radius (ranges from 0.2-1.3 meters taking 6 seconds to complete cycle)
      float rscalar = (cosf(DegToRad(m_AccumTime * 60.f)) * 0.5f + 0.5f) * 1.1f + 0.2f;

      // Offset (Orbit around origin of radius 'rscalar' taking 30 seconds to
      // complete orbit)
      Vector3 offset = Vector3(cosf(DegToRad(m_AccumTime * 12.f)) * rscalar, 0.0f, sinf(DegToRad(m_AccumTime * 12.f)) * rscalar);

      // Default Colour (not colliding)
      Vector4 col = CommonUtils::GenColour(0.3f, 0.5f);

      Object *orbiting_sphere1 = this->FindGameObject("orbiting_sphere1");
      if (orbiting_sphere1 != NULL)
        orbiting_sphere1->Physics()->SetPosition(ss_pos + offset);

      Object *orbiting_sphere2 = this->FindGameObject("orbiting_sphere2");
      if (orbiting_sphere2 != NULL)
        orbiting_sphere2->Physics()->SetPosition(sc_pos + offset);

      Object *rotating_cuboid1 = this->FindGameObject("rotating_cuboid3");
      if (rotating_cuboid1 != NULL)
      {
        rotating_cuboid1->Physics()->SetPosition(cc_pos + offset);

        // Rotate the cube about y axis
        Quaternion q = rotating_cuboid1->Physics()->GetOrientation();
        rotating_cuboid1->Physics()->SetOrientation(q * Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 1.0f, 0.0f), 1.0f));
      }
    }

    if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_B))
      m_Rotating = !m_Rotating;

    uint64_t drawFlags = PhysicsEngine::Instance()->GetDebugDrawFlags();

    NCLDebug::AddStatusEntry(Vector4(1.0f, 0.9f, 0.8f, 1.0f), "Physics:");
    NCLDebug::AddStatusEntry(Vector4(1.0f, 0.9f, 0.8f, 1.0f), "     Draw Collision Volumes : %s (Press C to toggle)",
                             (drawFlags & DEBUGDRAW_FLAGS_COLLISIONVOLUMES) ? "Enabled" : "Disabled");
    NCLDebug::AddStatusEntry(Vector4(1.0f, 0.9f, 0.8f, 1.0f), "     Draw Collision Normals : %s (Press N to toggle)",
                             (drawFlags & DEBUGDRAW_FLAGS_COLLISIONNORMALS) ? "Enabled" : "Disabled");

    NCLDebug::AddStatusEntry(Vector4(0.5f, 0.9f, 1.0f, 1.0f), "     Broadphase collision pairs: %d",
                             PhysicsEngine::Instance()->NumBroadphaseTests());

    NCLDebug::AddStatusEntry(Vector4(1.0f, 0.9f, 0.8f, 1.0f), "");
    NCLDebug::AddStatusEntry(Vector4(1.0f, 0.9f, 0.8f, 1.0f), "     Animation: %s (Press B to toggle)",
                             (m_Rotating) ? "Enabled" : "Disabled");

    if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_C))
      drawFlags ^= DEBUGDRAW_FLAGS_COLLISIONVOLUMES;

    if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_N))
      drawFlags ^= DEBUGDRAW_FLAGS_COLLISIONNORMALS;

    PhysicsEngine::Instance()->SetDebugDrawFlags(drawFlags);
  }
};