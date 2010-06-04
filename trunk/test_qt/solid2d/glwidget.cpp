
#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "glwidget.h"

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

using namespace Fem::Field;

GLWidget_Solid2d::GLWidget_Solid2d(QWidget *parent)
    : QGLWidget(parent)
{
    cur_time = 0;
    dt = 0.05;
    SetNewProblem();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(StepTime()));
    timer->start(20);
}

GLWidget_Solid2d::~GLWidget_Solid2d()
{
    makeCurrent();
}

void GLWidget_Solid2d::initializeGL()
{

}

void GLWidget_Solid2d::paintGL()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ::glMatrixMode(GL_MODELVIEW);
    ::glLoadIdentity();
    Com::View::SetModelViewTransform(camera);

    ::glMatrixMode(GL_PROJECTION);
    ::glLoadIdentity();
    Com::View::SetProjectionTransform(camera);

    drawer_ary.Draw();
}

void GLWidget_Solid2d::resizeGL(int w, int h)
{
    camera.SetWindowAspect((double)w/h);
    ::glViewport(0, 0, w, h);
    ::glMatrixMode(GL_PROJECTION);
    ::glLoadIdentity();
    Com::View::SetProjectionTransform(camera);
    updateGL();
}

void GLWidget_Solid2d::mousePressEvent(QMouseEvent *event)
{
}

void GLWidget_Solid2d::mouseMoveEvent(QMouseEvent *event)
{
}

void GLWidget_Solid2d::StepTime()
{
    cur_time += dt;
    world.FieldValueExec(cur_time);
    solid.Solve(world);
    if( id_field_equiv_stress != 0 ){ solid.SetEquivStressValue(id_field_equiv_stress,world); } // �e?�g?��??I?I?e?d?X?V
    if( id_field_stress       != 0 ){ solid.SetStressValue(     id_field_stress,      world); } // ��??I?I?e?d?X?V
    world.FieldValueDependExec();
    drawer_ary.Update(world);
    updateGL();
}

void GLWidget_Solid2d::SetNewProblem()
{
    const unsigned int nprob = 27;	// number of problem settings
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
        {	// ?`?d?i?e
            std::vector<Com::CVector2D> vec_ary;
            vec_ary.push_back( Com::CVector2D(0.0,0.0) );
            vec_ary.push_back( Com::CVector2D(5.0,0.0) );
            vec_ary.push_back( Com::CVector2D(5.0,1.0) );
            vec_ary.push_back( Com::CVector2D(0.0,1.0) );
            cad_2d.AddPolygon( vec_ary );
        }
        world.Clear();
        const unsigned int id_base = world.AddMesh( Msh::CMesher2D(cad_2d,0.1) );
        const Fem::Field::CIDConvEAMshCad conv = world.GetIDConverter(id_base);

        cur_time = 0;
        dt = 0.05;
        solid.Clear();
        solid.UpdateDomain_Field(id_base, world);
        solid.SetSaveStiffMat(false);
        solid.SetStationary(true);
        // �eS�eI?I?�N?��fl?d?Y�fe
        solid.SetYoungPoisson(10.0,0.3,true);	// ???�g?O?|?A?|?A?\?�g�ha?I?Y�fe(???E��??I)
        solid.SetGeometricalNonlinear(false);
        solid.SetGravitation(0.0,0.0);
        solid.SetTimeIntegrationParameter(dt,0.7);

        unsigned int id_field_bc0 = solid.AddFixElemAry(conv.GetIdEA_fromCad(2,Cad::EDGE),world);
        unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromCad(4,Cad::EDGE),world);
        {
            Fem::Field::CField& bc1_field = world.GetField(id_field_bc0);
            bc1_field.SetValue("sin(t*PI*2*0.1)", 1,Fem::Field::VALUE, world,true);	// bc1_field?Iy?A?W?E�fP?U�gR?d�fC��A
        }

        // ?`��a?I?u?W?F?N?g?I�go?^
        drawer_ary.Clear();
        id_field_disp = solid.GetIdField_Disp();
        drawer_ary.PushBack( new Fem::Field::View::CDrawerFace(id_field_disp,false,world) );
        drawer_ary.PushBack( new Fem::Field::View::CDrawerEdge(id_field_disp,false,world) );
        drawer_ary.PushBack( new Fem::Field::View::CDrawerEdge(id_field_disp,true ,world) );
        drawer_ary.InitTrans(camera);	// View?A?W?I?�E?I?Y�fe
    }
    else if( iprob == 1 )	// ???��?s?n?d?U�e��?��?A?C???A�gI?a�ee
    {
        solid.SetSaveStiffMat(true);
    }
    else if( iprob == 2 )	// ???��?s?n?d?U�e��?��?A?C�gR�gI?a�ee?d��d?-
    {
        solid.SetSaveStiffMat(true);
        solid.SetStationary(false);
    }
    else if( iprob == 3 )	// ?-?��?A?-?�E?e
    {
        solid.SetYoungPoisson(50,0.3,true);
    }
    else if( iprob == 4 )	// ?a???-?��?A?-?�E?e
    {
        solid.SetYoungPoisson(100,0.3,true);
    }
    else if( iprob == 5 )	// ?o��??w�gI�hn?u?`?d?l?|???C???A�gI��d?I
    {
        solid.SetStationary(true);
        solid.SetGeometricalNonlinear(true);
    }
    else if( iprob == 6 )	// ?o��??w�gI�hn?u?`?d?l?|???C�gR�gI��d?I
    {
        solid.SetYoungPoisson(10,0.0,true);
        solid.SetStationary(false);
        solid.SetGeometricalNonlinear(true);
    }
    else if( iprob == 7 )	// ?u?`?~?[?[?X��??I?I?��?u?E�fu?A?I?\?|
    {
        id_field_equiv_stress = world.MakeField_FieldElemDim(id_field_disp,2,SCALAR,VALUE,BUBBLE);
        solid.SetGeometricalNonlinear(false);
        solid.SetStationary(true);
        // ?`��a?I?u?W?F?N?g?I�go?^
        drawer_ary.Clear();
        id_field_disp = solid.GetIdField_Disp();
        drawer_ary.PushBack( new Fem::Field::View::CDrawerFace(id_field_equiv_stress,false,world,id_field_equiv_stress, 0,0.5) );
        drawer_ary.PushBack( new Fem::Field::View::CDrawerEdge(id_field_disp,false,world) );
        drawer_ary.PushBack( new Fem::Field::View::CDrawerEdge(id_field_disp,true ,world) );
        // View?A?W?I?�E?I?Y�fe
        drawer_ary.InitTrans(camera);
    }
    else if( iprob == 8 )	// ?u?`?~?[?[?X��??I?I?��?u?E�fu?A?I?\?|
    {
        // ?`��a?I?u?W?F?N?g?I�go?^
        drawer_ary.Clear();
        id_field_disp = solid.GetIdField_Disp();
        drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world,id_field_equiv_stress, 0,0.5) );
        drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
        drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
        // View?A?W?I?�E?I?Y�fe
        drawer_ary.InitTrans(camera);
    }
    else if( iprob == 9 )	// �hM��??I?a�ee
    {
        id_field_equiv_stress = 0;
        id_field_stress = 0;
        ////////////////
        Cad::CCadObj2D cad_2d;
        {	// ?`?d?i?e
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

        cur_time = 0;
        dt = 0.05;
        solid.UpdateDomain_Field(id_base,world);
        solid.SetSaveStiffMat(false);	// ???��?s?n?U�e��?��?E?��
        solid.SetStationary(true);	// ?A�gI?a�ee?E?Z?b?g
        // �eS�eI?I?�N?��fl?d?Z?b?g
        solid.SetYoungPoisson(10.0,0.3,true);	// ???�g?O?|?A?|?A?\?�g�ha?I?Y�fe(???E��??I)
        solid.SetGeometricalNonlinear(false);	// ?o��??w�gI�hn?u?`?��?3??
        solid.SetGravitation(0.0,-0.1);	// ?d?I?I?Y�fe
        solid.SetTimeIntegrationParameter(dt);	// ???O?X?e?b?v?Y�fe

        unsigned int id_field_bc0 = solid.AddFixElemAry(conv.GetIdEA_fromCad(2,Cad::EDGE),world);
        unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromCad(6,Cad::EDGE),world);

        ////////////////////////////////
        // ��E�gx?e?I?Y�fe
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
        drawer_ary.InitTrans(camera);	// View?A?W?I?�E?I?Y�fe
    }
    else if( iprob == 10 )	// �hM��??I?d?l?��?�E?e?�}?A?d?a?s?e
    {
        drawer_ary.Clear();
        drawer_ary.PushBack( new View::CDrawerFace(id_field_temp,true, world, id_field_temp, -1,1) );
        drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
        drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
        drawer_ary.InitTrans(camera);	// View?A?W?I?�E?I?Y�fe
    }
    else if( iprob == 11 )	// �hM��??I?d?l?��?�E?e?�}?A?d?a?s?e
    {
        solid.SetThermalStress(0);
    }
    else if( iprob == 12 )
    {
        Cad::CCadObj2D cad_2d;
        {	// ?3?u?`?E?e?`?I??
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
        CIDConvEAMshCad conv = world.GetIDConverter(id_base);		// ID?I?�E?N?��?X

        cur_time = 0;
        dt = 0.05;
        solid.SetDomain_FieldEA(id_base,conv.GetIdEA_fromCad(1,Cad::LOOP),world);
        solid.SetSaveStiffMat(true);
        solid.SetStationary(true);
        solid.SetTimeIntegrationParameter(dt);	// ?^?C???X?e?b?v?d?Y�fe
        solid.SetYoungPoisson(2.5,0.3,true);	// ???�g?O?|?A?|?A?\?�g�ha?I?Y�fe(???E��??I)
        solid.SetGeometricalNonlinear(false);	// ?o��??w�gI�hn?u?`?��?d?l?��?��?E?��
        solid.SetGravitation(0.0,0.0);	// ?d?I?O

        unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromCad(3,Cad::EDGE),world);
        {
            CField& field = world.GetField(id_field_bc1);
            field.SetValue("0.3*sin(1.5*t)", 0,Fem::Field::VALUE, world,true);
            field.SetValue("0.1*(cos(t)+1)", 1,Fem::Field::VALUE, world,true);
        }
        unsigned int id_field_bc2 = solid.AddFixElemAry(conv.GetIdEA_fromCad(1,Cad::EDGE),world);

        // ?`��a?I?u?W?F?N?g?I�go?^
        drawer_ary.Clear();
        id_field_disp = solid.GetIdField_Disp();
        drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
        drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
        drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
        drawer_ary.InitTrans(camera);	// View?A?W?I?�E?I?Y�fe
    }
    else if( iprob == 13 )
    {
        solid.SetSaveStiffMat(true);
    }
    else if( iprob == 14 )
    {
        solid.SetSaveStiffMat(false);
        solid.SetStationary(false);
    }
    else if( iprob == 15 ){
        solid.SetStationary(true);
        solid.SetGeometricalNonlinear(true);
    }
    else if( iprob == 16 ){
        solid.SetStationary(false);
        solid.SetGeometricalNonlinear(true);
    }
    else if( iprob == 17 ){
        Cad::CCadObj2D cad_2d;
        {	// ?3?u?`?E?a?Q?A?E?a??
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
        CIDConvEAMshCad conv = world.GetIDConverter(id_base);  // ID?I?�E?N?��?X

        cur_time = 0;
        dt = 0.05;
        solid.SetDomain_FieldEA(id_base,conv.GetIdEA_fromCad(2,Cad::LOOP),world);
        solid.SetTimeIntegrationParameter(dt);
        solid.SetSaveStiffMat(false);
        solid.SetStationary(true);

        solid.SetYoungPoisson(3.0,0.3,true);	// ???�g?O?|?A?|?A?\?�g�ha?I?Y�fe(???E��??I)
        solid.SetGeometricalNonlinear(false);	// ?o��??w�gI�hn?u?`?��?d?l?��?��?E?��
        solid.SetGravitation(0.0,0.0);

        unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromCad(3,Cad::EDGE),world);
        {
            CField& field = world.GetField(id_field_bc1);
            field.SetValue("0.3*sin(1.5*t)",     0,Fem::Field::VALUE, world,true);
            field.SetValue("0.1*(cos(t)+1)+0.1", 1,Fem::Field::VALUE, world,true);
        }
        unsigned int id_field_bc2 = solid.AddFixElemAry(conv.GetIdEA_fromCad(5,Cad::EDGE),world);

        // ?`��a?I?u?W?F?N?g?I�go?^
        drawer_ary.Clear();
        id_field_disp = solid.GetIdField_Disp();
        drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
        drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
//		drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
        drawer_ary.PushBack( new View::CDrawerEdge(id_base,true,world) );
        drawer_ary.InitTrans(camera);	// View?A?W?I?�E?I?Y�fe
    }
    else if( iprob == 18 )
    {
        solid.SetSaveStiffMat(true);
    }
    else if( iprob == 19 )
    {
        solid.SetSaveStiffMat(false);
        solid.SetStationary(false);
    }
    else if( iprob == 20 ){
        solid.SetStationary(true);
        solid.SetGeometricalNonlinear(true);
    }
    else if( iprob == 21 ){
        solid.SetStationary(false);
        solid.SetGeometricalNonlinear(true);
    }
    else if( iprob == 22 ){
        Cad::CCadObj2D cad_2d;
        {	// �f�E?u?`?a?S?A?E?a??
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
        const Fem::Field::CIDConvEAMshCad& conv = world.GetIDConverter(id_base);  // ID?I?�E?N?��?X

        cur_time = 0;
        dt = 0.05;
        solid.UpdateDomain_Field(id_base,world);	// ��d?I?I?a?d?Z?b?g
        solid.SetTimeIntegrationParameter(dt);	// ???O???Y?d?Z?b?g
        solid.SetSaveStiffMat(false);	// ???��?s?n?d?U�e��?��?E?��
        solid.SetStationary(true);	// ?A�gI?a�ee
        // �eS�eI?I?�N?��fn?d?Z?b?g
        solid.SetYoungPoisson(1.0,0.3,true);	// ???�g?O?|?A?|?A?\?�g�ha?I?Y�fe(???E��??I)
        solid.SetGeometricalNonlinear(false);
        solid.SetGravitation(0.0,-0.0);

        {	// St.Venant-Kirchhoff�eI
            Fem::Eqn::CEqn_Solid2D eqn = solid.GetEqnation(conv.GetIdEA_fromCad(1,Cad::LOOP));
            eqn.SetGeometricalNonlinear(true);
            solid.SetEquation(eqn);
        }
        {	// ?_?c?c?���fe?��eI
            Fem::Eqn::CEqn_Solid2D eqn = solid.GetEqnation(conv.GetIdEA_fromCad(2,Cad::LOOP));
            eqn.SetYoungPoisson(0.1,0.3,true);
            solid.SetEquation(eqn);
        }
        unsigned int id_field_temp = world.MakeField_FieldElemAry(id_base, conv.GetIdEA_fromCad(3,Cad::LOOP),
            SCALAR,VALUE,CORNER);
        {	// �hM?I?e
            CField& field = world.GetField(id_field_temp);
            field.SetValue("0.1*sin(3.14*4*y)*sin(2*t)", 0,Fem::Field::VALUE, world,true);
        }
        {	// �hM��??I?d?l?��?��???u?`�fe?��eI
            Fem::Eqn::CEqn_Solid2D eqn = solid.GetEqnation(conv.GetIdEA_fromCad(3,Cad::LOOP));
            eqn.SetThermalStress(id_field_temp);
            solid.SetEquation(eqn);
        }
        {	// ?d?��?u?`�fe?��eI
            Fem::Eqn::CEqn_Solid2D eqn = solid.GetEqnation(conv.GetIdEA_fromCad(4,Cad::LOOP));
            eqn.SetYoungPoisson(10,0.3,true);
            solid.SetEquation(eqn);
        }

        id_field_disp_fix0 = solid.AddFixElemAry(conv.GetIdEA_fromCad(2,Cad::EDGE),world);

        // ?`��a?I?u?W?F?N?g?I�go?^
        drawer_ary.Clear();
        id_field_disp = solid.GetIdField_Disp();
        drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
        drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
//		drawer_ary.PushBack( new View::CDrawerEdge(id_base,false,world) );
        drawer_ary.InitTrans(camera);	// View?A?W?I?�E?I?Y�fe
    }
    else if( iprob == 23 ){
        solid.SetRho(0.0001);
        solid.SetStationary(false);
        {	// ?I?E?I?e?d?a��o?E�fP?U�gR?E?Y�fe
            CField& field = world.GetField(id_field_disp_fix0);
            field.SetValue("0.5*cos(2*t)", 1,Fem::Field::VALUE, world,true);
        }
    }
    else if( iprob == 24 ){
        Cad::CCadObj2D cad_2d;
        {	// �f�E?u?`?a?S?A?E?a??
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
        const Fem::Field::CIDConvEAMshCad& conv = world.GetIDConverter(id_base);  // ID?I?�E?N?��?X

        cur_time = 0;
        dt = 0.05;
        solid.UpdateDomain_Field(id_base,world);	// ��d?I?I?a?d?Z?b?g
        solid.SetTimeIntegrationParameter(dt);	// ???O???Y?d?Z?b?g
        solid.SetSaveStiffMat(false);	// ???��?s?n?d?U�e��?��?E?��
        solid.SetStationary(false);	// ?A�gI?a�ee
        // �eS�eI?I?�N?��fn?d?Z?b?g
        solid.SetYoungPoisson(1.0,0.3,true);	// ???�g?O?|?A?|?A?\?�g�ha?I?Y�fe(???E��??I)
        solid.SetGeometricalNonlinear(false);
        solid.SetGravitation(0.0,-0.0);
        solid.SetRho(0.001);

        {	// ?_?c?c?���fe?��eI
            Fem::Eqn::CEqn_Solid2D eqn = solid.GetEqnation(conv.GetIdEA_fromCad(1,Cad::LOOP));
            eqn.SetYoungPoisson(0.1,0.3,true);
            solid.SetEquation(eqn);
        }
        {	// ?d?��?u?`�fe?��eI
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

        // ?`��a?I?u?W?F?N?g?I�go?^
        drawer_ary.Clear();
        id_field_disp = solid.GetIdField_Disp();
        drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
        drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
        drawer_ary.InitTrans(camera);	// View?A?W?I?�E?I?Y�fe
    }
    else if( iprob == 25 )
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
        Msh::CMesher2D mesh_2d(cad_2d,0.1);

        world.Clear();
        const unsigned int id_base = world.AddMesh(mesh_2d);
        const Fem::Field::CIDConvEAMshCad& conv = world.GetIDConverter(id_base);  // ID?I?�E?N?��?X
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
        cur_time = 0;
        dt = 0.05;
        solid.UpdateDomain_Field(id_base2, world);
        solid.SetSaveStiffMat(false);
        solid.SetStationary(true);
        // �eS�eI?I?�N?��fl?d?Y�fe
        solid.SetYoungPoisson(10.0,0.3,true);	// ???�g?O?|?A?|?A?\?�g�ha?I?Y�fe(???E��??I)
//		solid.SetRho(10);
        solid.SetGeometricalNonlinear(false);
        solid.SetGravitation(0.0,0.0);
        solid.SetTimeIntegrationParameter(dt,0.7);

        unsigned int id_field_bc0 = solid.AddFixElemAry(conv.GetIdEA_fromCad(3,Cad::EDGE),world);
        unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromCad(5,Cad::EDGE),world);
        {
            CField& bc1_field = world.GetField(id_field_bc0);
            bc1_field.SetValue("0.1*sin(t*PI*2*0.1)",     0,Fem::Field::VALUE, world,true);	// bc1_field?Iy?A?W?E�fP?U�gR?d�fC��A
            bc1_field.SetValue("0.1*(1-cos(t*PI*2*0.1))", 1,Fem::Field::VALUE, world,true);	// bc1_field?Iy?A?W?E�fP?U�gR?d�fC��A
        }

        // ?`��a?I?u?W?F?N?g?I�go?^
        drawer_ary.Clear();
        id_field_disp = solid.GetIdField_Disp();
        std::cout << "id_field_disp : " << id_field_disp << std::endl;
        drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
        drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
        drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
        drawer_ary.InitTrans(camera);	// View?A?W?I?�E?I?Y�fe
    }
    else if( iprob == 26 )
    {
        cur_time = 0;
        dt = 0.05;
        Cad::CCadObj2D cad_2d;
        unsigned int id_e;
        unsigned int id_l;
        {	// ?`?d?i?e
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
        dt = 0.05;
        const unsigned int id_base = world.AddMesh(mesh_2d);
        const Fem::Field::CIDConvEAMshCad conv = world.GetIDConverter(id_base);
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
        // Setting Material Parameter
        solid.SetYoungPoisson(10.0,0.3,true);	// ???�g?O?|?A?|?A?\?�g�ha?I?Y�fe(???E��??I)
        solid.SetGeometricalNonlinear(false);
        solid.SetGravitation(0.0,0.0);
        solid.SetTimeIntegrationParameter(dt,0.7);

        unsigned int id_field_bc0 = solid.AddFixElemAry(conv.GetIdEA_fromCad(2,Cad::EDGE),world);
        unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromCad(4,Cad::EDGE),world);
        {
            CField& bc1_field = world.GetField(id_field_bc0);
            bc1_field.SetValue("0.5*(1-cos(t*PI*2*0.1))", 0,Fem::Field::VALUE, world,true);	// bc1_field?Iy?A?W?E�fP?U�gR?d�fC��A
            bc1_field.SetValue("0.2*sin(t*PI*2*0.1)",     1,Fem::Field::VALUE, world,true);	// bc1_field?Iy?A?W?E�fP?U�gR?d�fC��A
        }

        id_field_disp = solid.GetIdField_Disp();
        id_field_equiv_stress = world.MakeField_FieldElemDim(id_field_disp,2,SCALAR,VALUE,BUBBLE);

        // Setting of Visualizatoin
        drawer_ary.Clear();
        drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world,id_field_equiv_stress) );
//		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
        drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
        drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
        drawer_ary.InitTrans(camera);   // Fit Transformation
    }
    else if( iprob == 27 )
    {
        Cad::CCadObj2D cad_2d;
        unsigned int id_e1, id_e2, id_e3;
        unsigned int id_l;
        {	// define shape
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
        dt = 0.05;
        const unsigned int id_base = world.AddMesh(mesh_2d);
        const Fem::Field::CIDConvEAMshCad conv = world.GetIDConverter(id_base);
        unsigned int id_base2 = 0;
        {
            std::vector<unsigned int> mapVal2Co;
            std::vector< std::vector<int> > aLnods;
            {
                std::vector<unsigned int> aIdMsh_Inc;
                aIdMsh_Inc.push_back( mesh_2d.GetElemID_FromCadID(id_l,Cad::LOOP) );	// ?J?b?g?3?e?e???[?v?d�go?^
                std::vector<unsigned int> aIdMshCut;
                aIdMshCut.push_back( mesh_2d.GetElemID_FromCadID(id_e1,Cad::EDGE) );	// ?J?b?g?�E?e?O?d�go?^
                aIdMshCut.push_back( mesh_2d.GetElemID_FromCadID(id_e2,Cad::EDGE) );	// ?J?b?g?�E?e?O?d�go?^
                mesh_2d.GetClipedMesh(aLnods,mapVal2Co, aIdMsh_Inc,aIdMshCut);
            }
            std::vector<unsigned int> aIdEA_Inc;
            aIdEA_Inc.push_back( conv.GetIdEA_fromCad(id_l,Cad::LOOP) );
            id_base2 = world.SetCustomBaseField(id_base,aIdEA_Inc, aLnods,mapVal2Co);
        }

        solid.UpdateDomain_Field(id_base2, world);
        solid.SetSaveStiffMat(false);
        solid.SetStationary(true);
        // Set Material Parameter
        solid.SetYoungPoisson(10.0,0.3,true);	// ???�g?O?|?A?|?A?\?�g�ha?I?Y�fe(???E��??I)
        solid.SetGeometricalNonlinear(false);
        solid.SetGravitation(0.0,0.0);
        solid.SetTimeIntegrationParameter(dt,0.7);

        unsigned int id_field_bc0 = solid.AddFixElemAry(conv.GetIdEA_fromCad(1,Cad::EDGE),world);
        unsigned int id_field_bc1 = solid.AddFixElemAry(conv.GetIdEA_fromCad(3,Cad::EDGE),world);
        {
            CField& bc1_field = world.GetField(id_field_bc0);
            bc1_field.SetValue("0.2*sin(t*PI*2*0.1)", 0,Fem::Field::VALUE, world,true);	// bc1_field?Iy?A?W?E�fP?U�gR?d�fC��A
        }
        id_field_disp = solid.GetIdField_Disp();
        std::cout << conv.GetIdEA_fromCad(id_e3,Cad::EDGE) << std::endl;
        unsigned int id_field_disp_edge = world.GetPartialField(id_field_disp,conv.GetIdEA_fromCad(id_e3,Cad::EDGE));
        std::cout << id_field_disp_edge << " " << id_e3 << std::endl;

        id_field_equiv_stress = world.MakeField_FieldElemDim(id_field_disp,2,SCALAR,VALUE,BUBBLE);

        // registration of drawing objects
        drawer_ary.Clear();
        drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world,id_field_equiv_stress,0,0.5) );
        drawer_ary.PushBack( new View::CDrawerFace(id_field_disp_edge,false,world) );
//		drawer_ary.PushBack( new View::CDrawerFace(id_field_disp,false,world) );
        drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,false,world) );
        drawer_ary.PushBack( new View::CDrawerEdge(id_field_disp,true ,world) );
        drawer_ary.InitTrans(camera);	// setting of model-view transformation
    }

    iprob++;
    if( iprob == nprob ){
        iprob = 0;
    }
}
