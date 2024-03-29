#pragma once

#include <nclgl/Matrix4.h>
#include <nclgl/Vector3.h>

/**
 * @class BoundingBox
 * @author Dan Nixon
 * @brief Represents a simple 3D bounding box.
 */
class BoundingBox
{
public:
  BoundingBox();
  BoundingBox(const Vector3 &lower, const Vector3 &upper);
  virtual ~BoundingBox();

  void Reset();

  void ExpandToFit(const Vector3 &point);
  void ExpandToFit(const BoundingBox &otherBox);
  void SetHalfDimensions(const Vector3 &halfDims);

  /**
   * @brief Gets the lower vertex of the bounding box.
   * @return Reference to vertex
   */
  Vector3 &Lower()
  {
    return m_lower;
  }

  /**
   * @brief Gets the lower vertex of the bounding box.
   * @return Const reference to vertex
   */
  const Vector3 &Lower() const
  {
    return m_lower;
  }

  /**
   * @brief Gets the upper vertex of the bounding box.
   * @return Reference to vertex
   */
  Vector3 &Upper()
  {
    return m_upper;
  }

  /**
   * @brief Gets the upper vertex of the bounding box.
   * @return Const reference to vertex
   */
  const Vector3 &Upper() const
  {
    return m_upper;
  }

  Vector3 Centre() const;

  float SphereRadius() const;

  BoundingBox Transform(const Matrix4 &transformation) const;

  bool Intersects(const Vector3 &point) const;
  bool Intersects(const BoundingBox &otherBox) const;

  virtual void DebugDraw(const Matrix4 &transform, const Vector4 &faceColour = Vector4(1.0f, 1.0f, 1.0f, 0.2f),
                         const Vector4 &edgeColour = Vector4(1.0f, 0.2f, 1.0f, 1.0f), float width = 0.02f) const;

protected:
  Vector3 m_lower; //!< Lower vertex of bounding box
  Vector3 m_upper; //!< Upper vertex of bounding box
};
