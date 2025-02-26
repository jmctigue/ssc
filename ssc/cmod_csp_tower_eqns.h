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


#ifndef _CMOD_CSP_TOWER_EQNS_H_
#define _CMOD_CSP_TOWER_EQNS_H_

#include "sscapi.h"

#ifdef __cplusplus
extern "C" {
#endif

    static const char* MSPT_System_Design_Equations_doc =
        "Sizes the design point system parameters of a molten salt power tower plant, as used on the System Design UI form\\n"
        "Input: var_table with key-value pairs\\n"
        "     'design_eff' - double [-]\\n"
        "     'gross_net_conversion_factor' - double [-]\\n"
        "     'P_ref' - double [MWe]\\n"
        "     'solarm' - double [-]\\n"
        "     'tshours' - double [hr]\\n"
        "Output: key-value pairs added to var_table\\n"
        "     'nameplate' - double [MWe]\\n"
        "     'q_pb_design' - double [MWt]\\n"
        "     'Q_rec_des' - double [MWt]\\n"
        "     'tshours_sf' - double [hr]";

    SSCEXPORT bool MSPT_System_Design_Equations(ssc_data_t data);


    static const char* Tower_SolarPilot_Solar_Field_Equations_doc =
        "Sizes and lays out the heliostat field for a molten salt power tower plant, as used on the Heliostat Field UI form\\n"
        "Input: var_table with key-value pairs\\n"
        "     'c_atm_0' - double [-]\\n"
        "     'c_atm_1' - double [-]\\n"
        "     'c_atm_2' - double [-]\\n"
        "     'c_atm_3' - double [-]\\n"
        "     'csp_pt_sf_fixed_land_area' - double [acres]\\n"
        "     'csp_pt_sf_land_overhead_factor' - double [-]\\n"
        "     'dens_mirror' - double [-]\\n"
        "     'dni_des' - double [W/m2]\\n"
        "     'h_tower' - double [m]\\n"
        "     'helio_height' - double [m]\\n"
        "     'helio_optical_error_mrad' - double [mrad]\\n"
        "     'helio_positions' - ssc_number_t [m]\\n"
        "     'helio_width' - double [m]\\n"
        "     'land_area_base' - double [acres]\\n"
        "     'land_max' - double [-]\\n"
        "     'land_min' - double [-]\\n"
        "     'override_layout' - int [-]\\n"
        "     'override_opt' - int [-]\\n"
        "     'q_rec_des' - double [MWt]\\n"
        "Output: key-value pairs added to var_table\\n"
        "     'A_sf_UI' - double [m2]\\n"
        "     'c_atm_info' - double [%]\\n"
        "     'csp_pt_sf_heliostat_area' - double [m2]\\n"
        "     'csp_pt_sf_total_land_area' - double [acres]\\n"
        "     'csp_pt_sf_total_reflective_area' - double [m2]\\n"
        "     'csp_pt_sf_tower_height' - double [m]\\n"
        "     'dni_des_calc' - double [W/m2]\\n"
        "     'error_equiv' - double [mrad]\\n"
        "     'field_model_type' - double [-]\\n"
        "     'helio_area_tot' - double [m2]\\n"
        "     'is_optimize' - int [-]\\n"
        "     'land_max_calc' - double [m]\\n"
        "     'land_min_calc' - double [m]\\n"
        "     'n_hel' - int [-]\\n"
        "     'opt_algorithm' - double [-]\\n"
        "     'opt_flux_penalty' - double [-]\\n"
        "     'q_design' - double [MWt]\\n";

    SSCEXPORT bool Tower_SolarPilot_Solar_Field_Equations(ssc_data_t data);


    static const char* MSPT_Receiver_Equations_doc =
        "Sizes the receiver and tower piping, as used on the Tower and Receiver UI form\\n"
        "Input: var_table with key-value pairs\\n"
        "     'csp_pt_rec_cav_ap_hw_ratio' - double [-]\\n"
        "     'csp_pt_rec_max_oper_frac' - double [-]\\n"
        "     'd_rec' - double [m]\\n"
        "     'field_fl_props' - double [-]\\n"
        "     'h_tower' - double [m]\\n"
        "     'piping_length_const' - double [m]\\n"
        "     'piping_length_mult' - double [-]\\n"
        "     'piping_loss' - double [Wt/m]\\n"
        "     'q_rec_des' - double [MWt]\\n"
        "     'rec_d_spec' - double [m]\\n"
        "     'rec_height' - double [m]\\n"
        "     'rec_htf' - int [-]\\n"
        "     't_htf_cold_des' - double [C]\\n"
        "     't_htf_hot_des' - double [C]\\n"
        "Output: key-value pairs added to var_table\\n"
        "     'csp_pt_rec_cav_ap_height' - double [m]\\n"
        "     'csp_pt_rec_cav_lip_height' - double [m]\\n"
        "     'csp_pt_rec_cav_panel_height' - double [m]\\n"
        "     'csp_pt_rec_htf_c_avg' - double [kJ/kg-K]\\n"
        "     'csp_pt_rec_htf_t_avg' - double [C]\\n"
        "     'csp_pt_rec_max_flow_to_rec' - double [kg/s]\\n"
        "     'piping_length' - double [m]\\n"
        "     'piping_loss_tot' - double [kWt]\\n"
        "     'rec_aspect' - double [-]\\n";

    SSCEXPORT bool MSPT_Receiver_Equations(ssc_data_t data);


    static const char* MSPT_System_Control_Equations_doc =
        "Calculates plant parasitics and dispatch limits, as used on the System Control UI form\\n"
        "Input: var_table with key-value pairs\\n"
        "     'aux_par' - double [MWe/MWcap]\\n"
        "     'aux_par_0' - double [-]\\n"
        "     'aux_par_1' - double [-]\\n"
        "     'aux_par_2' - double [-]\\n"
        "     'aux_par_f' - double [-]\\n"
        "     'bop_par' - double [MWe/MWcap]\\n"
        "     'bop_par_0' - double [-]\\n"
        "     'bop_par_1' - double [-]\\n"
        "     'bop_par_2' - double [-]\\n"
        "     'bop_par_f' - double [-]\\n"
        "     'constant' - double [%]\\n"
        "     'disp_wlim_maxspec' - double [-]\\n"
        "     'p_ref' - double [MWe]\\n"
        "Output: key-value pairs added to var_table\\n"
        "     'csp_pt_par_calc_aux' - double [MWe]\\n"
        "     'csp_pt_par_calc_bop' - double [MWe]\\n"
        "     'disp_wlim_max' - double [MWe]\\n"
        "     'wlim_series' - double [kWe]\\n";

    SSCEXPORT bool MSPT_System_Control_Equations(ssc_data_t data);


    static const char* Tower_SolarPilot_Capital_Costs_MSPT_Equations_doc =
        "Calculates plant capital costs, as used on the System Costs UI form\\n"
        "Input: var_table with key-value pairs\\n"
        "     'd_rec' - double [m]\\n"
        "     'receiver_type' - double [-]\\n"
        "     'csp_pt_rec_cav_ap_height' - double [m]\\n"
        "     'rec_d_spec' - double [m]\\n"
        "     'p_ref' - double [MWe]\\n"
        "     'design_eff' - double [-]\\n"
        "     'tshours' - double [hr]\\n"
        "     'a_sf_ui' - double [m2]\\n"
        "     'site_spec_cost' - double [$/m2]\\n"
        "     'heliostat_spec_cost' - double [$/m2]\\n"
        "     'cost_sf_fixed' - double [$]\\n"
        "     'h_tower' - double [m]\\n"
        "     'rec_height' - double [m]\\n"
        "     'helio_height' - double [m]\\n"
        "     'tower_fixed_cost' - double [$]\\n"
        "     'tower_exp' - double [-]\\n"
        "     'rec_ref_cost' - double [$]\\n"
        "     'rec_ref_area' - double []\\n"
        "     'rec_cost_exp' - double []\\n"
        "     'tes_spec_cost' - double [$/kWht]\\n"
        "     'plant_spec_cost' - double [$/kWe]\\n"
        "     'bop_spec_cost' - double [$/kWe]\\n"
        "     'fossil_spec_cost' - double [$/kWe]\\n"
        "     'contingency_rate' - double [%]\\n"
        "     'csp_pt_sf_total_land_area' - double [acre]\\n"
        "     'nameplate' - double [MWe]\\n"
        "     'csp_pt_cost_epc_per_acre' - double [$/acre]\\n"
        "     'csp_pt_cost_epc_percent' - double [%]\\n"
        "     'csp_pt_cost_epc_per_watt' - double [$/W]\\n"
        "     'csp_pt_cost_epc_fixed' - double [$]\\n"
        "     'land_spec_cost' - double [$/acre]\\n"
        "     'csp_pt_cost_plm_percent' - double [%]\\n"
        "     'csp_pt_cost_plm_per_watt' - double [$/W]\\n"
        "     'csp_pt_cost_plm_fixed' - double [$]\\n"
        "     'sales_tax_frac' - double [%]\\n"
        "     'sales_tax_rate' - double [%]\\n"
        "Output: key-value pairs added to var_table\\n"
        "     'csp_pt_cost_site_improvements' - double [$]\\n"
        "     'csp_pt_cost_heliostats' - double [$]\\n"
        "     'csp_pt_cost_tower' - double [$]\\n"
        "     'csp_pt_cost_receiver' - double [$]\\n"
        "     'csp_pt_cost_storage' - double [$]\\n"
        "     'csp_pt_cost_power_block' - double [$]\\n"
        "     'csp_pt_cost_bop' - double [$]\\n"
        "     'csp_pt_cost_fossil' - double [$]\\n"
        "     'ui_direct_subtotal' - double [$]\\n"
        "     'csp_pt_cost_contingency' - double [$]\\n"
        "     'total_direct_cost' - double [$]\\n"
        "     'csp_pt_cost_epc_total' - double [$]\\n"
        "     'csp_pt_cost_plm_total' - double [$]\\n"
        "     'csp_pt_cost_sales_tax_total' - double [$]\\n"
        "     'total_indirect_cost' - double [$]\\n"
        "     'total_installed_cost' - double [$]\\n"
        "     'csp_pt_cost_installed_per_capacity' - double [$]\\n";

    SSCEXPORT bool Tower_SolarPilot_Capital_Costs_MSPT_Equations(ssc_data_t data);


    static const char* Tower_SolarPilot_Capital_Costs_DSPT_Equations_doc =
        "Calculates plant capital costs, as used on the System Costs UI form\\n"
        "Input: var_table with key-value pairs\\n"
        "     'd_rec' - double [m]\\n"
        "     'demand_var' - double [Mwe]\\n"
        "     'a_sf_ui' - double [m2]\\n"
        "     'site_spec_cost' - double [$/m2]\\n"
        "     'heliostat_spec_cost' - double [$/m2]\\n"
        "     'cost_sf_fixed' - double [$]\\n"
        "     'h_tower' - double [m]\\n"
        "     'rec_height' - double [m]\\n"
        "     'helio_height' - double [m]\\n"
        "     'tower_fixed_cost' - double [$]\\n"
        "     'tower_exp' - double [-]\\n"
        "     'rec_ref_cost' - double [$]\\n"
        "     'rec_ref_area' - double []\\n"
        "     'rec_cost_exp' - double []\\n"
        "     'tes_spec_cost' - double [$/kWht]\\n"
        "     'plant_spec_cost' - double [$/kWe]\\n"
        "     'bop_spec_cost' - double [$/kWe]\\n"
        "     'fossil_spec_cost' - double [$/kWe]\\n"
        "     'contingency_rate' - double [%]\\n"
        "     'csp_pt_sf_total_land_area' - double [acre]\\n"
        "     'nameplate' - double [MWe]\\n"
        "     'csp_pt_cost_epc_per_acre' - double [$/acre]\\n"
        "     'csp_pt_cost_epc_percent' - double [%]\\n"
        "     'csp_pt_cost_epc_per_watt' - double [$/W]\\n"
        "     'csp_pt_cost_epc_fixed' - double [$]\\n"
        "     'land_spec_cost' - double [$/acre]\\n"
        "     'csp_pt_cost_plm_percent' - double [%]\\n"
        "     'csp_pt_cost_plm_per_watt' - double [$/W]\\n"
        "     'csp_pt_cost_plm_fixed' - double [$]\\n"
        "     'sales_tax_frac' - double [%]\\n"
        "     'sales_tax_rate' - double [%]\\n"
        "Output: key-value pairs added to var_table\\n"
        "     'csp_pt_cost_site_improvements' - double [$]\\n"
        "     'csp_pt_cost_heliostats' - double [$]\\n"
        "     'csp_pt_cost_tower' - double [$]\\n"
        "     'csp_pt_cost_receiver' - double [$]\\n"
        "     'csp_pt_cost_storage' - double [$]\\n"
        "     'csp_pt_cost_power_block' - double [$]\\n"
        "     'csp_pt_cost_bop' - double [$]\\n"
        "     'csp_pt_cost_fossil' - double [$]\\n"
        "     'ui_direct_subtotal' - double [$]\\n"
        "     'csp_pt_cost_contingency' - double [$]\\n"
        "     'total_direct_cost' - double [$]\\n"
        "     'csp_pt_cost_epc_total' - double [$]\\n"
        "     'csp_pt_cost_plm_total' - double [$]\\n"
        "     'csp_pt_cost_sales_tax_total' - double [$]\\n"
        "     'total_indirect_cost' - double [$]\\n"
        "     'total_installed_cost' - double [$]\\n"
        "     'csp_pt_cost_installed_per_capacity' - double [$]\\n";

    SSCEXPORT bool Tower_SolarPilot_Capital_Costs_DSPT_Equations(ssc_data_t data);


    static const char* Tower_SolarPilot_Capital_Costs_ISCC_Equations_doc =
        "Calculates plant capital costs, as used on the System Costs UI form\\n"
        "Input: var_table with key-value pairs\\n"
        "     'd_rec' - double [m]\\n"
        "     'receiver_type' - double [-]\\n"
        "     'rec_d_spec' - double [m]\\n"
        "     'csp_pt_rec_cav_ap_height' - double [m]\\n"
        "     'a_sf_ui' - double [m2]\\n"
        "     'site_spec_cost' - double [$/m2]\\n"
        "     'heliostat_spec_cost' - double [$/m2]\\n"
        "     'cost_sf_fixed' - double [$]\\n"
        "     'h_tower' - double [m]\\n"
        "     'rec_height' - double [m]\\n"
        "     'helio_height' - double [m]\\n"
        "     'tower_fixed_cost' - double [$]\\n"
        "     'tower_exp' - double [-]\\n"
        "     'rec_ref_cost' - double [$]\\n"
        "     'rec_ref_area' - double []\\n"
        "     'rec_cost_exp' - double []\\n"
        "     'tes_spec_cost' - double [$/kWht]\\n"
        "     'plant_spec_cost' - double [$/kWe]\\n"
        "     'bop_spec_cost' - double [$/kWe]\\n"
        "     'fossil_spec_cost' - double [$/kWe]\\n"
        "     'contingency_rate' - double [%]\\n"
        "     'csp_pt_sf_total_land_area' - double [acre]\\n"
        "     'nameplate' - double [MWe]\\n"
        "     'csp_pt_cost_epc_per_acre' - double [$/acre]\\n"
        "     'csp_pt_cost_epc_percent' - double [%]\\n"
        "     'csp_pt_cost_epc_per_watt' - double [$/W]\\n"
        "     'csp_pt_cost_epc_fixed' - double [$]\\n"
        "     'land_spec_cost' - double [$/acre]\\n"
        "     'csp_pt_cost_plm_percent' - double [%]\\n"
        "     'csp_pt_cost_plm_per_watt' - double [$/W]\\n"
        "     'csp_pt_cost_plm_fixed' - double [$]\\n"
        "     'sales_tax_frac' - double [%]\\n"
        "     'sales_tax_rate' - double [%]\\n"
        "Output: key-value pairs added to var_table\\n"
        "     'csp_pt_cost_site_improvements' - double [$]\\n"
        "     'csp_pt_cost_heliostats' - double [$]\\n"
        "     'csp_pt_cost_tower' - double [$]\\n"
        "     'csp_pt_cost_receiver' - double [$]\\n"
        "     'csp_pt_cost_storage' - double [$]\\n"
        "     'csp_pt_cost_power_block' - double [$]\\n"
        "     'csp_pt_cost_bop' - double [$]\\n"
        "     'csp_pt_cost_fossil' - double [$]\\n"
        "     'ui_direct_subtotal' - double [$]\\n"
        "     'csp_pt_cost_contingency' - double [$]\\n"
        "     'total_direct_cost' - double [$]\\n"
        "     'csp_pt_cost_epc_total' - double [$]\\n"
        "     'csp_pt_cost_plm_total' - double [$]\\n"
        "     'csp_pt_cost_sales_tax_total' - double [$]\\n"
        "     'total_indirect_cost' - double [$]\\n"
        "     'total_installed_cost' - double [$]\\n"
        "     'csp_pt_cost_installed_per_capacity' - double [$]\\n";

    SSCEXPORT bool Tower_SolarPilot_Capital_Costs_ISCC_Equations(ssc_data_t data);

#ifdef __cplusplus
}
#endif

#endif
