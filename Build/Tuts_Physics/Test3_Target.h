#pragma once

#include <ncltech\CommonUtils.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\Scene.h>
#include <ncltech\SceneManager.h>
#include <ncltech\SortAndSweepBroadphase.h>

class Test3_Target : public Scene
{
public:
  Test3_Target(const std::string &friendly_name)
      : Scene(friendly_name)
  {
    m_targetMesh = new OBJMesh(MESHDIR "target.obj");
    m_targetMesh->GenerateNormals();

    GLuint tex = SOIL_load_OGL_texture(TEXTUREDIR "target.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
    m_targetMesh->SetTexture(tex);
  }

  virtual void OnInitializeScene() override
  {
    PhysicsEngine::Instance()->SetBroadphase(new BruteForceBroadphase());
    PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_COLLISIONVOLUMES | DEBUGDRAW_FLAGS_AABB);

    SceneManager::Instance()->GetCamera()->SetPosition(Vector3(-3.0f, 10.0f, 15.0f));
    SceneManager::Instance()->GetCamera()->SetYaw(-10.f);
    SceneManager::Instance()->GetCamera()->SetPitch(-30.f);

    // Create Ground
    this->AddGameObject(CommonUtils::BuildCuboidObject("Ground", Vector3(0.0f, -1.001f, 0.0f), Vector3(20.0f, 1.0f, 20.0f), false,
      0.0f, false, false, Vector4(0.2f, 0.5f, 1.0f, 1.0f)));

    // Create target
    ObjectMesh *target = new ObjectMesh("target");
    target->SetMesh(m_targetMesh, false);
    target->CreatePhysicsNode();
    target->Physics()->SetPosition(Vector3(0.0f, 0.5f, 0.0f));
    target->Physics()->AddCollisionShape(new CuboidCollisionShape(Vector3(0.5f, 0.5f, 1.0f)));
    target->SetBoundingRadius(1.0f);
    target->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    this->AddGameObject(target);
  }

private:
  OBJMesh * m_targetMesh;
};