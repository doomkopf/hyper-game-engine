#ifndef MATHTYPES_H_
#define MATHTYPES_H_

#include <math.h>

void swap(float *, float *);
/*void swap(float *f1, float *f2)
{
	float f = *f1;

	*f1 = *f2;
	*f2 = f;
}*/

class CVector3f
{
public:
	CVector3f()
	{}

	CVector3f(float fx, float fy, float fz)
	{
		x = fx;
		y = fy;
		z = fz;
	}

	void Set(float fx, float fy, float fz)
	{
		x = fx;
		y = fy;
		z = fz;
	}

	void Inverse()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	CVector3f operator+(const CVector3f &add) const
	{
		return CVector3f(
			x + add.x,
			y + add.y,
			z + add.z);
	}

	CVector3f operator-(const CVector3f &sub) const
	{
		return CVector3f(
			x - sub.x,
			y - sub.y,
			z - sub.z);
	}

	CVector3f operator*(float scale) const
	{
		return CVector3f(
			x * scale,
			y * scale,
			z * scale);
	}

	CVector3f operator/(float div) const
	{
		div = 1 / div;

		return CVector3f(
			x * div,
			y * div,
			z * div);
	}

	void operator+=(const CVector3f &add)
	{
		x += add.x;
		y += add.y;
		z += add.z;
	}

	void operator-=(const CVector3f &sub)
	{
		x -= sub.x;
		y -= sub.y;
		z -= sub.z;
	}

	void operator*=(float scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
	}

	void operator/=(float div)
	{
		div = 1 / div;

		x *= div;
		y *= div;
		z *= div;
	}

	float Length() const
	{
		return sqrtf((x * x) + (y * y) + (z * z));
	}

	void Normalize()
	{
		float l = 1 / Length();

		x *= l;
		y *= l;
		z *= l;
	}

	void Normalize(float l)
	{
		l = 1 / l;

		x *= l;
		y *= l;
		z *= l;
	}

	void NormalFromPoints(const CVector3f &p1, const CVector3f &p2, const CVector3f &p3)
	{
		CVector3f v1, v2;

		v1 = p2 - p1;
		v2 = p3 - p1;

		x = (v1.y * v2.z) - (v1.z * v2.y);
		y = (v1.z * v2.x) - (v1.x * v2.z);
		z = (v1.x * v2.y) - (v1.y * v2.x);

		Normalize();
	}

	bool Equal(const CVector3f &v, float epsilon = 0) const
	{
		if((x <= v.x + epsilon && x >= v.x - epsilon) &&
			(y <= v.y + epsilon && y >= v.y - epsilon) &&
			(z <= v.z + epsilon && z >= v.z - epsilon))
			return true;

		return false;
	}

	float x, y, z;
};

class CMatrix3x3
{
public:
	CMatrix3x3()
	{
		//Identity();
	}

	CMatrix3x3(const CVector3f &v1, const CVector3f &v2, const CVector3f &v3)
	{
		Set(v1, v2, v3);
	}

	void Set(const CVector3f &v1, const CVector3f &v2, const CVector3f &v3)
	{
		m[0] = v1.x; m[3] = v2.x; m[6] = v3.x;
		m[1] = v1.y; m[4] = v2.y; m[7] = v3.y;
		m[2] = v1.z; m[5] = v2.z; m[8] = v3.z;
	}

	void Identity()
	{
		m[0] = 1; m[3] = 0; m[6] = 0;
		m[1] = 0; m[4] = 1; m[7] = 0;
		m[2] = 0; m[5] = 0; m[8] = 1;
	}

	void Transpose()
	{
		swap(&m[1], &m[3]);
		swap(&m[2], &m[6]);
		swap(&m[5], &m[7]);
	}

	void RotationX(float angle)
	{
		float s,c;

		s = sinf(angle);
		c = cosf(angle);

		m[0] =  1; m[3] =  0; m[6] =  0;
		m[1] =  0; m[4] =  c; m[7] =  s;
		m[2] =  0; m[5] = -s; m[8] =  c;
	}

	void RotationY(float angle)
	{
		float s,c;

		s = sinf(angle);
		c = cosf(angle);

		m[0] =  c; m[3] =  0; m[6] = -s;
		m[1] =  0; m[4] =  1; m[7] =  0;
		m[2] =  s; m[5] =  0; m[8] =  c;
	}

	void RotationZ(float angle)
	{
		float s,c;

		s = sinf(angle);
		c = cosf(angle);

		m[0] =  c; m[3] =  s; m[6] =  0;
		m[1] = -s; m[4] =  c; m[7] =  0;
		m[2] =  0; m[5] =  0; m[8] =  1;
	}

	void RotateX(float angle)
	{
		CMatrix3x3 tm;

		tm.RotationX(angle);
		Multiply(tm);
	}

	void RotateY(float angle)
	{
		CMatrix3x3 tm;

		tm.RotationY(angle);
		Multiply(tm);
	}

	void RotateZ(float angle)
	{
		CMatrix3x3 tm;

		tm.RotationZ(angle);
		Multiply(tm);
	}

	void Multiply(const CMatrix3x3 &m3x3)
	{
		CMatrix3x3 tm = *this;
		Multiply(tm, m3x3);
	}

	void Multiply(const CMatrix3x3 &m3x3_1, const CMatrix3x3 &m3x3_2)
	{
		m[0] = m3x3_1.m[0] * m3x3_2.m[0] +
					m3x3_1.m[3] * m3x3_2.m[1] +
					m3x3_1.m[6] * m3x3_2.m[2];
		m[1] = m3x3_1.m[1] * m3x3_2.m[0] +
					m3x3_1.m[4] * m3x3_2.m[1] +
					m3x3_1.m[7] * m3x3_2.m[2];
		m[2] = m3x3_1.m[2] * m3x3_2.m[0] +
					m3x3_1.m[5] * m3x3_2.m[1] +
					m3x3_1.m[8] * m3x3_2.m[2];
		m[3] = m3x3_1.m[0] * m3x3_2.m[3] +
					m3x3_1.m[3] * m3x3_2.m[4] +
					m3x3_1.m[6] * m3x3_2.m[5];
		m[4] = m3x3_1.m[1] * m3x3_2.m[3] +
					m3x3_1.m[4] * m3x3_2.m[4] +
					m3x3_1.m[7] * m3x3_2.m[5];
		m[5] = m3x3_1.m[2] * m3x3_2.m[3] +
					m3x3_1.m[5] * m3x3_2.m[4] +
					m3x3_1.m[8] * m3x3_2.m[5];
		m[6] = m3x3_1.m[0] * m3x3_2.m[6] +
					m3x3_1.m[3] * m3x3_2.m[7] +
					m3x3_1.m[6] * m3x3_2.m[8];
		m[7] = m3x3_1.m[1] * m3x3_2.m[6] +
					m3x3_1.m[4] * m3x3_2.m[7] +
					m3x3_1.m[7] * m3x3_2.m[8];
		m[8] = m3x3_1.m[2] * m3x3_2.m[6] +
					m3x3_1.m[5] * m3x3_2.m[7] +
					m3x3_1.m[8] * m3x3_2.m[8];
	}

	void operator *=(const CMatrix3x3 &m3x3)
	{
		Multiply(m3x3);
	}

	float m[9];

protected:
	//static CMatrix3x3 tm;
};

class CMatrix4x4
{
public:
	CMatrix4x4()
	{
		//Identity();
	}

	CMatrix4x4(const CVector3f &v1, const CVector3f &v2, const CVector3f &v3, const CVector3f &v4)
	{
		Set(v1, v2, v3, v4);
	}

	void Set(const CVector3f &v1, const CVector3f &v2, const CVector3f &v3, const CVector3f &v4)
	{
		m[0] = v1.x; m[4] = v2.x;  m[8] = v3.x; m[12] = v4.x;
		m[1] = v1.y; m[5] = v2.y;  m[9] = v3.y; m[13] = v4.y;
		m[2] = v1.z; m[6] = v2.z; m[10] = v3.z; m[14] = v4.z;
		m[3] = 0;    m[7] = 0;    m[11] = 0;    m[15] = 1;
	}

	bool Equal(const CMatrix4x4 &m4x4)
	{
		if(m[0] == m4x4.m[0] &&
			m[1] == m4x4.m[1] &&
			m[2] == m4x4.m[2] &&
			m[3] == m4x4.m[3] &&
			m[4] == m4x4.m[4] &&
			m[5] == m4x4.m[5] &&
			m[6] == m4x4.m[6] &&
			m[7] == m4x4.m[7] &&
			m[8] == m4x4.m[8] &&
			m[9] == m4x4.m[9] &&
			m[10] == m4x4.m[10] &&
			m[11] == m4x4.m[11] &&
			m[12] == m4x4.m[12] &&
			m[13] == m4x4.m[13] &&
			m[14] == m4x4.m[14] &&
			m[15] == m4x4.m[15])
			return true;

		return false;
	}

	void Identity()
	{
		m[0] = 1; m[4] = 0;  m[8] = 0; m[12] = 0;
		m[1] = 0; m[5] = 1;  m[9] = 0; m[13] = 0;
		m[2] = 0; m[6] = 0; m[10] = 1; m[14] = 0;
		m[3] = 0; m[7] = 0; m[11] = 0; m[15] = 1;
	}

	void Transpose()
	{
		swap(&m[1], &m[4]);
		swap(&m[2], &m[8]);
		swap(&m[3], &m[12]);
		swap(&m[7], &m[13]);
		swap(&m[11], &m[14]);
		swap(&m[6], &m[9]);
	}

	void RotationX(float angle)
	{
		float s, c;

		s = sinf(angle);
		c = cosf(angle);

		m[0] =  1; m[4] =  0;  m[8] =  0; m[12] = 0;
		m[1] =  0; m[5] =  c;  m[9] =  s; m[13] = 0;
		m[2] =  0; m[6] = -s; m[10] =  c; m[14] = 0;
		m[3] =  0; m[7] =  0; m[11] =  0; m[15] = 1;
	}

	void RotationY(float angle)
	{
		float s, c;

		s = sinf(angle);
		c = cosf(angle);

		m[0] =  c; m[4] =  0;  m[8] = -s; m[12] = 0;
		m[1] =  0; m[5] =  1;  m[9] =  0; m[13] = 0;
		m[2] =  s; m[6] =  0; m[10] =  c; m[14] = 0;
		m[3] =  0; m[7] =  0; m[11] =  0; m[15] = 1;
	}

	void RotationZ(float angle)
	{
		float s, c;

		s = sinf(angle);
		c = cosf(angle);

		m[0] =  c; m[4] =  s;  m[8] =  0; m[12] = 0;
		m[1] = -s; m[5] =  c;  m[9] =  0; m[13] = 0;
		m[2] =  0; m[6] =  0; m[10] =  1; m[14] = 0;
		m[3] =  0; m[7] =  0; m[11] =  0; m[15] = 1;
	}

	void Translation(float x, float y, float z)
	{
		m[0] = 1;       m[4] = 0;        m[8] = 0;       m[12] = 0;//trans.x;
		m[1] = 0;       m[5] = 1;        m[9] = 0;       m[13] = 0;//trans.y;
		m[2] = 0;       m[6] = 0;       m[10] = 1;       m[14] = 0;//trans.z;
		m[3] = x;       m[7] = y;       m[11] = z;       m[15] = 1;
		//m[3] = 0;     m[7] = 0;       m[11] = 0;       m[15] = 1;
	}

	void RotateX(float angle)
	{
		CMatrix4x4 tm;

		tm.RotationX(angle);
		Multiply(tm);
	}

	void RotateY(float angle)
	{
		CMatrix4x4 tm;

		tm.RotationY(angle);
		Multiply(tm);
	}

	void RotateZ(float angle)
	{
		CMatrix4x4 tm;

		tm.RotationZ(angle);
		Multiply(tm);
	}

	void Rotate(CMatrix3x3 m3x3)
	{
		CMatrix4x4 tm1 = *this, tm2;

		m3x3.Transpose();

		tm2.From3x3(m3x3);
		Multiply(tm2, tm1);
	}

	void Translate(float x, float y, float z)
	{
		CMatrix4x4 tm1 = *this, tm2;

		tm2.Translation(x, y, z);
		Multiply(tm2, tm1);
	}

	void Multiply(const CMatrix4x4 &m4x4)
	{
		CMatrix4x4 tm = *this;
		Multiply(tm, m4x4);
	}

	void operator *=(const CMatrix4x4 &m4x4)
	{
		Multiply(m4x4);
	}

	void Multiply(const CMatrix4x4 &m4x4_1, const CMatrix4x4 &m4x4_2)
	{
		m[0] = m4x4_1.m[0] * m4x4_2.m[0] +
					m4x4_1.m[4] * m4x4_2.m[1] +
					m4x4_1.m[8] * m4x4_2.m[2] +
					m4x4_1.m[12] * m4x4_2.m[3];
		m[1] = m4x4_1.m[1] * m4x4_2.m[0] +
					m4x4_1.m[5] * m4x4_2.m[1] +
					m4x4_1.m[9] * m4x4_2.m[2] +
					m4x4_1.m[13] * m4x4_2.m[3];
		m[2] = m4x4_1.m[2] * m4x4_2.m[0] +
					m4x4_1.m[4] * m4x4_2.m[1] +
					m4x4_1.m[10] * m4x4_2.m[2] +
					m4x4_1.m[14] * m4x4_2.m[3];
		m[3] = m4x4_1.m[3] * m4x4_2.m[0] +
					m4x4_1.m[7] * m4x4_2.m[1] +
					m4x4_1.m[11] * m4x4_2.m[2] +
					m4x4_1.m[15] * m4x4_2.m[3];
		m[4] = m4x4_1.m[0] * m4x4_2.m[4] +
					m4x4_1.m[4] * m4x4_2.m[5] +
					m4x4_1.m[8] * m4x4_2.m[6] +
					m4x4_1.m[12] * m4x4_2.m[7];
		m[5] = m4x4_1.m[1] * m4x4_2.m[4] +
					m4x4_1.m[5] * m4x4_2.m[5] +
					m4x4_1.m[9] * m4x4_2.m[6] +
					m4x4_1.m[13] * m4x4_2.m[7];
		m[6] = m4x4_1.m[2] * m4x4_2.m[4] +
					m4x4_1.m[6] * m4x4_2.m[5] +
					m4x4_1.m[10] * m4x4_2.m[6] +
					m4x4_1.m[14] * m4x4_2.m[7];
		m[7] = m4x4_1.m[3] * m4x4_2.m[4] +
					m4x4_1.m[7] * m4x4_2.m[5] +
					m4x4_1.m[11] * m4x4_2.m[6] +
					m4x4_1.m[15] * m4x4_2.m[7];
		m[8] = m4x4_1.m[0] * m4x4_2.m[8] +
					m4x4_1.m[4] * m4x4_2.m[9] +
					m4x4_1.m[8] * m4x4_2.m[10] +
					m4x4_1.m[12] * m4x4_2.m[11];
		m[9] = m4x4_1.m[1] * m4x4_2.m[8] +
					m4x4_1.m[5] * m4x4_2.m[9] +
					m4x4_1.m[9] * m4x4_2.m[10] +
					m4x4_1.m[13] * m4x4_2.m[11];
		m[10] = m4x4_1.m[2] * m4x4_2.m[8] +
					m4x4_1.m[6] * m4x4_2.m[9] +
					m4x4_1.m[10] * m4x4_2.m[10] +
					m4x4_1.m[14] * m4x4_2.m[11];
		m[11] = m4x4_1.m[3] * m4x4_2.m[8] +
					m4x4_1.m[7] * m4x4_2.m[9] +
					m4x4_1.m[11] * m4x4_2.m[10] +
					m4x4_1.m[15] * m4x4_2.m[11];
		m[12] = m4x4_1.m[0] * m4x4_2.m[12] +
					m4x4_1.m[4] * m4x4_2.m[13] +
					m4x4_1.m[8] * m4x4_2.m[14] +
					m4x4_1.m[12] * m4x4_2.m[15];
		m[13] = m4x4_1.m[1] * m4x4_2.m[12] +
					m4x4_1.m[5] * m4x4_2.m[13] +
					m4x4_1.m[9] * m4x4_2.m[14] +
					m4x4_1.m[13] * m4x4_2.m[15];
		m[14] = m4x4_1.m[2] * m4x4_2.m[12] +
					m4x4_1.m[6] * m4x4_2.m[13] +
					m4x4_1.m[10] * m4x4_2.m[14] +
					m4x4_1.m[14] * m4x4_2.m[15];
		m[15] = m4x4_1.m[3] * m4x4_2.m[12] +
					m4x4_1.m[7] * m4x4_2.m[13] +
					m4x4_1.m[11] * m4x4_2.m[14] +
					m4x4_1.m[15] * m4x4_2.m[15];
	}

	void PerspectiveRH(float fov, float aspect, float znear, float zfar)
	{
		float f = 1 / tanf(fov / 2); // cot(fov / 2)

		m[0] = f / aspect; m[4] = 0; m[8] = 0;                              m[12] =  0;
		m[1] = 0;          m[5] = f; m[9] = 0;                              m[13] =  0;
		m[2] = 0;          m[6] = 0; m[10] = zfar / (znear - zfar);         m[14] = -1;
		m[3] = 0;          m[7] = 0; m[11] = zfar * znear / (znear - zfar); m[15] =  0;
	}

	void PerspectiveLH(float fov, float aspect, float znear, float zfar)
	{
		float f = 1 / tanf(fov / 2); // cot(fov / 2)

		m[0] = f / aspect; m[4] = 0; m[8] =  0;                              m[12] =  0;
		m[1] = 0;          m[5] = f; m[9] =  0;                              m[13] =  0;
		m[2] = 0;          m[6] = 0; m[10] = zfar / (zfar - znear);          m[14] =  1;
		m[3] = 0;          m[7] = 0; m[11] = -znear * zfar / (zfar - znear); m[15] =  0;
	}

	void OrthographicRH(float w, float h, float znear, float zfar)
	{
		m[0] = 2 / w; m[4] = 0;     m[8] = 0;                       m[12] = 0;
		m[1] = 0;     m[5] = 2 / h; m[9] = 0;                       m[13] = 0;
		m[2] = 0;     m[6] = 0;     m[10] = 1 / (znear - zfar);     m[14] = 0;
		m[3] = 0;     m[7] = 0;     m[11] = znear / (znear - zfar); m[15] = 1;
	}

	void OrthographicLH(float w, float h, float znear, float zfar)
	{
		m[0] = 2 / w; m[4] = 0;     m[8] = 0;                       m[12] = 0;
		m[1] = 0;     m[5] = 2 / h; m[9] = 0;                       m[13] = 0;
		m[2] = 0;     m[6] = 0;     m[10] = 1 / (zfar - znear);     m[14] = 0;
		m[3] = 0;     m[7] = 0;     m[11] = znear / (znear - zfar); m[15] = 1;
	}

	void View2D(int x, int y)
	{
		m[0] = 1;             m[4] =  0;           m[8] = 0;  m[12] = 0;
		m[1] = 0;             m[5] = -1;           m[9] = 0;  m[13] = 0;
		m[2] = 0;             m[6] =  0;           m[10] = 1; m[14] = 0;
		m[3] = -float(x / 2); m[7] = float(y / 2); m[11] = 0; m[15] = 1;
	}

	void From3x3(const CMatrix3x3 &m3x3)
	{
		m[0] = m3x3.m[0]; m[4] = m3x3.m[3]; m[8] = m3x3.m[6];  m[12] = 0;
		m[1] = m3x3.m[1]; m[5] = m3x3.m[4]; m[9] = m3x3.m[7];  m[13] = 0;
		m[2] = m3x3.m[2]; m[6] = m3x3.m[5]; m[10] = m3x3.m[8]; m[14] = 0;
		m[3] = 0;         m[7] = 0;         m[11] = 0;         m[15] = 1;
	}

	float m[16];

protected:
	//static CMatrix4x4 tm;
};

class CQuaternion
{
public:
	CQuaternion()
	{
		Identity();
	}

	CQuaternion(float fx, float fy, float fz, float fw)
	{
		Set(fx, fy, fz, fw);
	}

	void Identity()
	{
		x = y = z = 0;
		w = 1;
	}

	void Set(float fx, float fy, float fz, float fw)
	{
		x = fx;
		y = fy;
		z = fz;
		w = fw;
	}

	void AxisAngle(const CVector3f &axis, float f)
	{
		f *= 0.5f;

		w = cosf(f);
		f = sinf(f);
		x = axis.x * f;
		y = axis.y * f;
		z = axis.z * f;
	}

	void GetMatrix(CMatrix3x3 *m) const
	{
		m->m[0] = 1.0f - 2.0f * ( y * y + z * z );
		m->m[1] = 2.0f * ( x * y - w * z );
		m->m[2] = 2.0f * ( x * z + w * y );

		m->m[3] = 2.0f * ( x * y + w * z );
		m->m[4] = 1.0f - 2.0f * ( x * x + z * z );
		m->m[5] = 2.0f * ( y * z - w * x );

		m->m[6] = 2.0f * ( x * z - w * y );
		m->m[7] = 2.0f * ( y * z + w * x );
		m->m[8] = 1.0f - 2.0f * ( x * x + y * y );
	}

	float x, y, z, w;
};

class CPlane
{
public:
	CPlane()
	{}

	CPlane(float A, float B, float C, float D)
	{
		normal.x = A;
		normal.y = B;
		normal.z = C;
		d = D;
	}

	void Create(const CVector3f &v1, const CVector3f &v2, const CVector3f &v3)
	{
		CVector3f p1, p2;

		p1 = v2 - v1;
		p2 = v3 - v1;

		//normal=CrossProduct(normal,v3-v1);
		normal.x = (p1.y * p2.z) - (p1.z * p2.y);
		normal.y = (p1.z * p2.x) - (p1.x * p2.z);
		normal.z = (p1.x * p2.y) - (p1.y * p2.x);

		normal.Normalize();

		d = -((normal.x * v1.x) + (normal.y * v1.y) + (normal.z * v1.z));
	}

	CVector3f normal;
	float d;
};

// --------------------------------------------------------------------

class CVector2d
{
public:
	CVector2d()
	{}

	CVector2d(int ix, int iy)
	{
		x = ix;
		y = iy;
	}

	CVector2d operator +(const CVector2d &v) const
	{
		CVector2d vt;

		vt.x = x + v.x;
		vt.y = y + v.y;

		return vt;
	}

	CVector2d operator -(const CVector2d &v) const
	{
		CVector2d vt;

		vt.x = x - v.x;
		vt.y = y - v.y;

		return vt;
	}

	CVector2d operator *(float f) const
	{
		CVector2d vt;

		vt.x = int(x * f);
		vt.y = int(y * f);

		return vt;
	}

	void operator +=(const CVector2d &v)
	{
		x += v.x;
		y += v.y;
	}

	void operator -=(const CVector2d &v)
	{
		x -= v.x;
		y -= v.y;
	}

	void operator *=(float f)
	{
		x = int(x * f);
		y = int(y * f);
	}

	float Length() const
	{
		return sqrtf(float((x * x) + (y * y)));
	}

	int x, y;
};

class CVector2f
{
public:
	CVector2f()
	{}

	CVector2f(float fx, float fy)
	{
		x = fx;
		y = fy;
	}

	CVector2f operator +(const CVector2f &v) const
	{
		CVector2f vt;

		vt.x = x + v.x;
		vt.y = y + v.y;

		return vt;
	}

	CVector2f operator -(const CVector2f &v) const
	{
		CVector2f vt;

		vt.x = x - v.x;
		vt.y = y - v.y;

		return vt;
	}

	void operator +=(const CVector2f &v)
	{
		x += v.x;
		y += v.y;
	}

	void operator -=(const CVector2f &v)
	{
		x -= v.x;
		y -= v.y;
	}

	float Length() const
	{
		return sqrtf((x * x) + (y * y));
	}

	float x, y;
};

#endif