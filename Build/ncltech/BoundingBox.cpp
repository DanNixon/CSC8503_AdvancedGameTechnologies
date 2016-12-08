#include "BoundingBox.h"

#include <algorithm>

const int BoundingBox::FAR_FACE[] = {0, 1, 2, 3};
const int BoundingBox::NEAR_FACE[] = {7, 6, 5, 4};
const int BoundingBox::TOP_FACE[] = {5, 6, 2, 1};
const int BoundingBox::BOTTOM_FACE[] = {0, 3, 7, 4};
const int BoundingBox::RIGHT_FACE[] = {6, 7, 3, 2};
const int BoundingBox::LEFT_FACE[] = {4, 5, 1, 0};

BoundingBox::BoundingBox()
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

BoundingBox::~BoundingBox()
{
}

void BoundingBox::ExpandToFit(const Vector3 &point)
{
  m_lower.x = std::min(m_lower.x, point.x);
  m_lower.y = std::min(m_lower.y, point.y);
  m_lower.z = std::min(m_lower.z, point.z);

  m_upper.x = std::max(m_upper.x, point.x);
  m_upper.y = std::max(m_upper.y, point.y);
  m_upper.z = std::max(m_upper.z, point.z);
}

void BoundingBox::SetHalfDimensions(const Vector3 &halfDims)
{
  m_lower = -halfDims;
  m_upper = halfDims;
}

BoundingBox BoundingBox::Transform(const Matrix4 &transformation) const
{
  BoundingBox retVal = BoundingBox();
  retVal.m_lower = transformation * m_lower;
  retVal.m_upper = transformation * m_upper;
  retVal.UpdateHull();

  return retVal;
}

bool BoundingBox::Intersects(const BoundingBox &other) const
{
  // TODO
  return false;
}

void BoundingBox::UpdateHull()
{
  // Lower
  m_vVertices[0].pos.x = m_lower.x;
  m_vVertices[0].pos.y = m_lower.y;
  m_vVertices[0].pos.z = m_lower.z;

  m_vVertices[1].pos.x = m_lower.x;
  m_vVertices[1].pos.z = m_lower.z;

  m_vVertices[2].pos.z = m_lower.z;

  m_vVertices[3].pos.y = m_lower.y;
  m_vVertices[3].pos.z = m_lower.z;

  m_vVertices[4].pos.x = m_lower.x;
  m_vVertices[4].pos.y = m_lower.y;

  m_vVertices[5].pos.x = m_lower.x;

  m_vVertices[7].pos.y = m_lower.y;

  // Upper
  m_vVertices[1].pos.y = m_upper.y;

  m_vVertices[2].pos.x = m_upper.x;
  m_vVertices[2].pos.y = m_upper.y;

  m_vVertices[3].pos.x = m_upper.x;

  m_vVertices[4].pos.z = m_upper.z;

  m_vVertices[5].pos.y = m_upper.y;
  m_vVertices[5].pos.z = m_upper.z;

  m_vVertices[6].pos.x = m_upper.x;
  m_vVertices[6].pos.y = m_upper.y;
  m_vVertices[6].pos.z = m_upper.z;

  m_vVertices[7].pos.x = m_upper.x;
  m_vVertices[7].pos.z = m_upper.z;
}