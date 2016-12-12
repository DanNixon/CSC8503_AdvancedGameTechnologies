#include "BoundingBox.h"

#include "NCLDebug.h"

#include <algorithm>

BoundingBox::BoundingBox()
{
  Reset();
}

BoundingBox::BoundingBox(const Vector3 &lower, const Vector3 &upper)
    : m_lower(lower)
    , m_upper(upper)
{
}

BoundingBox::~BoundingBox()
{
}

void BoundingBox::Reset()
{
  m_lower = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
  m_upper = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}

void BoundingBox::ExpandToFit(const Vector3 &point)
{
  m_lower.x = min(m_lower.x, point.x);
  m_lower.y = min(m_lower.y, point.y);
  m_lower.z = min(m_lower.z, point.z);

  m_upper.x = max(m_upper.x, point.x);
  m_upper.y = max(m_upper.y, point.y);
  m_upper.z = max(m_upper.z, point.z);
}

void BoundingBox::ExpandToFit(const BoundingBox &otherBox)
{
  m_lower.x = min(m_lower.x, otherBox.m_lower.x);
  m_lower.y = min(m_lower.y, otherBox.m_lower.y);
  m_lower.z = min(m_lower.z, otherBox.m_lower.z);

  m_upper.x = max(m_upper.x, otherBox.m_lower.x);
  m_upper.y = max(m_upper.y, otherBox.m_lower.y);
  m_upper.z = max(m_upper.z, otherBox.m_lower.z);

  m_lower.x = min(m_lower.x, otherBox.m_upper.x);
  m_lower.y = min(m_lower.y, otherBox.m_upper.y);
  m_lower.z = min(m_lower.z, otherBox.m_upper.z);

  m_upper.x = max(m_upper.x, otherBox.m_upper.x);
  m_upper.y = max(m_upper.y, otherBox.m_upper.y);
  m_upper.z = max(m_upper.z, otherBox.m_upper.z);
}

void BoundingBox::SetHalfDimensions(const Vector3 &halfDims)
{
  m_lower = -halfDims;
  m_upper = halfDims;
}

const Vector3 BoundingBox::Centre() const
{
  return m_lower + ((m_upper - m_lower) * 0.5f);
}

BoundingBox BoundingBox::Transform(const Matrix4 &transformation) const
{
  BoundingBox retVal;

  retVal.ExpandToFit(transformation * Vector3(m_lower.x, m_lower.y, m_lower.z));
  retVal.ExpandToFit(transformation * Vector3(m_upper.x, m_lower.y, m_lower.z));
  retVal.ExpandToFit(transformation * Vector3(m_lower.x, m_upper.y, m_lower.z));
  retVal.ExpandToFit(transformation * Vector3(m_upper.x, m_upper.y, m_lower.z));

  retVal.ExpandToFit(transformation * Vector3(m_lower.x, m_lower.y, m_upper.z));
  retVal.ExpandToFit(transformation * Vector3(m_upper.x, m_lower.y, m_upper.z));
  retVal.ExpandToFit(transformation * Vector3(m_lower.x, m_upper.y, m_upper.z));
  retVal.ExpandToFit(transformation * Vector3(m_upper.x, m_upper.y, m_upper.z));

  return retVal;
}

bool BoundingBox::Intersects(const Vector3 &point) const
{
  return m_lower <= point && point <= m_upper;
}

bool BoundingBox::Intersects(const BoundingBox &otherBox) const
{
  return (m_lower <= otherBox.m_lower && otherBox.m_lower <= m_upper) ||
         ((m_lower <= otherBox.m_upper && otherBox.m_upper <= m_upper));
}

void BoundingBox::DebugDraw(const Matrix4 &transform, const Vector4 &faceColour, const Vector4 &edgeColour, float width) const
{
  Vector3 uuu = transform * m_upper;
  Vector3 lll = transform * m_lower;

  Vector3 ull(uuu.x, lll.y, lll.z);
  Vector3 uul(uuu.x, uuu.y, lll.z);
  Vector3 ulu(uuu.x, lll.y, uuu.z);

  Vector3 luu(lll.x, uuu.y, uuu.z);
  Vector3 llu(lll.x, lll.y, uuu.z);
  Vector3 lul(lll.x, uuu.y, lll.z);

  // Draw edges
  NCLDebug::DrawThickLineNDT(luu, uuu, width, edgeColour);
  NCLDebug::DrawThickLineNDT(lul, uul, width, edgeColour);
  NCLDebug::DrawThickLineNDT(llu, ulu, width, edgeColour);
  NCLDebug::DrawThickLineNDT(lll, ull, width, edgeColour);

  NCLDebug::DrawThickLineNDT(lul, lll, width, edgeColour);
  NCLDebug::DrawThickLineNDT(uul, ull, width, edgeColour);
  NCLDebug::DrawThickLineNDT(luu, llu, width, edgeColour);
  NCLDebug::DrawThickLineNDT(uuu, ulu, width, edgeColour);

  NCLDebug::DrawThickLineNDT(lll, llu, width, edgeColour);
  NCLDebug::DrawThickLineNDT(ull, ulu, width, edgeColour);
  NCLDebug::DrawThickLineNDT(lul, luu, width, edgeColour);
  NCLDebug::DrawThickLineNDT(uul, uuu, width, edgeColour);
}
