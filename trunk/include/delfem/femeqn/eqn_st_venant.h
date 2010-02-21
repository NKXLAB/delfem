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
@brief St.Venant-Kirchhoff�̗̂v�f�����쐬���̃C���^�[�t�F�[�X
@author Nobuyuki Umetani
@sa http://ums.futene.net/wiki/FEM/46454D20666F722053742E56656E616E742D4B69726368686F6666204D6174657269616C.html
*/



#if !defined(EQN_ST_VENSNT_H)
#define EQN_ST_VENANT_H

#if defined(__VISUALC__)
#pragma warning( disable : 4786 )
#endif

#include "delfem/linearsystem_interface_eqnsys.h"

namespace Fem{
namespace Ls{
	class CLinearSystem_Field;
}
namespace Field{
	class CField;
	class CFieldWorld;
}
namespace Eqn
{
////////////////////////////////
// �Q�c�̕�����

/*! @defgroup eqn_stvenant St.Venant-Kirchhoff�̘̂A���ꎟ�������ւ̃}�[�W�֐��Q
@ingroup FemEqnMargeFunction
�@
@f$ S = \frac{\partial W}{\partial E}@f$
*/
/*!@{*/

/*!
@brief �Q�����ÓISt.Venant-Kirchhoff�̂̃}�[�W
@param [in,out] ls �}�[�W�����A���ꎟ������
@param lambda [in] �������萔 @f$ \lambda @f$
@param myu [in] �������萔 @f$ \mu @f$
@param rho [in] ���ʖ��x
@
*/
bool AddLinSys_StVenant2D_Static(
		Fem::Eqn::CLinearSystem_EqnInterface& ls,
		double lambda, double myu,
		double rho, double f_x, double f_y,
		const Fem::Field::CFieldWorld& world,
		unsigned int id_field_disp,
		unsigned int id_ea = 0);

/*!
@brief �Q�������ISt.Venant-Kirchhoff�̂̃}�[�W
@param [in,out] ls �A���ꎟ������
@param lambda [in] �������萔 @f$ \lambda @f$
@param myu [in] �������萔 @f$ \mu @f$
@param rho [in] ���ʖ��x
*/
bool AddLinSys_StVenant2D_NonStatic_NewmarkBeta(
		double dt, double gamma_newmark, double beta, 
        Fem::Eqn::CLinearSystem_EqnInterface& ls,
		double lambda, double myu,
		double rho, double g_x, double g_y,
		const Fem::Field::CFieldWorld& world,
		unsigned int id_field_disp, 
		bool is_initial,
		unsigned int id_ea = 0 );
		
////////////////////////////////
// �R�c�̕�����

/*!
@brief �R�����ÓISt.Venant-Kirchhoff�̂̃}�[�W
@param [in,out] ls �A���ꎟ������
@param lambda [in] �������萔 @f$ \lambda @f$
@param myu [in] �������萔 @f$ \mu @f$
@param rho [in] ���ʖ��x
@param g_x [in] x�����̑̐ϗ�
@param g_y [in] y�����̑̐ϗ�
@param g_z [in] z�����̑̐ϗ�
*/
bool AddLinSys_StVenant3D_Static(
        Fem::Eqn::CLinearSystem_EqnInterface& ls,
		double lambda, double myu,
		double  rho, double g_x, double g_y, double g_z,
		const Fem::Field::CFieldWorld& world,
		unsigned int id_field_disp,
		unsigned int id_ea = 0 );

/*!
@brief �R�������ISt.Venant-Kirchhoff�̂̃}�[�W
@param [in,out] ls �A���ꎟ������
@lambda [in] �������萔
@myu [in] �������萔
@rho [in] ���ʖ��x
@g_x [in] x�����̑̐ϗ�
@g_y [in] y�����̑̐ϗ�
@g_z [in] z�����̑̐ϗ�
*/
bool AddLinSys_StVenant3D_NonStatic_NewmarkBeta(
		double dt, double gamma, double beta,
        Fem::Eqn::CLinearSystem_EqnInterface& ls,
		double lambda, double myu,
		double  rho, double g_x, double g_y, double g_z,
		const Fem::Field::CFieldWorld& world,
		unsigned int id_field_disp,
		bool is_initial,
		unsigned int id_ea = 0);

/*!@}*/
}
}

#endif
