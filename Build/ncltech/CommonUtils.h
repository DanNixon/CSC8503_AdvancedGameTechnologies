#pragma once

#include "Object.h"

class CommonUtils
{
public:
  static Vector4 GenColour(float scalar, float alpha);

  static Object *BuildSphereObject(const std::string &name, const Vector3 &pos, float radius, bool physics_enabled = false,
                                   float inverse_mass = 0.0f, bool collidable = true, bool dragable = true,
                                   const Vector4 &color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  static Object *BuildCuboidObject(const std::string &name, const Vector3 &pos, const Vector3 &scale,
                                   bool physics_enabled = false, float inverse_mass = 0.0f, bool collidable = true,
                                   bool dragable = true, const Vector4 &color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));

  static Object *BuildSoftBodyDemo(Vector3 position, size_t xNodeCount, size_t yNodeCount, float xNodeSpacing = 2.0f, float yNodeSpacing = 2.0f, PhysicsObject * gravity = nullptr);
};