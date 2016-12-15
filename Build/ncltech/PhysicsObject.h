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
class Manifold;

/**
 * @class PhysicsObject
 * @brief Physical object in the simulation.
 */
class PhysicsObject
{
  friend class PhysicsEngine;

public:
  /**
   * @brief Callback function called whenever a collision is detected between two objects.
   *
   * First object is "this" object, second is the object collided with.
   * Returns a boolean indicating if the collision should be handled.
   */
  typedef std::function<bool(PhysicsObject *, PhysicsObject *)> PhysicsCollisionCallback;

  /**
   * @brief Callback function called whenever a manifold is generted for a collision.
   *
   * First object is "this" object, second is the object collided with.
   */
  typedef std::function<void(PhysicsObject *, PhysicsObject *, Manifold *)> OnCollisionManifoldCallback;

public:
  PhysicsObject();
  virtual ~PhysicsObject();

  /**
   * @brief Gets the at rest flag.
   * @return True if object is at rest
   */
  inline bool IsAtRest() const
  {
    return m_atRest;
  }

  /**
   * @brief Gets the at rest flag.
   * @return True if the object is not at rest
   */
  inline bool IsAwake() const
  {
    return !m_atRest;
  }

  /**
   * @brief Gets the collision detection ebale flag for this object.
   * @return True if collision detection is enabled
   */
  inline bool CanCollide() const
  {
    return m_collisionEnabled;
  }

  /**
   * @brief Gets the square of the rest velocity threshold.
   * @return Squared rest velocity
   */
  inline float GetRestVelocityThresholdSquared() const
  {
    return m_restVelocityThresholdSquared;
  }

  /**
   * @brief Gets the damping factor coefficient.
   * @return Damping coefficient
   */
  inline float GetDampingCoefficient() const
  {
    return m_dampingCoefficient;
  }

  /**
   * @brief Gets the local model bounding box.
   * @return Bounding box
   */
  inline BoundingBox GetLocalBoundingBox() const
  {
    return m_localBoundingBox;
  }

  BoundingBox GetWorldSpaceAABB() const;

  /**
   * @brief Gets the collision elasticity.
   * @return Elasticity
   */
  inline float GetElasticity() const
  {
    return m_elasticity;
  }

  /**
   * @brief Gets friction.
   * @return Friction
   */
  inline float GetFriction() const
  {
    return m_friction;
  }

  /**
   * @brief Gets position vector.
   * @return Position
   */
  inline const Vector3 &GetPosition() const
  {
    return m_position;
  }

  /**
   * @brief Gets linear velocity vector.
   * @return Linear velocity
   */
  inline const Vector3 &GetLinearVelocity() const
  {
    return m_linearVelocity;
  }

  /**
   * @brief Gets linear force vector.
   * @return Linear force
   */
  inline const Vector3 &GetForce() const
  {
    return m_linearForce;
  }

  /**
   * @brief Gets inverse object mass.
   * @return Inverse mass
   */
  inline float GetInverseMass() const
  {
    return m_inverseMass;
  }

  /**
   * @brief Gets object orientation.
   * @return Orientation
   */
  inline const Quaternion &GetOrientation() const
  {
    return m_orientation;
  }

  /**
   * @brief Gets angular velocity vector.
   * @return Angular velocity
   */
  inline const Vector3 &GetAngularVelocity() const
  {
    return m_angularVelocity;
  }

  /**
   * @brief Gets torque vector.
   * @return Torque
   */
  inline const Vector3 &GetTorque() const
  {
    return m_torque;
  }

  /**
   * @brief Gets inverse inertia matrix.
   * @return Inverse inertia
   */
  inline const Matrix3 &GetInverseInertia() const
  {
    return m_inverseInertia;
  }

  /**
   * @brief Gets the number of attached collision shapes.
   * @return Collision shape count
   */
  inline size_t NumCollisionShapes() const
  {
    return m_collisionShapes.size();
  }

  /**
   * @brief Gets a const iterator to the start of the vector of collision shapes.
   * @return Start iterator
   */
  inline std::vector<ICollisionShape *>::const_iterator CollisionShapesBegin() const
  {
    return m_collisionEnabled ? m_collisionShapes.cbegin() : m_collisionShapes.cend();
  }

  /**
   * @brief Gets a const iterator to the end of the vector of collision shapes.
   * @return End iterator
   */
  inline std::vector<ICollisionShape *>::const_iterator CollisionShapesEnd() const
  {
    return m_collisionShapes.cend();
  }

  /**
   * @brief Gets a pointer to the Object associated with this physical object.
   * @return Parent Object
   */
  inline Object *GetAssociatedObject() const
  {
    return m_parent;
  }

  const Matrix4 &GetWorldSpaceTransform() const;

  /**
   * @brief Sets if collision detection is enabled for this object.
   * @param enable If collisions are enabled
   */
  inline void SetCollisionsEnabled(bool enable)
  {
    m_collisionEnabled = enable;
  }

  /**
   * @brief Sets the at rest velocity sum threshold.
   * @param vel At rest velocity
   *
   * Set to a negative value to disable check.
   */
  inline void SetRestVelocityThreshold(float vel)
  {
    if (vel <= 0.0f)
      m_restVelocityThresholdSquared = -1.0f;
    else
      m_restVelocityThresholdSquared = vel * vel;
  }

  /**
   * @brief Sets the local bounding box.
   * @param bb Bounding box
   */
  inline void SetLocalBoundingBox(const BoundingBox &bb)
  {
    m_localBoundingBox = bb;
    m_wsAabbInvalidated = true;
  }

  /**
   * @brief Sets the object that this object shares point gravitation with.
   * @param obj Gravity target
   */
  inline void SetGravitationTarget(PhysicsObject *obj)
  {
    m_gravitationTarget = obj;
  }

  /**
   * @brief Sets the damping factor coefficient for object specific damping factor adjustments.
   * @param coeff Damping coefficient
   */
  inline void SetDampingCoefficient(float coeff)
  {
    m_dampingCoefficient = coeff;
  }

  /**
   * @brief Sets collision elasticity.
   * @param elasticity Elasticity
   */
  inline void SetElasticity(float elasticity)
  {
    m_elasticity = elasticity;
  }

  /**
   * @brief Sets friction of this object.
   * @param friction Friction
   */
  inline void SetFriction(float friction)
  {
    m_friction = friction;
  }

  /**
   * @brief Sets the position of this object.
   * @param v Position
   */
  inline void SetPosition(const Vector3 &v)
  {
    m_position = v;
    m_wsTransformInvalidated = true;
    m_wsAabbInvalidated = true;
    m_atRest = false;
  }

  /**
   * @brief Sets the linear velocity of this object.
   * @param v Linear velocity
   */
  inline void SetLinearVelocity(const Vector3 &v)
  {
    m_linearVelocity = v;
  }

  /**
   * @brief Adds a linear force to the object.
   * @param force Force vector to add
   */
  inline void ApplyForce(const Vector3 &force)
  {
    m_linearForce += force;
  }

  /**
   * @brief Sets linear force to zero.
   */
  inline void ClearForces()
  {
    m_linearForce.ToZero();
  }

  /**
   * @brief Sets the linear force on this object.
   * @param v Linear force
   */
  inline void SetForce(const Vector3 &v)
  {
    m_linearForce = v;
  }

  /**
   * @brief Sets the inverse mass of this object.
   * @param v Inverse mass
   */
  inline void SetInverseMass(const float &v)
  {
    m_inverseMass = v;
  }

  /**
   * @brief Sets the orientation of the object.
   * @param v Orientation
   */
  inline void SetOrientation(const Quaternion &v)
  {
    m_orientation = v;
    m_wsTransformInvalidated = true;
    m_atRest = false;
  }

  /**
   * @brief Sets the angular velocity of the object.
   * @param v Angular velocity vector
   */
  inline void SetAngularVelocity(const Vector3 &v)
  {
    m_angularVelocity = v;
  }

  /**
   * @brief Sets torque on the object.
   * @param v Torque vector
   */
  inline void SetTorque(const Vector3 &v)
  {
    m_torque = v;
  }

  /**
   * @brief Sets the inverse inertia of the object.
   * @param v Inverse inertia matrix
   */
  inline void SetInverseInertia(const Matrix3 &v)
  {
    m_inverseInertia = v;
  }

  /**
   * @brief Adds a collision shape to the object.
   * @param colShape Collision shape
   */
  inline void AddCollisionShape(ICollisionShape *colShape)
  {
    m_collisionShapes.push_back(colShape);
  }

  /**
   * @brief Sets the association between this physics object and an Object.
   *
   * Called automatically when PhysicsObject is created through Object::CreatePhysicsNode().
   */
  inline void SetAssociatedObject(Object *obj)
  {
    m_parent = obj;
  }

  void AutoResizeBoundingBox();

  /**
   * @brief Sets the collision callback handler.
   * @param callback Callback function
   */
  inline void SetOnCollisionCallback(PhysicsCollisionCallback callback)
  {
    m_onCollisionCallback = callback;
  }

  /**
   * @brief Adds a collision callback that is fired after tha manifold is generated.
   * @param callback Callback function
   *
   * Useful if the contact points are needed in the callback.
   */
  inline void AddOnCollisionManifoldCallback(OnCollisionManifoldCallback callback)
  {
    m_onCollisionManifoldCallbacks.push_back(callback);
  }

  /**
   * @brief Triggers the on collision callback.
   * @param obj_a First (this) object
   * @param obj_b Second (other) object
   * @return Boolean indicating if the collision should be handled
   */
  inline bool FireOnCollisionEvent(PhysicsObject *obj_a, PhysicsObject *obj_b)
  {
    bool handleCollision = (m_onCollisionCallback) ? m_onCollisionCallback(obj_a, obj_b) : true;

    // Wake up on collision
    if (handleCollision)
      m_atRest = false;

    return handleCollision;
  }

  /**
   * @brief Fires collision callbacks after manifold generation.
   * @param obj_a First (this) object
   * @param obj_b Second (other) object
   * @param manifold Collision manifold between objects
   */
  inline void FireOnCollisionManifoldCallback(PhysicsObject *a, PhysicsObject *b, Manifold *manifold)
  {
    for (auto it = m_onCollisionManifoldCallbacks.begin(); it != m_onCollisionManifoldCallbacks.end(); ++it)
      it->operator()(a, b, manifold);
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

  bool m_collisionEnabled;              //!< Flag indication if collision detection is enabled for this object
  bool m_atRest;                        //!< Flag indicating if this object is at rest
  float m_restVelocityThresholdSquared; //!< Squared velocity vector magnitude at which the object is deemed to be stationary
  float m_averageSummedVelocity;        //!< Exponential moving average of sum of magnitudes of linear and angular velocity

  float m_dampingCoefficient; //!< Damping coefficient for velocity

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

  std::vector<ICollisionShape *> m_collisionShapes;                        //!< Collection of collision shapes in this object
  PhysicsCollisionCallback m_onCollisionCallback;                          //!< Collision callback
  std::vector<OnCollisionManifoldCallback> m_onCollisionManifoldCallbacks; //!< Collision callbacks post manifold generation
};
