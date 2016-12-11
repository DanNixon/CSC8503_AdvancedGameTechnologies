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
#include "Constraint.h"
#include "IBroadphase.h"
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

class PhysicsEngine : public TSingleton<PhysicsEngine>
{
  friend class TSingleton<PhysicsEngine>;

public:
  const uint8_t MAX_UPDATES_PER_FRAME = 5;

public:
  // Reset Default Values like gravity/timestep - called when scene is switched out
  void SetDefaults();

  // Add/Remove Physics Objects
  void AddPhysicsObject(PhysicsObject *obj);
  void RemovePhysicsObject(PhysicsObject *obj);
  void RemoveAllPhysicsObjects(); // Delete all physics entities etc and
                                  // reset-physics environment for new scene to
                                  // be initialized

  // Add Constraints
  void AddConstraint(Constraint *c)
  {
    m_vpConstraints.push_back(c);
  }

  // Update Physics Engine
  void Update(float deltaTime); // Remember DeltaTime is 'seconds' since last
                                // update not milliseconds

  // Debug draw all physics objects, manifolds and constraints
  void DebugRender();

  // Getters / Setters
  bool IsPaused() const
  {
    return m_IsPaused;
  }

  void SetPaused(bool paused)
  {
    m_IsPaused = paused;
  }

  uint64_t GetDebugDrawFlags() const
  {
    return m_DebugDrawFlags;
  }

  void SetDebugDrawFlags(uint64_t flags)
  {
    m_DebugDrawFlags = flags;
  }

  float GetUpdateTimestep() const
  {
    return m_UpdateTimestep;
  }

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

  const Vector3 &GetGravity() const
  {
    return m_LinearGravity;
  }

  void SetGravity(const Vector3 &g)
  {
    m_LinearGravity = g;
  }

  float GetDampingFactor() const
  {
    return m_DampingFactor;
  }

  void SetDampingFactor(float d)
  {
    m_DampingFactor = d;
  }

  float GetDeltaTime() const
  {
    return m_UpdateTimestep;
  }

  bool SimulationIsAtRest() const;

protected:
  PhysicsEngine();
  ~PhysicsEngine();

  // The actual time-independant update function
  void UpdatePhysics();

  // Handles narrowphase collision detection
  void NarrowPhaseCollisions();

  // Updates all physics objects position, orientation, velocity etc
  void UpdatePhysicsObject(PhysicsObject *obj);

  // Solves all physical constraints (constraints and manifolds)
  void SolveConstraints();

protected:
  bool m_IsPaused;
  float m_UpdateTimestep, m_UpdateAccum;
  uint64_t m_DebugDrawFlags;

  IntegrationType m_integrationType; //!< Type of integration performed in object updates

  Vector3 m_LinearGravity;
  float m_PointGravity;
  float m_PointGravitation;

  float m_DampingFactor;

  IBroadphase *m_broadphaseDetection;                    //!< Handler used to find broadphase collision pairs
  std::vector<CollisionPair> m_BroadphaseCollisionPairs; //!< Set of collision paris found in broadphase
  size_t m_broadphaseCollisionPairCount;

  std::vector<PhysicsObject *> m_PhysicsObjects; //!< All physical objects in the simulation

  std::vector<Constraint *> m_vpConstraints; // Misc constraints applying to one or more physics objects
  std::vector<Manifold *> m_vpManifolds;     // Contact constraints between pairs of objects
};