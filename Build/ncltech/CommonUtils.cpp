#include "CommonUtils.h"

#include "CommonMeshes.h"
#include "CuboidCollisionShape.h"
#include "SpringConstraint.h"
#include "ObjectMesh.h"
#include "ObjectMeshDragable.h"
#include "PhysicsEngine.h"
#include "SphereCollisionShape.h"

/**
 * @brief Generates a unique colour based on scalar parameter in the range of 0-1.
 *
 * Works off HSV color format, in this case Saturation and Value are 100% and Hue is dependant on 'scalar'
 */
Vector4 CommonUtils::GenColour(float scalar, float alpha)
{
  Vector4 c;
  c.w = alpha;

  float t;
  c.x = abs(modf(scalar + 1.0f, &t) * 6.0f - 3.0f) - 1.0f;
  c.y = abs(modf(scalar + 2.0f / 3.0f, &t) * 6.0f - 3.0f) - 1.0f;
  c.z = abs(modf(scalar + 1.0f / 3.0f, &t) * 6.0f - 3.0f) - 1.0f;

  c.x = min(max(c.x, 0.0f), 1.0f);
  c.y = min(max(c.y, 0.0f), 1.0f);
  c.z = min(max(c.z, 0.0f), 1.0f);

  return c;
}

/**
 * @brief Generates a default sphere object.
 */
Object *CommonUtils::BuildSphereObject(const std::string &name, const Vector3 &pos, float radius, bool physics_enabled,
                                       float inverse_mass, bool collidable, bool dragable, const Vector4 &color)
{
  ObjectMesh *pSphere = dragable ? new ObjectMeshDragable(name) : new ObjectMesh(name);

  pSphere->SetMesh(CommonMeshes::Sphere(), false);
  pSphere->SetTexture(CommonMeshes::CheckerboardTex(), false);
  pSphere->SetLocalTransform(Matrix4::Scale(Vector3(radius, radius, radius)));
  pSphere->SetColour(color);
  pSphere->SetBoundingRadius(radius);

  if (!physics_enabled)
  {
    // If no physics object is present, just set the local transform
    // (modelMatrix) directly
    pSphere->SetLocalTransform(Matrix4::Translation(pos) * pSphere->GetLocalTransform());
  }
  else
  {
    // Otherwise create a physics object, and set it's position etc
    pSphere->CreatePhysicsNode();

    pSphere->Physics()->SetPosition(pos);
    pSphere->Physics()->SetInverseMass(inverse_mass);

    if (!collidable)
    {
      // Even without a collision shape, the inertia matrix for rotation has to
      // be derived from the objects shape
      pSphere->Physics()->SetInverseInertia(SphereCollisionShape(radius).BuildInverseInertia(inverse_mass));
    }
    else
    {
      ICollisionShape *pColshape = new SphereCollisionShape(radius);
      pSphere->Physics()->AddCollisionShape(pColshape);
      pSphere->Physics()->SetInverseInertia(pColshape->BuildInverseInertia(inverse_mass));
      pSphere->Physics()->AutoResizeBoundingBox();
    }
  }

  return pSphere;
}

/**
 * @brief Generates a default cuboid object.
 */
Object *CommonUtils::BuildCuboidObject(const std::string &name, const Vector3 &pos, const Vector3 &halfdims, bool physics_enabled,
                                       float inverse_mass, bool collidable, bool dragable, const Vector4 &color)
{
  ObjectMesh *pCuboid = dragable ? new ObjectMeshDragable(name) : new ObjectMesh(name);

  pCuboid->SetMesh(CommonMeshes::Cube(), false);
  pCuboid->SetTexture(CommonMeshes::CheckerboardTex(), false);
  pCuboid->SetLocalTransform(Matrix4::Scale(halfdims));
  pCuboid->SetColour(color);
  pCuboid->SetBoundingRadius(halfdims.Length());

  if (!physics_enabled)
  {
    // If no physics object is present, just set the local transform
    // (modelMatrix) directly
    pCuboid->SetLocalTransform(Matrix4::Translation(pos) * pCuboid->GetLocalTransform());
  }
  else
  {
    // Otherwise create a physics object, and set it's position etc
    pCuboid->CreatePhysicsNode();
    pCuboid->Physics()->SetPosition(pos);
    pCuboid->Physics()->SetInverseMass(inverse_mass);

    if (!collidable)
    {
      // Even without a collision shape, the inertia matrix for rotation has to
      // be derived from the objects shape
      pCuboid->Physics()->SetInverseInertia(CuboidCollisionShape(halfdims).BuildInverseInertia(inverse_mass));
    }
    else
    {
      ICollisionShape *pColshape = new CuboidCollisionShape(halfdims);
      pCuboid->Physics()->AddCollisionShape(pColshape);
      pCuboid->Physics()->SetInverseInertia(pColshape->BuildInverseInertia(inverse_mass));
      pCuboid->Physics()->AutoResizeBoundingBox();
    }
  }

  return pCuboid;
}

/**
 * @brief Creates a demo soft body mesh.
 * @param position Position of the soft body structure
 * @param xNodeCount Number of nodes in X axis
 * @param yNodeCount Number of nodes in Y axis
 * @param xNodeSpacing Distence between centres of nodes in X axis
 * @param yNodeSpacing Distence between centres of nodes in Y axis
 * @param gravity Point gravity target
 * @param k Spring constant
 * @param d Velocity damping
 * @return Object containing demo
 */
Object *CommonUtils::BuildSoftBodyDemo(Vector3 position, size_t xNodeCount, size_t yNodeCount, float xNodeSpacing,
                                       float yNodeSpacing, PhysicsObject *gravity, float k, float d)
{
  Object *softBody = new Object("soft_body");
  softBody->CreatePhysicsNode();

  float poleLength = (xNodeCount * xNodeSpacing) * 0.5f;

  Object *pole = CommonUtils::BuildCuboidObject("soft_body_pole", position + Vector3(poleLength - 1.0f, 20.0f, 0.0f),
                                                Vector3(poleLength + 2.0f, 0.25f, 0.25f), true, 0.0f, true, false,
                                                CommonUtils::GenColour(0.45f, 0.5f));
  softBody->AddChildObject(pole);
  pole->Physics()->SetGravitationTarget(gravity);

  // Generate soft body cloth mesh
  std::vector<Object *> softBodyNodes;
  softBodyNodes.reserve(xNodeCount * yNodeCount);

  for (size_t i = 0; i < yNodeCount; i++)
  {
    float y = i * yNodeSpacing;

    for (size_t j = 0; j < xNodeCount; j++)
    {
      float x = j * xNodeSpacing;

      Object *node = CommonUtils::BuildSphereObject("soft_body_node", position + Vector3(x, 19.0f - y, 0.0f), 0.5f, true, 10.0f,
                                                    true, true, CommonUtils::GenColour(0.5f, 1.0f));
      softBody->AddChildObject(node);
      softBodyNodes.push_back(node);
      node->Physics()->WakeUp();
      node->Physics()->SetGravitationTarget(gravity);

      // Add constraint to above node
      if (i > 0)
      {
        Object *o = softBodyNodes[softBodyNodes.size() - xNodeCount - 1];
        PhysicsEngine::Instance()->AddConstraint(
            new SpringConstraint(node->Physics(), o->Physics(), node->Physics()->GetPosition(), o->Physics()->GetPosition(), k, d));

        // Add constraint to left above node
        if (j > 0)
        {
          Object *o = softBodyNodes[softBodyNodes.size() - xNodeCount - 2];
          PhysicsEngine::Instance()->AddConstraint(
              new SpringConstraint(node->Physics(), o->Physics(), node->Physics()->GetPosition(), o->Physics()->GetPosition(), k, d));
        }
      }
      // Add constraint to pole
      else
      {
        Vector3 pos = pole->Physics()->GetPosition();
        pos.x = x;
        PhysicsEngine::Instance()->AddConstraint(
            new SpringConstraint(node->Physics(), pole->Physics(), node->Physics()->GetPosition(), pos, k, d));
      }

      // Add constraint to left node
      if (j > 0)
      {
        Object *o = softBodyNodes[softBodyNodes.size() - 2];
        PhysicsEngine::Instance()->AddConstraint(
            new SpringConstraint(node->Physics(), o->Physics(), node->Physics()->GetPosition(), o->Physics()->GetPosition(), k, d));
      }
    }
  }

  return softBody;
}
