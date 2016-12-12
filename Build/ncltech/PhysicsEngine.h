/******************************************************************************
Class: PhysicsEngine
Implements:
Author: Pieran Marris      <p.marris@newcastle.ac.uk> and YOU!
Description:

The glue that brings all our physics dreams together. This class is provided
pretty much as is, as most of the sub-systems are tied in to this already
with a bit of debugging code to visualise various parts of the physics engine.

If you want

The general runtime consists of:
	- Update(float dt)
	  - UpdatePhysics()
	     - Broadphase Collision Detection
		   Quickly identifies possible collisions in between objects, hopefully
		   with world space partitioning systems like Octrees.. but currently just
		   builds a list colliding all objects with all other objects. (Hopefully you
		   can find some free time to fix this =] )

		 - Narrowphase Collision Detection
		   Takes the list provided by the broadphase collision detection and
		   accurately collides all objects, building a collision manifold as
		   required. (Tutorial 4/5)

		 - Solve Constraints & Collisions
		   Solves all velocity constraints in the physics system, these include
		   both Collision Constraints (Tutorial 5,6) and misc world constraints
		   (Tutorial 3)

		 - Update Physics Objects
		   Moves all physics objects through time, updating positions/rotations
		   etc. (Tutorial 2)

		(\_/)
		( '_')
	 /""""""""""""\=========     -----D
	/"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*/ /////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IBroadphase.h"
#include "IConstraint.h"
#include "Manifold.h"
#include "PhysicsObject.h"
#include "TSingleton.h"
#include <mutex>
#include <vector>

#define SOLVER_ITERATIONS 50

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

#define DEBUGDRAW_FLAGS_CONSTRAINT 1
#define DEBUGDRAW_FLAGS_MANIFOLD 2
#define DEBUGDRAW_FLAGS_COLLISIONVOLUMES 4
#define DEBUGDRAW_FLAGS_COLLISIONNORMALS 8
#define DEBUGDRAW_FLAGS_AABB 16
#define DEBUGDRAW_FLAGS_LINEARVELOCITY 32
#define DEBUGDRAW_FLAGS_LINEARFORCE 64
#define DEBUGDRAW_FLAGS_BROADPHASE 128
#define DEBUGDRAW_FLAGS_BROADPHASE_PAIRS 256

/**
 * @brief Represents different integration schemes.
 */
enum IntegrationType
{
  INTEGRATION_EXPLICIT_EULER,
  INTEGRATION_SEMI_IMPLICIT_EULER,
  INTEGRATION_RUNGE_KUTTA_2,
  INTEGRATION_RUNGE_KUTTA_4
};

/**
 * @class PhysicsEngine
 * @brief Manages simulation of a physical system.
 */
class PhysicsEngine : public TSingleton<PhysicsEngine>
{
  friend class TSingleton<PhysicsEngine>;

public:
  /**
   * @brief Maximum number of physics updates that can be performed in a single frame.
   */
  const uint8_t MAX_UPDATES_PER_FRAME = 5;

public:
  void SetDefaults();

  void AddPhysicsObject(PhysicsObject *obj);
  void RemovePhysicsObject(PhysicsObject *obj);
  void RemoveAllPhysicsObjects();

  /**
   * @brief Adds a constraint to the physics system.
   * @param c Constraint to add
   */
  void AddConstraint(IConstraint *c)
  {
    m_vpConstraints.push_back(c);
  }

  void Update(float deltaTime);

  void DebugRender();

  /**
   * @brief Gets the simulation paused state.
   * @return True if simulation is paused
   */
  bool IsPaused() const
  {
    return m_IsPaused;
  }

  /**
   * @brief Pause or resume the simulation.
   * @param paused True if the simulation is to be paused
   */
  void SetPaused(bool paused)
  {
    m_IsPaused = paused;
  }

  /**
   * @brief Gets the active debug draw flags.
   * @return Debug draw flags
   */
  uint64_t GetDebugDrawFlags() const
  {
    return m_DebugDrawFlags;
  }

  /**
   * @brief Sets the active debug draw flags.
   * @param flags Debug draw flags
   */
  void SetDebugDrawFlags(uint64_t flags)
  {
    m_DebugDrawFlags = flags;
  }

  /**
   * @brief Gets the target update time step in seconds.
   * @return Target update timestep
   */
  float GetUpdateTimestep() const
  {
    return m_UpdateTimestep;
  }

  /**
   * @brief Sets the target update time step in seconds.
   * @param updateTimestep Target update timestep
   */
  void SetUpdateTimestep(float updateTimestep)
  {
    m_UpdateTimestep = updateTimestep;
  }

  /**
   * @brief Gets the broadphase collision detection handler.
   * @return Pointer to broadphase handler
   */
  inline IBroadphase *GetBroadphase()
  {
    return m_broadphaseDetection;
  }

  /**
   * @brief Sets the broadphase collision detection handler.
   * @param bp Pointer to broadphase handler
   *
   * Handler that is set at engine deconstruction will be deleted.
   */
  void SetBroadphase(IBroadphase *bp)
  {
    m_broadphaseDetection = bp;
  }

  /**
   * @brief Gets the number of broadphase collision paris generted in this update (over all possible frames).
   * @return Number of broadphase collision pairs
   */
  inline size_t NumBroadphaseTests() const
  {
    return m_broadphaseCollisionPairCount;
  }

  /**
   * @brief Gets the current integration scheme.
   * @return Integration scheme
   */
  inline IntegrationType GetIntegrationType() const
  {
    return m_integrationType;
  }

  /**
   * @brief Sets the integration scheme used in object updates.
   * @param type Integration scheme
   */
  void SetIntegrationType(IntegrationType type)
  {
    m_integrationType = type;
  }

  /**
   * @brief Gets the acceleration due to uniform linear gravity.
   * @return Acceleration due to gravity vector
   */
  const Vector3 &GetGravity() const
  {
    return m_LinearGravity;
  }

  /**
   * @brief Sets uniform linear gravity.
   * @param g Acceleration due to gravity vector
   *
   * A vector of zero magnitude disables linear gravity and enables point gravity for objects that have it configured.
   */
  void SetGravity(const Vector3 &g)
  {
    m_LinearGravity = g;
  }

  /**
   * @brief Sets the velocity damping factor.
   * @param d Damping factor
   */
  void SetDampingFactor(float d)
  {
    m_DampingFactor = d;
  }

  /**
   * @brief Gets the velocity damping factor.
   * @return Damping factor
   */
  float GetDampingFactor() const
  {
    return m_DampingFactor;
  }

  /**
   * @brief Gets the target update time step in seconds.
   * @return Target update timestep
   */
  float GetDeltaTime() const
  {
    return m_UpdateTimestep;
  }

  bool SimulationIsAtRest() const;

protected:
  PhysicsEngine();
  ~PhysicsEngine();

  void UpdatePhysics();
  void NarrowPhaseCollisions();
  void UpdatePhysicsObject(PhysicsObject *obj);
  void SolveConstraints();

protected:
  bool m_IsPaused; //!< Flag indicating phsyics updates are paused

  float m_UpdateTimestep; //!< Target update timestep in seconds
  float m_UpdateAccum;    //!< Accumulated time over the frame

  uint64_t m_DebugDrawFlags; //!< Debug draw state flags

  IntegrationType m_integrationType; //!< Type of integration performed in object updates

  Vector3 m_LinearGravity;  //!< Linear acceleration due to gravity (zero to disable linear gravity)
  float m_PointGravity;     //!< Acceleration due to point gravity (one object is stationary)
  float m_PointGravitation; //!< Gravitation constant for point gravity (both objects are movable)

  float m_DampingFactor; //!< Velocity dampening factor

  IBroadphase *m_broadphaseDetection;                    //!< Handler used to find broadphase collision pairs
  std::vector<CollisionPair> m_BroadphaseCollisionPairs; //!< Set of collision paris found in broadphase
  size_t m_broadphaseCollisionPairCount;                 //!< Cached count of braoadphase collision pairs

  std::vector<PhysicsObject *> m_PhysicsObjects; //!< All physical objects in the simulation

  std::vector<IConstraint *> m_vpConstraints; //!< Misc constraints applying to one or more physics objects
  std::vector<Manifold *> m_vpManifolds;      //!< Contact constraints between pairs of objects
};
