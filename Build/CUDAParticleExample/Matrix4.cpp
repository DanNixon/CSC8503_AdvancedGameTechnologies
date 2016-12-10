#define _USE_MATH_DEFINES
#include "Matrix4.h"
#include <math.h>

Matrix4::Matrix4()
{
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      data[c(i, j)] = 0;
}

Matrix4::Matrix4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32,
                 float m33, float m34, float m41, float m42, float m43, float m44)
{
  data[c(0, 0)] = m11;
  data[c(0, 1)] = m12;
  data[c(0, 2)] = m13;
  data[c(0, 3)] = m14;
  data[c(1, 0)] = m21;
  data[c(1, 1)] = m22;
  data[c(1, 2)] = m23;
  data[c(1, 3)] = m24;
  data[c(2, 0)] = m31;
  data[c(2, 1)] = m32;
  data[c(2, 2)] = m33;
  data[c(2, 3)] = m34;
  data[c(3, 0)] = m41;
  data[c(3, 1)] = m42;
  data[c(3, 2)] = m43;
  data[c(3, 3)] = m44;
}

Matrix4 Matrix4::diagonal(float m11, float m22, float m33, float m44)
{
  Matrix4 result;
  result.data[c(0, 0)] = m11;
  result.data[c(1, 1)] = m22;
  result.data[c(2, 2)] = m33;
  result.data[c(3, 3)] = m44;
  return result;
}

Matrix4 Matrix4::translate(const Vector3 &trans)
{
  Matrix4 result = diagonal(1, 1, 1, 1);
  for (int i = 0; i < 3; i++)
    result.data[c(i, 3)] = trans[i];
  return result;
}

// delta tensor (otherwise known as the identity matrix)
#define DELTA(i, j) ((i) == (j) ? 1 : 0)
// finds the k which makes epsilon_ikj non zero
#define MISSING(i, j) ((2 * (i) + 2 * (j)) % 3)
// the only non zero value of epsilon_ikj, given that i and j are fixed, and k varies
#define EPSILON(i, j) ((i) == (j) ? 0 : ((((j) + 1) % 3) == (i) ? 1 : -1))

/**
 * mathematically (s.c.): R_ij = n_i n_j (1 - cos(theta)) + delta_ij cos(theta) + n_k epsilon_ikj sin(theta)
 */
Matrix4 Matrix4::rotateAboutAxis(float angle, const Vector3 &axis)
{
  Matrix4 result;
  Vector3 n = axis.normalize();

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
    {
      result.data[c(i, j)] = n[i] * n[j] * (1 - cos(angle));
      result.data[c(i, j)] += DELTA(i, j) * cos(angle);
      result.data[c(i, j)] += n[MISSING(i, j)] * EPSILON(i, j) * sin(angle);
    }
  result.data[c(3, 3)] = 1;
  return result;
}

// should fix all this, more efficient to use a single constructor
Matrix4 Matrix4::rotateAboutXAxis(float angle)
{
  return rotateAboutAxis(angle, Vector3(1, 0, 0));
}

Matrix4 Matrix4::rotateAboutYAxis(float angle)
{
  return rotateAboutAxis(angle, Vector3(0, 1, 0));
}

Matrix4 Matrix4::rotateAboutZAxis(float angle)
{
  return rotateAboutAxis(angle, Vector3(0, 0, 1));
}

Matrix4 Matrix4::operator*(const Matrix4 &other) const
{
  Matrix4 result;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k < 4; k++)
        result.data[c(i, j)] += data[c(i, k)] * other.data[c(k, j)];
  return result;
}

Vector3 Matrix4::operator*(const Vector3 &other) const
{
  Vector3 result;
  float w = 0;
  for (int i = 0; i < 3; i++)
  {
    result = result + other[i] * cols[i].col;
    w += other[i] * cols[i].w;
  }
  result = result + cols[3].col;
  w += cols[3].w;
  return result * (1.0f / w);
}

Matrix4 Matrix4::operator+(const Matrix4 &other) const
{
  Matrix4 result;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      result.data[c(i, j)] = data[c(i, j)] + other.data[c(i, j)];
  return result;
}

Matrix4 Matrix4::operator-(const Matrix4 &other) const
{
  Matrix4 result;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      result.data[c(i, j)] = data[c(i, j)] - other.data[c(i, j)];
  return result;
}

Matrix4 Matrix4::perspective(float fov, float aspect, float near, float far)
{
  float e = 1.0f / tan(fov * (float)M_PI / 360.0f);
  return Matrix4(e, 0, 0, 0, 0, e / aspect, 0, 0, 0, 0, -(far + near) / (far - near), -2 * far * near / (far - near), 0, 0, -1,
                 0);
}

Matrix4 Matrix4::orthographic(float left, float right, float bottom, float top, float near, float far)
{
  return Matrix4(2 / (right - left), 0, 0, -(right + left) / (right - left), 0, 2 / (top - bottom), 0,
                 -(top + bottom) / (top - bottom), 0, 0, -2 / (far - near), -2 * (far + near) / (far - near), 0, 0, 0, 1);
}

Matrix4 Matrix4::transpose() const
{
  Matrix4 result;
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      result.data[c(i, j)] = data[c(j, i)];
    }
  }
  return result;
}