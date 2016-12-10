#include "Vector3.h"
#include <math.h>

Vector3::Vector3()
{
  for (int i = 0; i < 3; i++)
    data[i] = 0;
}
Vector3::Vector3(float v1, float v2, float v3)
{
  data[0] = v1;
  data[1] = v2;
  data[2] = v3;
}

Vector3 Vector3::operator+(const Vector3 &other) const
{
  Vector3 result;
  for (int i = 0; i < 3; i++)
    result.data[i] = data[i] + other.data[i];
  return result;
}

Vector3 Vector3::operator-(const Vector3 &other) const
{
  Vector3 result;
  for (int i = 0; i < 3; i++)
    result.data[i] = data[i] - other.data[i];
  return result;
}

Vector3 Vector3::operator-() const
{
  return Vector3(-data[0], -data[1], -data[2]);
}

Vector3 Vector3::operator%(const Vector3 &other) const
{
  return Vector3(y() * other.z() - z() * other.y(), z() * other.x() - x() * other.z(), x() * other.y() - y() * other.x());
}

float Vector3::operator*(const Vector3 &other) const
{
  float result = 0;
  for (int i = 0; i < 3; i++)
    result += data[i] * other.data[i];
  return result;
}

Vector3 Vector3::operator*(float value) const
{
  Vector3 result;
  for (int i = 0; i < 3; i++)
    result.data[i] = data[i] * value;
  return result;
}

Vector3 Vector3::normalize() const
{
  Vector3 result;
  float rmag = 1 / mag();
  for (int i = 0; i < 3; i++)
    result.data[i] = data[i] * rmag;
  return result;
}

float Vector3::mag() const
{
  return sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
}

float Vector3::mag2() const
{
  return data[0] * data[0] + data[1] * data[1] + data[2] * data[2];
}

Vector3 operator*(float value, const Vector3 &vec)
{
  Vector3 result;
  for (int i = 0; i < 3; i++)
    result.data[i] = vec.data[i] * value;
  return result;
}