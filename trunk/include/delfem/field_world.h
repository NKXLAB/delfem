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
@brief ��Ǘ��N���X(Fem::Field::CFieldWorld)�̃C���^�[�t�F�[�X
@author Nobuyuki Umetani
*/

#if !defined(FIELD_WORLD_H)
#define FIELD_WORLD_H

#include <map>

#include "delfem/elem_ary.h"	// CElemAry�̎Q�ƕԂ������Ă���̂ŕK�v
#include "delfem/node_ary.h"	// CNodeAry�̎Q�ƕԂ������Ă���̂ŕK�v
#include "delfem/objset.h"		// ID�Ǘ��e���v���[�g�N���X
#include "delfem/cad_com.h"		// ID�Ǘ��e���v���[�g�N���X

namespace Msh{
	class IMesh;	
}

namespace Fem{
namespace Field{

//! ��̎��
enum FIELD_TYPE{ 
	NO_VALUE,	//!< �ݒ肳��Ă��Ȃ�
	SCALAR,		//!< ���X�J���[
	VECTOR2,	//!< �Q���x�N�g��
	VECTOR3,	//!< �R���x�N�g��
	ZSCALAR		//!< ���f�X�J���[
};

class CField;	

/*! 
@brief �v�f�z��C���b�V���CCAD��ID����������N���X
@ingroup Fem
*/
class CIDConvEAMshCad
{
	friend class CFieldWorld;
public:
	void Clear(){
		m_aIdAry.clear();
	}
    bool IsIdEA(unsigned int id_ea) const{
        for(unsigned int iid=0;iid<m_aIdAry.size();iid++){
            if( m_aIdAry[iid].id_ea == id_ea ) return true;
        }
        return false;
    }
	unsigned int GetIdEA_fromMsh(unsigned int id_part_msh) const {
		for(unsigned int iid=0;iid<m_aIdAry.size();iid++){
			if( m_aIdAry[iid].id_part_msh == id_part_msh ){
				return m_aIdAry[iid].id_ea;
			}
		}
		return 0;
	}
	unsigned int GetIdEA_fromMshExtrude(unsigned int id_part_msh, unsigned int inum_ext) const {
		for(unsigned int iid=0;iid<m_aIdAry.size();iid++){
			if(    m_aIdAry[iid].id_part_msh_before_extrude == id_part_msh
                && m_aIdAry[iid].inum_extrude == inum_ext ){
				return m_aIdAry[iid].id_ea;
			}
		}
		return 0;
	}
	// itype_cad_part : Vertex(0), Edge(1), Loop(2)
    unsigned int GetIdEA_fromCad(unsigned int id_part_cad, Cad::CAD_ELEM_TYPE itype_cad_part, unsigned int inum_ext = 0) const {
		for(unsigned int iid=0;iid<m_aIdAry.size();iid++){
			if(    m_aIdAry[iid].id_part_cad    == id_part_cad 
				&& m_aIdAry[iid].itype_part_cad == itype_cad_part 
                && m_aIdAry[iid].inum_extrude   == inum_ext ){
				return m_aIdAry[iid].id_ea;
			}
		}
		return 0;
	}
	void GetIdCad_fromIdEA(unsigned int id_ea, unsigned int& id_part_cad, Cad::CAD_ELEM_TYPE& itype_part_cad ) const{
		for(unsigned int iid=0;iid<m_aIdAry.size();iid++){
			if( m_aIdAry[iid].id_ea == id_ea ){
				id_part_cad    = m_aIdAry[iid].id_part_cad;
				itype_part_cad = m_aIdAry[iid].itype_part_cad;
				return;
			}
		}
		id_part_cad    = 0;
		itype_part_cad = Cad::NOT_SET;
	}
private:
	class CInfoCadMshEA{
	public:
		unsigned int id_ea;
		unsigned int id_part_msh;
		unsigned int id_part_cad;
		Cad::CAD_ELEM_TYPE itype_part_cad;
		unsigned int id_part_msh_before_extrude;    // �˂��o���O�̃��b�V��ID
		unsigned int inum_extrude;  // �˂��o����ĂȂ�(0), ���(1), ����(2), ���(3)
	};
	std::vector<CInfoCadMshEA> m_aIdAry;
};

/*! 
@brief ��Ǘ��N���X
@ingroup Fem
*/
class CFieldWorld{
public:
	// �f�t�H���g�R���X�g���N�^
	CFieldWorld();
	// �f�X�g���N�^
	~CFieldWorld();
    /*!
	@brief �R�������b�V������L���v�f�@��ԏ���\�z
	@remarks �������b�V���ɂ��Ή��ł���悤��Mesh��ID��Ԃ�����
	*/
	unsigned int AddMesh(const Msh::IMesh& mesh);

	unsigned int SetCustomBaseField(unsigned int id_base,
		std::vector<unsigned int> aIdEA_Inc,
		std::vector< std::vector<int> >& aLnods,
		std::vector<unsigned int>& mapVal2Co);

	CIDConvEAMshCad GetIDConverter(unsigned int id_field_base) const {
        std::map<unsigned int,CIDConvEAMshCad>::const_iterator itr = m_map_field_conv.find(id_field_base);
        if( itr == m_map_field_conv.end() ){
            CIDConvEAMshCad conv;
            return conv;
        }
        return itr->second;
    }

	//! �l��S�č폜���ď���������
	void Clear();

	bool UpdateMeshCoord(    const unsigned int id_base, const Msh::IMesh& mesh);
	bool UpdateConnectivity( const unsigned int id_base, const Msh::IMesh& mesh );
	bool UpdateConnectivity_CustomBaseField(const unsigned int id_base,
		const std::vector<unsigned int>& aIdEA_Inc, 
		const std::vector< std::vector<int> >& aLnods,
		const std::vector<unsigned int>& mapVal2Co);

	////////////////////////////////////////////////////////////////
	// �v�f�z��Ɋ֌W����֐��Q

	//! �v�f�z���ID���ǂ������ׂ�
	bool IsIdEA( unsigned int id_ea ) const;
	//! �v�f�z���ID��S�Ĕz��œ���
	const std::vector<unsigned int>& GetAry_IdEA() const;
	//! �v�f�z��̎Q�Ƃ𓾂�֐�(const)
	const CElemAry& GetEA(unsigned int id_ea) const;
	//! �v�f�z��̎Q�Ƃ𓾂�֐�(��const)
	CElemAry& GetEA(unsigned int id_ea);
	//! �v�f�z����O������ǉ�����֐�(.net����̒ǉ��Ɏg��)
	unsigned int AddElemAry(unsigned int size, ELEM_TYPE elem_type);
	bool AddIncludeRelation(unsigned int id_ea, unsigned int id_ea_inc);	 // AddElemAry�Ɠ��ꂵ�����D

	////////////////////////////////////////////////////////////////
	// �ߓ_�z��Ɋ֌W����֐��Q

	//! �ߓ_�z���ID���ǂ������ׂ�
	bool IsIdNA( unsigned int id_na ) const;
	//! �ߓ_�z���ID��S�Ĕz��œ���
	const std::vector<unsigned int>& GetAry_IdNA() const;
	//! �ߓ_�z��̎Q�Ƃ𓾂�֐�(const)
	const CNodeAry& GetNA(unsigned int id_na) const;
	//! �ߓ_�z��̎Q�Ƃ𓾂�֐�(��const)
	CNodeAry& GetNA(unsigned int id_na);
	//! �ߓ_�z����O������ǉ�����֐�(.net����̒ǉ��Ɏg��)
	unsigned int AddNodeAry(unsigned int size);

	////////////////////////////////////////////////////////////////
	// Field�Ɋ֌W����֐��Q

	//! Field��ID���ǂ�����Ԃ�
	bool IsIdField( unsigned int id_field ) const;
	//! Field��ID��S�Ĕz��œ���
	const std::vector<unsigned int>& GetAry_IdField() const;
	//! ��N���X(Fem::Field::CField)�̎Q�Ƃ𓾂�֐�(const)
	const CField& GetField(unsigned int id_field) const;
	//! ��N���X(Fem::Field::CField)�̎Q�Ƃ𓾂�֐�(��const)
	CField& GetField(unsigned int id_field);

	////////////////////////////////////////////////////////////////
	// Field�ǉ��֐�

	/*! 
	@brief ���͂��ꂽ�`��̍��W�������S�̂ɒǉ�
	@param[in] field_type �l
	@param[in] derivative_type ���x�A�����x����邩�ǂ����w�肷��B�O�̏ꍇ�͉������Ȃ�)
	@param[in] node_configuration_type ��Ԃ̎��
	*/
//	unsigned int MakeField_AllRegion(Field::FIELD_TYPE field_type = NO_VALUE, const int derivative_type = 1, const int node_configuration_type = 1 );
	unsigned int MakeField_FieldElemAry(unsigned int id_field, unsigned int id_ea, Field::FIELD_TYPE field_type = NO_VALUE, const int derivative_type = 1, const int node_configuration_type = 1 );
	unsigned int MakeField_FieldElemDim(unsigned int id_field, int idim_elem,      Field::FIELD_TYPE field_type = NO_VALUE, const int derivative_type = 1, const int node_configuration_type = 1 );
	//! �v�fID(id_ea)���琬�镔����̎擾
	unsigned int GetPartialField(unsigned int id_field, unsigned int IdEA );
	//! �v�fID�z��(id_ea)���琬�镔����̎擾
	unsigned int GetPartialField(unsigned int id_field, std::vector<unsigned int> aIdEA);

	void FieldValueExec(double time);
	void FieldValueDependExec();

	////////////////////////////////////////////////////////////////
	// �t�@�C���Ɋ֌W����֐��Q

	int InitializeFromFile(const std::string& file_name, long& offset);
	int WriteToFile(const std::string& file_name, long& offset) const;
private:        
/*	unsigned int SetBaseField(
		unsigned int id_na, unsigned int id_ns_co,
		const std::vector< std::pair< unsigned int, unsigned int> >& pEaEs );*/
private:
	Com::CObjSet<CElemAry*> m_apEA;		//!< �v�f�z��W��
	Com::CObjSet<CNodeAry*> m_apNA;		//!< �ߓ_�z��W��
	Com::CObjSet<CField*> m_apField;	//!< ��W��

    std::map<unsigned int,CIDConvEAMshCad> m_map_field_conv;
};

}	// end namespace Field
}	// end namespace Fem

#endif
