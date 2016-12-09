#pragma once

#include <ncltech\BruteForceBroadphase.h>
#include <ncltech\CommonUtils.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\Scene.h>
#include <ncltech\SceneManager.h>

class Test2_SoftBody : public Scene
{
public:
  Test2_SoftBody(const std::string &friendly_name)
      : Scene(friendly_name)
  {
  }

  virtual void OnInitializeScene() override
  {
    SceneManager::Instance()->GetCamera()->SetPosition(Vector3(0.0f, 25.0f, 25.0f));

    PhysicsEngine::Instance()->SetBroadphase(new BruteForceBroadphase());
    PhysicsEngine::Instance()->SetDebugDrawFlags(DEBUGDRAW_FLAGS_CONSTRAINT);

    const float xNodeSpacing = 1.5f;
    const float yNodeSpacing = 2.0f;

    const size_t xNodeCount = 20;
    const size_t yNodeCount = 10;

    float poleLength = (xNodeCount * xNodeSpacing) * 0.5f;

    Object *pole =
        CommonUtils::BuildCuboidObject("", Vector3(poleLength - 1.0f, 20.0f, 0.0f), Vector3(poleLength + 2.0f, 0.25f, 0.25f),
                                       true, 0.0f, true, true, CommonUtils::GenColour(0.45f, 0.5f));
    this->AddGameObject(pole);

    // Generate soft body cloth mesh
    std::vector<Object *> softBodyNodes;
    softBodyNodes.reserve(xNodeCount * yNodeCount);

    for (size_t i = 0; i < yNodeCount; i++)
    {
      float y = i * yNodeSpacing;

      for (size_t j = 0; j < xNodeCount; j++)
      {
        float x = j * xNodeSpacing;

        Object *node = CommonUtils::BuildSphereObject("", Vector3(x, 19.0f - y, 0.0f), 0.5f, true, 10.0f, true, true,
                                                      CommonUtils::GenColour(0.5f, 1.0f));
        this->AddGameObject(node);
        softBodyNodes.push_back(node);
        node->Physics()->WakeUp();

        // Add constraint to above node
        if (i > 0)
        {
          Object *o = softBodyNodes[softBodyNodes.size() - xNodeCount - 1];
          PhysicsEngine::Instance()->AddConstraint(
              new DistanceConstraint(node->Physics(), o->Physics(), node->Physics()->GetPosition(), o->Physics()->GetPosition()));

          // Add constraint to left above node
          if (j > 0)
          {
            Object *o = softBodyNodes[softBodyNodes.size() - xNodeCount - 2];
            PhysicsEngine::Instance()->AddConstraint(new DistanceConstraint(
                node->Physics(), o->Physics(), node->Physics()->GetPosition(), o->Physics()->GetPosition()));
          }
        }
        // Add constraint to pole
        else
        {
          Vector3 pos = pole->Physics()->GetPosition();
          pos.x = x;
          PhysicsEngine::Instance()->AddConstraint(
              new DistanceConstraint(node->Physics(), pole->Physics(), node->Physics()->GetPosition(), pos));
        }

        // Add constraint to left node
        if (j > 0)
        {
          Object *o = softBodyNodes[softBodyNodes.size() - 2];
          PhysicsEngine::Instance()->AddConstraint(
              new DistanceConstraint(node->Physics(), o->Physics(), node->Physics()->GetPosition(), o->Physics()->GetPosition()));
        }

        // TODO: add constraints on diagonal nodes
      }
    }
  }
};