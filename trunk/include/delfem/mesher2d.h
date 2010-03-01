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
@brief �Q�������b�V���N���X(Msh::CMesher2D)�̃C���^�[�t�F�[�X
@author Nobuyuki Umetani
*/

#if !defined(MSHER_2D_H)
#define MSHER_2D_H

#if defined(__VISUALC__)
	#pragma warning( disable : 4786 )
#endif

#include <vector>
#include <set>
#include <map>

#include "delfem/vector2d.h"
#include "delfem/serialize.h"
#include "delfem/mesh_interface.h"
#include "delfem/cad2d_interface.h"

#include "delfem/msh/meshkernel2d.h"

////////////////////////////////////////////////

namespace Msh{

/*! 
@addtogroup Msh2D
*/
// @{

//! ���_�\����
struct SVertex{
public:
	SVertex() : id(0), id_v_cad(0){}
public:
	unsigned int id;	//!< ID
	unsigned int id_v_cad;	//!< CAD�̒��_ID�iCAD�Ɋ֘A����ĂȂ���΂O�j
	unsigned int v;	//!< �_��ID
};

//! ���v�f�z��
class CBarAry{
public:
	CBarAry() : id(0), id_e_cad(0){}
public:
	unsigned int id;	//!< ID
	unsigned int id_e_cad;	//!< CAD�̕�ID�iCAD�Ɋ֘A����ĂȂ���΂O�j
	unsigned int id_se[2];
	unsigned int id_lr[2];
	std::vector<SBar> m_aBar;	//!< �ӗv�f�̔z��
};

//! �Q�����R�p�`�v�f�z��
class CTriAry2D{
public:
	CTriAry2D() : id(0), id_l_cad(0), ilayer(0){}
public:
	unsigned int id;	//!< ID
	unsigned int id_l_cad;	//!< CAD�̖�ID�iCAD�Ɋ֘A����ĂȂ���΂O�j
	int ilayer;
	std::vector<STri2D> m_aTri;	//!< �R�p�`�v�f�̔z��
};

//! �Q�����S�p�`�v�f�z��
class CQuadAry2D{
public:
	CQuadAry2D() : id(0), id_l_cad(0), ilayer(0){}
public:
	unsigned int id;	//!< ID
	unsigned int id_l_cad;	//!< CAD�̖�ID(CAD�Ɋ֘A����ĂȂ���΂O)
	int ilayer;
	std::vector<SQuad2D> m_aQuad;	//!< �S�p�`�v�f�̔z��
};

////////////////////////////////////////////////

/*!
@brief �Q�������b�V���N���X
@ingroup Msh2D

�v�f�̎�ނɈˑ������ɒʂ���ID���U���Ă���D
�ӗv�f�Ɋւ��ẮCCAD�̕ӂƓ��������ɗv�f�ԍ����ɕ���ł���D�i�׏d���E��������̐���j
*/
class CMesher2D : public IMesh
{
public:
	//! �ł��邾�����Ȃ��v�f���Ń��b�V����؂�
	CMesher2D(const Cad::ICad2D& cad_2d){ this->Tesselation(cad_2d); }
	//! �v�f�̒�����elen�ƂȂ�悤�Ƀ��b�V����؂�
	CMesher2D(const Cad::ICad2D& cad_2d, double elen){ this->Meshing_ElemLength(cad_2d,elen); }
	//! �f�t�H���g�R���X�g���N�^
	CMesher2D(){}
    virtual ~CMesher2D(){}

	////////////////////////////////
	
	//! �f�t�H���g�R���X�g���N�^�ŏ��������ꂽ���CClear���ꂽ��Ń��b�V����؂�(elen : ���b�V����)
	bool Meshing_ElemLength(const Cad::ICad2D& cad_2d,double elen, unsigned int id_l=0);
	bool Meshing_ElemLength(const Cad::ICad2D& cad_2d,double elen,  const std::vector<unsigned int>& aIdLoop);
	//! �f�t�H���g�R���X�g���N�^�ŏ��������ꂽ���CClear���ꂽ��Ń��b�V����؂�(esize : �v�f��)
	bool Meshing_ElemSize( const Cad::ICad2D& cad_2d, unsigned int esize, unsigned int id_l = 0);
	bool Meshing_ElemSize( const Cad::ICad2D& cad_2d, unsigned int esize, const std::vector<unsigned int>& aIdLoop);
	//! �o���邾���_��ǉ����Ȃ��悤�ɁA���b�V����؂�
	bool Tesselation(const Cad::ICad2D& cad_2d, unsigned int id_l = 0);
	bool Tesselation(const Cad::ICad2D& cad_2d, const std::vector<unsigned int>& aIdLoop);

	////////////////////////////////

	virtual unsigned int GetDimention() const{ return 2; }	//!< ���W�̎����i�Q�j��Ԃ�
	virtual void GetInfo(unsigned int id_msh,
        unsigned int& id_cad, unsigned int& id_msh_before_ext, unsigned int& inum_ext,
		int& ilayer) const
    { 
		const int itype = m_ElemType[id_msh];
		const int iloc = m_ElemLoc[id_msh];
		if(      itype == 0 ){ id_cad = m_aVertex[ iloc].id_v_cad; }
		else if( itype == 1 ){ id_cad = m_aBarAry[ iloc].id_e_cad; }
		else if( itype == 2 ){ id_cad = m_aTriAry[ iloc].id_l_cad; }
        else if( itype == 3 ){ id_cad = m_aQuadAry[iloc].id_l_cad; }
		else{ assert(0); }
        id_msh_before_ext = 0;
        inum_ext = 0;
	}
	virtual void GetCoord(std::vector<double>& coord) const{
		unsigned int nnode = aVec2D.size();
		coord.resize( nnode*2 );
		for(unsigned int inode=0;inode<nnode;inode++){
			coord[inode*2  ] = aVec2D[inode].x;
			coord[inode*2+1] = aVec2D[inode].y;
		}
	}
	virtual std::vector<unsigned int> GetAry_ID() const{
		std::vector<unsigned int> id_ary;
		for(unsigned int id=1;id<m_ElemLoc.size();id++){
			if( m_ElemLoc[id] == -1 ) continue;
			id_ary.push_back(id);
		}
		return id_ary;
	}
	virtual std::vector<unsigned int> GetIncludeElemIDAry(unsigned int id_msh) const{
		{	// �G���[�̏ꍇ�͋�̔z���Ԃ�
			std::vector<unsigned int> id_ary;
			if( id_msh >= m_ElemLoc.size() ) return id_ary;
			if( m_ElemLoc[id_msh] == -1 ) return id_ary;
			if( id_msh >= this->m_include_relation.size() ) return id_ary;
		}
		return m_include_relation[id_msh];
	}
	/*! ���b�V���̐ڑ��֌W���擾����
	�R�s�[�𖳂������f�[�^�̎󂯓n������������\��
	*/
	MSH_TYPE GetConnectivity(unsigned int id_msh,std::vector<int>& lnods) const;

	bool GetClipedMesh(
		std::vector< std::vector<int> >& lnods_tri, 
		std::vector<unsigned int>& mapVal2Co, 
		const std::vector<unsigned int>& aIdMsh_Ind,
		const std::vector<unsigned int>& aIdMshBar_Cut );

	//! �f�[�^�����ׂăN���A����
	virtual void Clear();

	void SmoothingMesh_Laplace(unsigned int num_iter);
	void SmoothingMesh_Delaunay(unsigned int& num_reconnect);

	////////////////////////////////
	// const�֐�
	
	unsigned int GetElemID_FromCadID(unsigned int id_cad,  Cad::CAD_ELEM_TYPE type_cad) const;
	bool IsID(unsigned int id) const;
	bool GetMshInfo(unsigned int id, unsigned int& nelem, MSH_TYPE& msh_type, unsigned int& iloc,  unsigned int& id_cad ) const;

	////////////////////////////////
	// �v�f�z��A�ߓ_�z��Ɋւ���Get���\�b�h
    
	const std::vector<CTriAry2D>& GetTriArySet() const { return m_aTriAry; }
	const std::vector<CQuadAry2D>& GetQuadArySet() const { return m_aQuadAry; }
	const std::vector<CBarAry>& GetBarArySet() const { return m_aBarAry; }
	const std::vector<SVertex>& GetVertexAry() const { return m_aVertex; }
	const std::vector<Com::CVector2D>& GetVectorAry() const { return aVec2D; }

	////////////////////////////////////////////////////////////////
	// IO���\�b�h
	
	//! �ǂݍ��ݏ����o��
	bool Serialize( Com::CSerializer& serialize );	
	//! GiD���b�V���̓ǂݍ���
	bool ReadFromFile_GiDMsh(const std::string& file_name);

protected:
	unsigned int FindMaxID() const;
	int CheckMesh();	// �ُ킪�Ȃ���΂O��Ԃ�
	unsigned int GetFreeObjID();
	void MakeElemLocationType(); // �v�f�̏ꏊ�Ǝ�ނ��n�b�V���iID�������j���Ă���z���������
	void MakeIncludeRelation(const Cad::ICad2D& cad);	// include_relation�����

	////////////////
	bool MakeBoundary_SplitBarAry(std::vector<SBar>& aBar, unsigned int id_inner);
	// ������is_inverted == true�Ȃ�max_aspect�͖����Ȓl������
	void CheckMeshQuality(bool& is_inverted, double& max_aspect, const double ave_edge_len);

	////////////////
	// ���b�V���؂�֌W�̃��[�e�B��

	bool MakeMesh_Edge(const Cad::ICad2D& cad_2d, unsigned int id_e, const double len);
	bool MakeMesh_Loop(const Cad::ICad2D& cad_2d, unsigned int id_l, const double len);

	bool Tesselate_LoopAround(const Cad::ICad2D& cad_2d, const unsigned int id_l);
	bool Tessalate_Edge(const Cad::ICad2D& cad_2d, const unsigned int id_e );
	bool Tesselate_Loop(const Cad::ICad2D& cad_2d, const unsigned int id_l);

	////////////////////////////////
	// �g�|���W�[�擾

	bool GetMesh_FrontBackStartEnd(unsigned int id_msh_bar, 
		unsigned int& id_msh_f, unsigned int& id_msh_b, 
		unsigned int& id_msh_s, unsigned int& id_msh_e) const;	
	
	class CTriAround
	{
	public:
		CTriAround(unsigned int id_msh, unsigned int ielem, unsigned int inoel )
			: id_msh(id_msh), ielem(ielem), inoel(inoel){}
		unsigned int id_msh;
		unsigned int ielem;
		unsigned int inoel;
	};

	bool GetTriMesh_Around(
		std::vector< CTriAround >& aTriAround,
		unsigned int ibarary, 
		unsigned int ibar, unsigned int inobar, bool is_left,
		const std::vector<unsigned int>& aFlgMshCut );

	////////////////////////////////
	// CAD�Ƃ̐ڑ��֌W�̃��[�e�B��

	// ���������Ƀ��b�V�����؂��Ă��Ȃ���false��Ԃ�
	bool FindElemLocType_CadIDType(
		unsigned int& iloc, unsigned int& itype, 
		unsigned int id_cad_part, Cad::CAD_ELEM_TYPE itype_cad );

	// ���̃��[�v�̖ʐςƁC���ݐ؂��Ă��郁�b�V������œK�ȕӂ̒��������肷��
	double GetAverageEdgeLength(const Cad::ICad2D& cad_2d, 
        const std::set<unsigned int>& aIdL);

protected:
	std::vector<int> m_ElemType;	// vertex(0) bar(1) tri(2) quad(3)	���ł��L��(�Ȃ����-1���Ԃ�)
	std::vector<int> m_ElemLoc;		// index of elem_ary				���ł��L��(�Ȃ����-1���Ԃ�)�@
	std::vector< std::vector<unsigned int> > m_include_relation;	// �ǂ̗v�f�z�񂪂ǂ̗v�f�z����܂�ł邩

	std::vector<SVertex> m_aVertex;		// type(0)
	std::vector<CBarAry> m_aBarAry;		// type(1)
	std::vector<CTriAry2D> m_aTriAry;	// type(2)
	std::vector<CQuadAry2D> m_aQuadAry;	// type(3)

	std::vector<Com::CVector2D> aVec2D;
};

// @}
}

#endif
