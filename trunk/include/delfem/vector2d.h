/*
DelFEM (Finite Element Analysis)
Copyright (C) 2009  Nobuyuki Umetani    n.umetani@gmail.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*! @file
@brief two-dimensional vector class (Com::CVector2D)
@author Nobuyuki Umetani
*/

#if !defined(VECTOR2D_H)
#define VECTOR2D_H

#if defined(__VISUALC__)
	#pragma warning(disable:4786)
#endif

#include <vector>
#include "math.h"

namespace Com
{

class CVector2D;

CVector2D operator*(double, const CVector2D&);
CVector2D operator*(const CVector2D&, double);

//! �Q�����x�N�g���N���X
class CVector2D{
public:
	CVector2D(){}
	CVector2D( const CVector2D& rhs ){
		this->x = rhs.x;
		this->y = rhs.y;
	}
	CVector2D(double x, double y){
		this->x = x;
		this->y = y;
	}
	
	friend double Dot(const CVector2D&, const CVector2D&);

	// �I�y���[�^��`
	inline CVector2D& operator+=(const CVector2D& rhs){
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	inline CVector2D& operator-=(const CVector2D& rhs){
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
	inline CVector2D& operator*=(double scale){
		x *= scale;
		y *= scale;
		return *this;
    }
	inline CVector2D operator+(const CVector2D& rhs) const {
		CVector2D v = *this;
		return v += rhs;
	}
	inline CVector2D operator-(const CVector2D& rhs) const {
		CVector2D v = *this;
		return v -= rhs;
	}
	//! �����𐳋K������
	inline void Normalize(){
		const double mag = Length();
		x /= mag;
		y /= mag;
	}
	//! ���W�l�ɂO��������
	inline void SetZero(){
		x = 0.0;
		y = 0.0;
	}
	//! �x�N�g���̒������v�Z����
	double Length() const{
		return sqrt( x*x+y*y );
	}
	//! �x�N�g���̒����̂Q����v�Z����
	double SqLength() const{
		return x*x+y*y;
	}
public:
	double x;	//!< �����W�l
	double y;	//!< �����W�l
};



inline CVector2D operator*(double c, const CVector2D& v0)
{
    return Com::CVector2D(v0.x*c,v0.y*c);
}

inline CVector2D operator*(const CVector2D& v0, double c)
{
    return Com::CVector2D(v0.x*c,v0.y*c);
}


////////////////////////////////////////////////////////////////

//! Area of the Triangle
inline double TriArea(const CVector2D& v1, const CVector2D& v2, const CVector2D& v3){
	return 0.5*( (v2.x-v1.x)*(v3.y-v1.y) - (v3.x-v1.x)*(v2.y-v1.y) );
}
//! Area of the Triangle (3 indexes and vertex array)
inline double TriArea(const int iv1, const int iv2, const int iv3, 
				   const std::vector<CVector2D>& point ){
	return TriArea(point[iv1],point[iv2],point[iv3]);
}


////////////////

//! �����̂Q��
inline double SquareLength(const CVector2D& ipo0, const CVector2D& ipo1){
	return	( ipo1.x - ipo0.x )*( ipo1.x - ipo0.x ) + ( ipo1.y - ipo0.y )*( ipo1.y - ipo0.y );
}

//! �����̂Q��
inline double SquareLength(const CVector2D& point){
	return	point.x*point.x + point.y*point.y;
}

////////////////

//! ����
inline double Length(const CVector2D& ipo0, const CVector2D& ipo1){
	return	sqrt( ( ipo1.x - ipo0.x )*( ipo1.x - ipo0.x ) + ( ipo1.y - ipo0.y )*( ipo1.y - ipo0.y ) );
}

////////////////

//! �R�p�`�̍���
inline double TriHeight(const CVector2D& v1, const CVector2D& v2, const CVector2D& v3){
	const double area = TriArea(v1,v2,v3);
	const double len = sqrt( SquareLength(v2,v3) );
	return area*2.0/len;
}

////////////////

//! ���ς̌v�Z
inline double Dot(const CVector2D& ipo0, const CVector2D& ipo1){
	return	ipo0.x*ipo1.x + ipo0.y*ipo1.y;
}

//! �O�ډ~�̔��a�̂Q��
inline double SquareCircumradius(
		const CVector2D& p0, 
		const CVector2D& p1, 
		const CVector2D& p2 )
{
	const double area = TriArea(p0,p1,p2);

	const double dtmp0 = SquareLength(p1,p2);
	const double dtmp1 = SquareLength(p0,p2);
	const double dtmp2 = SquareLength(p0,p1);

	return dtmp0*dtmp1*dtmp2/(16.0*area*area);
}

//! �O�ډ~�̒��S
inline bool CenterCircumcircle(
      const CVector2D& p0, 
      const CVector2D& p1, 
      const CVector2D& p2, 
      CVector2D& center){

   const double area = TriArea(p0,p1,p2);
   if( fabs(area) < 1.0e-10 ){ return false; }
   const double tmp_val = 1.0/(area*area*16.0);

   const double dtmp0 = SquareLength(p1,p2);
   const double dtmp1 = SquareLength(p0,p2);
   const double dtmp2 = SquareLength(p0,p1);

   const double etmp0 = tmp_val*dtmp0*(dtmp1+dtmp2-dtmp0);
   const double etmp1 = tmp_val*dtmp1*(dtmp0+dtmp2-dtmp1);
   const double etmp2 = tmp_val*dtmp2*(dtmp0+dtmp1-dtmp2);

   center.x = etmp0*p0.x + etmp1*p1.x + etmp2*p2.x;
   center.y = etmp0*p0.y + etmp1*p1.y + etmp2*p2.y;
   return true;
}


////////////////////////////////

//! �h���l�[�����𖞂������ǂ������ׂ�
inline int DetDelaunay(
		const CVector2D& p0, 
		const CVector2D& p1, 
		const CVector2D& p2, 
		const CVector2D& p3)
{
	const double area = TriArea(p0,p1,p2);
	if( fabs(area) < 1.0e-10 ){
		return 3;
	}
	const double tmp_val = 1.0/(area*area*16.0);

	const double dtmp0 = SquareLength(p1,p2);
	const double dtmp1 = SquareLength(p0,p2);
	const double dtmp2 = SquareLength(p0,p1);

	const double etmp0 = tmp_val*dtmp0*(dtmp1+dtmp2-dtmp0);
	const double etmp1 = tmp_val*dtmp1*(dtmp0+dtmp2-dtmp1);
	const double etmp2 = tmp_val*dtmp2*(dtmp0+dtmp1-dtmp2);

	const CVector2D out_center(
		etmp0*p0.x + etmp1*p1.x + etmp2*p2.x,
		etmp0*p0.y + etmp1*p1.y + etmp2*p2.y );

	const double qradius = SquareLength(out_center,p0);
	const double qdistance = SquareLength(out_center,p3);

//	assert( fabs( qradius - SquareLength(out_center,p1) ) < 1.0e-10*qradius );
//	assert( fabs( qradius - SquareLength(out_center,p2) ) < 1.0e-10*qradius );

	const double tol = 1.0e-20;
	if( qdistance > qradius*(1.0+tol) ){ return 2; }	// �O�ډ~�̊O
	else{
		if( qdistance < qradius*(1.0-tol) ){ return 0; }	// �O�ډ~�̒�
		else{ return 1;	}	// �O�ډ~��
	}
	return 0;
}

////////////////////////////////////////////////

} // end namespace Com

#endif // VEC_KER_H


