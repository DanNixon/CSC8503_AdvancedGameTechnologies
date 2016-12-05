#include "AABB.h"

const int AABB::FAR_FACE[] = {0, 1, 2, 3};
const int AABB::NEAR_FACE[] = {7, 6, 5, 4};
const int AABB::TOP_FACE[] = {5, 6, 2, 1};
const int AABB::BOTTOM_FACE[] = {0, 3, 7, 4};
const int AABB::RIGHT_FACE[] = {6, 7, 3, 2};
const int AABB::LEFT_FACE[] = {4, 5, 1, 0};

AABB::AABB()
{
  // Vertices
  AddVertex(Vector3(-1.0f, -1.0f, -1.0f)); // 0 lll
  AddVertex(Vector3(-1.0f, 1.0f, -1.0f));  // 1 lul
  AddVertex(Vector3(1.0f, 1.0f, -1.0f));   // 2 uul
  AddVertex(Vector3(1.0f, -1.0f, -1.0f));  // 3 ull
  AddVertex(Vector3(-1.0f, -1.0f, 1.0f));  // 4 llu
  AddVertex(Vector3(-1.0f, 1.0f, 1.0f));   // 5 luu
  AddVertex(Vector3(1.0f, 1.0f, 1.0f));    // 6 uuu
  AddVertex(Vector3(1.0f, -1.0f, 1.0f));   // 7 ulu

  // Faces
  AddFace(Vector3(0.0f, 0.0f, -1.0f), 4, FAR_FACE);
  AddFace(Vector3(0.0f, 0.0f, 1.0f), 4, NEAR_FACE);
  AddFace(Vector3(0.0f, 1.0f, 0.0f), 4, TOP_FACE);
  AddFace(Vector3(0.0f, -1.0f, 0.0f), 4, BOTTOM_FACE);
  AddFace(Vector3(1.0f, 0.0f, 0.0f), 4, RIGHT_FACE);
  AddFace(Vector3(-1.0f, 0.0f, 0.0f), 4, LEFT_FACE);
}

AABB::~AABB()
{
}

void AABB::SetHalfDimensions(const Vector3 &halfDims)
{
  SetLower(-halfDims);
  SetUpper(halfDims);
}

void AABB::SetUpper(const Vector3 &upper)
{
  m_vVertices[1].pos.y = upper.y;

  m_vVertices[2].pos.x = upper.x;
  m_vVertices[2].pos.y = upper.y;

  m_vVertices[3].pos.x = upper.x;

  m_vVertices[4].pos.z = upper.z;

  m_vVertices[5].pos.y = upper.y;
  m_vVertices[5].pos.z = upper.z;

  m_vVertices[6].pos.x = upper.x;
  m_vVertices[6].pos.y = upper.y;
  m_vVertices[6].pos.z = upper.z;

  m_vVertices[7].pos.x = upper.x;
  m_vVertices[7].pos.z = upper.z;
}

void AABB::SetLower(const Vector3 &lower)
{
  m_vVertices[0].pos.x = lower.x;
  m_vVertices[0].pos.y = lower.y;
  m_vVertices[0].pos.z = lower.z;

  m_vVertices[1].pos.x = lower.x;
  m_vVertices[1].pos.z = lower.z;

  m_vVertices[2].pos.z = lower.z;

  m_vVertices[3].pos.y = lower.y;
  m_vVertices[3].pos.z = lower.z;

  m_vVertices[4].pos.x = lower.x;
  m_vVertices[4].pos.y = lower.y;

  m_vVertices[5].pos.x = lower.x;

  m_vVertices[7].pos.y = lower.y;
}

Vector3 AABB::GetUpper() const
{
  return Vector3(m_vVertices[6].pos.x, m_vVertices[6].pos.y, m_vVertices[6].pos.z);
}

Vector3 AABB::GetLower() const
{
  return Vector3(m_vVertices[0].pos.x, m_vVertices[0].pos.y, m_vVertices[0].pos.z);
}

bool AABB::Intersects(const AABB &other) const
{
  // TODO
  return false;
}
