/*******************************************************************************************************
Copyright 2017 - pvyield GmbH / Timo Richert
Copyright 2019 Alliance for Sustainable Energy, LLC
Redistribution and use in source and binary forms, with or without modification, are permitted provided
that the following conditions are met :
1.	Redistributions of source code must retain the above copyright notice, this list of conditions
and the following disclaimer.
2.	Redistributions in binary form must reproduce the above copyright notice, this list of conditions
and the following disclaimer in the documentation and/or other materials provided with the distribution.
3.	Neither the name of the copyright holder nor the names of its contributors may be used to endorse
or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER, CONTRIBUTORS, UNITED STATES GOVERNMENT OR UNITED STATES
DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************************************/
#ifndef __mlmodel_h
#define __mlmodel_h


#include "lib_pvmodel.h"
//#include "mlm_spline.h"
#include "bspline.h"

using namespace SPLINTER;


class mlmodel_module_t : public pvmodule_t
{
public:
	int N_series;
	int N_parallel;
	int N_diodes;

	double Width;
	double Length;

	double V_mp_ref;
	double I_mp_ref;
	double V_oc_ref;
	double I_sc_ref;

	double S_ref;
	double T_ref;

	double R_shref;
	double R_sh0;
	double R_shexp;
	double R_s;
	double alpha_isc;
	double beta_voc_spec;
	double E_g;
	double n_0;
	double mu_n;
	double D2MuTau;
	
	int T_mode;
	double T_c_no_tnoct;
	int T_c_no_mounting;
	int T_c_no_standoff;
	double T_c_fa_alpha;
	double T_c_fa_U0;
	double T_c_fa_U1;

	int AM_mode;
	double AM_c_sa[5];
	double AM_c_lp[6];

	int IAM_mode;
	double IAM_c_as;
	double IAM_c_sa[6];
	int IAM_c_cs_elements;
	double IAM_c_cs_incAngle[100];
	double IAM_c_cs_iamValue[100];

	double groundRelfectionFraction;

	mlmodel_module_t();

	virtual double AreaRef() { return (Width * Length); }
	virtual double VmpRef() { return V_mp_ref; }
	virtual double ImpRef() { return I_mp_ref; }
	virtual double VocRef() { return V_oc_ref; }
	virtual double IscRef() { return I_sc_ref; }
	virtual bool operator() (pvinput_t &input, double TcellC, double opvoltage, pvoutput_t &output);
	virtual void initializeManual();

private:
	bool isInitialized;
	double nVT;
	double I_0ref;
	double I_Lref;
	double Vbi;
//	tk::spline iamSpline;
	BSpline m_bspline3;

};

class mock_celltemp_t : public pvcelltemp_t
{
public:
	virtual bool operator() (pvinput_t &input, pvmodule_t &module, double opvoltage, double &Tcell);
};

#endif
