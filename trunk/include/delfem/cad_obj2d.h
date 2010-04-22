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
@brief �Q�����b�`�c�N���X(Cad::CCadObj2Dm)�̃C���^�[�t�F�[�X
@author Nobuyuki Umetani
*/

#if !defined(CAD_OBJ_2D_M_H)
#define CAD_OBJ_2D_M_H

#include "delfem/vector2d.h"
#include "delfem/serialize.h"
#include "delfem/cad2d_interface.h"
#include "delfem/objset.h"
#include "delfem/cad/brep2d.h"
#include "delfem/cad/cad_elem2d.h"

namespace Cad{

class CVertex2D;
class CLoop2D;
class CEdge2D;
class CTopology;

/*! 
@brief �Q�����b�`�c���f���N���X
@ingroup CAD
*/
class CCadObj2D : public Cad::ICad2D
{
public:
	//! Iterator which go around loop (�ʂ̎�������C�e���[�^)
	class CItrLoop : public Cad::ICad2D::CItrLoop
	{
	public:
		CItrLoop(const CCadObj2D* pCadObj2D, unsigned int id_l)
			: itrl(pCadObj2D->m_BRep.GetItrLoop(id_l)){}
		void Begin(){ itrl.Begin(); }	//! ���[�v�̏��߂̕ӂɖ߂�
		void operator++(){ itrl++; } //!< move to next edge
		void operator++(int n){ itrl++; }	//!< dummy operator ( work same as ++)
		//! return current id of edge and return if this edge is counter-clock wise
		bool GetIdEdge(unsigned int& id_e, bool& is_same_dir) const { return itrl.GetIdEdge(id_e,is_same_dir); }
		//! Shift next child loop. if this is end of loop, return false
		bool ShiftChildLoop(){ return itrl.ShiftChildLoop(); }
		bool IsEndChild() const { return itrl.IsEndChild(); }	//!< return true if iterator go around
		unsigned int GetIdVertex() const {        return itrl.GetIdVertex(); }	//!< ���݂̒��_��Ԃ�
		unsigned int GetIdVertex_Ahead()  const { return itrl.GetIdVertex_Ahead(); }	//!< ���̒��_��Ԃ�
        unsigned int GetIdVertex_Behind() const { return itrl.GetIdVertex_Behind(); }	//!< ��O�̒��_��Ԃ�		
		bool IsEnd() const { return itrl.IsEnd(); }	//!< return true if went around this loop
	private:
		CBRep2D::CItrLoop itrl;
	};
	//! iterator go around vertex
	class CItrVertex : public Cad::ICad2D::CItrVertex
	{
	public:		
		CItrVertex(const CCadObj2D* pCadObj2D, unsigned int id_v)
			: itrv(pCadObj2D->m_BRep.GetItrVertex(id_v)){}
		void operator++(){ itrv++; }	//!< go around (cc-wise) loop around vertex 
		void operator++(int n){ itrv++; }	//!< dummy operator (for ++)		
		//! cc-wise ahead  edge-id and its direction(true root of edge is this vertex)
		bool GetIdEdge_Ahead( unsigned int& id_e, bool& is_same_dir) const { return itrv.GetIdEdge_Ahead( id_e,is_same_dir); }
		//! cc-wise behind edge-id and its direction(true root of edge is this vertex)
		bool GetIdEdge_Behind(unsigned int& id_e, bool& is_same_dir) const { return itrv.GetIdEdge_Behind(id_e,is_same_dir); }
		unsigned int GetIdLoop() const{ return itrv.GetIdLoop(); } //!< get loop-id		
		bool IsEnd() const { return itrv.IsEnd(); }	//!< return true if iterator go around
	private:
		CBRep2D::CItrVertex itrv;
	};
    friend class CItrLoop;
    friend class CItrVertex;
	//! �f�t�H���g�R���X�g���N�^
	CCadObj2D();
	//! �f�X�g���N�^
	virtual ~CCadObj2D();
	//! �S�Ă̗v�f���폜���ď�����
	void Clear();

	////////////////////////////////
	// Get���\�b�h

	//! ID:id_l�̃��[�v���\�����钸�_��ӂ��߂���C�e���[�^��Ԃ��֐�
    virtual std::auto_ptr<Cad::ICad2D::CItrLoop> GetItrLoop(unsigned int id_l) const{
        return std::auto_ptr<Cad::ICad2D::CItrLoop>( new CItrLoop(this,id_l) );	// �C���X�^���X�𐶐�
	}
	//! ID:id_l�̃��[�v���\�����钸�_��ӂ��߂���C�e���[�^��Ԃ��֐�
    virtual std::auto_ptr<Cad::ICad2D::CItrVertex> GetItrVertex(unsigned int id_v) const{
        return std::auto_ptr<Cad::ICad2D::CItrVertex>( new CItrVertex(this,id_v) );	// �C���X�^���X�𐶐�
	}
	////////////////////////////////////////////////
	// �\���v�f�ւ̃A�N�Z�X
	virtual bool IsElemID(Cad::CAD_ELEM_TYPE,unsigned int id) const;
	virtual const std::vector<unsigned int> GetAryElemID(Cad::CAD_ELEM_TYPE itype) const;

	////////////////////////////////
	// ���C���֌W�̊֐�
    virtual int GetLayer(Cad::CAD_ELEM_TYPE, unsigned int id) const;
	virtual void GetLayerMinMax(int& layer_min, int& layer_max) const;

	////////////////////////////////
	// ���[�v�̃����o�֐�

	//! @{
    //! ID:id_l�̃��[�v�̐F��Ԃ�
    virtual bool GetColor_Loop(unsigned int id_l, double color[3] ) const;
    //! ID:id_l�̃��[�v�̐F��ݒ肷��
    virtual bool SetColor_Loop(unsigned int id_l, const double color[3] );
	//! ID:id_l�̃��[�v�̖ʐς�Ԃ���
	virtual double GetArea_Loop(unsigned int id_l) const;
	bool ShiftLayer_Loop(unsigned int id_l, bool is_up);
	//! @}

	////////////////////////////////
	// �ӂ̃����o�֐�
	// �@�@�~�ʂɂ���
	// �@�@�@�@is_left_side==true�Ȃ炱�̉~�ʂ͎n�_�I�_�����Ԑ��̎n�_���猩�č����ɂ���Dis_left_side==false�Ȃ�E��
	// �@�@�@�@dist�͉~�̒��S����n�_�I�_�����Ԑ��̒��_�������ɂǂꂾ������Ă��邩�Ƃ����l

	//! @{
	//! �ӂ̎n�_�ƏI�_�̒��_��ID��Ԃ�
	virtual bool GetIdVertex_Edge(unsigned int &id_v_s, unsigned int& id_v_e, unsigned int id_e) const;
    //! �ӂ̗����̃��[�v��ID��Ԃ�
    virtual bool GetIdLoop_Edge(unsigned int &id_l_l, unsigned int& id_l_r, unsigned int id_e) const;
	/*!
	@brief �ӂ̌`��^�C�v��Ԃ�
    @retval 0 ����
    @retval 1 �~��
    @retval 2 ���b�V��
	*/
	virtual int GetEdgeCurveType(const unsigned int& id_e) const;
	//! ID:id_e�̕ӂ̏��𓾂�
	virtual bool GetCurve_Arc(unsigned int id_e, bool& is_left_side, double& dist) const;
	virtual bool GetCurve_Polyline(unsigned int id_e, std::vector<double>& aRelCoMesh) const;
    virtual bool GetCurve_Polyline(unsigned int id_e, std::vector<Com::CVector2D>& aCo, double elen = -1) const;    
    //! ID:id_e�̕ӂ�ndiv�ɕ����������̂𓾂�. �A���n�_�C�I�_�͂��ꂼ��ps,pe�Ƃ���
    virtual bool GetCurve_Polyline(unsigned int id_e, std::vector<Com::CVector2D>& aCo,
		unsigned int ndiv, const Com::CVector2D& ps, const Com::CVector2D& pe) const;
	//! @}

    bool GetPointOnCurve_OnCircle(unsigned int id_e,
                                  const Com::CVector2D& v0, double len, bool is_front,
                                  bool& is_exceed, Com::CVector2D& out) const;
    //! ���͓_����ł��߂��ӏ�̓_�Ƌ�����Ԃ�
    Com::CVector2D GetNearestPoint(unsigned int id_e, const Com::CVector2D& po_in) const;

	////////////////////////////////
	// ���_�̃����o�֐�

	//! @{
	/*!
	@brief ���_�̍��W�𓾂�
	@param[in] id_v ���_��ID
	@retval ���_�̍��W
	*/
	virtual const Com::CVector2D& GetVertexCoord(unsigned int id_v) const;
	//! @}

	////////////////////////////////////////////////
	// �`�󑀍�֐�(�g�|���W�[��ς���)
	
	//! @{ 
	/*!
	@brief ���_�W������ʂ����
	@param[in] vec_ary �����̒��_�̔z��
	@param[in] id_l ���p�`��ǉ�����ʂ�ID, �ȗ����ꂽ�ꍇ�́C�̈�̊O���ɖʂ������D
	@retval �V�����ł����ʂh�c ���������ꍇ
	@retval �O ���s�����ꍇ
	*/
	unsigned int AddPolygon( const std::vector<Com::CVector2D>& vec_ary, unsigned int id_l = 0);

	/*!
	@brief ���_��������֐�
	@param[in] itype ���_���������̗v�f�̃^�C�v
	@param[in] id ���_���������̗v�f��ID
	@param[in] vec ���_�̍��W
	@retval �V�����ł������_��ID ���������ꍇ
	@retval �O ���s�����ꍇ
	@remarks itype == Cad::NOT_SET��������C�`��̊O���ɒ��_��ǉ�
	*/
	unsigned int AddVertex(Cad::CAD_ELEM_TYPE itype, unsigned int id, const Com::CVector2D& vec);
	
	/*!
	@brief �v�f����������֐�
	@param[in] itype �v�f�̃^�C�v
	@param[in] id �v�f��ID
	@retval �����������ǂ���
	@pre �ӂ��������鎞�͕ӂ̗������ʂłȂ���΂Ȃ�Ȃ�
	*/
	bool RemoveElement(Cad::CAD_ELEM_TYPE itype, unsigned int id);
	/*!
	@brief �Q�̒��_(ID:id_v1,id_v2)����Edge�����A
	@retval �V�����ł����ӂ̂h�c ���������ꍇ
	@retval �O ���s�����ꍇ
	*/
	unsigned int ConnectVertex(CEdge2D edge);
	/*!
	@brief �Q�̒��_(ID:id_v1,id_v2)����Edge�����A
	@retval �V�����ł����ӂ̂h�c ���������ꍇ
	@retval �O ���s�����ꍇ
	*/
	unsigned int ConnectVertex_Line(unsigned int id_v1, unsigned int id_v2){
		Cad::CEdge2D e(id_v1,id_v2, 0, 0, 0 );
		return this->ConnectVertex(e);
	}
	//! @}

	////////////////////////////////////////////////
	// �`�󑀍�֐��i�g�|���W�[��ς��Ȃ��j

    bool SetCurve_Polyline(const unsigned int id_e);
	//! ID:id_e�̕ӂ����b�V���ɃZ�b�g����
	bool SetCurve_Polyline(const unsigned int id_e, const std::vector<Com::CVector2D>& aVec);
	//! ID:id_e�̕ӂ��~�ʂɃZ�b�g����
    bool SetCurve_Arc(const unsigned int id_e, bool is_left_side=false, double rdist=10.0);
    //! ID:id_e�̕ӂ𒼐��ɃZ�b�g����
	bool SetCurve_Line(const unsigned int id_e);

	////////////////////////////////////////////////
	// IO���[�e�B��

	//! DXF�֏����o��
	bool WriteToFile_dxf(const std::string& file_name) const;
	//! �ǂݍ��ݏ����o��
	bool Serialize( Com::CSerializer& serialize );	
protected:
	// id_vs, id_ve, po_s, po_e��������ꂽEdge��Ԃ�
	const CEdge2D& GetEdge(unsigned int id_e) const;
	// id_vs, id_ve, po_s, po_e��������ꂽEdge��Ԃ�
	CEdge2D& GetEdge(unsigned int id_e);
	int AssertValid() const;
	bool CheckIsPointInside_ItrLoop(ICad2D::CItrLoop& itrl, const Com::CVector2D& p1) const;

//    �Ԃ�l���O�Ȃ�id_ul1�̒��̓_�͑S��id_ul2�̒�
//    �Ԃ�l���P�Ȃ�id_ul1�̒��̓_�̈ꕔ��id_ul2�̒��ňꕔ�͊O
//    �Ԃ�l���Q�Ȃ�id_ul1�̒��̓_�͑S��id_ul2�̊O
	unsigned int CheckInOut_ItrLoopPoint_ItrLoop(ICad2D::CItrLoop& itrl1, ICad2D::CItrLoop& itrl2) const;
	
	bool CheckIsPointInsideLoop(unsigned int id_l1, const Com::CVector2D& point) const;
//    ���[�v�����S���ǂ������ׂ�D�O�Ȃ猒�S
    int CheckLoop(unsigned int id_l) const;

	// �Ԃ�l�͔��ӂ�ID��id_v��point�����Ԕ���������id_v�𒆐S�Ɏ��v����ɍŏ��ɏo�����
	// ���̔��ӂ̑����郋�[�v�͂��̔������Əd�Ȃ��Ă���B
	// id_v�����V�_�̏ꍇ�͕��V�_����̃��[�v���A��
	CBRep2D::CItrVertex FindCorner_HalfLine(unsigned int id_v, const Com::CVector2D& point) const;
	bool CheckLoopIntersection(unsigned int id_l=0) const;
	double GetArea_ItrLoop(ICad2D::CItrLoop& itrl) const;
protected:
	////////////////
	Com::CObjSet<CLoop2D>   m_LoopSet;
	Com::CObjSet<CEdge2D>   m_EdgeSet;
	Com::CObjSet<CVertex2D> m_VertexSet;
	////////////////
	CBRep2D m_BRep;	// �ʑ����������Ă���N���X
};

}	// end namespace CAD

#endif
