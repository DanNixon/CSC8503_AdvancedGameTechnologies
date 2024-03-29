/******************************************************************************
Class: Hull
Implements:
Author: Pieran Marris      <p.marris@newcastle.ac.uk> and YOU!
Description:

This is an elaborate and /slow/ version of the Mesh class from Graphics for Games.
It keeps track of faces, vertices and edges of a given mesh aswell as all the adjancy
information.

This means that you can retrieve a face and instanty have a list of all of it's
adjancent faces and contained vertices/edges without having to do expensive lookups.
Yep.. the expensive part is that these are all precomputed when the hull is first created O(n^3).

They can be quite useful for debugging shapes and experimenting with new 3D algorithms.
In this framework they are used to represent discrete collision shapes which have distinct non-curved,
faces such as the CuboidCollisionShape.

Note: One of the big changes from a normal Mesh is that the faces can have any number of vertices,
so you could represent your mesh as a series of pentagons, quads etc or any combination of these.

		(\_/)
		( '_')
	 /""""""""""""\=========     -----D
	/"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*/ /////////////////////////////////////////////////////////////////////////////

#pragma once

#include <nclgl\Matrix4.h>
#include <nclgl\Vector3.h>
#include <vector>

class BoundingBox;

/**
 * @brief Stores data related to a vertex in a Hull.
 */
struct HullVertex
{
  int idx;                          //!< Vertex index
  Vector3 pos;                      //!< Position
  std::vector<int> enclosing_edges; //!< Indices of connected edges
  std::vector<int> enclosing_faces; //!< Indices of connected faces
};

/**
 * @brief Stores data related to an edge in a Hull.
 */
struct HullEdge
{
  int idx;                             //!< Edge index
  int vStart;                          //!< Index of start vertex
  int vEnd;                            //!< Index of end vertex
  std::vector<int> adjoining_edge_ids; //!< Indices of connected edges
  std::vector<int> enclosing_faces;    //!< Indices of connected faces
};

/**
 * @brief Stores data related to an edge in a Hull.
 */
struct HullFace
{
  int idx;                             //!< Face index
  Vector3 _normal;                     //!< Face normal
  std::vector<int> vert_ids;           //!< Indices of vertices
  std::vector<int> edge_ids;           //!< Indices of edges
  std::vector<int> adjoining_face_ids; //!< Indices of connected faces
};

class Hull
{
public:
  Hull();
  virtual ~Hull();

  void AddVertex(const Vector3 &v);

  void AddFace(const Vector3 &_normal, int nVerts, const int *verts);

  void AddFace(const Vector3 &_normal, const std::vector<int> &vert_ids)
  {
    AddFace(_normal, vert_ids.size(), &vert_ids[0]);
  }

  int FindEdge(int v0_idx, int v1_idx);

  const HullVertex &GetVertex(int idx) const
  {
    return m_vVertices[idx];
  }

  const HullEdge &GetEdge(int idx) const
  {
    return m_vEdges[idx];
  }

  const HullFace &GetFace(int idx) const
  {
    return m_vFaces[idx];
  }

  size_t GetNumVertices() const
  {
    return m_vVertices.size();
  }

  size_t GetNumEdges() const
  {
    return m_vEdges.size();
  }

  size_t GetNumFaces() const
  {
    return m_vFaces.size();
  }

  void GetMinMaxVerticesInAxis(const Vector3 &local_axis, int *out_min_vert, int *out_max_vert) const;

  BoundingBox GetBoundingBox() const;

  virtual void DebugDraw(const Matrix4 &transform, const Vector4 &faceColour = Vector4(1.0f, 1.0f, 1.0f, 0.2f),
                         const Vector4 &edgeColour = Vector4(1.0f, 0.2f, 1.0f, 1.0f)) const;

protected:
  int ConstructNewEdge(int parent_face_idx, int vert_start, int vert_end); // Called by AddFace

protected:
  std::vector<HullVertex> m_vVertices;
  std::vector<HullEdge> m_vEdges;
  std::vector<HullFace> m_vFaces;
};
