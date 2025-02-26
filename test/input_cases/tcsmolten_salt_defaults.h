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

#ifndef _TCSMOLTEN_SALT_DEFAULTS_H_
#define _TCSMOLTEN_SALT_DEFAULTS_H_

#include <stdio.h>
#include "../input_cases/code_generator_utilities.h"

//const char * SSCDIR = std::getenv("SSCDIR");
//const char * SAMNTDIR = std::getenv("SAMNTDIR");

char dispatch_factors_path[512];
char ud_ind_od_path[512];
char wlim_series_path[512];
char helio_positions_path[512];
int nmspt1 = sprintf(dispatch_factors_path, "%s/test/input_cases/moltensalt_data/dispatch_factors_ts.csv", std::getenv("SSCDIR"));
int nmspt2 = sprintf(ud_ind_od_path, "%s/test/input_cases/moltensalt_data/ud_ind_od.csv", std::getenv("SSCDIR"));
int nmspt3 = sprintf(wlim_series_path, "%s/test/input_cases/moltensalt_data/wlim_series.csv", std::getenv("SSCDIR"));
int nmspt4 = sprintf(helio_positions_path, "%s/test/input_cases/moltensalt_data/helio_positions.csv", std::getenv("SSCDIR"));

/**
*  Default parameters for tcsmolten_salt technology model
*/
ssc_data_t tcsmolten_salt_defaults()
{
    ssc_data_t data = ssc_data_create();

	char solar_resource_path[512];
    // This is a copy of the actual weather file used, which has been copied to the ssc repo so it can be found by Travis CI for its tests.
    //  The actual weather file used by SAM could change and thus change the UI output values (different input (i.e., weather file) -> different outputs)
	int n1 = sprintf(solar_resource_path, "%s/test/input_cases/moltensalt_data/daggett_ca_34.865371_-116.783023_psmv3_60_tmy.csv", std::getenv("SSCDIR"));

	ssc_data_set_string(data, "solar_resource_file", solar_resource_path);
	ssc_data_set_number(data, "ppa_multiplier_model", 0);
	set_array(data, "dispatch_factors_ts", dispatch_factors_path, 8760);
    ssc_data_set_number(data, "field_model_type", 2);
    ssc_data_set_number(data, "gross_net_conversion_factor", 0.90000000000000002);
    ssc_data_set_number(data, "helio_width", 12.199999999999999);
    ssc_data_set_number(data, "helio_height", 12.199999999999999);
    ssc_data_set_number(data, "helio_optical_error_mrad", 1.53);
    ssc_data_set_number(data, "helio_active_fraction", 0.98999999999999999);
    ssc_data_set_number(data, "dens_mirror", 0.96999999999999997);
    ssc_data_set_number(data, "helio_reflectance", 0.90000000000000002);
    ssc_data_set_number(data, "rec_absorptance", 0.93999999999999995);
    ssc_data_set_number(data, "rec_hl_perm2", 30);
    ssc_data_set_number(data, "land_max", 9.5);
    ssc_data_set_number(data, "land_min", 0.75);
    ssc_data_set_number(data, "dni_des", 950);
    ssc_data_set_number(data, "p_start", 0.025000000000000001);
    ssc_data_set_number(data, "p_track", 0.055);
    ssc_data_set_number(data, "hel_stow_deploy", 8);
    ssc_data_set_number(data, "v_wind_max", 15);
    ssc_data_set_number(data, "c_atm_0", 0.0067889999999999999);
    ssc_data_set_number(data, "c_atm_1", 0.1046);
    ssc_data_set_number(data, "c_atm_2", -0.017000000000000001);
    ssc_data_set_number(data, "c_atm_3", 0.0028449999999999999);
    ssc_data_set_number(data, "tanks_in_parallel", 1);
    ssc_data_set_number(data, "n_facet_x", 2);
    ssc_data_set_number(data, "n_facet_y", 8);
    ssc_data_set_number(data, "focus_type", 1);
    ssc_data_set_number(data, "cant_type", 1);
    ssc_data_set_number(data, "n_flux_days", 8);
    ssc_data_set_number(data, "delta_flux_hrs", 2);
    ssc_data_set_number(data, "water_usage_per_wash", 0.69999999999999996);
    ssc_data_set_number(data, "washing_frequency", 63);
    ssc_data_set_number(data, "check_max_flux", 0);
    ssc_data_set_number(data, "sf_excess", 1);
    ssc_data_set_number(data, "tower_fixed_cost", 3000000);
    ssc_data_set_number(data, "tower_exp", 0.011299999999999999);
    ssc_data_set_number(data, "rec_ref_cost", 103000000);
    ssc_data_set_number(data, "rec_ref_area", 1571);
    ssc_data_set_number(data, "rec_cost_exp", 0.69999999999999996);
    ssc_data_set_number(data, "site_spec_cost", 16);
    ssc_data_set_number(data, "heliostat_spec_cost", 140);
    ssc_data_set_number(data, "plant_spec_cost", 1040);
    ssc_data_set_number(data, "bop_spec_cost", 290);
    ssc_data_set_number(data, "tes_spec_cost", 22);
    ssc_data_set_number(data, "land_spec_cost", 10000);
    ssc_data_set_number(data, "contingency_rate", 7);
    ssc_data_set_number(data, "sales_tax_rate", 5);
    ssc_data_set_number(data, "sales_tax_frac", 80);
    ssc_data_set_number(data, "cost_sf_fixed", 0);
    ssc_data_set_number(data, "fossil_spec_cost", 0);
    ssc_data_set_number(data, "flux_max", 1000);
    ssc_data_set_number(data, "opt_init_step", 0.059999999999999998);
    ssc_data_set_number(data, "opt_max_iter", 200);
    ssc_data_set_number(data, "opt_conv_tol", 0.001);
    ssc_data_set_number(data, "opt_flux_penalty", 0.25);
    ssc_data_set_number(data, "opt_algorithm", 1);
    ssc_data_set_number(data, "csp.pt.cost.epc.per_acre", 0);
    ssc_data_set_number(data, "csp.pt.cost.epc.percent", 13);
    ssc_data_set_number(data, "csp.pt.cost.epc.per_watt", 0);
    ssc_data_set_number(data, "csp.pt.cost.epc.fixed", 0);
    ssc_data_set_number(data, "csp.pt.cost.plm.percent", 0);
    ssc_data_set_number(data, "csp.pt.cost.plm.per_watt", 0);
    ssc_data_set_number(data, "csp.pt.cost.plm.fixed", 0);
    ssc_data_set_number(data, "csp.pt.sf.fixed_land_area", 45);
    ssc_data_set_number(data, "csp.pt.sf.land_overhead_factor", 1);
    ssc_data_set_number(data, "T_htf_cold_des", 290);
    ssc_data_set_number(data, "T_htf_hot_des", 574);
    ssc_data_set_number(data, "P_ref", 115);
    ssc_data_set_number(data, "design_eff", 0.41199999999999998);
    ssc_data_set_number(data, "tshours", 10);
    ssc_data_set_number(data, "solarm", 2.3999999999999999);
    ssc_data_set_number(data, "N_panels", 20);
    ssc_data_set_number(data, "d_tube_out", 40);
    ssc_data_set_number(data, "th_tube", 1.25);
    ssc_data_set_number(data, "mat_tube", 2);
    ssc_data_set_number(data, "rec_htf", 17);
    ssc_number_t p_field_fl_props[9] = { 1, 7, 0, 0, 0, 0, 0, 0, 0 };
    ssc_data_set_matrix(data, "field_fl_props", p_field_fl_props, 1, 9);
    ssc_data_set_number(data, "Flow_type", 1);
    ssc_data_set_number(data, "epsilon", 0.88);
    ssc_data_set_number(data, "hl_ffact", 1);
    ssc_data_set_number(data, "f_rec_min", 0.25);
    ssc_data_set_number(data, "rec_su_delay", 0.20000000000000001);
    ssc_data_set_number(data, "rec_qf_delay", 0.25);
    ssc_data_set_number(data, "csp.pt.rec.max_oper_frac", 1.2);
    ssc_data_set_number(data, "eta_pump", 0.84999999999999998);
    ssc_data_set_number(data, "piping_loss_coefficient", 2.);
    ssc_data_set_number(data, "piping_length_mult", 2.6000000000000001);
    ssc_data_set_number(data, "piping_length_const", 0);
    ssc_data_set_number(data, "is_rec_model_trans", 0);
    ssc_data_set_number(data, "is_rec_startup_trans", 0);
    ssc_data_set_number(data, "rec_tm_mult", 1);
    ssc_data_set_number(data, "riser_tm_mult", 1);
    ssc_data_set_number(data, "downc_tm_mult", 1);
    ssc_data_set_number(data, "u_riser", 4);
    ssc_data_set_number(data, "th_riser", 15);
    ssc_data_set_number(data, "heat_trace_power", 500);
    ssc_data_set_number(data, "preheat_flux", 50);
    ssc_data_set_number(data, "startup_ramp_time", 0);
    ssc_data_set_number(data, "preheat_target_Tdiff", 25);
    ssc_data_set_number(data, "startup_target_Tdiff", -5);
    ssc_data_set_number(data, "is_rec_startup_from_T_soln", 0);
    ssc_data_set_number(data, "is_rec_enforce_min_startup", 0);
    ssc_data_set_number(data, "csp.pt.tes.init_hot_htf_percent", 30);
    ssc_data_set_number(data, "h_tank", 12);
    ssc_data_set_number(data, "cold_tank_max_heat", 15);
    ssc_data_set_number(data, "u_tank", 0.40000000000000002);
    ssc_data_set_number(data, "tank_pairs", 1);
    ssc_data_set_number(data, "cold_tank_Thtr", 280);
    ssc_data_set_number(data, "h_tank_min", 1);
    ssc_data_set_number(data, "hot_tank_Thtr", 500);
    ssc_data_set_number(data, "hot_tank_max_heat", 30);
    ssc_data_set_number(data, "h_ctes_tank_min", 1);
    ssc_data_set_number(data, "ctes_tshours", 15);
    ssc_data_set_number(data, "ctes_field_fl", 4);
    ssc_data_set_number(data, "h_ctes_tank", 30);
    ssc_data_set_number(data, "u_ctes_tank", 0.40000000000000002);
    ssc_data_set_number(data, "ctes_tankpairs", 1);
    ssc_data_set_number(data, "T_ctes_cold_design", 5);
    ssc_data_set_number(data, "T_ctes_warm_design", 10);
    ssc_data_set_number(data, "T_ctes_warm_ini", 20);
    ssc_data_set_number(data, "T_ctes_cold_ini", 10);
    ssc_data_set_number(data, "f_ctes_warm_ini", 0);
    ssc_data_set_number(data, "rad_multiplier", 1.5);
    ssc_data_set_number(data, "m_dot_radpanel", 8);
    ssc_data_set_number(data, "n_rad_tubes", 100);
    ssc_data_set_number(data, "W_rad_tubes", 0.050000000000000003);
    ssc_data_set_number(data, "L_rad", 100);
    ssc_data_set_number(data, "th_rad_panel", 0.002);
    ssc_data_set_number(data, "D_rad_tubes", 0.02);
    ssc_data_set_number(data, "k_panel", 235);
    ssc_data_set_number(data, "epsilon_radtop", 0.94999999999999996);
    ssc_data_set_number(data, "epsilon_radbot", 0.070000000000000007);
    ssc_data_set_number(data, "epsilon_radgrnd", 0.90000000000000002);
    ssc_data_set_number(data, "L_rad_sections", 10);
    ssc_data_set_number(data, "epsilon_radHX", 0.80000000000000004);
    ssc_data_set_number(data, "ctes_type", 0);
    ssc_data_set_number(data, "helio_area_tot", 1269054.5);
    ssc_data_set_number(data, "radiator_unitcost", 13);
    ssc_data_set_number(data, "radiator_installcost", 22);
    ssc_data_set_number(data, "radiator_fluidcost", 0.34000000357627869);
    ssc_data_set_number(data, "radfluid_vol_ratio", 3);
    ssc_data_set_number(data, "ctes_cost", 0.69999998807907104);
    ssc_data_set_number(data, "rad_pressuredrop", 75);
    ssc_data_set_number(data, "pc_config", 0);
    ssc_data_set_number(data, "pb_pump_coef", 0.55000000000000004);
    ssc_data_set_number(data, "startup_time", 0.5);
    ssc_data_set_number(data, "startup_frac", 0.5);
    ssc_data_set_number(data, "cycle_max_frac", 1.05);
    ssc_data_set_number(data, "cycle_cutoff_frac", 0.20000000000000001);
    ssc_data_set_number(data, "q_sby_frac", 0.20000000000000001);
    ssc_data_set_number(data, "dT_cw_ref", 10);
    ssc_data_set_number(data, "T_amb_des", 42);
    ssc_data_set_number(data, "P_boil", 100);
    ssc_data_set_number(data, "CT", 2);
    ssc_data_set_number(data, "T_approach", 5);
    ssc_data_set_number(data, "T_ITD_des", 16);
    ssc_data_set_number(data, "P_cond_ratio", 1.0027999999999999);
    ssc_data_set_number(data, "pb_bd_frac", 0.02);
    ssc_data_set_number(data, "P_cond_min", 2);
    ssc_data_set_number(data, "n_pl_inc", 8);
    ssc_number_t p_F_wc[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    ssc_data_set_array(data, "F_wc", p_F_wc, 9);
    ssc_data_set_number(data, "tech_type", 1);
    ssc_data_set_number(data, "ud_T_amb_des", 43);
    ssc_data_set_number(data, "ud_f_W_dot_cool_des", 0);
    ssc_data_set_number(data, "ud_m_dot_water_cool_des", 0);
    ssc_data_set_number(data, "ud_T_htf_low", 500);
    ssc_data_set_number(data, "ud_T_htf_high", 580);
    ssc_data_set_number(data, "ud_T_amb_low", 0);
    ssc_data_set_number(data, "ud_T_amb_high", 55);
    ssc_data_set_number(data, "ud_m_dot_htf_low", 0.29999999999999999);
    ssc_data_set_number(data, "ud_m_dot_htf_high", 1.2);
	set_matrix(data, "ud_ind_od", ud_ind_od_path, 180, 7);
    ssc_data_set_number(data, "sco2_cycle_config", 1);
    ssc_data_set_number(data, "eta_c", 0.89000000000000001);
    ssc_data_set_number(data, "eta_t", 0.90000000000000002);
    ssc_data_set_number(data, "recup_eff_max", 0.95999999999999996);
    ssc_data_set_number(data, "P_high_limit", 25);
    ssc_data_set_number(data, "deltaT_PHX", 20);
    ssc_data_set_number(data, "fan_power_perc_net", 1.5);
    ssc_data_set_number(data, "sco2_T_amb_des", 35);
    ssc_data_set_number(data, "sco2_T_approach", 10);
    ssc_data_set_number(data, "is_sco2_preprocess", 0);
    ssc_data_set_number(data, "sco2ud_T_htf_cold_calc", 9.9999999999999998e+37);
    ssc_data_set_number(data, "sco2ud_T_htf_low", 0);
    ssc_data_set_number(data, "sco2ud_T_htf_high", 0);
    ssc_data_set_number(data, "sco2ud_T_amb_low", 0);
    ssc_data_set_number(data, "sco2ud_T_amb_high", 0);
    ssc_data_set_number(data, "sco2ud_m_dot_htf_low", 0);
    ssc_data_set_number(data, "sco2ud_m_dot_htf_high", 0);
    ssc_number_t p_sco2ud_T_htf_ind_od[39] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    ssc_data_set_matrix(data, "sco2ud_T_htf_ind_od", p_sco2ud_T_htf_ind_od, 3, 13);
    ssc_number_t p_sco2ud_T_amb_ind_od[39] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    ssc_data_set_matrix(data, "sco2ud_T_amb_ind_od", p_sco2ud_T_amb_ind_od, 3, 13);
    ssc_number_t p_sco2ud_m_dot_htf_ind_od[39] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    ssc_data_set_matrix(data, "sco2ud_m_dot_htf_ind_od", p_sco2ud_m_dot_htf_ind_od, 3, 13);
    ssc_data_set_number(data, "_sco2_P_high_limit", 9.9999999999999998e+37);
    ssc_data_set_number(data, "_sco2_P_ref", 9.9999999999999998e+37);
    ssc_data_set_number(data, "_sco2_T_amb_des", 9.9999999999999998e+37);
    ssc_data_set_number(data, "_sco2_T_approach", 9.9999999999999998e+37);
    ssc_data_set_number(data, "_sco2_T_htf_hot_des", 9.9999999999999998e+37);
    ssc_data_set_number(data, "_sco2_deltaT_PHX", 9.9999999999999998e+37);
    ssc_data_set_number(data, "_sco2_design_eff", 9.9999999999999998e+37);
    ssc_data_set_number(data, "_sco2_eta_c", 9.9999999999999998e+37);
    ssc_data_set_number(data, "_sco2_eta_t", 9.9999999999999998e+37);
    ssc_data_set_number(data, "_sco2_recup_eff_max", 9.9999999999999998e+37);
    ssc_data_set_number(data, "time_start", 0);
    ssc_data_set_number(data, "time_stop", 31536000);
    ssc_data_set_number(data, "pb_fixed_par", 0.0054999999999999997);
    ssc_data_set_number(data, "aux_par", 0.023);
    ssc_data_set_number(data, "aux_par_f", 1);
    ssc_data_set_number(data, "aux_par_0", 0.48299999999999998);
    ssc_data_set_number(data, "aux_par_1", 0.57099999999999995);
    ssc_data_set_number(data, "aux_par_2", 0);
    ssc_data_set_number(data, "bop_par", 0);
    ssc_data_set_number(data, "bop_par_f", 1);
    ssc_data_set_number(data, "bop_par_0", 0);
    ssc_data_set_number(data, "bop_par_1", 0.48299999999999998);
    ssc_data_set_number(data, "bop_par_2", 0);
    ssc_number_t p_f_turb_tou_periods[9] = { 1.05, 1, 1, 1, 1, 1, 1, 1, 1 };
    ssc_data_set_array(data, "f_turb_tou_periods", p_f_turb_tou_periods, 9);
    ssc_number_t p_weekday_schedule[288] = { 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5 };
    ssc_data_set_matrix(data, "weekday_schedule", p_weekday_schedule, 12, 24);
    ssc_number_t p_weekend_schedule[288] = { 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
    ssc_data_set_matrix(data, "weekend_schedule", p_weekend_schedule, 12, 24);
    ssc_data_set_number(data, "is_dispatch", 0);
    ssc_data_set_number(data, "disp_horizon", 48);
    ssc_data_set_number(data, "disp_frequency", 24);
    ssc_data_set_number(data, "disp_max_iter", 35000);
    ssc_data_set_number(data, "disp_timeout", 5);
    ssc_data_set_number(data, "disp_mip_gap", 0.001);
    ssc_data_set_number(data, "disp_time_weighting", 0.98999999999999999);
    ssc_data_set_number(data, "disp_rsu_cost", 950);
    ssc_data_set_number(data, "disp_csu_cost", 10000);
    ssc_data_set_number(data, "disp_pen_delta_w", 0.10000000000000001);
    ssc_data_set_number(data, "is_wlim_series", 0);
	set_array(data, "wlim_series", wlim_series_path, 8760);
    ssc_number_t p_dispatch_sched_weekday[288] = { 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5 };
    ssc_data_set_matrix(data, "dispatch_sched_weekday", p_dispatch_sched_weekday, 12, 24);
    ssc_number_t p_dispatch_sched_weekend[288] = { 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
    ssc_data_set_matrix(data, "dispatch_sched_weekend", p_dispatch_sched_weekend, 12, 24);
    ssc_data_set_number(data, "dispatch_factor1", 2.0640000000000001);
    ssc_data_set_number(data, "dispatch_factor2", 1.2);
    ssc_data_set_number(data, "dispatch_factor3", 1);
    ssc_data_set_number(data, "dispatch_factor4", 1.1000000000000001);
    ssc_data_set_number(data, "dispatch_factor5", 0.80000000000000004);
    ssc_data_set_number(data, "dispatch_factor6", 0.69999999999999996);
    ssc_data_set_number(data, "dispatch_factor7", 1);
    ssc_data_set_number(data, "dispatch_factor8", 1);
    ssc_data_set_number(data, "dispatch_factor9", 1);
    ssc_data_set_number(data, "is_dispatch_series", 0);
    ssc_number_t p_dispatch_series[1] = { 0 };
    ssc_data_set_array(data, "dispatch_series", p_dispatch_series, 1);
    ssc_data_set_number(data, "rec_height", 21.602900000000002);
    ssc_data_set_number(data, "D_rec", 17.649999999999999);
    ssc_data_set_number(data, "h_tower", 193.458);
	set_matrix(data, "helio_positions", helio_positions_path, 8790, 2);
    ssc_data_set_number(data, "land_area_base", 1847.04);
    ssc_data_set_number(data, "const_per_interest_rate1", 4);
    ssc_data_set_number(data, "const_per_interest_rate2", 0);
    ssc_data_set_number(data, "const_per_interest_rate3", 0);
    ssc_data_set_number(data, "const_per_interest_rate4", 0);
    ssc_data_set_number(data, "const_per_interest_rate5", 0);
    ssc_data_set_number(data, "const_per_months1", 24);
    ssc_data_set_number(data, "const_per_months2", 0);
    ssc_data_set_number(data, "const_per_months3", 0);
    ssc_data_set_number(data, "const_per_months4", 0);
    ssc_data_set_number(data, "const_per_months5", 0);
    ssc_data_set_number(data, "const_per_percent1", 100);
    ssc_data_set_number(data, "const_per_percent2", 0);
    ssc_data_set_number(data, "const_per_percent3", 0);
    ssc_data_set_number(data, "const_per_percent4", 0);
    ssc_data_set_number(data, "const_per_percent5", 0);
    ssc_data_set_number(data, "const_per_upfront_rate1", 1);
    ssc_data_set_number(data, "const_per_upfront_rate2", 0);
    ssc_data_set_number(data, "const_per_upfront_rate3", 0);
    ssc_data_set_number(data, "const_per_upfront_rate4", 0);
    ssc_data_set_number(data, "const_per_upfront_rate5", 0);
    ssc_data_set_number(data, "adjust:constant", 4);
    ssc_data_set_number(data, "sf_adjust:constant", 0);
    ssc_data_set_number(data, "ppa_soln_mode", 0);
    return data;
}

#endif
