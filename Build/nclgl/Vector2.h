#pragma once
/*
Class:Vector2
Implements:
Author:Rich Davison
Description:VERY simple Vector2 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*/
#include <iostream>

class Vector2
{
public:
  Vector2(void)
  {
    ToZero();
  }

  Vector2(const float x, const float y)
  {
    this->x = x;
    this->y = y;
  }

  ~Vector2(void)
  {
  }

  float x;
  float y;

  void ToZero()
  {
    x = 0.0f;
    y = 0.0f;
  }

  inline Vector2 operator-(const Vector2 &a) const
  {
    return Vector2(x - a.x, y - a.y);
  }

  inline Vector2 operator+(const Vector2 &a) const
  {
    return Vector2(x + a.x, y + a.y);
  }

  inline float operator[](int i) const
  {
    switch (i)
    {
    case 0:
      return x;
    case 1:
      return y;
    default:
      return 0.0f;
    }
  }

  /**
   * @brief Outputs the component values of a vector to a strem in the format "[x,y]".
   * @param stream The stream to output to
   * @param v The vector to output
   */
  inline friend std::ostream &operator<<(std::ostream &stream, const Vector2 &v)
  {
    stream << '[' << v.x << ',' << v.y << ']';
    return stream;
  }

  /**
   * @brief Reads component values of a vector from a stream in format "[x,y]".
   * @param stream Stream to read from
   * @param v Vector to store values in
   */
  inline friend std::istream &operator>>(std::istream &stream, Vector2 &v)
  {
    float x, y;
    char delim;
    stream >> delim >> x >> delim >> y >> delim;
    v = Vector2(x, y);
    return stream;
  }
};
