////////////////////////////////////////////////////////////////
//                                                            //
//		DelFEM demo : Solid2D                                 //
//                                                            //
//          Copy Rights (c) Nobuyuki Umetani 2008             //
//          e-mail : numetani@gmail.com                       //
////////////////////////////////////////////////////////////////

#if defined(__VISUALC__)
#pragma warning( disable : 4786 )
#endif
#define for if(0); else for

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <math.h>
#include <fstream>
#include <time.h>
#include <cstdlib> //(exit)

#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "delfem/camera.h"

#include "delfem/cad_obj2d.h"
#include "delfem/mesh3d.h"

#include "delfem/field.h"
#include "delfem/field_world.h"
#include "delfem/drawer_field.h"
#include "delfem/drawer_field_face.h"
#include "delfem/drawer_field_edge.h"
#include "delfem/drawer_field_vector.h"

#include "delfem/eqnsys_solid.h"

using namespace Fem::Ls;
using namespace Fem::Field;

Com::View::CCamera mvp_trans;
double mov_begin_x, mov_begin_y;
bool is_animation = true;


void RenderBitmapString(float x, float y, void *font,char *string)
{
  char *c;
  ::glRasterPos2f(x, y);
  for (c=string; *c != '\0'; c++) {
	  ::glutBitmapCharacter(font, *c);
  }
}


void ShowFPS()
{
	int* font = (int*)GLUT_BITMAP_8_BY_13;
	static char s_fps[30];
	{
		static int frame, timebase;
		int time;
		frame++;
		time=glutGet(GLUT_ELAPSED_TIME);
		if (time - timebase > 500) {
			sprintf(s_fps,"FPS:%4.2f",frame*1000.0/(time-timebase));
			timebase = time;
			frame = 0;
		}
	}
	char s_tmp[30];

	GLint viewport[4];
	::glGetIntegerv(GL_VIEWPORT,viewport);
	const int win_w = viewport[2];
	const int win_h = viewport[3];

	::glMatrixMode(GL_PROJECTION);
	::glPushMatrix();
	::glLoadIdentity();
	::gluOrtho2D(0, win_w, 0, win_h);
	::glMatrixMode(GL_MODELVIEW);
	::glPushMatrix();
	::glLoadIdentity();
	::glScalef(1, -1, 1);
	::glTranslatef(0, -win_h, 0);
	::glDisable(GL_LIGHTING);
//	::glDisable(GL_DEPTH_TEST);
	::glColor3d(1.0, 0.0, 0.0);
	strcpy(s_tmp,"DelFEM demo");
	RenderBitmapString(10,15, (void*)font, s_tmp);
	::glColor3d(0.0, 0.0, 1.0);
	strcpy(s_tmp,"Press \"space\" key!");
	RenderBitmapString(120,15, (void*)font, s_tmp);
	::glColor3d(0.0, 0.0, 0.0);
	RenderBitmapString(10,30, (void*)font, s_fps);
//	::glEnable(GL_LIGHTING);
	::glEnable(GL_DEPTH_TEST);
	::glPopMatrix();
	::glMatrixMode(GL_PROJECTION);
	::glPopMatrix();
	::glMatrixMode(GL_MODELVIEW);
}


// ���T�C�Y���̃R�[���o�b�N�֐�
void myGlutResize(int w, int h)
{
	mvp_trans.SetWindowAspect((double)w/h);
	::glViewport(0, 0, w, h);
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();
	Com::View::SetProjectionTransform(mvp_trans);
	::glutPostRedisplay();
}

void myGlutMotion( int x, int y ){
	GLint viewport[4];
	::glGetIntegerv(GL_VIEWPORT,viewport);
	const int win_w = viewport[2];
	const int win_h = viewport[3];
	const double mov_end_x = (2.0*x-win_w)/win_w;
	const double mov_end_y = (win_h-2.0*y)/win_h;
//	mvp_trans.MouseRotation(mov_begin_x,mov_begin_y,mov_end_x,mov_end_y); 
	mvp_trans.MousePan(mov_begin_x,mov_begin_y,mov_end_x,mov_end_y); 
	mov_begin_x = mov_end_x;
	mov_begin_y = mov_end_y;
	::glutPostRedisplay();
}

void myGlutMouse(int button, int state, int x, int y){
	GLint viewport[4];
	::glGetIntegerv(GL_VIEWPORT,viewport);
	const int win_w = viewport[2];
	const int win_h = viewport[3];
	mov_begin_x = (2.0*x-win_w)/win_w;
	mov_begin_y = (win_h-2.0*y)/win_h;
}

void SetNewProblem();
void myGlutKeyboard(unsigned char Key, int x, int y)
{
	switch(Key)
	{
	case 'q':
	case 'Q':
	case '\033':
		exit(0);  /* '\033' ? ESC ? ASCII ??? */
	case 'a':
		if( is_animation ){ is_animation = false; }
		else{ is_animation = true; }
		break;
	case ' ':
		SetNewProblem();
		break;
	}
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();
	Com::View::SetProjectionTransform(mvp_trans);
	::glutPostRedisplay();
}

void myGlutSpecial(int Key, int x, int y)
{
	switch(Key)
	{
	case GLUT_KEY_PAGE_UP:
		if( ::glutGetModifiers() && GLUT_ACTIVE_SHIFT ){
			if( mvp_trans.IsPers() ){
				const double tmp_fov_y = mvp_trans.GetFovY() + 10.0;
				mvp_trans.SetFovY( tmp_fov_y );
			}
		}
		else{
			const double tmp_scale = mvp_trans.GetScale() * 0.9;
			mvp_trans.SetScale( tmp_scale );
		}
		break;
	case GLUT_KEY_PAGE_DOWN:
		if( ::glutGetModifiers() && GLUT_ACTIVE_SHIFT ){
			if( mvp_trans.IsPers() ){
				const double tmp_fov_y = mvp_trans.GetFovY() - 10.0;
				mvp_trans.SetFovY( tmp_fov_y );
			}
		}
		else{
			const double tmp_scale = mvp_trans.GetScale() * 1.111;
			mvp_trans.SetScale( tmp_scale );
		}
		break;
	case GLUT_KEY_HOME :
		mvp_trans.Fit();
		break;
	case GLUT_KEY_END :
		if( mvp_trans.IsPers() ) mvp_trans.SetIsPers(false);
		else{ mvp_trans.SetIsPers(true); }
		break;
	default:
		break;
	}
	
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();
	Com::View::SetProjectionTransform(mvp_trans);
	::glutPostRedisplay();
}

// �A�C�h�����̃R�[���o�b�N�֐�
void myGlutIdle(){
	::glutPostRedisplay();
}

////////////////////////////////


Fem::Field::CFieldWorld world;
View::CDrawerArrayField drawer_ary;
double cur_time = 0.0;
double dt = 0.05;
Fem::Eqn::CEqnSystem_Solid2D solid;
unsigned int id_field_disp;
unsigned int id_field_equiv_stress;	// �������͂̏�
unsigned int id_field_stress;	// �������͂̏�


// �`�掞�̃R�[���o�b�N�֐�
void myGlutDisplay(void)
{
	::glClearColor(1.0, 1.0, 1.0, 1.0);
	::glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	::glEnable(GL_DEPTH_TEST);

	::glEnable(GL_POLYGON_OFFSET_FILL );
	::glPolygonOffset( 1.1f, 4.0f );

	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();
	Com::View::SetModelViewTransform(mvp_trans);

	if( is_animation ){
		cur_time += dt;
		world.FieldValueExec(cur_time);
		solid.Solve(world);
		if( id_field_equiv_stress != 0 ){ solid.SetEquivStressValue(id_field_equiv_stress,world); } // �������͂̏���X�V
		if( id_field_stress       != 0 ){ solid.SetStressValue(     id_field_stress,      world); } // ���͂̏���X�V
		if( solid.GetAry_ItrNormRes().size() > 0 ){
			std::cout << "Iter : " << solid.GetAry_ItrNormRes()[0].first << " ";
			std::cout << "Res : " << solid.GetAry_ItrNormRes()[0].second << std::endl;
		}
		world.FieldValueDependExec();
		drawer_ary.Update(world);
	}

	drawer_ary.Draw();
	ShowFPS();
	::glutSwapBuffers();
}

void SetNewProblem()
{
	const unsigned int nprob = 10;	// ��萔
	static unsigned int iprob = 0;

	static unsigned int id_field_disp_fix0 = 0;
	static unsigned int id_field_temp = 0;

	if( iprob == 0 )
	{
		id_field_disp_fix0 = 0;
		id_field_temp = 0;
		id_field_stress = 0;
		id_field_equiv_stress = 0;
		////////////////
		Cad::CCadObj2D cad_2d;
		{	// �`�����
			std::vector<Com::CVector2D> vec_ary;
			vec_ary.push_back( Com::CVector2D(0.0,0.0) );
			vec_ary.push_back( Com::CVector2D(5.0,0.0) );
			vec_ary.push_back( Com::CVector2D(5.0,1.0) );
			vec_ary.push_back( Com::CVector2D(0.0,1.0) );
			cad_2d.AddPolygon( vec_ary );
		}
		world.Clear();
		const unsigned int id_base = world.AddMesh( Msh::CMesher2D(cad_2d,0.1) );
		const CIDConvEAMshCad conv = world.GetIDConverter(id_base);

		solid.Clear();
	    solid.UpdateDomain_Field(id_base, world);
		solid.SetSaveStiffMat(false);	
		solid.SetStationary(true);
		// �S�̂̕����l��ݒ�
		solid.SetYoungPoisson(10.0,0.3,true);	// �����O���ƃ|�A�\����̐ݒ�(���ʉ���)
		solid.SetGeometricalNonlinear(false);
		solid.SetGravitation(0.0,0.0);
		solid.SetTimeIntegrationParameter(dt,0.7);

		unsigned int id_field_bc0 = solid.AddFixElemAry(conv.GetIdEA_fromCad(2,Cad::EDGE),world);
		unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromCad(4,Cad::EDGE),world);
		{
			CField& bc1_field = world.GetField(id_field_bc0);
			bc1_field.SetValue("sin(t*PI*2*0.1)", 1,Fem::Field::VALUE, world,true);	// bc1_field��y���W�ɒP�U����ǉ�
		}

		// �`��I�u�W�F�N�g�̓o�^
		drawer_ary.Clear();
		id_field_disp = solid.GetIdField_Disp();
		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
		drawer_ary.InitTrans(mvp_trans);	// View���W�ϊ��̐ݒ�
	}
	else if( iprob == 1 )	// �����s���ۑ����āC���ÓI���
	{
		solid.SetSaveStiffMat(true);
	}
	else if( iprob == 2 )	// �����s���ۑ����āC���I��������
	{
		solid.SetSaveStiffMat(true);
		solid.SetStationary(false);
	}
	else if( iprob == 3 )	// �����ł�����
	{
		solid.SetYoungPoisson(50,0.3,true);
	}
	else if( iprob == 4 )	// ���������ł�����
	{
		solid.SetYoungPoisson(100,0.3,true);
	}
	else if( iprob == 5 )	// �􉽊w�I����`���l�����C���ÓI���
	{
		solid.SetStationary(true);
		solid.SetGeometricalNonlinear(true);
	}
	else if( iprob == 6 )	// �􉽊w�I����`���l�����C���I���
	{
		solid.SetYoungPoisson(10,0.0,true);
		solid.SetStationary(false);
		solid.SetGeometricalNonlinear(true);
	}
	else if( iprob == 7 )	// ���`�~�[�[�X���͂̏����ʒu�ł̕\��
	{
		id_field_equiv_stress = world.MakeField_FieldElemDim(id_field_disp,2,SCALAR,VALUE,BUBBLE);
		solid.SetGeometricalNonlinear(false);
		solid.SetStationary(true);
		// �`��I�u�W�F�N�g�̓o�^
		drawer_ary.Clear();
		id_field_disp = solid.GetIdField_Disp();
		drawer_ary.PushBack( new View::CDrawerFace(id_field_equiv_stress,false,world,id_field_equiv_stress, 0,0.5) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
		// View���W�ϊ��̐ݒ�
		drawer_ary.InitTrans(mvp_trans);
	}
	else if( iprob == 8 )	// ���`�~�[�[�X���͂̏����ʒu�ł̕\��
	{
		// �`��I�u�W�F�N�g�̓o�^
		drawer_ary.Clear();
		id_field_disp = solid.GetIdField_Disp();
		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world,id_field_equiv_stress, 0,0.5) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
		// View���W�ϊ��̐ݒ�
		drawer_ary.InitTrans(mvp_trans);
	}
	else if( iprob == 9 )	// ���͂̏����ʒu�ł̕\��
	{
		id_field_equiv_stress = 0;
		id_field_stress = world.MakeField_FieldElemDim(id_field_disp,2,STSR2,VALUE,BUBBLE);
		// �`��I�u�W�F�N�g�̓o�^
		drawer_ary.Clear();
		id_field_disp = solid.GetIdField_Disp();
		drawer_ary.PushBack( new View::CDrawerVector(id_field_stress,world) );
		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,true,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
		// View���W�ϊ��̐ݒ�
		drawer_ary.InitTrans(mvp_trans);
	}
	else if( iprob == 10 )	// �M���͖��
	{
		id_field_equiv_stress = 0;
		id_field_stress = 0;
		////////////////
		Cad::CCadObj2D cad_2d;
 		{	// �`�����
			std::vector<Com::CVector2D> vec_ary;
			vec_ary.push_back( Com::CVector2D(0.0,0.0) );
			vec_ary.push_back( Com::CVector2D(3.0,0.0) );
			vec_ary.push_back( Com::CVector2D(3.0,1.0) );
			vec_ary.push_back( Com::CVector2D(2.0,1.0) );
			vec_ary.push_back( Com::CVector2D(1.0,1.0) );
			vec_ary.push_back( Com::CVector2D(0.0,1.0) );
			cad_2d.AddPolygon( vec_ary );
		}
		world.Clear();
		const unsigned int id_base = world.AddMesh( Msh::CMesher2D(cad_2d,0.1) );
		CIDConvEAMshCad conv = world.GetIDConverter(id_base);

		solid.UpdateDomain_Field(id_base,world);
		solid.SetSaveStiffMat(false);	// �����s��ۑ����Ȃ�
		solid.SetStationary(true);	// �ÓI���ɃZ�b�g
		// �S�̂̕����l���Z�b�g
		solid.SetYoungPoisson(10.0,0.3,true);	// �����O���ƃ|�A�\����̐ݒ�(���ʉ���)
		solid.SetGeometricalNonlinear(false);	// �􉽊w�I����`������
		solid.SetGravitation(0.0,-0.1);	// �d�͂̐ݒ�
		solid.SetTimeIntegrationParameter(dt);	// ���ԃX�e�b�v�ݒ�
		
		unsigned int id_field_bc0 = solid.AddFixElemAry(conv.GetIdEA_fromCad(2,Cad::EDGE),world);
		unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromCad(6,Cad::EDGE),world);

		////////////////////////////////
		// ���x��̐ݒ�
		id_field_temp = world.MakeField_FieldElemDim(id_field_disp,2,SCALAR,VALUE,CORNER);
		{
			CField& field = world.GetField(id_field_temp);
			field.SetValue("sin(6.28*y)*sin(x)*sin(t)", 0,Fem::Field::VALUE, world,true);
		}
		solid.SetThermalStress(id_field_temp);
		solid.ClearFixElemAry(3,world);

		drawer_ary.Clear();
		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false, world, id_field_temp, -1,1) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
		drawer_ary.InitTrans(mvp_trans);	// View���W�ϊ��̐ݒ�
	}
	else if( iprob == 11 )	// �M���͂��l�����邱�Ƃ���߂�
	{
		drawer_ary.Clear();
		drawer_ary.PushBack( new View::CDrawerFace(id_field_temp,true, world, id_field_temp, -1,1) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
		drawer_ary.InitTrans(mvp_trans);	// View���W�ϊ��̐ݒ�
	}
	else if( iprob == 12 )	// �M���͂��l�����邱�Ƃ���߂�
	{
		solid.SetThermalStress(0);
	}
	else if( iprob == 13 )
	{
		Cad::CCadObj2D cad_2d;
		{	// �����`�ɋ�`�̌�
			std::vector<Com::CVector2D> vec_ary;
			vec_ary.push_back( Com::CVector2D(0.0,0.0) );
			vec_ary.push_back( Com::CVector2D(1.0,0.0) );
			vec_ary.push_back( Com::CVector2D(1.0,1.0) );
			vec_ary.push_back( Com::CVector2D(0.0,1.0) );
			const unsigned int id_l = cad_2d.AddPolygon( vec_ary );
			const unsigned int id_v1 = cad_2d.AddVertex(Cad::LOOP,id_l,Com::CVector2D(0.3,0.2));
			const unsigned int id_v2 = cad_2d.AddVertex(Cad::LOOP,id_l,Com::CVector2D(0.7,0.2));
			const unsigned int id_v3 = cad_2d.AddVertex(Cad::LOOP,id_l,Com::CVector2D(0.7,0.8));
			const unsigned int id_v4 = cad_2d.AddVertex(Cad::LOOP,id_l,Com::CVector2D(0.3,0.8));
			cad_2d.ConnectVertex_Line(id_v1,id_v2);
			cad_2d.ConnectVertex_Line(id_v2,id_v3);
			cad_2d.ConnectVertex_Line(id_v3,id_v4);
			cad_2d.ConnectVertex_Line(id_v4,id_v1);
		}
		world.Clear();
		const unsigned int id_base = world.AddMesh( Msh::CMesher2D(cad_2d,0.05) );
		CIDConvEAMshCad conv = world.GetIDConverter(id_base);		// ID�ϊ��N���X

		solid.SetDomain_FieldEA(id_base,conv.GetIdEA_fromCad(1,Cad::LOOP),world);
		solid.SetSaveStiffMat(true);
		solid.SetStationary(true);
		solid.SetTimeIntegrationParameter(dt);	// �^�C���X�e�b�v��ݒ�
		solid.SetYoungPoisson(2.5,0.3,true);	// �����O���ƃ|�A�\����̐ݒ�(���ʉ���)
		solid.SetGeometricalNonlinear(false);	// �􉽊w�I����`�����l�����Ȃ�
		solid.SetGravitation(0.0,0.0);	// �d�͂O

		unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromCad(3,Cad::EDGE),world);
		{
			CField& field = world.GetField(id_field_bc1);
			field.SetValue("0.3*sin(1.5*t)", 0,Fem::Field::VALUE, world,true);
			field.SetValue("0.1*(cos(t)+1)", 1,Fem::Field::VALUE, world,true);
		}
		unsigned int id_field_bc2 = solid.AddFixElemAry(conv.GetIdEA_fromCad(1,Cad::EDGE),world);

		// �`��I�u�W�F�N�g�̓o�^
		drawer_ary.Clear();
		id_field_disp = solid.GetIdField_Disp();
		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
		drawer_ary.InitTrans(mvp_trans);	// View���W�ϊ��̐ݒ�
	}
	else if( iprob == 14 )
	{
		solid.SetSaveStiffMat(true);
	}
	else if( iprob == 15 )
	{
		solid.SetSaveStiffMat(false);
		solid.SetStationary(false);
	}
	else if( iprob == 16 ){
		solid.SetStationary(true);
		solid.SetGeometricalNonlinear(true);
	}
	else if( iprob == 17 ){
		solid.SetStationary(false);
		solid.SetGeometricalNonlinear(true);
	}
	else if( iprob == 18 ){
		Cad::CCadObj2D cad_2d;
		{	// �����`�ɂ��Q�ɕ���
			std::vector<Com::CVector2D> vec_ary;
			vec_ary.push_back( Com::CVector2D(0.0,0.0) );
			vec_ary.push_back( Com::CVector2D(1.0,0.0) );
			vec_ary.push_back( Com::CVector2D(1.0,1.0) );
			vec_ary.push_back( Com::CVector2D(0.0,1.0) );
			cad_2d.AddPolygon( vec_ary );
			const unsigned int id_v1 = cad_2d.AddVertex(Cad::EDGE,1,Com::CVector2D(0.5,0.0));
			const unsigned int id_v2 = cad_2d.AddVertex(Cad::EDGE,3,Com::CVector2D(0.5,1.0));
			cad_2d.ConnectVertex_Line(id_v1,id_v2);	
		}

		world.Clear();
		const unsigned int id_base = world.AddMesh( Msh::CMesher2D(cad_2d,0.05) );
		CIDConvEAMshCad conv = world.GetIDConverter(id_base);  // ID�ϊ��N���X

		solid.SetDomain_FieldEA(id_base,conv.GetIdEA_fromCad(2,Cad::LOOP),world);
		solid.SetTimeIntegrationParameter(dt);
		solid.SetSaveStiffMat(false);
		solid.SetStationary(true);

		solid.SetYoungPoisson(3.0,0.3,true);	// �����O���ƃ|�A�\����̐ݒ�(���ʉ���)
		solid.SetGeometricalNonlinear(false);	// �􉽊w�I����`�����l�����Ȃ�
		solid.SetGravitation(0.0,0.0);

		unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromCad(3,Cad::EDGE),world);
		{
			CField& field = world.GetField(id_field_bc1);
			field.SetValue("0.3*sin(1.5*t)",     0,Fem::Field::VALUE, world,true);
			field.SetValue("0.1*(cos(t)+1)+0.1", 1,Fem::Field::VALUE, world,true);
		}
		unsigned int id_field_bc2 = solid.AddFixElemAry(conv.GetIdEA_fromCad(5,Cad::EDGE),world);

		// �`��I�u�W�F�N�g�̓o�^
		drawer_ary.Clear();
		id_field_disp = solid.GetIdField_Disp();
		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
//		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_base,true,world) );
		drawer_ary.InitTrans(mvp_trans);	// View���W�ϊ��̐ݒ�
	}
	else if( iprob == 19 )
	{
		solid.SetSaveStiffMat(true);
	}
	else if( iprob == 20 )
	{
		solid.SetSaveStiffMat(false);
		solid.SetStationary(false);
	}
	else if( iprob == 21 ){
		solid.SetStationary(true);
		solid.SetGeometricalNonlinear(true);
	}
	else if( iprob == 22 ){
		solid.SetStationary(false);
		solid.SetGeometricalNonlinear(true);
	}
	else if( iprob == 23 ){
		Cad::CCadObj2D cad_2d;
		{	// �����`���S�ɕ���
			std::vector<Com::CVector2D> vec_ary;
			vec_ary.push_back( Com::CVector2D(0.0,0.0) );
			vec_ary.push_back( Com::CVector2D(2.0,0.0) );
			vec_ary.push_back( Com::CVector2D(2.0,0.5) );
			vec_ary.push_back( Com::CVector2D(0.0,0.5) );
			cad_2d.AddPolygon( vec_ary );
			const unsigned int id_v5 = cad_2d.AddVertex(Cad::EDGE,1,Com::CVector2D(1.5,0.0));
			const unsigned int id_v3 = cad_2d.AddVertex(Cad::EDGE,1,Com::CVector2D(1.0,0.0));
			const unsigned int id_v1 = cad_2d.AddVertex(Cad::EDGE,1,Com::CVector2D(0.5,0.0));
			const unsigned int id_v2 = cad_2d.AddVertex(Cad::EDGE,3,Com::CVector2D(0.5,0.5));
			const unsigned int id_v4 = cad_2d.AddVertex(Cad::EDGE,3,Com::CVector2D(1.0,0.5));
			const unsigned int id_v6 = cad_2d.AddVertex(Cad::EDGE,3,Com::CVector2D(1.5,0.5));
			cad_2d.ConnectVertex_Line(id_v1,id_v2);
			cad_2d.ConnectVertex_Line(id_v3,id_v4);
			cad_2d.ConnectVertex_Line(id_v5,id_v6);
		}

		world.Clear();
		const unsigned int id_base = world.AddMesh( Msh::CMesher2D(cad_2d,0.05) );
		const CIDConvEAMshCad& conv = world.GetIDConverter(id_base);  // ID�ϊ��N���X

		solid.UpdateDomain_Field(id_base,world);	// ��͗̈���Z�b�g
		solid.SetTimeIntegrationParameter(dt);	// ���ԍ��݂��Z�b�g
		solid.SetSaveStiffMat(false);	// �����s���ۑ����Ȃ�	
		solid.SetStationary(true);	// �ÓI���
		// �S�̂̕����n���Z�b�g
		solid.SetYoungPoisson(1.0,0.3,true);	// �����O���ƃ|�A�\����̐ݒ�(���ʉ���)
		solid.SetGeometricalNonlinear(false);
		solid.SetGravitation(0.0,-0.0);

		{	// St.Venant-Kirchhoff��
			Fem::Eqn::CEqn_Solid2D eqn = solid.GetEqnation(conv.GetIdEA_fromCad(1,Cad::LOOP));
			eqn.SetGeometricalNonlinear(true);
			solid.SetEquation(eqn);
		}
		{	// �_�炩���e����
			Fem::Eqn::CEqn_Solid2D eqn = solid.GetEqnation(conv.GetIdEA_fromCad(2,Cad::LOOP));
			eqn.SetYoungPoisson(0.1,0.3,true);
			solid.SetEquation(eqn);
		}
		unsigned int id_field_temp = world.MakeField_FieldElemAry(id_base, conv.GetIdEA_fromCad(3,Cad::LOOP),
			SCALAR,VALUE,CORNER);
		{	// �M�̏�
			CField& field = world.GetField(id_field_temp);
			field.SetValue("0.1*sin(3.14*4*y)*sin(2*t)", 0,Fem::Field::VALUE, world,true);
		}
		{	// �M���͂��l���������`�e����
			Fem::Eqn::CEqn_Solid2D eqn = solid.GetEqnation(conv.GetIdEA_fromCad(3,Cad::LOOP));
			eqn.SetThermalStress(id_field_temp);
			solid.SetEquation(eqn);
		}
		{	// �d�����`�e����
			Fem::Eqn::CEqn_Solid2D eqn = solid.GetEqnation(conv.GetIdEA_fromCad(4,Cad::LOOP));
			eqn.SetYoungPoisson(10,0.3,true);
			solid.SetEquation(eqn);
		}

		id_field_disp_fix0 = solid.AddFixElemAry(conv.GetIdEA_fromCad(2,Cad::EDGE),world);

		// �`��I�u�W�F�N�g�̓o�^
		drawer_ary.Clear();
		id_field_disp = solid.GetIdField_Disp();
		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
//		drawer_ary.PushBack( new View::CDrawerEdge(id_base,false,world) );
		drawer_ary.InitTrans(mvp_trans);	// View���W�ϊ��̐ݒ�
	}
	else if( iprob == 24 ){
		solid.SetRho(0.0001);
		solid.SetStationary(false);
		{	// �ψʂ̏���㉺�ɒP�U���ɐݒ�
			CField& field = world.GetField(id_field_disp_fix0);
			field.SetValue("0.5*cos(2*t)", 1,Fem::Field::VALUE, world,true);
		}
	}
	else if( iprob == 25 ){
		Cad::CCadObj2D cad_2d;
		{	// �����`���S�ɕ���
			std::vector<Com::CVector2D> vec_ary;
			vec_ary.push_back( Com::CVector2D(0.0,0.0) );
			vec_ary.push_back( Com::CVector2D(2.0,0.0) );
			vec_ary.push_back( Com::CVector2D(2.0,1.0) );
			vec_ary.push_back( Com::CVector2D(0.0,1.0) );
			cad_2d.AddPolygon( vec_ary );
			const unsigned int id_v1 = cad_2d.AddVertex(Cad::EDGE,1,Com::CVector2D(1.0,0.0));
			const unsigned int id_v2 = cad_2d.AddVertex(Cad::EDGE,3,Com::CVector2D(1.0,1.0));
			cad_2d.ConnectVertex_Line(id_v1,id_v2);	
		}

		world.Clear();
		const unsigned int id_base = world.AddMesh( Msh::CMesher2D(cad_2d,0.05) );
		const CIDConvEAMshCad& conv = world.GetIDConverter(id_base);  // ID�ϊ��N���X

		solid.UpdateDomain_Field(id_base,world);	// ��͗̈���Z�b�g
		solid.SetTimeIntegrationParameter(dt);	// ���ԍ��݂��Z�b�g
		solid.SetSaveStiffMat(false);	// �����s���ۑ����Ȃ�	
		solid.SetStationary(false);	// �ÓI���
		// �S�̂̕����n���Z�b�g
		solid.SetYoungPoisson(1.0,0.3,true);	// �����O���ƃ|�A�\����̐ݒ�(���ʉ���)
		solid.SetGeometricalNonlinear(false);
		solid.SetGravitation(0.0,-0.0);
        solid.SetRho(0.001);

		{	// �_�炩���e����
			Fem::Eqn::CEqn_Solid2D eqn = solid.GetEqnation(conv.GetIdEA_fromCad(1,Cad::LOOP));
			eqn.SetYoungPoisson(0.1,0.3,true);
			solid.SetEquation(eqn);
		}
		{	// �d�����`�e����
			Fem::Eqn::CEqn_Solid2D eqn = solid.GetEqnation(conv.GetIdEA_fromCad(2,Cad::LOOP));
			eqn.SetYoungPoisson(100000000,0.3,true);
			solid.SetEquation(eqn);
		}

//		id_field_disp_fix0 = solid.AddFixElemAry(conv.GetIdEA_fromCad(2,1),world);
		const unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromCad(4,Cad::EDGE),world);
		{
			CField& field = world.GetField(id_field_bc1);
			field.SetValue("0.3*sin(1.5*t)",     0,Fem::Field::VALUE, world,true);
			field.SetValue("0.1*(cos(t)+1)+0.1", 1,Fem::Field::VALUE, world,true);
		}

		// �`��I�u�W�F�N�g�̓o�^
		drawer_ary.Clear();
		id_field_disp = solid.GetIdField_Disp();
		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
		drawer_ary.InitTrans(mvp_trans);	// View���W�ϊ��̐ݒ�
	}
	else if( iprob == 26 )
	{
		Cad::CCadObj2D cad_2d;
		unsigned int id_l;
		unsigned int id_e1,id_e2,id_e3,id_e4,id_e5;
		{
			std::vector<Com::CVector2D> vec_ary;
			vec_ary.push_back( Com::CVector2D(0.0,0.0) );
			vec_ary.push_back( Com::CVector2D(0.2,0.0) );
			vec_ary.push_back( Com::CVector2D(1.0,0.0) );
			vec_ary.push_back( Com::CVector2D(1.0,1.0) );
			vec_ary.push_back( Com::CVector2D(0.0,1.0) );
			id_l = cad_2d.AddPolygon( vec_ary );
			unsigned int id_v1 = cad_2d.AddVertex(Cad::LOOP, id_l, Com::CVector2D(0.2,0.5) );
			id_e1 = cad_2d.ConnectVertex_Line(2,id_v1);
			unsigned int id_v2 = cad_2d.AddVertex(Cad::LOOP, id_l, Com::CVector2D(0.5,0.2) );
			unsigned int id_v3 = cad_2d.AddVertex(Cad::LOOP, id_l, Com::CVector2D(0.5,0.5) );
			unsigned int id_v4 = cad_2d.AddVertex(Cad::LOOP, id_l, Com::CVector2D(0.5,0.8) );
			unsigned int id_v5 = cad_2d.AddVertex(Cad::LOOP, id_l, Com::CVector2D(0.8,0.5) );
			unsigned int id_v6 = cad_2d.AddVertex(Cad::LOOP, id_l, Com::CVector2D(0.3,0.5) );
			id_e2 = cad_2d.ConnectVertex_Line(id_v2,id_v3);
			id_e3 = cad_2d.ConnectVertex_Line(id_v3,id_v4);
			id_e4 = cad_2d.ConnectVertex_Line(id_v3,id_v5);
			id_e5 = cad_2d.ConnectVertex_Line(id_v3,id_v6);
		}
		Msh::CMesher2D mesh_2d;
		mesh_2d.Meshing_ElemLength(cad_2d,0.1);

		world.Clear();
		const unsigned int id_base = world.AddMesh(mesh_2d);
		const CIDConvEAMshCad& conv = world.GetIDConverter(id_base);  // ID�ϊ��N���X
		unsigned int id_base2 = 0;
		{
			std::vector<unsigned int> mapVal2Co;
			std::vector< std::vector<int> > aLnods;
			{
				std::vector<unsigned int> aIdMsh_Inc;
				aIdMsh_Inc.push_back( mesh_2d.GetElemID_FromCadID(id_l,Cad::LOOP) );
				std::vector<unsigned int> aIdMshCut;
				aIdMshCut.push_back( mesh_2d.GetElemID_FromCadID(id_e1,Cad::EDGE) );
				aIdMshCut.push_back( mesh_2d.GetElemID_FromCadID(id_e2,Cad::EDGE) );
				aIdMshCut.push_back( mesh_2d.GetElemID_FromCadID(id_e3,Cad::EDGE) );
				aIdMshCut.push_back( mesh_2d.GetElemID_FromCadID(id_e4,Cad::EDGE) );
				aIdMshCut.push_back( mesh_2d.GetElemID_FromCadID(id_e5,Cad::EDGE) );
				mesh_2d.GetClipedMesh(aLnods,mapVal2Co, aIdMsh_Inc,aIdMshCut);
			}
			std::vector<unsigned int> aIdEA_Inc;
			aIdEA_Inc.push_back( conv.GetIdEA_fromCad(1,Cad::LOOP) );
			id_base2 = world.SetCustomBaseField(id_base,aIdEA_Inc, aLnods,mapVal2Co);
		}

	    solid.UpdateDomain_Field(id_base2, world);
		solid.SetSaveStiffMat(false);	
		solid.SetStationary(true);
		// �S�̂̕����l��ݒ�
		solid.SetYoungPoisson(10.0,0.3,true);	// �����O���ƃ|�A�\����̐ݒ�(���ʉ���)
//		solid.SetRho(10);
		solid.SetGeometricalNonlinear(false);
		solid.SetGravitation(0.0,0.0);
		solid.SetTimeIntegrationParameter(dt,0.7);

		unsigned int id_field_bc0 = solid.AddFixElemAry(conv.GetIdEA_fromCad(3,Cad::EDGE),world);
		unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromCad(5,Cad::EDGE),world);
		{
			CField& bc1_field = world.GetField(id_field_bc0);
			bc1_field.SetValue("0.1*sin(t*PI*2*0.1)",     0,Fem::Field::VALUE, world,true);	// bc1_field��y���W�ɒP�U����ǉ�
			bc1_field.SetValue("0.1*(1-cos(t*PI*2*0.1))", 1,Fem::Field::VALUE, world,true);	// bc1_field��y���W�ɒP�U����ǉ�
		}

		// �`��I�u�W�F�N�g�̓o�^
		drawer_ary.Clear();
		id_field_disp = solid.GetIdField_Disp();
        std::cout << "id_field_disp : " << id_field_disp << std::endl;
		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
		drawer_ary.InitTrans(mvp_trans);	// View���W�ϊ��̐ݒ�
	}
	else if( iprob == 27 )
	{
		Cad::CCadObj2D cad_2d;
		unsigned int id_e;
		unsigned int id_l;
		{	// �`�����
			std::vector<Com::CVector2D> vec_ary;
			vec_ary.push_back( Com::CVector2D(0.0,0.0) );
			vec_ary.push_back( Com::CVector2D(5.0,0.0) );
			vec_ary.push_back( Com::CVector2D(5.0,2.0) );
			vec_ary.push_back( Com::CVector2D(0.0,2.0) );
			id_l = cad_2d.AddPolygon( vec_ary );
			unsigned int id_v1 = cad_2d.AddVertex(Cad::EDGE,3,Com::CVector2D(2.5,2.0));
			unsigned int id_v2 = cad_2d.AddVertex(Cad::LOOP,id_l,Com::CVector2D(2.5,1.0));
			id_e = cad_2d.ConnectVertex_Line(id_v1,id_v2);
		}
		Msh::CMesher2D mesh_2d(cad_2d,0.2);
		world.Clear();
		cur_time = 0;
		const unsigned int id_base = world.AddMesh(mesh_2d);
		const CIDConvEAMshCad conv = world.GetIDConverter(id_base);
		unsigned int id_base2 = 0;
		{
			std::vector<unsigned int> mapVal2Co;
			std::vector< std::vector<int> > aLnods;
			{
				std::vector<unsigned int> aIdMsh_Inc;
				aIdMsh_Inc.push_back( mesh_2d.GetElemID_FromCadID(id_l,Cad::LOOP) );
				std::vector<unsigned int> aIdMshCut;
				aIdMshCut.push_back( mesh_2d.GetElemID_FromCadID(id_e,Cad::EDGE) );
				mesh_2d.GetClipedMesh(aLnods,mapVal2Co, aIdMsh_Inc,aIdMshCut);
			}
			std::vector<unsigned int> aIdEA_Inc;
			aIdEA_Inc.push_back( conv.GetIdEA_fromCad(id_l,Cad::LOOP) );
			id_base2 = world.SetCustomBaseField(id_base,aIdEA_Inc, aLnods,mapVal2Co);
		}

	    solid.UpdateDomain_Field(id_base2, world);
		solid.SetSaveStiffMat(false);	
		solid.SetStationary(true);
		// �S�̂̕����l��ݒ�
		solid.SetYoungPoisson(10.0,0.3,true);	// �����O���ƃ|�A�\����̐ݒ�(���ʉ���)
		solid.SetGeometricalNonlinear(false);
		solid.SetGravitation(0.0,0.0);
		solid.SetTimeIntegrationParameter(dt,0.7);

		unsigned int id_field_bc0 = solid.AddFixElemAry(conv.GetIdEA_fromCad(2,Cad::EDGE),world);
		unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromCad(4,Cad::EDGE),world);
		{
			CField& bc1_field = world.GetField(id_field_bc0);
			bc1_field.SetValue("0.5*(1-cos(t*PI*2*0.1))", 0,Fem::Field::VALUE, world,true);	// bc1_field��y���W�ɒP�U����ǉ�
			bc1_field.SetValue("0.2*sin(t*PI*2*0.1)",     1,Fem::Field::VALUE, world,true);	// bc1_field��y���W�ɒP�U����ǉ�
		}
		
		id_field_disp = solid.GetIdField_Disp();
		id_field_equiv_stress = world.MakeField_FieldElemDim(id_field_disp,2,SCALAR,VALUE,BUBBLE);

		// �`��I�u�W�F�N�g�̓o�^
		drawer_ary.Clear();
		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world,id_field_equiv_stress) );
//		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
		drawer_ary.InitTrans(mvp_trans);	// View���W�ϊ��̐ݒ�
	}	
	else if( iprob == 28 )
	{
		Cad::CCadObj2D cad_2d;
		unsigned int id_e1, id_e2, id_e3;
		unsigned int id_l;
		{	// �`�����
			std::vector<Com::CVector2D> vec_ary;
			vec_ary.push_back( Com::CVector2D(0.0,0.0) );
			vec_ary.push_back( Com::CVector2D(5.0,0.0) );
			vec_ary.push_back( Com::CVector2D(5.0,2.0) );
			vec_ary.push_back( Com::CVector2D(0.0,2.0) );
			vec_ary.push_back( Com::CVector2D(0.0,1.0) );
			id_l = cad_2d.AddPolygon( vec_ary );
			unsigned int id_v1 = cad_2d.AddVertex(Cad::LOOP,id_l,Com::CVector2D(1.5,1.0));
			unsigned int id_v2 = cad_2d.AddVertex(Cad::LOOP,id_l,Com::CVector2D(3.0,1.0));
			unsigned int id_v3 = cad_2d.AddVertex(Cad::LOOP,id_l,Com::CVector2D(3.0,1.5));
			id_e1 = cad_2d.ConnectVertex_Line(id_v1,id_v2);
			id_e2 = cad_2d.ConnectVertex_Line(id_v2,id_v3);
			id_e3 = cad_2d.ConnectVertex_Line(5,id_v1);
		}
		Msh::CMesher2D mesh_2d(cad_2d,0.2);
		std::cout << "mesh node size : " << mesh_2d.GetVectorAry().size() << std::endl;
		world.Clear();
		cur_time = 0;
		const unsigned int id_base = world.AddMesh(mesh_2d);
		const CIDConvEAMshCad conv = world.GetIDConverter(id_base);
		unsigned int id_base2 = 0;
		{
			std::vector<unsigned int> mapVal2Co;
			std::vector< std::vector<int> > aLnods;
			{
				std::vector<unsigned int> aIdMsh_Inc;
				aIdMsh_Inc.push_back( mesh_2d.GetElemID_FromCadID(id_l,Cad::LOOP) );	// �J�b�g����郋�[�v��o�^
				std::vector<unsigned int> aIdMshCut;
				aIdMshCut.push_back( mesh_2d.GetElemID_FromCadID(id_e1,Cad::EDGE) );	// �J�b�g����ӂ�o�^
				aIdMshCut.push_back( mesh_2d.GetElemID_FromCadID(id_e2,Cad::EDGE) );	// �J�b�g����ӂ�o�^
				mesh_2d.GetClipedMesh(aLnods,mapVal2Co, aIdMsh_Inc,aIdMshCut);
			}
			std::vector<unsigned int> aIdEA_Inc;
			aIdEA_Inc.push_back( conv.GetIdEA_fromCad(id_l,Cad::LOOP) );
			id_base2 = world.SetCustomBaseField(id_base,aIdEA_Inc, aLnods,mapVal2Co);
		}

	    solid.UpdateDomain_Field(id_base2, world);
		solid.SetSaveStiffMat(false);	
		solid.SetStationary(true);
		// �S�̂̕����l��ݒ�
		solid.SetYoungPoisson(10.0,0.3,true);	// �����O���ƃ|�A�\����̐ݒ�(���ʉ���)
		solid.SetGeometricalNonlinear(false);
		solid.SetGravitation(0.0,0.0);
		solid.SetTimeIntegrationParameter(dt,0.7);

		unsigned int id_field_bc0 = solid.AddFixElemAry(conv.GetIdEA_fromCad(1,Cad::EDGE),world);
		unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromCad(3,Cad::EDGE),world);
		{
			CField& bc1_field = world.GetField(id_field_bc0);
			bc1_field.SetValue("0.2*sin(t*PI*2*0.1)", 0,Fem::Field::VALUE, world,true);	// bc1_field��y���W�ɒP�U����ǉ�
		}
		id_field_disp = solid.GetIdField_Disp();
		std::cout << conv.GetIdEA_fromCad(id_e3,Cad::EDGE) << std::endl;
		unsigned int id_field_disp_edge = world.GetPartialField(id_field_disp,conv.GetIdEA_fromCad(id_e3,Cad::EDGE));
		std::cout << id_field_disp_edge << " " << id_e3 << std::endl;
		
		id_field_equiv_stress = world.MakeField_FieldElemDim(id_field_disp,2,SCALAR,VALUE,BUBBLE);

		// �`��I�u�W�F�N�g�̓o�^
		drawer_ary.Clear();
		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world,id_field_equiv_stress,0,0.5) );
		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp_edge,false,world) );
//		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
		drawer_ary.InitTrans(mvp_trans);	// View���W�ϊ��̐ݒ�
	}

	iprob++;
	if( iprob == nprob ){
		iprob = 0;
	}
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

int main(int argc,char* argv[])
{
	// GLUT�̏�����
	glutInitWindowPosition(200,200);
	glutInitWindowSize(400, 300);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutCreateWindow("FEM View");

	// �R�[���o�b�N�֐��̐ݒ�
	glutDisplayFunc(myGlutDisplay);
	glutReshapeFunc(myGlutResize);
	glutMotionFunc(myGlutMotion);
	glutMouseFunc(myGlutMouse);
	glutKeyboardFunc(myGlutKeyboard);
	glutSpecialFunc(myGlutSpecial);
	glutIdleFunc(myGlutIdle);
	
	// ���̐ݒ�
	SetNewProblem();

	// ���C�����[�v
	glutMainLoop();
	return 0;
}

