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
@brief ���ۂQ�����b�`�c�N���X(Cad::CCadObj2D)�̃C���^�[�t�F�[�X
@remarks ���̃C���^�[�t�F�[�X�����p���ł���΁AMesh��؂�����CDrawerCAD�ŕ`��ł���
@author Nobuyuki Umetani
*/

#if !defined(CAD_2D_INTERFACE_H)
#define CAD_2D_INTERFACE_H

#include <memory>   // autoptr�̂��߂ɕK�v

#include "delfem/vector2d.h"
#include "delfem/cad_com.h"

namespace Cad{

/*! 
@ingroup CAD
@brief 2D CAD model class (�Q�����b�`�c���f���N���X)
*/
class ICad2D
{
public:
    //! iterator go around loop
	class CItrLoop
	{
	public:
		virtual void Begin() = 0;	//!< back to initial point of current use-loop
		virtual void operator++() = 0; //!< move to next edge
		virtual void operator++(int n)= 0;	//!< dummy operator (for ++)
		//! return current edge id and whether if this edge is same dirrection as loop
		virtual bool GetIdEdge(unsigned int& id_e, bool& is_same_dir) const = 0;	
		virtual bool ShiftChildLoop() = 0;	//!< move to next use-loop in this loop
		virtual bool IsEndChild() const = 0;	//!< return true if iterator go around
		virtual unsigned int GetIdVertex() const = 0;	//!< return current vertex id
		virtual unsigned int GetIdVertex_Ahead()  const = 0;	//!< return next vertex
		virtual unsigned int GetIdVertex_Behind() const = 0;	//!< return previous vertex
		virtual bool IsEnd() const = 0;	//!< return true if iterator go around
	};
	//! iterator go around vertex
	class CItrVertex
	{ 
	public:		
		virtual void operator++() = 0;	//!< go around (cc-wise) loop around vertex 
		virtual void operator++(int n) = 0;	//!< dummy operator (for ++)		
		//! cc-wise ahead  edge-id and its direction(true root of edge is this vertex)
		virtual bool GetIdEdge_Ahead(unsigned int& id_e, bool& is_same_dir) const = 0;
		//! cc-wise behind edge-id and its direction(true root of edge is this vertex)
		virtual bool GetIdEdge_Behind(unsigned int& id_e, bool& is_same_dir) const = 0;
		virtual unsigned int GetIdLoop() const = 0; //!< get loop-id		
		virtual bool IsEnd() const = 0;	//!< return true if iterator go around
	};
public:	
    ICad2D(){}	//!< needs defalut constructor
    virtual ~ICad2D(){}	//!< virtual destructor is must for interface class

	////////////////////////////////
	// Get���\�b�h

  //! �ӂ̗����̃��[�v��ID��Ԃ�
  virtual bool GetIdLoop_Edge(unsigned int &id_l_l, unsigned int& id_l_r, unsigned int id_e) const = 0;
	//! �ӂ̎n�_�ƏI�_�̒��_��ID��Ԃ�
	virtual bool GetIdVertex_Edge(unsigned int &id_v_s, unsigned int& id_v_e, unsigned int id_e) const = 0;  

	////////////////////////////////////////////////
	// �\���v�f�ւ̃A�N�Z�X

	//!@{
	//! id���g���Ă��邩�ǂ����𒲂ׂ�֐�
	virtual bool IsElemID(Cad::CAD_ELEM_TYPE,unsigned int id) const = 0;
	//! ���ׂĂ�ID��z��ɂ��ĕԂ��֐�
	virtual const std::vector<unsigned int> GetAryElemID(Cad::CAD_ELEM_TYPE) const = 0;
	//!@}
	
	// ���C���֌W�̊֐�
    virtual int GetLayer(Cad::CAD_ELEM_TYPE, unsigned int id) const = 0;
	virtual void GetLayerMinMax(int& layer_min, int& layer_max) const = 0;

	////////////////////////////////
	// ���[�v�̃����o�֐�

	//! @{
    //! ID:id_l�̃��[�v�̐F��Ԃ�(�{�����̃N���X�͈ʑ��Ɗ􉽏��ȊO�����ׂ��ł͂Ȃ���������Ȃ��̂Ŏb��I)
    virtual bool GetColor_Loop(unsigned int id_l, double color[3] ) const = 0;
	//! ID:id_l�̃��[�v�̖ʐς�Ԃ���
	virtual double GetArea_Loop(unsigned int id_l) const = 0;
	//! ID:id_l�̃��[�v���\�����钸�_��ӂ��߂���C�e���[�^��Ԃ��֐�
    virtual std::auto_ptr<CItrLoop> GetItrLoop(unsigned int id_l) const = 0;
	//! ID:id_v�̒��_���\������ӂ⃋�[�v���߂���C�e���[�^��Ԃ��֐�
    virtual std::auto_ptr<CItrVertex> GetItrVertex(unsigned int id_v) const = 0;
	//! @}

	////////////////////////////////
	// �ӂ̃����o�֐�
	// �@�@�~�ʂɂ���
	// �@�@�@�@is_left_side==true�Ȃ炱�̉~�ʂ͎n�_�I�_�����Ԑ��̎n�_���猩�č����ɂ���Dis_left_side==false�Ȃ�E��
	// �@�@�@�@dist�͉~�̒��S����n�_�I�_�����Ԑ��̒��_�������ɂǂꂾ������Ă��邩�Ƃ����l
	//! @{
    //��������ȗ��ł������H(����interface�͕`��ƃ��b�V���؂̂��߂��Ƃ���)
    /*!
	@brief �ӂ̌`��^�C�v��Ԃ�
	@retval �O ����
    @retval �P �~��
    @retval �Q ���b�V��
	*/
	virtual int GetEdgeCurveType(const unsigned int& id_e) const = 0;
	//! ID:id_e�̕ӂ̏��𓾂�
	virtual bool GetCurve_Arc(unsigned int id_e, bool& is_left_side, double& dist) const = 0;
	virtual bool GetCurve_Polyline(unsigned int id_e, std::vector<double>& aRelCoMesh) const = 0;
    //�����܂ŏȗ��ł������H

    //! ID:id_e�̕ӂ̃��b�V�������𓾂�(elen<=0�Ȃ�ł��邾���ڍׂɃ��b�V����؂낤�Ƃ���)
    virtual bool GetCurve_Polyline(unsigned int id_e, std::vector<Com::CVector2D>& aCo, double elen) const = 0;
    //! ID:id_e�̕ӂ�ndiv�ɕ����������̂𓾂�. �A���n�_�C�I�_�͂��ꂼ��ps,pe�Ƃ���
    virtual bool GetCurve_Polyline(unsigned int id_e, std::vector<Com::CVector2D>& aCo,
                               unsigned int ndiv, const Com::CVector2D& ps, const Com::CVector2D& pe) const = 0;
	//! @}

	////////////////////////////////
	// ���_�̃����o�֐�

	//! @{
	/*!
	@brief ���_�̍��W�𓾂�
	@param[in] id_v ���_��ID
	@retval ���_�̍��W
	*/
	virtual const Com::CVector2D& GetVertexCoord(unsigned int id_v) const = 0;
	//! @}
};

}	// end namespace CAD

#endif
