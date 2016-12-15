#pragma once
/*
Class:Vector3
Implements:
Author:Rich Davison
Description:VERY simple Vector3 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*/
#include <cmath>
#include <iostream>

class Vector3
{
public:
  Vector3(void)
  {
    ToZero();
  }

  Vector3(const float x, const float y, const float z)
  {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  ~Vector3(void)
  {
  }

  float x;
  float y;
  float z;

  void Normalise()
  {
    float length = Length();

    if (length != 0.0f)
    {
      length = 1.0f / length;
      x = x * length;
      y = y * length;
      z = z * length;
    }
  }

  void ToZero()
  {
    x = y = z = 0.0f;
  }

  float Length() const
  {
    return sqrt((x * x) + (y * y) + (z * z));
  }

  float LengthSquared() const
  {
    return (x * x + y * y + z * z);
  }

  void Invert()
  {
    x = -x;
    y = -y;
    z = -z;
  }

  Vector3 Inverse() const
  {
    return Vector3(-x, -y, -z);
  }

  static float Dot(const Vector3 &a, const Vector3 &b)
  {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
  }

  static Vector3 Cross(const Vector3 &a, const Vector3 &b)
  {
    return Vector3((a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x));
  }

  inline Vector3 operator+(const Vector3 &a) const
  {
    return Vector3(x + a.x, y + a.y, z + a.z);
  }

  inline Vector3 operator-(const Vector3 &a) const
  {
    return Vector3(x - a.x, y - a.y, z - a.z);
  }

  inline Vector3 operator-() const
  {
    return Vector3(-x, -y, -z);
  }

  inline void operator+=(const Vector3 &a)
  {
    x += a.x;
    y += a.y;
    z += a.z;
  }

  inline void operator-=(const Vector3 &a)
  {
    x -= a.x;
    y -= a.y;
    z -= a.z;
  }

  inline Vector3 operator*(const float a) const
  {
    return Vector3(x * a, y * a, z * a);
  }

  inline Vector3 operator*(const Vector3 &a) const
  {
    return Vector3(x * a.x, y * a.y, z * a.z);
  }

  inline Vector3 operator/(const Vector3 &a) const
  {
    return Vector3(x / a.x, y / a.y, z / a.z);
  };

  inline Vector3 operator/(const float v) const
  {
    return Vector3(x / v, y / v, z / v);
  };

  inline bool operator<(const Vector3 &other) const
  {
    return x < other.x && y < other.y && z < other.z;
  }

  inline bool operator<=(const Vector3 &other) const
  {
    return x <= other.x && y <= other.y && z <= other.z;
  }

  inline bool operator>(const Vector3 &other) const
  {
    return x > other.x && y > other.y && z > other.z;
  }

  inline bool operator>=(const Vector3 &other) const
  {
    return x >= other.x && y >= other.y && z >= other.z;
  }

  inline bool operator==(const Vector3 &A) const
  {
    return (A.x == x && A.y == y && A.z == z) ? true : false;
  };

  inline bool operator!=(const Vector3 &A) const
  {
    return (A.x == x && A.y == y && A.z == z) ? false : true;
  };

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
    default:
      return 0.0f;
    }
  }

  /**
   * @brief Outputs the component values of a vector to a strem in the format "[x,y,z]".
   * @param stream The stream to output to
   * @param v The vector to output
   */
  inline friend std::ostream &operator<<(std::ostream &stream, const Vector3 &v)
  {
    stream << '[' << v.x << ',' << v.y << ',' << v.z << ']';
    return stream;
  }

  /**
   * @brief Reads component values of a vector from a stream in format "[x,y,z]".
   * @param stream Stream to read from
   * @param v Vector to store values in
   */
  inline friend std::istream &operator>>(std::istream &stream, Vector3 &v)
  {
    float x, y, z;
    char delim;
    stream >> delim >> x >> delim >> y >> delim >> z >> delim;
    v = Vector3(x, y, z);
    return stream;
  }
};
