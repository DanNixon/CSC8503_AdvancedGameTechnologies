#include "BoundingBoxHull.h"

const int BoundingBoxHull::FAR_FACE[] = {0, 1, 2, 3};
const int BoundingBoxHull::NEAR_FACE[] = {7, 6, 5, 4};
const int BoundingBoxHull::TOP_FACE[] = {5, 6, 2, 1};
const int BoundingBoxHull::BOTTOM_FACE[] = {0, 3, 7, 4};
const int BoundingBoxHull::RIGHT_FACE[] = {6, 7, 3, 2};
const int BoundingBoxHull::LEFT_FACE[] = {4, 5, 1, 0};

BoundingBoxHull::BoundingBoxHull()
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

BoundingBoxHull::~BoundingBoxHull()
{
}

void BoundingBoxHull::UpdateHull()
{
  if (m_vVertices.size() != 8)
    return;

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