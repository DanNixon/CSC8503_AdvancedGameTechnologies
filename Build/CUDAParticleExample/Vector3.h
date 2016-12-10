#pragma once

class Vector3
{
public:
  // zero vector
  Vector3();
  Vector3(float v1, float v2, float v3);

  Vector3 operator+(const Vector3 &other) const;
  Vector3 operator-(const Vector3 &other) const;
  Vector3 operator-() const;
  float operator*(const Vector3 &other) const;
  // cross product
  Vector3 operator%(const Vector3 &other) const;
  Vector3 operator*(float value) const;
  float operator[](int i) const
  {
    return data[i];
  }
  float &operator[](int i)
  {
    return data[i];
  }

  Vector3 normalize() const;
  float mag() const;
  float mag2() const;

  float x() const
  {
    return data[0];
  }
  float y() const
  {
    return data[1];
  }
  float z() const
  {
    return data[2];
  }

  friend Vector3 operator*(float value, const Vector3 &vec);

  const float *getData() const
  {
    return data;
  }

private:
  float data[3];
};