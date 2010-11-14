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
@brief interface of FEM descretization field class (Fem::Field::CField)
@author Nobuyuki Umetani
*/

#if !defined(FIELD_H)
#define FIELD_H

#if defined(__VISUALC__)
    #pragma warning ( disable : 4996 )
#endif

#include <string>
#include <stdio.h>

//#include "delfem/field_world.h"
#include "delfem/eval.h"
#include "delfem/elem_ary.h"
#include "delfem/node_ary.h"

//(Field��MatVec�͕���������������C���̂������̃w�b�_�ւ̃����N�͍폜)
#include "delfem/matvec/bcflag_blk.h"

namespace Fem{
namespace Field{
  
class CFieldWorld;

//! the type of field
enum FIELD_TYPE{ 
  NO_VALUE,	//!< not setted   
  SCALAR,		//!< real value scalar
  VECTOR2,	//!< 2D vector
  VECTOR3,	//!< 3D vector
  STSR2,		//!< 2D symmetrical tensor
  ZSCALAR		//!< complex value scalar
};

//! types of the value derivaration
enum FIELD_DERIVATION_TYPE
{ 
	VALUE=1,		//!< not being derivated
	VELOCITY=2,		//!< first time derivative (velocity)
	ACCELERATION=4	//!< 2nd time derivative (acceralation)
};

// ���̃l�[�~���O�K��������ɂ��������H
// cbef(corner,bubble,edge,face)�̏�. �ԍ��͂��ꂼ��Ɋ���_�����邩�D
// [�v�f�^�C�v] (���W)[cbef],(�l)[cbef]�̂悤�ɒ�`����
// bef�̂������W�C�l�Ƃ��O�Ȃ�΁C��납��(feb�̏���)�ȗ����Ă悢
////////////////
//! the types of interpolation
enum INTERPOLATION_TYPE{
	LINE11,		//!< line element (1st order)
	TRI11,		//!< triangle element (1st order)
	TRI1001,	//!< triangle element (constatnt value in elemnet)
	TRI1011,	//!< triangle element (bubble interpolation)
	TET11,		//!< tetrahedral element (1st order)
	TET1001,	//!< tetrahedral element (constant value in element)
	HEX11,		//!< hexagonal element (1st order,iso-parametric)
	HEX1001		//!< hexagonal element (constant value in element)
};	

/*! 
@brief fem discretization class
@ingroup Fem
*/
class CField
{
public:
	//! store ID of element segment
	class CElemInterpolation{
	public:
		CElemInterpolation
    (unsigned int id_ea, 
     unsigned int id_es_c_va, unsigned int id_es_c_co,
     unsigned int id_es_e_va, unsigned int id_es_e_co,
     unsigned int id_es_b_va, unsigned int id_es_b_co)
    : id_ea(id_ea), 
    id_es_c_va(id_es_c_va), id_es_c_co(id_es_c_co),
    id_es_e_va(id_es_e_va), id_es_e_co(id_es_e_co),
    id_es_b_va(id_es_b_va), id_es_b_co(id_es_b_co),
    ilayer(0){}
	public:
		unsigned int id_ea;	//!< ID of element array
		unsigned int id_es_c_va, id_es_c_co;	//!< ID of element segment of corner (value or coordinate )
		unsigned int id_es_e_va, id_es_e_co;	//!< ID of element segment of edge   (value or coordinate )
		unsigned int id_es_b_va, id_es_b_co;	//!< ID of element segment of bubble (value or coordinate )
		int ilayer;
	};
public: 
	//! store ID of node segment in eace node configuration (CORNER, BUBBLE or EDGE..etc)
	class CNodeSegInNodeAry{
  public:
		unsigned int id_na_co;
    bool is_part_co;		//!< �v�f�ɂ���Ă��̍��WNA���S�ĎQ�Ƃ���邩�ǂ���//!< �v�f�ɂ���Ă��̒lNA���S�ĎQ�Ƃ���邩�ǂ���
    unsigned int id_ns_co;
		unsigned int id_na_va;
    bool is_part_va;
		unsigned int id_ns_va;	//!< value
		unsigned int id_ns_ve;	//!< velocity
    unsigned int id_ns_ac;	//!< acceleration
  public:
		CNodeSegInNodeAry() 
			: id_na_co(0), id_ns_co(0), 
			id_na_va(0), id_ns_va(0), id_ns_ve(0), id_ns_ac(0){}
		CNodeSegInNodeAry
    (unsigned int id_na_co, bool is_part_co, unsigned int id_ns_co, 			
     unsigned int id_na_va, bool is_part_va, 
     unsigned int id_ns_va, unsigned int id_ns_ve, unsigned int id_ns_ac )
    : id_na_co(id_na_co), is_part_co(is_part_co),
    id_ns_co(id_ns_co),
    id_na_va(id_na_va), is_part_va(is_part_va),
    id_ns_va(id_ns_va), id_ns_ve(id_ns_ve), id_ns_ac(id_ns_ac){}
		CNodeSegInNodeAry( const CNodeSegInNodeAry& nsna )
    : id_na_co(nsna.id_na_co), is_part_co(nsna.is_part_co), id_ns_co(nsna.id_ns_co), 
    id_na_va(nsna.id_na_va), is_part_va(nsna.is_part_va), 
    id_ns_va(nsna.id_ns_va), id_ns_ve(nsna.id_ns_ve), id_ns_ac(nsna.id_ns_ac){}
	};
public:	
	//! ��̒l��`�N���X�i���̂������ۃN���X�ɂ���\��j
	class CValueFieldDof{
	public:
		CValueFieldDof(double val){ this->SetValue(val); }
		CValueFieldDof(const std::string str){ 	this->SetValue(str); }
		CValueFieldDof(){ itype=0; }
		////////////////
		void SetValue(std::string str){
			itype = 2;
			math_exp = str;
		}
		void SetValue(double val){
			itype =1;
			this->val = val;
		}
		bool IsTimeDependent() const{
			if( itype != 2 ) return false;
			Fem::Field::CEval eval;
			eval.SetKey("t",0);
			eval.SetExp(math_exp);
			return eval.IsKeyUsed("t");
		}
    bool GetValue(double cur_t, double& value) const;
		const std::string GetString() const{
			if( itype == 1 ){
				char buff[16];
				sprintf(buff,"%lf",val);
				return std::string(buff);
			}
			else{
				return math_exp;
			}
		}
	public:
		int itype; // 0�Ȃ�ݒ肳��Ă��Ȃ��B1�Ȃ�l���ݒ肳��Ă�B2�Ȃ琔�����ݒ肳��Ă�B
		double val;
		std::string math_exp;
	};

public:
	CField
  (unsigned int id_field_parent,	// parent field
   const std::vector<CElemInterpolation>& aEI, 
   const CNodeSegInNodeAry& nsna_c, const CNodeSegInNodeAry& nsna_b, 
   CFieldWorld& world );
  CField(const CField& field);
	CField(){};	//! default constructor

	//////////////// 
	// Get���\�b�h

	bool IsValid() const{ return m_is_valid; }	//!< get the validation flag
	bool AssertValid(CFieldWorld& world) const;	//!< check if it is valid
	//! ���W�̎����𓾂�
	unsigned int GetNDimCoord() const{ return m_ndim_coord; }
	//! �l�̒����𓾂�
	unsigned int GetNLenValue() const { return m_DofSize; }
	//! �t�B�[���h�̎�ނ𓾂�
	FIELD_TYPE GetFieldType() const { return m_field_type; }
	unsigned int GetFieldDerivativeType() const { return m_field_derivative_type; }
	//! ���O�̎擾
	std::string GetName() const {return m_name; }

	// TODO:�����͍�����Ԃł͗v��������(Partial���ǂ�����ELSEG_TYPE���ƂɈႤ)
	//! �����ꂩ�ǂ����𒲂ׂ�
	bool IsPartial() const { return this->m_na_c.is_part_va; }
	//! �ˑ��ꂩ�ǂ������ׂ�
	bool IsDepend() const { 
		if( m_id_field_dep == 0 ) return false;
		return true;
	}
	// TODO:�����͍�����Ԃł͗v��������(Partial���ǂ�����ELSEG_TYPE���ƂɈႤ)
	// �e�t�B�[���h�Ȃ�O��Ԃ�
	unsigned int GetIDFieldParent() const {	return this->m_id_field_parent; }

	// ��Ԃ̃^�C�v���擾����
	std::vector<unsigned int> GetAry_IdElemAry() const {
		std::vector<unsigned int> aIdEA;
		for(unsigned int iei=0;iei<m_aElemIntp.size();iei++){
			aIdEA.push_back( m_aElemIntp[iei].id_ea );
		}
		return aIdEA;
	}
	//! �⊮�̎�ނ𓾂�
	INTERPOLATION_TYPE GetInterpolationType(unsigned int id_ea,const CFieldWorld& world) const;
	//! ElemSeg�𓾂�֐�
	const CElemAry::CElemSeg& GetElemSeg(unsigned int id_ea, ELSEG_TYPE elseg_type, bool is_value, const CFieldWorld& world ) const;
	unsigned int GetIdElemSeg(unsigned int id_ea, ELSEG_TYPE elseg_type, bool is_value, const CFieldWorld& world ) const;
	//! NodeSeg�𓾂�֐�(const)
	const CNodeAry::CNodeSeg& GetNodeSeg(ELSEG_TYPE elseg_type, bool is_value, const CFieldWorld& world, unsigned int derivative_type=1) const;
	//! NodeSeg�𓾂�֐�(��const)
	CNodeAry::CNodeSeg& GetNodeSeg(ELSEG_TYPE elseg_type, bool is_value, CFieldWorld& world, unsigned int derivative_type=1);
	//! NodeSegment��ID���ǂ����𒲂ׂ�
	bool IsNodeSeg(ELSEG_TYPE elseg_type, bool is_value, const CFieldWorld& world, unsigned int derivative_type=7) const;
	const CNodeSegInNodeAry& GetNodeSegInNodeAry( Field::ELSEG_TYPE es_type ) const
	{
		if( es_type == CORNER ){ return m_na_c; }
		if( es_type == BUBBLE ){ return m_na_b; }
		if( es_type != EDGE   ){ assert(0); }
		return m_na_e;
	}
	//! ���Ԉˑ����ǂ������ׂ�
	bool IsTimeDependent() const;
	//! �ő�l�ŏ��l���擾
	void GetMinMaxValue(double& min, double& max, const CFieldWorld& world, 
		unsigned int idof=0, const int fdt=VALUE) const;
	void SetName(const std::string&  name){ m_name = name; }
	// ��̒ǉ�
	bool SetValueType( Field::FIELD_TYPE field_type, const int fdt, CFieldWorld& world);
	int GetLayer(unsigned int id_ea) const;

	////////////////////////////////
	// �l��ݒ肷��֐� (Will be moved to another class)

	//! �Z�[�u���ꂽ�l��S���R�x�ɃZ�b�g����
	bool ExecuteValue(double time, CFieldWorld& world);
	//! �萔���Z�[�u���Z�b�g����
	void SetValue(double val, unsigned int idofns, FIELD_DERIVATION_TYPE fdt, CFieldWorld& world, bool is_save);
	//! �������Z�[�u���Z�b�g����
	bool SetValue(std::string str_exp, unsigned int idofns, FIELD_DERIVATION_TYPE fdt, CFieldWorld& world, bool is_save);
	//! ��������Z�[�u���Z�b�g����
	void SetValueRandom(CFieldWorld& world) const;
	//! �ʂ̏�̒l���R�s�[���ăZ�b�g����
	void SetVelocity(unsigned int id_field, CFieldWorld& world);
	//! ���z���Z�b�g����
	bool SetGradient(unsigned int id_field, CFieldWorld& world, bool is_save);
	
	// TODO: ��x���̊֐����Ă񂾂�C���ɌĂ񂾎��͍����ɏ��������悤�ɁC�n�b�V�����\�z����
	// TODO: ���W��R�l�N�e�B�r�e�B�̕ύX���������ꍇ�́C�n�b�V�����폜����
	bool FindVelocityAtPoint(double velo[], 
		unsigned int& id_ea_stat, unsigned int& ielem_stat, double& r1, double& r2,
		const double co[], const Fem::Field::CFieldWorld& world ) const;

	////////////////////////////////
	// ���E������ݒ肷��֐�(Field��MatVec�͕���������������C���̂������̊֐��͍폜�[��LiearSystem_Field�ɒǉ�)

	// ���E�������Z�b�g����iLinearSystem����Ă΂��)
	void BoundaryCondition(const Field::ELSEG_TYPE& elseg_type, unsigned int idofns, MatVec::CBCFlag& bc_flag, const CFieldWorld& world) const;
	// ���E�������Z�b�g����iLinearSystem����Ă΂��)
	void BoundaryCondition(const Field::ELSEG_TYPE& elseg_type, MatVec::CBCFlag& bc_flag, const CFieldWorld& world, unsigned int ioffset = 0) const;

	unsigned int GetMapVal2Co(unsigned int inode_va) const {
		if( m_map_val2co.size() == 0 ) return inode_va;
		assert( inode_va < m_map_val2co.size() );
		return m_map_val2co[inode_va];
	}

	////////////////////////////////
	// �h�n���o�͂̂��߂̊֐�

	// MicroAVS inp�t�@�C���ւ̏����o��
	bool ExportFile_Inp(const std::string& file_name, const CFieldWorld& world);

private:
    bool SetBCFlagToES(MatVec::CBCFlag& bc_flag, const Fem::Field::CElemAry& ea, unsigned int id_es, unsigned int idofblk) const;
	// �Z�[�u���ꂽ�l��idofns�̎��R�x�ɃZ�b�g����
	bool SetValue(double time, unsigned int idofns, FIELD_DERIVATION_TYPE fdt, const CValueFieldDof& val, CFieldWorld& world);
	bool SetGradientValue(unsigned int id_field, CFieldWorld& world);
private:
	bool m_is_valid;
	unsigned int m_id_field_parent;	// �e�t�B�[���h�́A�v�f���l�ߓ_��S�Q�Ƃ��Ă��Ȃ���΂Ȃ�Ȃ�
	unsigned int m_ndim_coord;
	std::vector<CElemInterpolation> m_aElemIntp;

	// ���̂���vector�z��ɂ���\��(Edge,Face�ȂǕ������邩��)
	CNodeSegInNodeAry m_na_c;
	CNodeSegInNodeAry m_na_e;
	CNodeSegInNodeAry m_na_b;

	FIELD_TYPE m_field_type;
	unsigned int m_field_derivative_type;
	std::string m_name;	// ��͖��O�������Ă���i���̖��O�Ő�����]���������j

	std::vector<unsigned int> m_map_val2co;

	////////////////
	unsigned int m_DofSize;
	std::vector<CValueFieldDof> m_aValueFieldDof;	// ��̒l�������Ă���z��ADofSize*[val,velo,acc]

	// �O���ˑ���̏ꍇ�iFieldWorld�Ɉˑ��֌W�̖؂��쐬�������j
	// ���z�Ƙc�i���`�C����`�j���炢�H
	unsigned int m_id_field_dep;
	bool m_is_gradient;
};	// end class CField;

}	// end namespace Field
}	// end namespace Fem


#endif
