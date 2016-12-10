#pragma once
#include "Vector3.h"

class Matrix4 {
public:
	// all to zero
	Matrix4();
	Matrix4(float m11, float m12, float m13, float m14,
		float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34,
		float m41, float m42, float m43, float m44);
	
	static Matrix4 diagonal(float m11, float m22, float m33, float m44);
	static Matrix4 translate(const Vector3& trans);
	static Matrix4 rotateAboutAxis(float angle, const Vector3& axis);
	static Matrix4 rotateAboutXAxis(float angle);
	static Matrix4 rotateAboutYAxis(float angle);
	static Matrix4 rotateAboutZAxis(float angle);
	static Matrix4 perspective(float fov, float aspect, float near, float far);
	static Matrix4 orthographic(float left, float right, float bottom, float top, float near, float far);
	Matrix4 transpose() const;
	
	Matrix4 operator*(const Matrix4& other) const;
	Vector3 operator*(const Vector3& other) const;
	Matrix4 operator+(const Matrix4& other) const;
	Matrix4 operator-(const Matrix4& other) const;

	const float* getData() const { return data; }
private:

	// column major order:
	// opposite way to usual as this is how opengl works
	// also matches the vectors
	static int c(int r, int c) { return r + 4*c; }

	union {
		float data[16];
		struct{
			struct {
				Vector3 col;
				float w;
			} cols[4];
		};
	};
};