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
@brief ���`�e���̗̂v�f�����쐬���̃C���^�[�t�F�[�X
@author Nobuyuki Umetani
@sa http://ums.futene.net/wiki/FEM/46454D20666F72204C696E65617220456C6173746963.html
*/

#if !defined(EQN_LINEAR_SOLID_3D_H)
#define EQN_LINEAR_SOLID_3D_H

#include <vector>

#if defined(__VISUALC__)
#pragma warning( disable : 4786 )
#endif

namespace Fem{
namespace Ls
{
class CLinearSystem_Field;
class CLinearSystem_Save;
class CLinearSystem_SaveDiaM_NewmarkBeta;
class CLinearSystem_Eigen;
class CPreconditioner;
}

namespace Field
{
class CField;
class CFieldWorld;
}

namespace Eqn{

class ILinearSystem_Eqn;

/*! @defgroup eqn_linear_solid ���`�e���̂̕��������}�[�W����֐��Q
@ingroup FemEqnMargeFunction
�@
*/
//! @{

// �ÓI���`�e����
bool AddLinSys_LinearSolid3D_Static(
		Fem::Ls::CLinearSystem_Field& ls,
		double lambda, double myu,
		double  rho, double g_x, double g_y, double g_z,
		const Fem::Field::CFieldWorld& world,
		unsigned int id_field_disp );

// ���I���`�e����
bool AddLinSys_LinearSolid3D_NonStatic_NewmarkBeta(
		double dt, double gamma, double beta,
		Eqn::ILinearSystem_Eqn& ls,
		double lambda, double myu,
		double  rho, double g_x, double g_y, double g_z,
		const Fem::Field::CFieldWorld& world,
		unsigned int id_field_disp );

// �ÓI���`�e����(�����s���ۑ�)
bool AddLinSys_LinearSolid3D_Static_SaveStiffMat(
		Fem::Ls::CLinearSystem_Save& ls,
		double lambda, double myu,
		double  rho, double g_x, double g_y, double g_z,
		const Fem::Field::CFieldWorld& world,
		unsigned int id_field_disp );

// ���I�e���̂̌ŗL�l��͗p�̍s������
bool AddLinSys_LinearSolid3D_Eigen(
		Fem::Ls::CLinearSystem_Eigen& ls,
		double lambda, double myu, double rho,
		const Fem::Field::CFieldWorld& world,
		unsigned int id_field_disp );
//! @}
}
}

#endif
