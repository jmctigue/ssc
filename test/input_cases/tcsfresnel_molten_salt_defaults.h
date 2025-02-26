/**
BSD-3-Clause
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
*/

#ifndef _TCSFRESNEL_MOLTEN_SALT_DEFAULTS_H_
#define _TCSFRESNEL_MOLTEN_SALT_DEFAULTS_H_

#include <stdio.h>
#include "../input_cases/code_generator_utilities.h"

//const char * SSCDIR = std::getenv("SSCDIR");
//const char * SAMNTDIR = std::getenv("SAMNTDIR");

char nlfms_ud_ind_od_path[512];
int nlfms1 = sprintf(nlfms_ud_ind_od_path, "%s/test/input_cases/linearfresnel_molten_salt_data/ud_ind_od.csv", std::getenv("SSCDIR"));

/**
*  Default data for tcsfresnel_molten_salt technology model
*/
ssc_data_t tcsfresnel_molten_salt_defaults()
{
    ssc_data_t data = ssc_data_create();

	char solar_resource_path[512];
	// This is a copy of the actual weather file used, which has been copied to the ssc repo so it can be found by Travis CI for its tests.
	//  The actual weather file used by SAM could change and thus change the UI output values (different input (i.e., weather file) -> different outputs)
	int n1 = sprintf(solar_resource_path, "%s/test/input_cases/linearfresnel_molten_salt_data/tucson_az_32.116521_-110.933042_psmv3_60_tmy.csv", std::getenv("SSCDIR"));

	ssc_data_set_string(data, "file_name", solar_resource_path);
    ssc_data_set_number(data, "track_mode", 1);
    ssc_data_set_number(data, "tilt", 0);
    ssc_data_set_number(data, "azimuth", 0);
    ssc_data_set_number(data, "system_capacity", 99999.8984375);
    ssc_number_t p_weekday_schedule[288] = { 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5 };
    ssc_data_set_matrix(data, "weekday_schedule", p_weekday_schedule, 12, 24);
    ssc_number_t p_weekend_schedule[288] = { 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
    ssc_data_set_matrix(data, "weekend_schedule", p_weekend_schedule, 12, 24);
    ssc_data_set_number(data, "nMod", 16);
    ssc_data_set_number(data, "nLoops", 150);
    ssc_data_set_number(data, "eta_pump", 0.84999999999999998);
    ssc_data_set_number(data, "HDR_rough", 4.57e-05);
    ssc_data_set_number(data, "theta_stow", 170);
    ssc_data_set_number(data, "theta_dep", 10);
    ssc_data_set_number(data, "FieldConfig", 2);
    ssc_data_set_number(data, "T_startup", 400);
    ssc_data_set_number(data, "pb_rated_cap", 111.11100006103516);
    ssc_data_set_number(data, "m_dot_htfmin", 3.0158900000000002);
    ssc_data_set_number(data, "m_dot_htfmax", 14.4763);
    ssc_data_set_number(data, "T_loop_in_des", 293);
    ssc_data_set_number(data, "T_loop_out", 525);
    ssc_data_set_number(data, "Fluid", 18);
    ssc_data_set_number(data, "T_field_ini", 300);
    ssc_number_t p_field_fl_props[7] = { 0, 0, 0, 0, 0, 0, 0 };
    ssc_data_set_matrix(data, "field_fl_props", p_field_fl_props, 1, 7);
    ssc_data_set_number(data, "T_fp", 263);
    ssc_data_set_number(data, "I_bn_des", 950);
    ssc_data_set_number(data, "V_hdr_max", 3);
    ssc_data_set_number(data, "V_hdr_min", 2);
    ssc_data_set_number(data, "Pipe_hl_coef", 0.45000000000000001);
    ssc_data_set_number(data, "SCA_drives_elec", 125);
    ssc_data_set_number(data, "fthrok", 1);
    ssc_data_set_number(data, "fthrctrl", 2);
    ssc_data_set_number(data, "ColAz", 0);
    ssc_data_set_number(data, "solar_mult", 2.2999999523162842);
    ssc_data_set_number(data, "mc_bal_hot", 0.20000000000000001);
    ssc_data_set_number(data, "mc_bal_cold", 0.20000000000000001);
    ssc_data_set_number(data, "mc_bal_sca", 4.5);
    ssc_data_set_number(data, "water_per_wash", 0.02);
    ssc_data_set_number(data, "washes_per_year", 120);
    ssc_data_set_number(data, "opt_model", 2);
    ssc_data_set_number(data, "A_aperture", 470.30000000000001);
    ssc_data_set_number(data, "reflectivity", 0.93500000000000005);
    ssc_data_set_number(data, "TrackingError", 1);
    ssc_data_set_number(data, "GeomEffects", 1);
    ssc_data_set_number(data, "Dirt_mirror", 0.94999999999999996);
    ssc_data_set_number(data, "Error", 0.73199999999999998);
    ssc_data_set_number(data, "L_mod", 44.799999999999997);
    ssc_number_t p_IAM_T_coefs[5] = { 0.98960000000000004, 0.043999999999999997, -0.072099999999999997, -0.23269999999999999, 0 };
    ssc_data_set_array(data, "IAM_T_coefs", p_IAM_T_coefs, 5);
    ssc_number_t p_IAM_L_coefs[5] = { 1.0031000000000001, -0.22589999999999999, 0.53680000000000005, -1.6434, 0.72219999999999995 };
    ssc_data_set_array(data, "IAM_L_coefs", p_IAM_L_coefs, 5);
    ssc_number_t p_OpticalTable[121] = { 0, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 0, 1, 0.97894000000000003, 0.95382, 0.94864000000000004, 0.91161999999999999, 0.86104000000000003, 0.7036, 0.48455999999999999, 0.23608999999999999, 0, 10, 0.97790999999999995, 0.95731999999999995, 0.93274999999999997, 0.92767999999999995, 0.89148000000000005, 0.84201999999999999, 0.68806, 0.47386, 0.23086999999999999, 0, 20, 0.92188999999999999, 0.90246999999999999, 0.87931999999999999, 0.87453999999999998, 0.84040999999999999, 0.79378000000000004, 0.64863999999999999, 0.44671, 0.21765000000000001, 0, 30, 0.83048999999999995, 0.81299999999999994, 0.79213999999999996, 0.78783999999999998, 0.75709000000000004, 0.71509, 0.58433000000000002, 0.40242, 0.19606999999999999, 0, 40, 0.70118999999999998, 0.68642000000000003, 0.66881000000000002, 0.66517999999999999, 0.63922000000000001, 0.60375000000000001, 0.49336000000000002, 0.33977000000000002, 0.16553999999999999, 0, 50, 0.53359999999999996, 0.52236000000000005, 0.50895999999999997, 0.50619000000000003, 0.48643999999999998, 0.45945000000000003, 0.37544, 0.25856000000000001, 0.12598000000000001, 0, 60, 0.32562999999999998, 0.31877, 0.31058999999999998, 0.30891000000000002, 0.29685, 0.28038000000000002, 0.22911000000000001, 0.15779000000000001, 0.076880000000000004, 0, 70, 0.1173, 0.11483, 0.11187999999999999, 0.11128, 0.10693, 0.10100000000000001, 0.082530000000000006, 0.056840000000000002, 0.027689999999999999, 0, 80, 0.01103, 0.010800000000000001, 0.01052, 0.010460000000000001, 0.010059999999999999, 0.0094999999999999998, 0.0077600000000000004, 0.0053400000000000001, 0.0025999999999999999, 0, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    ssc_data_set_matrix(data, "OpticalTable", p_OpticalTable, 11, 11);
    ssc_data_set_number(data, "rec_model", 2);
    ssc_number_t p_HCE_FieldFrac[4] = { 0.98499999999999999, 0.01, 0.0050000000000000001, 0 };
    ssc_data_set_array(data, "HCE_FieldFrac", p_HCE_FieldFrac, 4);
    ssc_number_t p_D_abs_in[4] = { 0.066000000000000003, 0.066000000000000003, 0.066000000000000003, 0.066000000000000003 };
    ssc_data_set_array(data, "D_abs_in", p_D_abs_in, 4);
    ssc_number_t p_D_abs_out[4] = { 0.070000000000000007, 0.070000000000000007, 0.070000000000000007, 0.070000000000000007 };
    ssc_data_set_array(data, "D_abs_out", p_D_abs_out, 4);
    ssc_number_t p_D_glass_in[4] = { 0.115, 0.115, 0.115, 0.115 };
    ssc_data_set_array(data, "D_glass_in", p_D_glass_in, 4);
    ssc_number_t p_D_glass_out[4] = { 0.12, 0.12, 0.12, 0.12 };
    ssc_data_set_array(data, "D_glass_out", p_D_glass_out, 4);
    ssc_number_t p_D_plug[4] = { 0, 0, 0, 0 };
    ssc_data_set_array(data, "D_plug", p_D_plug, 4);
    ssc_number_t p_Flow_type[4] = { 1, 1, 1, 1 };
    ssc_data_set_array(data, "Flow_type", p_Flow_type, 4);
    ssc_number_t p_Rough[4] = { 4.5000000000000003e-05, 4.5000000000000003e-05, 4.5000000000000003e-05, 4.5000000000000003e-05 };
    ssc_data_set_array(data, "Rough", p_Rough, 4);
    ssc_number_t p_alpha_env[4] = { 0.02, 0.02, 0, 0 };
    ssc_data_set_array(data, "alpha_env", p_alpha_env, 4);
    ssc_number_t p_epsilon_abs_1[18] = { 100, 0.064000000000000001, 150, 0.066500000000000004, 200, 0.070000000000000007, 250, 0.074499999999999997, 300, 0.080000000000000002, 350, 0.086499999999999994, 400, 0.094, 450, 0.10249999999999999, 500, 0.112 };
    ssc_data_set_matrix(data, "epsilon_abs_1", p_epsilon_abs_1, 9, 2);
    ssc_number_t p_epsilon_abs_2[2] = { 0, 0.65000000000000002 };
    ssc_data_set_matrix(data, "epsilon_abs_2", p_epsilon_abs_2, 1, 2);
    ssc_number_t p_epsilon_abs_3[2] = { 0, 0.65000000000000002 };
    ssc_data_set_matrix(data, "epsilon_abs_3", p_epsilon_abs_3, 1, 2);
    ssc_number_t p_epsilon_abs_4[1] = { 0 };
    ssc_data_set_matrix(data, "epsilon_abs_4", p_epsilon_abs_4, 1, 1);
    ssc_number_t p_alpha_abs[4] = { 0.95999999999999996, 0.95999999999999996, 0.80000000000000004, 0 };
    ssc_data_set_array(data, "alpha_abs", p_alpha_abs, 4);
    ssc_number_t p_Tau_envelope[4] = { 0.96299999999999997, 0.96299999999999997, 1, 0 };
    ssc_data_set_array(data, "Tau_envelope", p_Tau_envelope, 4);
    ssc_number_t p_epsilon_glass[4] = { 0.85999999999999999, 0.85999999999999999, 1, 0 };
    ssc_data_set_array(data, "epsilon_glass", p_epsilon_glass, 4);
    ssc_number_t p_GlazingIntactIn[4] = { 1, 1, 0, 1 };
    ssc_data_set_array(data, "GlazingIntactIn", p_GlazingIntactIn, 4);
    ssc_number_t p_P_a[4] = { 0.0001, 750, 750, 0 };
    ssc_data_set_array(data, "P_a", p_P_a, 4);
    ssc_number_t p_AnnulusGas[4] = { 27, 1, 1, 27 };
    ssc_data_set_array(data, "AnnulusGas", p_AnnulusGas, 4);
    ssc_number_t p_AbsorberMaterial[4] = { 1, 1, 1, 1 };
    ssc_data_set_array(data, "AbsorberMaterial", p_AbsorberMaterial, 4);
    ssc_number_t p_Shadowing[4] = { 0.95999999999999996, 0.95999999999999996, 0.95999999999999996, 0.96299999999999997 };
    ssc_data_set_array(data, "Shadowing", p_Shadowing, 4);
    ssc_number_t p_dirt_env[4] = { 0.97999999999999998, 0.97999999999999998, 1, 0.97999999999999998 };
    ssc_data_set_array(data, "dirt_env", p_dirt_env, 4);
    ssc_number_t p_Design_loss[4] = { 150, 1100, 1500, 0 };
    ssc_data_set_array(data, "Design_loss", p_Design_loss, 4);
    ssc_data_set_number(data, "L_mod_spacing", 1);
    ssc_data_set_number(data, "L_crossover", 15);
    ssc_number_t p_HL_T_coefs[5] = { 0, 0.67200000000000004, 0.0025560000000000001, 0, 0 };
    ssc_data_set_array(data, "HL_T_coefs", p_HL_T_coefs, 5);
    ssc_number_t p_HL_w_coefs[5] = { 1, 0, 0, 0, 0 };
    ssc_data_set_array(data, "HL_w_coefs", p_HL_w_coefs, 5);
    ssc_data_set_number(data, "DP_nominal", 2.5);
    ssc_number_t p_DP_coefs[4] = { 0, 1, 0, 0 };
    ssc_data_set_array(data, "DP_coefs", p_DP_coefs, 4);
    ssc_data_set_number(data, "rec_htf_vol", 1);
    ssc_data_set_number(data, "T_amb_sf_des", 42);
    ssc_data_set_number(data, "V_wind_des", 4);
    ssc_data_set_number(data, "I_b", 0);
    ssc_data_set_number(data, "T_db", 15);
    ssc_data_set_number(data, "V_wind", 1.5);
    ssc_data_set_number(data, "P_amb", 1);
    ssc_data_set_number(data, "T_dp", 10);
    ssc_data_set_number(data, "T_cold_in", 274.70001220703125);
    ssc_data_set_number(data, "defocus", 1);
    ssc_data_set_number(data, "field_fluid", 18);
    ssc_number_t p_store_fl_props[9] = { 1, 7, 0, 0, 0, 0, 0, 0, 0 };
    ssc_data_set_matrix(data, "store_fl_props", p_store_fl_props, 1, 9);
    ssc_data_set_number(data, "store_fluid", 18);
    ssc_data_set_number(data, "tshours", 4);
    ssc_data_set_number(data, "is_hx", 0);
    ssc_data_set_number(data, "dt_hot", 5);
    ssc_data_set_number(data, "dt_cold", 5);
    ssc_data_set_number(data, "hx_config", 2);
    ssc_data_set_number(data, "q_max_aux", 643.71612548828125);
    ssc_data_set_number(data, "T_set_aux", 525);
    ssc_data_set_number(data, "V_tank_hot_ini", 1254.615478515625);
    ssc_data_set_number(data, "T_tank_hot_ini", 525);
    ssc_data_set_number(data, "T_tank_cold_ini", 293);
    ssc_data_set_number(data, "vol_tank", 6273.0771484375);
    ssc_data_set_number(data, "h_tank", 20);
    ssc_data_set_number(data, "h_tank_min", 1);
    ssc_data_set_number(data, "u_tank", 0.40000000000000002);
    ssc_data_set_number(data, "tank_pairs", 1);
    ssc_data_set_number(data, "cold_tank_Thtr", 263);
    ssc_data_set_number(data, "hot_tank_Thtr", 425);
    ssc_data_set_number(data, "tank_max_heat", 9.6806000000000001);
    ssc_data_set_number(data, "T_field_in_des", 293);
    ssc_data_set_number(data, "T_field_out_des", 525);
    ssc_data_set_number(data, "q_pb_design", 279.8765869140625);
    ssc_data_set_number(data, "W_pb_design", 111.11100006103516);
    ssc_data_set_number(data, "cycle_max_frac", 1.05);
    ssc_data_set_number(data, "cycle_cutoff_frac", 0.20000000000000001);
    ssc_data_set_number(data, "solarm", 2.2999999523162842);
    ssc_data_set_number(data, "pb_pump_coef", 0.55000000000000004);
    ssc_data_set_number(data, "tes_pump_coef", 0.14999999999999999);
    ssc_data_set_number(data, "pb_fixed_par", 0.0054999999999999997);
    ssc_number_t p_bop_array[5] = { 0, 1, 0.48299999999999998, 0.51700000000000002, 0 };
    ssc_data_set_array(data, "bop_array", p_bop_array, 5);
    ssc_number_t p_aux_array[5] = { 0.02273, 1, 0.48299999999999998, 0.51700000000000002, 0 };
    ssc_data_set_array(data, "aux_array", p_aux_array, 5);
    ssc_data_set_number(data, "tes_temp", 409);
    ssc_data_set_number(data, "fossil_mode", 1);
    ssc_data_set_number(data, "fthr_ok", 1);
    ssc_data_set_number(data, "nSCA", 16);
    ssc_data_set_number(data, "fc_on", 0);
    ssc_data_set_number(data, "t_standby_reset", 2);
    ssc_data_set_number(data, "tes_type", 1);
    ssc_number_t p_tslogic_a[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    ssc_data_set_array(data, "tslogic_a", p_tslogic_a, 9);
    ssc_number_t p_tslogic_b[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    ssc_data_set_array(data, "tslogic_b", p_tslogic_b, 9);
    ssc_number_t p_tslogic_c[9] = { 1.05, 1, 1, 1, 1, 1, 1, 1, 1 };
    ssc_data_set_array(data, "tslogic_c", p_tslogic_c, 9);
    ssc_number_t p_ffrac[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    ssc_data_set_array(data, "ffrac", p_ffrac, 9);
    ssc_data_set_number(data, "tc_fill", 7);
    ssc_data_set_number(data, "tc_void", 0.25);
    ssc_data_set_number(data, "t_dis_out_min", 500);
    ssc_data_set_number(data, "t_ch_out_max", 500);
    ssc_data_set_number(data, "nodes", 2000);
    ssc_data_set_number(data, "f_tc_cold", 2);
    ssc_data_set_number(data, "V_tes_des", 1.8500000000000001);
    ssc_data_set_number(data, "custom_tes_p_loss", 0);
    ssc_number_t p_k_tes_loss_coeffs[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    ssc_data_set_array(data, "k_tes_loss_coeffs", p_k_tes_loss_coeffs, 11);
    ssc_data_set_number(data, "custom_sgs_pipe_sizes", 0);
    ssc_number_t p_sgs_diams[1] = { -1 };
    ssc_data_set_array(data, "sgs_diams", p_sgs_diams, 1);
    ssc_number_t p_sgs_wallthicks[1] = { -1 };
    ssc_data_set_array(data, "sgs_wallthicks", p_sgs_wallthicks, 1);
    ssc_number_t p_sgs_lengths[11] = { 0, 90, 100, 120, 0, 0, 0, 0, 80, 120, 80 };
    ssc_data_set_array(data, "sgs_lengths", p_sgs_lengths, 11);
    ssc_data_set_number(data, "DP_SGS", 0);
    ssc_data_set_number(data, "tanks_in_parallel", 1);
    ssc_data_set_number(data, "has_hot_tank_bypass", 0);
    ssc_data_set_number(data, "T_tank_hot_inlet_min", 400);
    ssc_data_set_number(data, "calc_design_pipe_vals", 0);
    ssc_data_set_number(data, "pc_config", 0);
    ssc_data_set_number(data, "P_ref", 111.111);
    ssc_data_set_number(data, "eta_ref", 0.39700000000000002);
    ssc_data_set_number(data, "startup_time", 0.5);
    ssc_data_set_number(data, "startup_frac", 0.20000000000000001);
    ssc_data_set_number(data, "q_sby_frac", 0.20000000000000001);
    ssc_data_set_number(data, "dT_cw_ref", 10);
    ssc_data_set_number(data, "T_amb_des", 42);
    ssc_data_set_number(data, "P_boil", 100);
    ssc_data_set_number(data, "CT", 2);
    ssc_data_set_number(data, "T_approach", 5);
    ssc_data_set_number(data, "T_ITD_des", 16);
    ssc_data_set_number(data, "P_cond_ratio", 1.0027999999999999);
    ssc_data_set_number(data, "pb_bd_frac", 0.02);
    ssc_data_set_number(data, "P_cond_min", 1.25);
    ssc_data_set_number(data, "n_pl_inc", 8);
    ssc_number_t p_F_wc[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    ssc_data_set_array(data, "F_wc", p_F_wc, 9);
    ssc_data_set_number(data, "tech_type", 2);
    ssc_data_set_number(data, "ud_T_amb_des", 43);
    ssc_data_set_number(data, "ud_f_W_dot_cool_des", 0);
    ssc_data_set_number(data, "ud_m_dot_water_cool_des", 0);
    ssc_data_set_number(data, "ud_T_htf_low", 390);
    ssc_data_set_number(data, "ud_T_htf_high", 545);
    ssc_data_set_number(data, "ud_T_amb_low", 0);
    ssc_data_set_number(data, "ud_T_amb_high", 55);
    ssc_data_set_number(data, "ud_m_dot_htf_low", 0.29999999999999999);
    ssc_data_set_number(data, "ud_m_dot_htf_high", 1.2);
	set_matrix(data, "ud_ind_od", nlfms_ud_ind_od_path, 180, 7);
    ssc_data_set_number(data, "eta_lhv", 0.90000000000000002);
    ssc_data_set_number(data, "eta_tes_htr", 0.97999999999999998);
    ssc_data_set_number(data, "fp_mode", 2);
    ssc_data_set_number(data, "T_htf_hot_ref", 525);
    ssc_data_set_number(data, "T_htf_cold_ref", 293);
    ssc_data_set_number(data, "adjust:constant", 4);

    return data;
}

#endif
