/*
 * CES Utilities
 *
 * Copyright(c) FUJITSU NAGANO SYSTEMS ENGINEERING LIMITED
 *      CES Project, 2002-2004, All Right Reserved.
 */

#ifndef _CES_UTIL_MATH_H_
#define _CES_UTIL_MATH_H_

#include <stdio.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace CES {

  //--------------------------------------------------------------------
  // Vec3 : (x, y, z)=(m_v[0], m_v[1], m_v[2])
  //
  template <class T> struct Vec3 {
    T m_v[3];
    Vec3() {m_v[0] = m_v[1] = m_v[2] = (T)0;}
    Vec3(const T& x, const T& y, const T& z) {
      m_v[0] = x; m_v[1] = y; m_v[2] = z;
    }
    Vec3(const Vec3<T>& org) {*this = org;}
    Vec3(const T* org) {*this = org;}

    void operator=(const Vec3<T>& org) {
      m_v[0] = org.m_v[0]; m_v[1] = org.m_v[1]; m_v[2] = org.m_v[2];
    }
    void operator=(const T* v) {
      m_v[0] = v[0]; m_v[1] = v[1]; m_v[2] = v[2];
    }
    T& operator[](const size_t i) {return m_v[i];}
    T operator[](const size_t i) const {return m_v[i];}

    Vec3<T> operator+(const Vec3<T>& t) const {
      Vec3<T> x(m_v);
      x.m_v[0] += t.m_v[0]; x.m_v[1] += t.m_v[1]; x.m_v[2] += t.m_v[2];
      return x;
    }
    Vec3<T> operator-(const Vec3<T>& t) const {
      Vec3<T> x(m_v);
      x.m_v[0] -= t.m_v[0]; x.m_v[1] -= t.m_v[1]; x.m_v[2] -= t.m_v[2];
      return x;
    }
    Vec3<T> operator*(const T& v) const {
      Vec3<T> x(m_v);
      x.m_v[0] *= v; x.m_v[1] *= v; x.m_v[2] *= v;
      return x;
    }

    // dot product
    T operator|(const Vec3<T>& t) const {
      T x = m_v[0]*t.m_v[0] + m_v[1]*t.m_v[1] + m_v[2]*t.m_v[2];
      return x;
    }
    // cross product
    Vec3<T> operator^(const Vec3<T>& t) const {
      Vec3<T> x;
      x.m_v[0] = m_v[1]*t.m_v[2] - m_v[2]*t.m_v[1];
      x.m_v[1] = m_v[2]*t.m_v[0] - m_v[0]*t.m_v[2];
      x.m_v[2] = m_v[0]*t.m_v[1] - m_v[1]*t.m_v[0];
      return x;
    }

    void GetVal(float a[3]) const {
      a[0] = m_v[0]; a[1] = m_v[1]; a[2] = m_v[2];
    }

    T Length() const {
      return (T)sqrt(m_v[0]*m_v[0] + m_v[1]*m_v[1] + m_v[2]*m_v[2]);
    }
    void UnitVec() {
      T len = Length();
      if ( (double)len < 1e-8 ) {*this = Vec3<T>(0,0,1); return;}
      m_v[0] /= len; m_v[1] /= len; m_v[2] /= len;
    }
  };

  //--------------------------------------------------------------------
  // Mat4 : 4x4 matrix
  //        m_v[ 0] m_v[ 4] m_v[ 8] m_v[12]
  //        m_v[ 1] m_v[ 5] m_v[ 9] m_v[13]
  //        m_v[ 2] m_v[ 6] m_v[10] m_v[14]
  //        m_v[ 3] m_v[ 7] m_v[11] m_v[15]
  //
  template <class T> struct Mat4 {
    T m_v[16];
    Mat4() {Identity();}
    Mat4(const Mat4<T>& org) {*this = org;}
    Mat4(const T* org) {*this = org;}
    
    void operator=(const Mat4<T>& org) {
      for ( register size_t i = 0; i < 16; i++ )
	m_v[i] = org.m_v[i];
    }
    void operator=(const T* v) {
      for ( register size_t i = 0; i < 16; i++ )
	m_v[i] = v[i];
    }
    T& operator[](size_t i) {return m_v[i];}
    T operator[](size_t i) const {return m_v[i];}

    void Identity() {
      for ( register size_t i = 1; i < 15; i++ )
	m_v[i] = (T)0;
      m_v[0] = m_v[5] = m_v[10] = m_v[15] = (T)1;
    }

    Mat4<T> operator*(const Mat4<T>& m) const {
      Mat4<T> w;
      register size_t i, j, k;
      for ( i = 0; i < 4; i++ ) {
	for ( j = 0; j < 4; j++ ) {
	  w.m_v[i*4 +j] = (T)0;
	  for ( k = 0; k < 4; k++)
	    w.m_v[i*4 +j] += (m_v[j + k*4] * m.m_v[i*4 + k]);
	}
      }
      return w;
    }
    Vec3<T> operator*(const Vec3<T>& v) const {
      Vec3<T> w;
      register size_t i, k;
      for ( i = 0; i < 3; i++ ) {
	w.m_v[i] = (T)0;
	for ( k = 0; k < 3; k++)
	  w.m_v[i] += (m_v[i + k*4] * v.m_v[k]);
	w.m_v[i] += m_v[i + k*4];
      }
      return w;
    }
    void Scale(const Vec3<T>& s) {
      Mat4<T> w;
      w.m_v[0] = s.m_v[0]; w.m_v[5] = s.m_v[1]; w.m_v[10] = s.m_v[2];
      *this = (*this) * w;
    }
    void Translate(const Vec3<T>& t) {
      Mat4<T> w;
      w.m_v[12] = t.m_v[0]; w.m_v[13] = t.m_v[1]; w.m_v[14] = t.m_v[2];
      *this = (*this) * w;
    }
    void RotX(const T rx) {
      Mat4<T> w;
      w.m_v[ 5] = (T)cos((double)rx);
      w.m_v[ 6] = (T)sin((double)rx);
      w.m_v[ 9] =-(T)sin((double)rx);
      w.m_v[10] = (T)cos((double)rx);
      *this = (*this) * w;
    }
    void RotY(const T ry) {
      Mat4<T> w;
      w.m_v[ 0] = (T)cos((double)ry);
      w.m_v[ 2] =-(T)sin((double)ry);
      w.m_v[ 8] = (T)sin((double)ry);
      w.m_v[10] = (T)cos((double)ry);
      *this = (*this) * w;
    }
    void RotZ(const T rz) {
      Mat4<T> w;
      w.m_v[ 0] = (T)cos((double)rz);
      w.m_v[ 1] = (T)sin((double)rz);
      w.m_v[ 4] =-(T)sin((double)rz);
      w.m_v[ 5] = (T)cos((double)rz);
      *this = (*this) * w;
    }
    void Rotation(const T a, const Vec3<T>& v) {
      register size_t i, j, k;
      Mat4<T> m, uut, ciu, s;
      T sa = (T)sin((double)a);
      T ca = (T)cos((double)a);
      Vec3<T> u(v); u.UnitVec();
    
      uut.m_v[0] = u.m_v[0]*u.m_v[0];
      uut.m_v[5] = u.m_v[1]*u.m_v[1];
      uut.m_v[10]= u.m_v[2]*u.m_v[2];
      uut.m_v[1] = uut.m_v[4] = u.m_v[0] * u.m_v[1];
      uut.m_v[2] = uut.m_v[8] = u.m_v[0] * u.m_v[2];
      uut.m_v[6] = uut.m_v[9] = u.m_v[1] * u.m_v[2];
      for ( i = 0; i < 3; i++ )
	for ( j = 0; j < 3; j++ ) {
          k = j*4 + i;
	  ciu.m_v[k] -= uut.m_v[k];
	  ciu.m_v[k] *= ca;
	}
      s.m_v[0] = s.m_v[5] = s.m_v[10] = (T)0;
      s.m_v[1] =  u.m_v[2] * sa;
      s.m_v[2] = -u.m_v[1] * sa;
      s.m_v[4] = -u.m_v[2] * sa;
      s.m_v[6] =  u.m_v[0] * sa;
      s.m_v[8] =  u.m_v[1] * sa;
      s.m_v[9] = -u.m_v[0] * sa;
      for ( i = 0; i < 3; i++ )
	for ( j = 0; j < 3; j++ ) {
          k = j*4 + i;
	  m.m_v[k] = uut.m_v[k] + ciu.m_v[k] + s.m_v[k];
        }

      *this = (*this) * m;
    }

    Mat4<T> Transpose() const {
      Mat4<T> x = (*this);
      x.m_v[ 4] = m_v[ 1]; x.m_v[ 1] = m_v[ 4];
      x.m_v[ 8] = m_v[ 2]; x.m_v[ 2] = m_v[ 8];
      x.m_v[12] = m_v[ 3]; x.m_v[ 3] = m_v[12];
      x.m_v[ 9] = m_v[ 6]; x.m_v[ 6] = m_v[ 9];
      x.m_v[13] = m_v[ 7]; x.m_v[ 7] = m_v[13];
      x.m_v[14] = m_v[11]; x.m_v[11] = m_v[14];
      return x;
    }
    Mat4<T> Inverse() const {
      Mat4<T> RET;
      const int ip = 4;
      register int i, j, k;
      int l[4], m[4];
      double b, t;
      double r[4][4] = {
	{(double)m_v[ 0], (double)m_v[ 4], (double)m_v[ 8], (double)m_v[12]},
	{(double)m_v[ 1], (double)m_v[ 5], (double)m_v[ 9], (double)m_v[13]},
	{(double)m_v[ 2], (double)m_v[ 6], (double)m_v[10], (double)m_v[14]},
	{(double)m_v[ 3], (double)m_v[ 7], (double)m_v[11], (double)m_v[15]}
      };
      for ( k = 0; k < ip; k++ ) {
	m[k] = l[k] = k;
	b = r[k][k];
	for ( j = k; j < ip; j++ )
	  for ( i = k; i < ip; i++ )
	    if ( fabs(r[j][i]) > fabs(b) ) {
	      b = r[j][i]; l[k] = i; m[k] = j;
	    }
	if ( fabs(b) < 1e-12 ) {
	  for ( i = 0; i < 16; i++ ) RET.m_v[i] = (T)0;
	  return RET; // Failed: returns Zero.
	}
	j = l[k];
	if ( j > k )
	  for ( i = 0; i < ip; i++ ) {
	    t = -r[i][k];
	    r[i][k] = r[i][j];
	    r[i][j] = t;
	  }
	i = m[k];
	if ( i > k )
	  for ( j = 0; j < ip; j++ ) {
	    t = -r[k][j];
	    r[k][j] = r[i][j];
	    r[i][j] = t;
	  }
	for ( i = 0; i < ip; i++ )
	  if ( i != k ) r[k][i] /= -b;
	for ( i = 0; i < ip; i++ )
	  for ( j = 0; j < ip; j++ )
	    if ( i != k && j != k ) r[j][i] += r[k][i] * r[j][k];
	for ( j = 0; j < ip; j++ )
	  if ( j != k ) r[j][k] /= b;
	r[k][k] = 1. / b;
      }
      for ( k = ip - 2; k >= 0; k-- ) {
	i = l[k];
	if ( i > k )
	  for ( j = 0; j < ip; j++ ) {
	    t = r[k][j];
	    r[k][j] = -r[i][j];
	    r[i][j] = t;
	  }
	j = m[k];
	if ( j > k )
	  for ( i = 0; i < ip; i++ ) {
	    t = r[i][k];
	    r[i][k] = -r[i][j];
	    r[i][j] = t;
	  }
      }
      
      RET.m_v[ 0] = (T)r[0][0]; RET.m_v[ 1] = (T)r[1][0];
      RET.m_v[ 2] = (T)r[2][0]; RET.m_v[ 3] = (T)r[3][0];
      RET.m_v[ 4] = (T)r[0][1]; RET.m_v[ 5] = (T)r[1][1];
      RET.m_v[ 6] = (T)r[2][1]; RET.m_v[ 7] = (T)r[3][1];
      RET.m_v[ 8] = (T)r[0][2]; RET.m_v[ 9] = (T)r[1][2];
      RET.m_v[10] = (T)r[2][2]; RET.m_v[11] = (T)r[3][2];
      RET.m_v[12] = (T)r[0][3]; RET.m_v[13] = (T)r[1][3];
      RET.m_v[14] = (T)r[2][3]; RET.m_v[15] = (T)r[3][3];
      return RET;
    }
  };

  //--------------------------------------------------------------------
  // Quaternions : (xi, yj, zk, w) = (m_v[0], m_v[1], m_v[2], m_w)
  //
  template <class T> struct Quat4 {
    Vec3<T> m_v;
    T       m_w;
    Quat4() {m_w = (T)1;}
    Quat4(const Vec3<T>& v, const T& w) {m_v = v; m_w = w;}
    Quat4(const T& x, const T& y, const T& z, const T& w) {
      m_v = Vec3<T>(x, y, z); m_w = w;
    }
    Quat4(const Quat4<T>& org) {*this = org;}
    Quat4(const T* org) {*this = org;}

    void operator=(const Quat4<T>& org) {
      m_v = org.m_v; m_w = org.m_w;
    }
    void operator=(const T* v) {
      m_v = Vec3<T>(v[0], v[1], v[2]); m_w = v[3];
    }
    T& operator[](const size_t i) {
      switch ( i ) {
      case 0: case 1: case 2: return m_v[i];
      case 3: default: return m_w;
      }
    }
    T operator[](const size_t i) const {
      switch ( i ) {
      case 0: case 1: case 2: return m_v[i];
      case 3: default: return m_w;
      }
    }
    void GetVal(float a[4]) const {
      a[0] = m_v[0]; a[1] = m_v[1]; a[2] = m_v[2]; a[3] = m_w;
    }

    Quat4<T> Conjugate() const {
      Quat4<T> x(*this);
      x.m_v[0] = -x.m_v[0]; x.m_v[1] = -x.m_v[1]; x.m_v[2] = -x.m_v[2];
      return x;
    }

    T Norm() const {return (T)(m_w*m_w + (m_v|m_v));}
    T Length() const {return (T)sqrt((double)Norm());}
    Quat4<T> Inverse() const {return Conjugate() / Norm();}

    void UnitQuat() {
      T len = Length();
      if ( (double)len < 1e-8 ) {*this = Quat4<T>(0,0,0,1); return;}
      m_v[0] /= len; m_v[1] /= len; m_v[2] /= len; m_v[3] /= len;
    }

    Quat4<T> operator+(const Quat4<T>& t) const {
      Quat4<T> x;
      x.m_v = this->m_v + t.m_v;
      x.m_w = this->m_w + t.m_w;
      return x;
    }
    Quat4<T> operator-(const Quat4<T>& t) const {
      Quat4<T> x;
      x.m_v = this->m_v - t.m_v;
      x.m_w = this->m_w - t.m_w;
      return x;
    }

    // dot product of imagenary part
    T operator|(const Quat4<T>& t) const {return (m_v|t.m_v);}

    // cross product of imagenary part
    Vec3<T> operator^(const Vec3<T>& t) const {return (m_v^t.m_v);}

    // multiplication : returns (v x v' + w v' + w'v, w w' - (v,v'))
    Quat4<T> operator*(const Quat4<T>& t) const {
      Quat4<T> x;
      x.m_v = (*this)^t + t.m_v * this->m_w + this->m_v * t.m_w;
      x.m_w = this->m_w * t.m_w - (*this)|t;
      return x;
    }

    Quat4<T> operator*(const T t) const {
      Quat4<T> x(*this);
      x[0] *= t; x[1] *= t; x[2] *= t; x[3] *= t;
      return x;
    }

    void Rotation(const T a, const Vec3<T>& v) {
      Vec3<T> u(v); u.UnitVec();
      m_v = u * (T)sin((double)a/2.0);
      m_w = (T)cos((double)a/2.0);
    }
    Mat4<T> GetRotMat() const {
      Mat4<T> mat;
      T N = this->Norm(); if ( (double)N < 1e-8 ) return mat;
      N = (T)(2. / N);
      T xx, xy, xz, xw, yy, yz, yw, zz, zw;
      xx = m_v.m_v[0]*m_v.m_v[0]; xy = m_v.m_v[0]*m_v.m_v[1];
      xz = m_v.m_v[0]*m_v.m_v[2]; xw = m_v.m_v[0]*m_w;
      yy = m_v.m_v[1]*m_v.m_v[1]; yz = m_v.m_v[1]*m_v.m_v[2];
      yw = m_v.m_v[1]*m_w;
      zz = m_v.m_v[2]*m_v.m_v[2]; zw = m_v.m_v[2]*m_w;
      mat.m_v[0] = 1-N*(yy+zz); mat.m_v[4] =   N*(xy-zw); mat.m_v[8] =   N*(xz+yw);
      mat.m_v[1] =   N*(xy+zw); mat.m_v[5] = 1-N*(xx+zz); mat.m_v[9] =   N*(yz-xw);
      mat.m_v[2] =   N*(xz-yw); mat.m_v[6] =   N*(yz+xw); mat.m_v[10]= 1-N*(xx+yy);
      mat.m_v[ 3] = mat.m_v[ 7] = mat.m_v[11] = (T)0;
      mat.m_v[12] = mat.m_v[13] = mat.m_v[14] = (T)0;
      mat.m_v[15] = (T)1;
      return mat;
    }
  };

  //--------------------------------------------------------------------
  // Slerp of Quaternions : 
  template <class T> Quat4<T> QuatSlerp(const Quat4<T>& q1,
					const Quat4<T>& q2, const T t) {
    T s0, s1;
    double cosX = (double)((q1|q2) + q1.m_w*q2.m_w);
    double X  = acos(fabs(cosX));
    double sinX = sin(X);

    if ( fabs(sinX) > 1e-8 ) {
      s0 = (T)sin((1-t)* X) / sinX;
      s1 = (T)sin(   t * X) / sinX;
    } else {
      s0 = (T)1 - t;
      s1 = t;
    }
    if ( cosX < 0 ) s1 = -s1;
    return (q1 * s0) + (q2 * s1);
  }

  //--------------------------------------------------------------------
  // Square of a value :
  template <class T> T Sqr(const T x) {return (x * x);}

  //--------------------------------------------------------------------
  // Degree and Radian :
  template <class T> T Deg2Rad(const T x) {return (T)(x * M_PI / 180.0);}
  template <class T> T Rad2Deg(const T x) {return (T)(x / M_PI * 180.0);}

}; // end of namespace CES

#endif // _CES_UTIL_MATH_H_
