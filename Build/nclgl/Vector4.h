/*
Class:Vector4
Implements:
Author:Rich Davison
Description:VERY simple Vector4 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*/
#pragma once

#include "Vector3.h"

class Vector4
{
public:
  Vector4(void)
  {
    x = y = z = w = 1.0f;
  }

  Vector4(float x, float y, float z, float w)
  {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
  }

  Vector3 ToVector3() const
  {
    return Vector3(x, y, z);
  }

  ~Vector4(void)
  {
  }

  float x;
  float y;
  float z;
  float w;

  Vector4 operator+(const Vector4 &rhs) const
  {
    return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
  }

  Vector4 operator-(const Vector4 &rhs) const
  {
    return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
  }

  Vector4 &operator+=(const Vector4 &rhs)
  {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
  }

  Vector4 &operator-=(const Vector4 &rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
  }

  inline float operator[](int i) const
  {
    switch (i)
    {
    case 0:
      return x;
    case 1:
      return y;
    case 2:
      return z;
    case 3:
      return w;
    default:
      return 0.0f;
    }
  }

  /**
   * @brief Outputs the component values of a vector to a strem in the format "[x,y,z,w]".
   * @param stream The stream to output to
   * @param v The vector to output
   */
  inline friend std::ostream &operator<<(std::ostream &stream, const Vector4 &v)
  {
    stream << '[' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ']';
    return stream;
  }

  /**
   * @brief Reads component values of a vector from a stream in format "[x,y,z,w]".
   * @param stream Stream to read from
   * @param v Vector to store values in
   */
  inline friend std::istream &operator>>(std::istream &stream, Vector4 &v)
  {
    float x, y, z, w;
    char delim;
    stream >> delim >> x >> delim >> y >> delim >> z >> delim >> w >> delim;
    v = Vector4(x, y, z, w);
    return stream;
  }
};
