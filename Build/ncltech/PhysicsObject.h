/******************************************************************************
Class: PhysicsObject
Implements:
Author: Author: Pieran Marris      <p.marris@newcastle.ac.uk> and YOU!
Description: 

This defines all the physical properties of an element in the world, such
as velocity, position, mass etc..

		(\_/)							
		( '_')						
	 /""""""""""""\=========     -----D	
	/"""""""""""""""""""""""\		
....\_@____@____@____@____@_/			

*/ /////////////////////////////////////////////////////////////////////////////
#pragma once

#include "BoundingBox.h"
#include "ICollisionShape.h"
#include <functional>
#include <nclgl\Matrix3.h>
#include <nclgl\Quaternion.h>

class PhysicsEngine;
class Object;

// Callback function called whenever a collision is detected between two objects
// Params:
//	PhysicsObject* this_obj			- The current object class that contains the callback
//	PhysicsObject* colliding_obj	- The object that is colliding with the given object
// Return:
//  True	- The physics engine should process the collision as normal
//	False	- The physics engine should drop the collision pair and not do any further collision resolution/manifold
// generation
//			  > This can be useful for AI to see if a player/agent is inside an area/collision volume
typedef std::function<bool(PhysicsObject *this_obj, PhysicsObject *colliding_obj)> PhysicsCollisionCallback;

class PhysicsObject
{
  friend class PhysicsEngine;

public:
  PhysicsObject();
  virtual ~PhysicsObject();

  //<--------- GETTERS ------------->
  inline bool IsAtRest() const
  {
    return m_atRest;
  }

  inline bool IsAwake() const
  {
    return !m_atRest;
  }

  inline float GetRestVelocityThresholdSquared() const
  {
    return m_restVelocityThresholdSquared;
  }

  inline BoundingBox GetLocalBoundingBox() const
  {
    return m_localBoundingBox;
  }

  BoundingBox GetWorldSpaceAABB() const;

  inline float GetElasticity() const
  {
    return m_elasticity;
  }

  inline float GetFriction() const
  {
    return m_friction;
  }

  inline const Vector3 &GetPosition() const
  {
    return m_position;
  }

  inline const Vector3 &GetLinearVelocity() const
  {
    return m_linearVelocity;
  }

  inline const Vector3 &GetForce() const
  {
    return m_linearForce;
  }

  inline float GetInverseMass() const
  {
    return m_inverseMass;
  }

  inline const Quaternion &GetOrientation() const
  {
    return m_orientation;
  }

  inline const Vector3 &GetAngularVelocity() const
  {
    return m_angularVelocity;
  }

  inline const Vector3 &GetTorque() const
  {
    return m_torque;
  }

  inline const Matrix3 &GetInverseInertia() const
  {
    return m_inverseInertia;
  }

  inline size_t NumCollisionShapes() const
  {
    return m_collisionShapes.size();
  }

  inline std::vector<ICollisionShape *>::const_iterator CollisionShapesBegin() const
  {
    return m_collisionShapes.cbegin();
  }

  inline std::vector<ICollisionShape *>::const_iterator CollisionShapesEnd() const
  {
    return m_collisionShapes.cend();
  }

  inline Object *GetAssociatedObject() const
  {
    return m_parent;
  }

  const Matrix4 &GetWorldSpaceTransform() const; // Built from scratch or returned from cached value

  //<--------- SETTERS ------------->
  inline void SetRestVelocityThreshold(float vel)
  {
    m_restVelocityThresholdSquared = vel * vel;
  }

  inline void SetLocalBoundingBox(const BoundingBox &bb)
  {
    m_localBoundingBox = bb;
    m_localBoundingBox.UpdateHull();
    m_wsAabbInvalidated = true;
  }

  inline void SetGravitationTarget(PhysicsObject *obj)
  {
    m_gravitationTarget = obj;
  }

  inline void SetElasticity(float elasticity)
  {
    m_elasticity = elasticity;
  }

  inline void SetFriction(float friction)
  {
    m_friction = friction;
  }

  inline void SetPosition(const Vector3 &v)
  {
    m_position = v;
    m_wsTransformInvalidated = true;
    m_wsAabbInvalidated = true;
    m_atRest = false;
  }

  inline void SetLinearVelocity(const Vector3 &v)
  {
    m_linearVelocity = v;
  }

  inline void ApplyForce(const Vector3 &force)
  {
    m_linearForce += force;
  }

  inline void ClearForces()
  {
    m_linearForce.ToZero();
  }

  inline void SetForce(const Vector3 &v)
  {
    m_linearForce = v;
  }

  inline void SetInverseMass(const float &v)
  {
    m_inverseMass = v;
  }

  inline void SetOrientation(const Quaternion &v)
  {
    m_orientation = v;
    m_wsTransformInvalidated = true;
    m_atRest = false;
  }

  inline void SetAngularVelocity(const Vector3 &v)
  {
    m_angularVelocity = v;
  }

  inline void SetTorque(const Vector3 &v)
  {
    m_torque = v;
  }

  inline void SetInverseInertia(const Matrix3 &v)
  {
    m_inverseInertia = v;
  }

  inline void AddCollisionShape(ICollisionShape *colShape)
  {
    m_collisionShapes.push_back(colShape);
  }

  // Called automatically when PhysicsObject is created through Object::CreatePhysicsNode()
  inline void SetAssociatedObject(Object *obj)
  {
    m_parent = obj;
  }

  //<---------- CALLBACKS ------------>
  inline void SetOnCollisionCallback(PhysicsCollisionCallback callback)
  {
    m_onCollisionCallback = callback;
  }

  inline bool FireOnCollisionEvent(PhysicsObject *obj_a, PhysicsObject *obj_b)
  {
    bool handleCollision = (m_onCollisionCallback) ? m_onCollisionCallback(obj_a, obj_b) : true;

    // Wake up on collision
    if (handleCollision)
      m_atRest = false;

    return handleCollision;
  }

  void DoAtRestTest();

  /**
   * @brief Clears the at rest flag.
   */
  inline void WakeUp()
  {
    m_atRest = false;
  }

  virtual void DebugDraw(uint64_t flags) const;

protected:
  Object *m_parent; //!< Attached GameObject or NULL if none set

  bool m_atRest;                        //!< Flag indicating if this object is at rest
  float m_restVelocityThresholdSquared; //!< Squared velocity vector magnitude at which the object is deemed to be stationary
  float m_averageSummedVelocity;        //!< Exponential moving average of sum of magnitudes of linear and angular velocity

  PhysicsObject *m_gravitationTarget; //!< Physical object that this object is attracted to through gravity

  mutable bool m_wsTransformInvalidated; //!< Flag indicating if the cached world space transoformation is invalid
  mutable Matrix4 m_wsTransform;         //!< Cached world space transformation matrix

  BoundingBox m_localBoundingBox;   //!< Model orientated bounding box in model space
  mutable bool m_wsAabbInvalidated; //!< Flag indicating if the cached world space transoformed AABB is invalid
  mutable BoundingBox m_wsAabb;     //!< Axis aligned bounding box of this object in world space

  float m_elasticity; //!< Value from 0-1 definiing how much the object bounces off other objects
  float m_friction;   //!< Value from 0-1 defining how much the object can slide off other objects

  Vector3 m_position;       //!< Object position
  Vector3 m_linearVelocity; //!< Linear velcoity
  Vector3 m_linearForce;    //!< Linear force
  float m_inverseMass;      //!< 1 / object mass

  Quaternion m_orientation;  //!< Object orientation
  Vector3 m_angularVelocity; //!< Axis angular velocity
  Vector3 m_torque;          //!< Axis torque
  Matrix3 m_inverseInertia;  //!< Inverse intertia matrix

  std::vector<ICollisionShape *> m_collisionShapes; //!< Collection of collision shapes in this object
  PhysicsCollisionCallback m_onCollisionCallback;   //!< Collision callback
};