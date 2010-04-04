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
@brief ���A���^�C���L���v�f�@��̓N���X
@author Nobuyuki Umetani
*/

#if !defined(GUI_LISTENER_INTERACTIVE_ANALYSIS_2D_H)
#define GUI_LISTENER_INTERACTIVE_ANALYSIS_2D_H

#if defined(_WIN32)
#  include <windows.h>
#if defined(__VISUALC__)
#  pragma comment (lib, "winmm.lib")      /* link with Windows MultiMedia lib */
#  pragma comment (lib, "opengl32.lib")  /* link with Microsoft OpenGL lib */
#  pragma comment (lib, "glu32.lib")     /* link with Microsoft OpenGL Utility lib */
#endif
#endif  /* _WIN32 */

#include <GL/gl.h>
#include <GL/glu.h>

#include <set>
#include <map>
#include <vector>

#include "delfem/camera.h"
#include "delfem/drawer_gl_utility.h"

#include "delfem/cad_obj2d_move.h"
#include "delfem/drawer_cad.h"
#include "delfem/mesher2d_edit.h"
#include "delfem/drawer_msh.h"

#include "delfem/field.h"

#include "delfem/analysis2d_interface.h"

//! ���A���^�C���L���v�f�@��̓N���X
class CGuiListner_Analysis2D_Interactive
{
public:
	class CCadAgent : public Cad::ICad2D
	{
	public:
        CCadAgent(CGuiListner_Analysis2D_Interactive* pAnalysis);
        virtual ~CCadAgent(){}

        virtual int GetLayer(Cad::CAD_ELEM_TYPE type,unsigned int id) const;
		virtual void GetLayerMinMax(int& ilayer_min, int& ilayer_max) const;

        //! �ӂ̗����̃��[�v��ID��Ԃ�
        virtual bool GetIdLoop_Edge(unsigned int &id_l_l, unsigned int& id_l_r, unsigned int id_e) const;

		//! id���g���Ă��邩�ǂ����𒲂ׂ�֐�
		virtual bool IsElemID(Cad::CAD_ELEM_TYPE,unsigned int id) const;
		//! ���ׂĂ�ID��z��ɂ��ĕԂ��֐�
		virtual const std::vector<unsigned int> GetAryElemID(Cad::CAD_ELEM_TYPE) const;
		//! ID:id_l�̃��[�v�̐F��Ԃ�(�{�����̃N���X�͈ʑ��Ɗ􉽏��ȊO�����ׂ��ł͂Ȃ���������Ȃ��̂Ŏb��I)
		virtual bool GetColor_Loop(unsigned int id_l, double color[3] ) const;
		//! ID:id_l�̃��[�v�̖ʐς�Ԃ���
		virtual double GetArea_Loop(unsigned int id_l) const;
		//! ID:id_l�̃��[�v���\�����钸�_��ӂ��߂���C�e���[�^��Ԃ��֐�
        virtual std::auto_ptr<ICad2D::CItrLoop>   GetItrLoop(  unsigned int id_l) const;
        //! ID:id_v�̃��[�v���\�����钸�_��ӂ��߂���C�e���[�^��Ԃ��֐�
        virtual std::auto_ptr<ICad2D::CItrVertex> GetItrVertex(unsigned int id_v) const;

		//! �ӂ̎n�_�ƏI�_�̒��_��ID��Ԃ�
		virtual bool GetIdVertex_Edge(unsigned int &id_v_s, unsigned int& id_v_e, unsigned int id_e) const;
		//! �ӂ̌`��^�C�v��Ԃ� (�O ����, �P �~��);
		virtual int GetEdgeCurveType(const unsigned int& id_e) const;
		//! ID:id_e�̕ӂ̏��𓾂�
		virtual bool GetCurve_Arc(unsigned int id_e, bool& is_left_side, double& dist) const;
        virtual bool GetCurve_Polyline(unsigned int id_e, std::vector<double>& aRelCoMesh) const;
        virtual bool GetCurve_Polyline(unsigned int id_e, std::vector<Com::CVector2D>& aCo, double elen = -1) const;
        //! ID:id_e�̕ӂ�ndiv�ɕ����������̂𓾂�. �A���n�_�C�I�_�͂��ꂼ��ps,pe�Ƃ���
        virtual bool GetCurve_Polyline(unsigned int id_e, std::vector<Com::CVector2D>& aCo,
                                   unsigned int ndiv, const Com::CVector2D& ps, const Com::CVector2D& pe) const;
		//! ���_�̍��W�𓾂� 
		virtual const Com::CVector2D& GetVertexCoord(unsigned int id_v) const;

		////////////////
        //! ID:id_l�̃��[�v�̐F��ݒ肷��(�{�����̃N���X�͈ʑ��Ɗ􉽏��ȊO�����ׂ��ł͂Ȃ���������Ȃ��̂Ŏb��I)
        bool SetColor_Loop(unsigned int id_l, double color[3] );
        bool Move(Cad::CAD_ELEM_TYPE itype_cad, unsigned int id_part_cad,
				const Com::CVector2D& pos_obj_pre, const Com::CVector2D& pos_obj_cur, double tor=-1.0){
            return m_pAnalysis->Cad_Move(itype_cad,id_part_cad, pos_obj_pre,pos_obj_cur,tor);
		}

	private:
		CGuiListner_Analysis2D_Interactive* m_pAnalysis;
	};
    friend class CCadAgent;
public:
	//! �R���X�g���N�^
	CGuiListner_Analysis2D_Interactive(){
		is_updated_coord = false;
		is_updated_edge = false;
		m_is_solve_cad_change = true;
		pDrawerCAD = 0;
		pDrawerMsh = 0;
	}
	//! �f�X�g���N�^
    virtual ~CGuiListner_Analysis2D_Interactive(){
		if( pDrawerCAD != 0 ) delete pDrawerCAD;
		if( pDrawerMsh != 0 ) delete pDrawerMsh;
	}
	////////////////
	// public�ȏ������z�֐�

	void SetAnalysisInitialize(IAnalysis2D* pAnalysis);
	void Draw(unsigned int idaw_obj = 0);
	void DrawSelection();
	void Solve_fromCad();	//!< �`��f�[�^��������Ȃ����֐�
	void Solve_ifNeeded();	//!< �K�v�ɉ����ĉ����֐�
	void Serialize( Com::CSerializer& arch );	//!< �t�@�C���ւ�(�ۑ�/�ǂݏo��)�֐�

	//! �`�󂪕ς�����Ƃ��ɁC���������������ǂ���
	void Enable_SolveCadChange(bool flg){ m_is_solve_cad_change = flg; }

	////////////////////////////////
	// GUI�Ɋւ���N���X

    std::auto_ptr<Cad::ICad2D> Cad()
	{
        std::auto_ptr<Cad::ICad2D> p( new CCadAgent(this) );
		return p;
	}

	// �o�E���f�B���O�E�{�b�N�X�擾
	// rot : �R�~�R�̉�]�s��
	Com::CBoundingBox GetBoundingBox(double rot[]) const;

	// CAD�ɒǏ]���Ă��Ȃ����b�V����Ǐ]������(Idle���Ɏ��s����)
	void FollowMshToCad_ifNeeded();

	////////////////////////////////
	// �s�b�N�֌W�̊֐�

	void SetSelection(const std::vector<Com::View::SSelectedObject>& aSelecObj);
	void Cad_GetPicked(Cad::CAD_ELEM_TYPE& itype, unsigned int& id, double& x, double& y) const ;

	////////////////////////////////
	// CAD�Ɋւ���֐�

    void HilightCadTypeID(Cad::CAD_ELEM_TYPE itype, unsigned int id){
        if( !this->cad_2d.IsElemID(itype,id) ) return;
		pDrawerCAD->ClearSelected();
        pDrawerCAD->AddSelected(itype,id);
	}

	////////////////////////////////
	// Msh�Ɋւ���֐�

	// �v�f����ݒ�
	void Msh_SetElemSize(unsigned int esize){
		if( esize == 0 ) return;
		mesh_2d.m_esize = esize;
		this->Solve_fromCad();
	}

	// ����CAD�̃��[�v����̓��b�V�����؂��Ă��邩���ǂ����𒲂ׂ�
    bool Msh_IsMesh_CadLoop(unsigned int id_l) const {
		return mesh_2d.setIdLCad_CutMesh.find(id_l) != mesh_2d.setIdLCad_CutMesh.end();
    }

	// ���[�v�����ɐݒ肷��
    void Msh_DelMesh_CadLoop(unsigned int id_l){
		if( !cad_2d.IsElemID(Cad::LOOP,id_l) ) return;
		std::set<unsigned int>::iterator itr = mesh_2d.setIdLCad_CutMesh.find(id_l);
		if( itr == mesh_2d.setIdLCad_CutMesh.end() ) return;
        mesh_2d.setIdLCad_CutMesh.erase( itr );
		this->Solve_fromCad();
	}
	
	// ���ɂȂ��Ă��郋�[�v�𖄂߂�
	void Msh_SetMesh_CadLoop(unsigned int id_l){
		if( !cad_2d.IsElemID(Cad::LOOP,id_l) ) return;
        if( Msh_IsMesh_CadLoop(id_l) ) return;
		mesh_2d.setIdLCad_CutMesh.insert( id_l );
		this->Solve_fromCad();
	}
	void Msh_SetMeshingMode_Length(double elen){
		mesh_2d.m_imode_meshing = 2;
		mesh_2d.m_elen = elen;
	}
	void Msh_SetMeshingMode_ElemSize(unsigned int esize){
		mesh_2d.m_imode_meshing = 1;
		mesh_2d.m_esize = esize;
	}

	//! CAD���f����DXF�ɕۑ�����
	void File_WriteDXF(const std::string& fname) const {
		cad_2d.WriteToFile_dxf(fname);
    }
public:
	unsigned int Cad_MakeRectRegion(unsigned int id_l, const Com::CVector2D& pos_obj0, const Com::CVector2D& pos_obj1 );
    unsigned int Cad_ConnectVertex_Line(unsigned int id_v0, unsigned int id_v1);
    unsigned int Cad_ConnectVertex(const Cad::CEdge2D& e);
    bool Cad_Remove(Cad::CAD_ELEM_TYPE itype, unsigned int id);
    unsigned int Cad_AddVertex(Cad::CAD_ELEM_TYPE itype, unsigned int id, const Com::CVector2D& pos_obj0);
    void Cad_SetColor_Loop(unsigned int id_l, double color[3]);	
	bool Cad_SetCurveType(unsigned int id_e, unsigned int itype);	//! �����̃J�[�u�ɏo���邾�������悤�ɐV�����J�[�u��ݒ肷��
    bool Cad_SetCurve_Polyline(unsigned int id_e, const std::vector<Com::CVector2D>& aVec);
    bool Cad_DragArc(unsigned int id_part_cad,const Com::CVector2D& pos_obj0, double tol = -1);
    bool Cad_Move(Cad::CAD_ELEM_TYPE itype_cad, unsigned int id_part_cad,
		const Com::CVector2D& pos_obj_pre, const Com::CVector2D& pos_obj_cur, double tor = -1.0);    
    //! id_e�����b�V���ӂȂ�C�X���[�W���O�������Ċ��炩�ɂ���(radius�����Ȃ�S�̂��X���[�W���O����)
    bool Cad_SmoothingPolylineEdge(unsigned int id_e, unsigned int niter, const Com::CVector2D& pos, double radius);
protected:
	void CreatedLoop(unsigned int id_l_created, unsigned int id_l_from){}
	void InitDrawer();
protected:
	// Cad�֌W�̕ϐ�
    Cad::CCadObj2D_Move cad_2d;
	std::set<unsigned int> setIdVCad_NeedFollow;
	Cad::View::CDrawer_Cad2D* pDrawerCAD;

	// ���b�V���֌W�̕ϐ�
	Msh::CMesher2D_Edit mesh_2d;
	Msh::View::CDrawerMsh2D* pDrawerMsh;
	
    // �s�b�N���ꂽ��
	double m_picked_x, m_picked_y;
	unsigned int m_id_cad_part;
	Cad::CAD_ELEM_TYPE m_itype_cad_part;

	bool m_is_solve_cad_change;
	bool is_updated_cad;
	bool is_updated_coord;	// �v�f�̍��W��Update���ꂽ���ǂ���
	bool is_updated_edge;	// �v�f�̕ӂ�Update���ꂽ���ǂ���

    IAnalysis2D* pAnalysis;
};

#endif
