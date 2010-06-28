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
@brief �ߓ_�z��N���X(Fem::Field::CNodeAry)�̃C���^�[�t�F�[�X
@author Nobuyuki Umetani
*/

#if !defined(NODE_ARY_H)
#define NODE_ARY_H

#if defined(__VISUALC__)
    #pragma warning( disable : 4786 )
#endif

#ifndef for 
#define for if(0); else for
#endif

#include <iostream>
#include <cassert>

#include "delfem/complex.h"
#include "delfem/objset.h"
#include "delfem/elem_ary.h"	// ElemAry�Ɉˑ����Ȃ��悤�Ɋ撣��

////////////////////////////////
// ���̃N���X��`

namespace MatVec{
	class  CVector_Blk;
	class CZVector_Blk;
    class CBCFlag;
}

namespace Fem{
namespace Field
{

/*! 
@brief �ߓ_�z��N���X
@ingroup Fem
*/
class CNodeAry
{
public:
	//! �ߓ_�Z�O�����g�N���X
	class CNodeSeg{
		friend class CNodeAry;
	public:
		CNodeSeg(const unsigned int& len, const std::string& name)
			: len(len), name(name){}
		unsigned int GetLength() const { return len; }	//!< The length of value
		unsigned int GetNnode() const { return nnode; }	//!< The number of nodes
		void GetValue(unsigned int inode, double* aVal ) const	//!< get value from node
		{
			for(unsigned int i=0;i<len;i++){
				aVal[i] = paValue[inode*DofSize+idofval_begin+i];
			}
		}
		void GetValue(unsigned int inode, Com::Complex* aVal ) const	//!< get complex value from node
		{
			const unsigned int n = len/2;
			for(unsigned int i=0;i<n;i++){
				double dr = paValue[inode*DofSize+idofval_begin+i*2];
				double di = paValue[inode*DofSize+idofval_begin+i*2+1];
				aVal[i] = Com::Complex(dr,di);
			}
		}
		void SetValue(unsigned int inode, unsigned int idofns, double val )	//!< set value to node 
		{
			paValue[inode*DofSize+idofval_begin+idofns] = val;
		}
		void AddValue(unsigned int inode, unsigned int idofns, double val )	//!< add value to node
		{
			paValue[inode*DofSize+idofval_begin+idofns] += val;
		}
		void SetZero()	//!< set zero to all value
		{
			for(unsigned int ino=0;ino<nnode;ino++){
			for(unsigned int ilen=0;ilen<len;ilen++){
				paValue[ino*DofSize+idofval_begin+ilen] = 0;
			}
			}
		}
	private:
        unsigned int len;	//!< the size of value
        std::string name;	//!< name
	private: // not need when initialize 
		unsigned int idofval_begin;	//!< offset of value
	private: // the variables given by CNodeAry
		mutable double* paValue;	//!< value list
		mutable unsigned int DofSize;	//!< the width of node array
		mutable unsigned int nnode;	//!< number of nodes
	};

private:
	class CEaEsInc
	{
	public:
		unsigned int id_ea;
		unsigned int id_es;
		std::vector<unsigned int> aIndEaEs_Include;	//!< ����Es���܂�ł�iidEaEs
	};

public:
	CNodeAry(const unsigned int size);
	CNodeAry();
	virtual ~CNodeAry();

	bool ClearSegment();

	////////////////////////////////
	// Get���\�b�h	

	const std::string& Name() const { return m_str_name; }  //!< ���O�̎擾
	unsigned int Size()	const { return m_Size; }  //!< �ߓ_�̐��̎擾

	//! �g���Ă��Ȃ��ߓ_�Z�O�����g��ID���P�擾����֐�
	unsigned int GetFreeSegID() const { return m_aSeg.GetFreeObjID(); }
	//! �g���Ă��Ȃ��ߓ_�Z�O�����g��ID��num�����擾����֐�
	std::vector<unsigned int> GetFreeSegID(unsigned int num) const { return m_aSeg.GetFreeObjID(num); }
	//! �ߓ_�Z�O�����g��ID���ǂ����𒲂ׂ�֐�
	bool IsSegID( unsigned int id_ns ) const{ return m_aSeg.IsObjID(id_ns); }
	const std::vector<unsigned int>& GetAry_SegID() const { return this->m_aSeg.GetAry_ObjID(); }
	//! �ߓ_�Z�O�����g���擾����֐�
	const CNodeSeg& GetSeg(unsigned int id_ns) const{		
		assert( this->m_aSeg.IsObjID(id_ns) );
		if( !m_aSeg.IsObjID(id_ns) ) throw;
		const CNodeSeg& ns = m_aSeg.GetObj(id_ns);
		assert( m_paValue != 0 );
		ns.paValue = m_paValue;
		ns.DofSize = m_DofSize;
		ns.nnode = m_Size;
		return ns;
	}
	//! �ߓ_�Z�O�����g���擾����֐�
	CNodeSeg& GetSeg(unsigned int id_ns){			
		assert( this->m_aSeg.IsObjID(id_ns) );
		if( !m_aSeg.IsObjID(id_ns) ) throw;
		CNodeSeg& ns = m_aSeg.GetObj(id_ns);
		assert( m_paValue != 0 );
		ns.paValue = m_paValue;
		ns.DofSize = m_DofSize;
		ns.nnode = m_Size;
		return ns;
	}

	//! �ߓ_�Z�O�����g�̒l��vec�ɑ������
	bool GetValueFromNodeSegment(unsigned int id_ns, MatVec::CVector_Blk& vec, unsigned int ioffset=0) const; 
	bool AddValueFromNodeSegment(double alpha, unsigned int id_ns, MatVec::CVector_Blk& vec, unsigned int ioffset=0) const;

	////////////////
	// �Q�Ɨv�f�Z�O�����g�ǉ����\�b�h

	void AddEaEs( std::pair<unsigned int, unsigned int> eaes ){
		unsigned int ieaes = this->GetIndEaEs( eaes );
		if( ieaes != m_aEaEs.size() ) return;
		{
			CEaEsInc eaesinc;
			eaesinc.id_ea = eaes.first;
			eaesinc.id_es = eaes.second;
			m_aEaEs.push_back( eaesinc );
		}
	}

	std::vector< std::pair<unsigned int, unsigned int> > GetAryEaEs() const {
		std::vector< std::pair<unsigned int, unsigned int> > aEaEs;
		for(unsigned int ieaes=0;ieaes<m_aEaEs.size();ieaes++){
			aEaEs.push_back( std::make_pair(m_aEaEs[ieaes].id_ea,m_aEaEs[ieaes].id_es) );
		}
		return aEaEs;
	}

	void SetIncludeEaEs_InEaEs( std::pair<unsigned int, unsigned int> eaes_included,
		std::pair<unsigned int, unsigned int> eaes_container )
	{
		unsigned int ieaes_included = this->GetIndEaEs( eaes_included );
		if( ieaes_included == m_aEaEs.size() ) return;
		unsigned int ieaes_container = this->GetIndEaEs( eaes_container );
		if( ieaes_container == m_aEaEs.size() ) return;
//        std::cout << "SetInclude " << eaes_included.first << " " << eaes_included.second << "    in    ";
//        std::cout << eaes_container.first << " " << eaes_container.second << std::endl;
		m_aEaEs[ieaes_container].aIndEaEs_Include.push_back(ieaes_included);
	}

	
	bool IsIncludeEaEs_InEaEs( std::pair<unsigned int, unsigned int> eaes_inc,
		std::pair<unsigned int, unsigned int> eaes_in ) const
	{
		unsigned int ieaes_inc = this->GetIndEaEs( eaes_inc );
		if( ieaes_inc == m_aEaEs.size() ) return false;
		unsigned int ieaes_in = this->GetIndEaEs( eaes_in );
		if( ieaes_in == m_aEaEs.size() ) return false;
		const std::vector<unsigned int>& inc = m_aEaEs[ieaes_in].aIndEaEs_Include;
		for(unsigned int iieaes=0;iieaes<inc.size();iieaes++){
			if( inc[iieaes] == ieaes_inc ){
				return true;
			}
		}
		return false;
	}

	std::vector< std::pair<unsigned int, unsigned int> > GetAry_EaEs_Min() const
	{
		std::vector< std::pair<unsigned int, unsigned int> > aEaEs;
		std::vector<unsigned int> aflg;
		aflg.resize(m_aEaEs.size(),1);
		for(unsigned int ieaes=0;ieaes<m_aEaEs.size();ieaes++){
			const std::vector<unsigned int>& inc = m_aEaEs[ieaes].aIndEaEs_Include;
			for(unsigned int jjeaes=0;jjeaes<inc.size();jjeaes++){
				unsigned int jeaes = inc[jjeaes];
				assert( jeaes < m_aEaEs.size() );
				aflg[jeaes] = 0;
			}
		}
		for(unsigned int ieaes=0;ieaes<m_aEaEs.size();ieaes++){
			if( aflg[ieaes] == 1 ){
				aEaEs.push_back( std::make_pair(m_aEaEs[ieaes].id_ea,m_aEaEs[ieaes].id_es) );
			}
		}
		return aEaEs;
	}

	unsigned int IsContainEa_InEaEs(std::pair<unsigned int, unsigned int>eaes, unsigned int id_ea) const
	{
		const unsigned int ieaes = this->GetIndEaEs( eaes );
		if( ieaes == m_aEaEs.size() ) return 0;
		if( m_aEaEs[ieaes].id_ea == id_ea ){
			return m_aEaEs[ieaes].id_es;
		}
		const std::vector<unsigned int>& inc = m_aEaEs[ieaes].aIndEaEs_Include;
		for(unsigned int jjeaes=0;jjeaes<inc.size();jjeaes++){
			unsigned int jeaes = inc[jjeaes];
			assert( jeaes < m_aEaEs.size() );
			if( m_aEaEs[jeaes].id_ea == id_ea ){
				return m_aEaEs[jeaes].id_es;
			}
		}
		return 0;
	}



	////////////////
	// �ύX�E�폜�E�ǉ����\�b�h

	const std::vector<int> AddSegment( const std::vector< std::pair<unsigned int,CNodeSeg> >& seg_vec ); //!< ���������Ȃ�
	const std::vector<int> AddSegment( const std::vector< std::pair<unsigned int,CNodeSeg> >& seg_vec, const double& val);  //!< �lval�ŏ�����
	const std::vector<int> AddSegment( const std::vector< std::pair<unsigned int,CNodeSeg> >& seg_vec, const std::vector<double>& val_vec);  //!< �x�N�g��val�ŏ�����

	////////////////
	// �l�̕ύX���\�b�h

	//! Segment��vector�̒l��������
	bool SetValueToNodeSegment(unsigned int id_ns, const MatVec::CVector_Blk& vec, unsigned int ioffset=0); // Segment��vec���Z�b�g����

	//! ���E�����ݒ�Ɏg����B
	bool SetValueToNodeSegment(const Field::CElemAry& ea, const unsigned int id_es, const unsigned int id_ns, const unsigned int idofns, const double val){
		assert( m_aSeg.IsObjID(id_ns) );
		if( !m_aSeg.IsObjID(id_ns) ) return false;
		const CNodeSeg& ns = m_aSeg.GetObj(id_ns);
		assert( idofns < ns.len );

		assert( ea.IsSegID(id_es) );
		const CElemAry::CElemSeg& es = ea.GetSeg(id_es);

		unsigned int noes[256];
		unsigned int nnoes = es.GetSizeNoes();
		for(unsigned int ielem=0;ielem<ea.Size();ielem++){
			es.GetNodes(ielem,noes);
			for(unsigned int inoes=0;inoes<nnoes;inoes++){
				const unsigned int inode0 = noes[inoes];
				m_paValue[inode0*m_DofSize+ns.idofval_begin+idofns] = val;
			}
		}
		return true;
	}

	
	bool AddValueToNodeSegment(unsigned int id_ns, const MatVec::CVector_Blk& vec, double alpha,  unsigned int ioffset = 0); //!< Segment��alpha�{���ꂽvec��������
	bool AddValueToNodeSegment(unsigned int id_ns, const MatVec::CZVector_Blk& vec, double alpha); //!< Segment��alpha�{���ꂽvec��������
	bool AddValueToNodeSegment(unsigned int id_ns_to, unsigned int id_ns_from, double alpha );	//!< ns_to��alpha�{���ꂽns_from��������


	//! load from file
	int InitializeFromFile(const std::string& file_name, long& offset);
	//! write to file
	int WriteToFile(const std::string& file_name, long& offset, unsigned int id ) const;
	int DumpToFile_UpdatedValue(const std::string& file_name, long& offset, unsigned int id ) const;

private:
	unsigned int GetIndEaEs( std::pair<unsigned int, unsigned int> eaes ) const
	{
		unsigned int ieaes;
		for(ieaes=0;ieaes<m_aEaEs.size();ieaes++){
			if( m_aEaEs[ieaes].id_ea == eaes.first && m_aEaEs[ieaes].id_es == eaes.second ) break;
		}
		return ieaes;
	}
private:
	std::string m_str_name;	//!< name
	unsigned int m_Size;	//!< number of nodes
	
	Com::CObjSet<CNodeSeg> m_aSeg;	//!< the array of node segment

	double* m_paValue;		//!< the values in nodes
	unsigned int m_DofSize;		//!< the size of DOF in node

	std::vector< CEaEsInc > m_aEaEs;	//!< whitch element segments this node is included
};

}	// end namespace field
}	// end namespace Fem

#endif // !defined(NODE_ARY_H)