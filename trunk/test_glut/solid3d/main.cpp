////////////////////////////////////////////////////////////////
//                                                            //
//		DelFEM Test_glut Solid 3D                             //
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
	int* font=(int*)GLUT_BITMAP_8_BY_13;
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
	mvp_trans.MouseRotation(mov_begin_x,mov_begin_y,mov_end_x,mov_end_y); 
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
		::glMatrixMode(GL_PROJECTION);
		::glLoadIdentity();
		Com::View::SetProjectionTransform(mvp_trans);
		break;
	}
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
double dt = 0.1;
Fem::Eqn::CEqn_Solid3D_Linear solid;
unsigned int id_field_disp;
unsigned int id_base;


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
    
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();
    Com::View::SetProjectionTransform(mvp_trans);
/*
    ////////////////////////////////
    // �w�i�`��
	::glMatrixMode(GL_MODELVIEW);
    ::glPushMatrix();
	::glLoadIdentity();
	::glMatrixMode(GL_PROJECTION);
    ::glPushMatrix();
	::glLoadIdentity();
    ::glDisable(GL_DEPTH_TEST);
    ::glBegin(GL_QUADS);
    ::glColor3d(0.2,0.7,0.7);
    ::glVertex3d(-1,-1,0);
    ::glVertex3d( 1,-1,0);
    ::glColor3d(1,1,1);
    ::glVertex3d( 1, 1,0);
    ::glVertex3d(-1, 1,0);
    ::glEnd();
    ::glEnable(GL_DEPTH_TEST);
	::glMatrixMode(GL_PROJECTION);
    ::glPopMatrix();
	::glMatrixMode(GL_MODELVIEW);
    ::glPopMatrix();
    ////////////////////////////////
*/
	if( is_animation ){
		cur_time += dt;
		world.FieldValueExec(cur_time);
		solid.Solve(world);
		if( solid.GetAry_ItrNormRes().size() > 0 ){
			std::cout << "Iter : " << solid.GetAry_ItrNormRes()[0].first << " ";
			std::cout << "Res : " << solid.GetAry_ItrNormRes()[0].second << std::endl;
		}
		drawer_ary.Update(world);
	}

	drawer_ary.Draw();
	ShowFPS();



	::glutSwapBuffers();
}

void SetNewProblem()
{
	const unsigned int nprob = 9;	// ��萔
	static unsigned int iprob = 0;

	if( iprob == 0 )
	{
		Cad::CCadObj2D cad_2d;
 		{	// �`�����
			std::vector<Com::CVector2D> vec_ary;
			vec_ary.push_back( Com::CVector2D(0.0,0.0) );
			vec_ary.push_back( Com::CVector2D(5.0,0.0) );
			vec_ary.push_back( Com::CVector2D(5.0,1.0) );
			vec_ary.push_back( Com::CVector2D(0.0,1.0) );
			cad_2d.AddPolygon( vec_ary );
		}
		Msh::CMesh3D_Extrude msh_3d;
		msh_3d.Extrude( Msh::CMesher2D(cad_2d,0.4), 1.0, 0.4 );	// �˂��o���ɂ����3�������b�V�������
		world.Clear();
		id_base = world.AddMesh( msh_3d );
        const CIDConvEAMshCad& conv = world.GetIDConverter(id_base);
		solid.SetDomain_Field(id_base,world);
		solid.SetYoungPoisson(250, 0.3);	// �����O���ƃ|�A�\����̐ݒ�
//		solid.SetGeometricalNonLinear();	// �􉽊w�I����`�����l������
		solid.UnSetGeometricalNonLinear();
		solid.SetStationary();
        dt = 0.1;
        solid.SetTimeIntegrationParameter(dt,0.6);
//		solid.SetSaveStiffMat();

//		unsigned int id_field_bc0 = solid.AddFixElemAry(7,world);
		unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromCad(2,Cad::EDGE,2),world);
		{
			CField& bc1_field = world.GetField(id_field_bc1);
			bc1_field.SetValue("sin(5*sin(0.1*t))", 1,Fem::Field::VALUE, world,true);	// bc1_field��y���W�ɒP�U����ǉ�
			bc1_field.SetValue("cos(5*sin(0.1*t))", 2,Fem::Field::VALUE, world,true);	// bc1_field��y���W�ɒP�U����ǉ�
		}
		// �`��I�u�W�F�N�g�̓o�^
		drawer_ary.Clear();
		id_field_disp = solid.GetIdField_Disp();
		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
		drawer_ary.InitTrans(mvp_trans);
	}
	else if( iprob == 1 ){
		solid.UnSetStationary();
		solid.UnSetGeometricalNonLinear();	// �􉽊w�I����`�����l�����Ȃ�
		solid.UnSetSaveStiffMat();			// �S�����s���ۑ�����D
	}
	else if( iprob == 2 ){
		solid.SetGeometricalNonLinear();
	}
	else if( iprob == 3 ){
        const CIDConvEAMshCad& conv = world.GetIDConverter(id_base);
		unsigned int id_field_bc0 = solid.AddFixElemAry(conv.GetIdEA_fromCad(4,Cad::EDGE,2),world);
		solid.SetStationary();
	}
	else if( iprob == 4 ){
		solid.UnSetStationary();
	}
	else if( iprob == 5 )
	{
		Msh::CMesher3D mesh_3d;
		mesh_3d.ReadFromFile_GiDMsh("../input_file/cylinder_tet.msh");
		world.Clear();
		id_base = world.AddMesh( mesh_3d );
		solid.SetDomain_Field(id_base,world);
		solid.SetYoungPoisson(200, 0.43);	// �����O���ƃ|�A�\����̐ݒ�
		solid.UnSetGeometricalNonLinear();	// �􉽊w�I����`�����l������D
		solid.UnSetStationary();
		solid.UnSetSaveStiffMat();
		unsigned int id_field_bc0 = solid.AddFixElemAry(3,world);
		unsigned int id_field_bc1 = solid.AddFixElemAry(4,world);
		{
			CField& bc1_field = world.GetField(id_field_bc1);
			bc1_field.SetValue("5*sin(2*t)^2", 0,Fem::Field::VALUE, world,true);	// bc1_field��y���W�ɒP�U����ǉ�
		}
		// �`��I�u�W�F�N�g�̓o�^
		drawer_ary.Clear();
		id_field_disp = solid.GetIdField_Disp();
		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
		drawer_ary.InitTrans(mvp_trans);
	}
	else if( iprob == 6 ){
		solid.SetStationary();
	}
	else if( iprob == 7 ){
		Msh::CMesher2D mesh_2d;
		mesh_2d.ReadFromFile_GiDMsh("../input_file/rect_quad.msh");
		Msh::CMesh3D_Extrude mesh_3d;
		mesh_3d.Extrude(mesh_2d, 5.0, 0.5 );
        world.Clear();
		id_base = world.AddMesh(mesh_3d);
        const CIDConvEAMshCad& conv = world.GetIDConverter(id_base);
		solid.SetDomain_Field(id_base,world);
		solid.SetYoungPoisson(100, 0.3);	// �����O���ƃ|�A�\����̐ݒ�
		solid.UnSetSaveStiffMat();
		solid.UnSetStationary();
        dt = 0.1;
        solid.SetTimeIntegrationParameter(dt,0.52);
//		solid.SetGeometricalNonLinear();	// �􉽊w�I����`�����l������D
		solid.UnSetGeometricalNonLinear();	// �􉽊w�I����`�����l������D
		unsigned int id_field_bc0 = solid.AddFixElemAry(conv.GetIdEA_fromMshExtrude(1,1),world);
		unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromMshExtrude(3,2),world);
		{
			CField& bc1_field = world.GetField(id_field_bc1);
			bc1_field.SetValue("2*sin(t)", 0,Fem::Field::VALUE, world,true);	// bc1_field��y���W�ɒP�U����ǉ�
		}
		// �`��I�u�W�F�N�g�̓o�^
		drawer_ary.Clear();
		id_field_disp = solid.GetIdField_Disp();
		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
		drawer_ary.InitTrans(mvp_trans);
	}
	else if( iprob == 8 ){
		solid.UnSetGeometricalNonLinear();	// �􉽊w�I����`�����l�����Ȃ�
		solid.SetStationary();
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
