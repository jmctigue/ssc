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

#include <gtest/gtest.h>
#include "tcsmolten_salt_defaults.h"
#include "cmod_csp_tower_eqns.h"
#include "csp_common_test.h"
#include "vs_google_test_explorer_namespace.h"

namespace csp_tower {}
using namespace csp_tower;

//========Tests===================================================================================
NAMESPACE_TEST(csp_tower, PowerTowerCmod, Default_NoFinancial)
{
    ssc_data_t defaults = tcsmolten_salt_defaults();
    CmodUnderTest power_tower = CmodUnderTest("tcsmolten_salt", defaults);
    int errors = power_tower.RunModule();
    EXPECT_FALSE(errors);
    if (!errors) {
        EXPECT_NEAR_FRAC(power_tower.GetOutput("annual_energy"), 574526336, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("land_area_base"), 1847, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("capacity_factor"), 63.4, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("annual_W_cycle_gross"), 641513000, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("kwh_per_kw"), 5550.98, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("conversion_factor"), 89.558, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("N_hel"), 8790, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("rec_height"), 21.60, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("A_sf"), 1269054, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("D_rec"), 17.65, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("annual_total_water_use"), 98573, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("csp.pt.cost.total_land_area"), 1892, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("h_tower"), 193.5, kErrorToleranceHi);
    }

    //ssc_data_t defaults = singleowner_defaults();
    //CmodUnderTest singleowner = CmodUnderTest("singleowner", defaults);
    //int errors = singleowner.RunModule();
    //EXPECT_FALSE(errors);
    //if (!errors) {
    //    EXPECT_NEAR_FRAC(singleowner.GetOutput(""), , kErrorToleranceLo);
    //}
}

NAMESPACE_TEST(csp_tower, PowerTowerCmod, SlidingPressure_NoFinancial)
{
    ssc_data_t defaults = tcsmolten_salt_defaults();
    CmodUnderTest power_tower = CmodUnderTest("tcsmolten_salt", defaults);
    power_tower.SetInput("tech_type", 3);          // change to sliding pressure
    int errors = power_tower.RunModule();
    EXPECT_FALSE(errors);
    if (!errors)
    {
        EXPECT_NEAR_FRAC(power_tower.GetOutput("annual_energy"), 580437568, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("land_area_base"), 1847, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("capacity_factor"), 64.0, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("annual_W_cycle_gross"), 648058000, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("kwh_per_kw"), 5608.09, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("conversion_factor"), 89.57, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("N_hel"), 8790, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("rec_height"), 21.60, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("A_sf"), 1269054, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("D_rec"), 17.65, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("annual_total_water_use"), 98399.9, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("csp.pt.cost.total_land_area"), 1892, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("h_tower"), 193.5, kErrorToleranceHi);
    }
}

NAMESPACE_TEST(csp_tower, PowerTowerCmod, FlowPattern_NoFinancial)
{
    ssc_data_t defaults = tcsmolten_salt_defaults();
    CmodUnderTest power_tower = CmodUnderTest("tcsmolten_salt", defaults);
    power_tower.SetInput("Flow_type", 8);
    int errors = power_tower.RunModule();
    EXPECT_FALSE(errors);
    if (!errors)
    {
        EXPECT_NEAR_FRAC(power_tower.GetOutput("annual_energy"), 521952672, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("land_area_base"), 1847, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("capacity_factor"), 57.6, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("annual_W_cycle_gross"), 645601000, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("kwh_per_kw"), 5043.02, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("conversion_factor"), 80.8475, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("N_hel"), 8790, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("rec_height"), 21.60, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("A_sf"), 1269054, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("D_rec"), 17.65, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("annual_total_water_use"), 98840.3, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("csp.pt.cost.total_land_area"), 1892, kErrorToleranceHi);
        EXPECT_NEAR_FRAC(power_tower.GetOutput("h_tower"), 193.5, kErrorToleranceHi);
    }
}

void CopyVarTableAndGetValue(var_table* vartab, std::string var_name, double* var_value) {
    var_table vartab_copy;
    vartab_copy = *vartab;  // uses copy assignment operator, which is fine
    *var_value = vartab->as_double(var_name);
    //return vartab_copy;     // this copy constructor operator causes a problem
    return;
}

NAMESPACE_TEST(csp_tower, PowerTowerCmod, CopyingVarTable) {
    // Get an ssc_data_t with default input values for the molten salt tower model
    ssc_data_t data = tcsmolten_salt_defaults();

    // Verify var_tables can be copied by first converting the ssc_data_t to a var_table
    var_table* vartab = static_cast<var_table*>(data);

    std::string test_variable_name = "tower_exp";
    double test_value = vartab->as_double(test_variable_name);
    double test_value_from_orig_table_after_copied;

    // If the copying is not in a function like this, there may not be a problem until after the test exits.
    //var_table var_table_copy = CopyVarTableAndGetValue(vartab, test_variable_name, &test_value_from_orig_table_after_copied);
    CopyVarTableAndGetValue(vartab, test_variable_name, &test_value_from_orig_table_after_copied);

    double test_value_from_orig_table_after_copied_and_fun_returned;
    try
    {
        test_value_from_orig_table_after_copied_and_fun_returned = vartab->as_double(test_variable_name);       // throws error
    }
    catch (...) {
        test_value_from_orig_table_after_copied_and_fun_returned = std::numeric_limits<double>::quiet_NaN();
    }

    ASSERT_DOUBLE_EQ(test_value, test_value_from_orig_table_after_copied);
    ASSERT_DOUBLE_EQ(test_value, test_value_from_orig_table_after_copied_and_fun_returned);
    
    ssc_data_free(data);
}

/// Test tcsmolten_salt with alternative condenser type: Evaporative
/// Rest default configurations with respect to the single owner financial model
//TEST_F(CMTcsMoltenSalt, Rankine_Evap_Condenser_SingleOwner_cmod_tcsmolten_salt) {
//
//    ssc_data_t data = ssc_data_create();
//    ssc_data_set_number(data, "CT", 1);
//
//    ...

/// Test tcsmolten_salt with alternative condenser type: Hybrid
/// Rest default configurations with respect to the single owner financial model
//TEST_F(CMTcsMoltenSalt, Rankine_Hybrid_Condenser_SingleOwner_cmod_tcsmolten_salt) {
//
//	ssc_data_t data = ssc_data_create();
//  ssc_data_set_number(data, "CT", 3);
//
//	...

/// Test tcsmolten_salt with alternative condenser type: Radiative
/// Rest default configurations with respect to the single owner financial model
//TEST_F(CMTcsMoltenSalt, Rankine_Radiative_Condenser_SingleOwner_cmod_tcsmolten_salt) {
//
//	ssc_data_t data = ssc_data_create();
//	ssc_data_set_number(data, "h_ctes_tank_min", 1);
//	ssc_data_set_number(data, "ctes_tshours", 15);
//	ssc_data_set_number(data, "ctes_field_fl", 4);
//	ssc_data_set_number(data, "h_ctes_tank", 30);
//	ssc_data_set_number(data, "u_ctes_tank", 0.4);
//	ssc_data_set_number(data, "ctes_tankpairs", 1);
//	ssc_data_set_number(data, "T_ctes_cold_design", 5);
//	ssc_data_set_number(data, "T_ctes_warm_design", 10);
//	ssc_data_set_number(data, "T_ctes_warm_ini", 20);
//	ssc_data_set_number(data, "T_ctes_cold_ini", 10);
//	ssc_data_set_number(data, "f_ctes_warm_ini", 0);
//	ssc_data_set_number(data, "rad_multiplier", 1.5);
//	ssc_data_set_number(data, "m_dot_radpanel", 8);
//	ssc_data_set_number(data, "n_rad_tubes", 100);
//	ssc_data_set_number(data, "W_rad_tubes", 0.05);
//	ssc_data_set_number(data, "L_rad", 100);
//	ssc_data_set_number(data, "th_rad_panel", 0.002);
//	ssc_data_set_number(data, "D_rad_tubes", 0.02);
//	ssc_data_set_number(data, "k_panel", 235);
//	ssc_data_set_number(data, "epsilon_radtop", 0.95);
//	ssc_data_set_number(data, "epsilon_radbot", 0.07);
//	ssc_data_set_number(data, "epsilon_radgrnd", 0.9);
//	ssc_data_set_number(data, "L_rad_sections", 10);
//	ssc_data_set_number(data, "epsilon_radHX", 0.8);
//	ssc_data_set_number(data, "ctes_type", 0);
//	ssc_data_set_number(data, "helio_area_tot", 1269054.5);
//	ssc_data_set_number(data, "radiator_unitcost", 13);
//	ssc_data_set_number(data, "radiator_installcost", 22);
//	ssc_data_set_number(data, "radiator_fluidcost", 0.34);
//	ssc_data_set_number(data, "radfluid_vol_ratio", 3);
//	ssc_data_set_number(data, "ctes_cost", 0.7);
//	ssc_data_set_number(data, "rad_pressuredrop", 75);
//	ssc_data_set_number(data, "CT", 4);
//
//	...

/// Test tcsmolten_salt with alternative Location: Tucson, Arizona
/// Rest default configurations with respect to the single owner financial model
//TEST_F(CMTcsMoltenSalt, Rankine_Location_SingleOwner_cmod_tcsmolten_salt) {
//
//	ssc_data_t data = ssc_data_create();
//	char solar_resource_path_tucson[512];
//	int n = sprintf(solar_resource_path_tucson, "%s/test/input_cases/moltensalt_data/tucson_az_32.116521_-110.933042_psmv3_60_tmy.csv", std::getenv("SSCDIR"));
//	ssc_data_set_string(data, "solar_resource_file", solar_resource_path_tucson);
//
//	...

/// Test tcsmolten_salt with power cycle alternative: User Defined
/// Rest default configurations with respect to the single owner financial model
//TEST_F(CMTcsMoltenSalt, User_Defined_SingleOwner_cmod_tcsmolten_salt) {
//
//	ssc_data_t data = ssc_data_create();
//	set_matrix(data, "ud_ind_od", ud_ind_od_path, 180, 7);
//	ssc_data_set_number(data, "pc_config", 1);
//	ssc_data_set_number(data, "ud_m_dot_htf_low", 0.3);
//	ssc_data_set_number(data, "ud_m_dot_htf_high", 1.2);
//
//	...

/// Test tcsmolten_salt with alternative power cycle: Super Critical CO2
/// Rest default configurations with respect to the single owner financial model
//TEST_F(CMTcsMoltenSalt, SCO2_Default_SingleOwner_cmod_tcsmolten_salt) {
//
//	ssc_data_t data = ssc_data_create();
//	ssc_data_set_number(data, "is_sco2_preprocess", 1);
//
//	ssc_data_set_number(data, "pc_config", 2);
//	ssc_data_set_number(data, "cycle_cutoff_frac", 0.5);
//	// Start of super critical CO2 metrics
//	ssc_data_set_number(data, "sco2ud_T_htf_cold_calc", 406.04);
//	ssc_data_set_number(data, "sco2ud_T_htf_low", 554);
//	ssc_data_set_number(data, "sco2ud_T_htf_high", 589);
//	ssc_data_set_number(data, "sco2ud_T_amb_high", 45);
//	ssc_data_set_number(data, "sco2ud_m_dot_htf_low", 0.5);
//	ssc_data_set_number(data, "sco2ud_m_dot_htf_high", 1.05);
//	ssc_number_t p_sco2ud_T_htf_ind_od[65] = { 554, 0.50001432179685623, 0.96932004909372826, 0.97558944094670463, 0.56452945008574118, 0.99212119137385479, 0.9938261663184943, 0.071046760698792344, 1.0516901234966409, 1.0396677815546937, 1, 1, 1, 562.75, 0.50000677254611736, 0.98273822965810698, 0.98931005948737616, 0.55442312464765453, 0.99593839560635933, 0.99813862855158975, 0.060760248142716182, 1.0280675561194188, 1.0216094704370726, 1, 1, 1, 571.5, 0.49997035485832741, 0.99560036541912367, 1.0030665958141747, 0.54540906175814596, 0.99964495720997093, 1.0023929148428683, 0.052633463645261176, 1.0056543163255209, 1.0033307681195913, 1, 1, 1, 580.25, 0.50001678576431519, 0.99981330235493204, 1.0149012834308118, 0.53755371644427918, 0.99437061481748712, 1.0057187757624548, 0.046124783184180999, 0.93267305740289486, 0.97564080126247599, 1, 1, 1, 589, 0.49998766910986947, 0.99997642865903091, 1.0280965929880774, 0.53035396718324523, 0.98587808938966315, 1.0103635810622889, 0.040778740942652915, 0.84986379353651254, 0.96107738725047209, 1, 1, 1 };
//	ssc_data_set_matrix(data, "sco2ud_T_htf_ind_od", p_sco2ud_T_htf_ind_od, 5, 13);
//	ssc_number_t p_sco2ud_T_amb_ind_od[130] = { 0, 0.99994012767121321, 0.99974326147411507, 0.99987485850649338, 0.970379288808072, 0.95027470619969945, 0.93683012616718919, 0.059164848919520591, 0.049933864673572616, 0.04548467867978704, 1, 1, 1, 5, 0.99994012767121321, 0.99974326147411507, 0.99987485850649338, 0.970379288808072, 0.95027470619969945, 0.93683012616718919, 0.097133136927079936, 0.082248959886362452, 0.075269394493921518, 1, 1, 1, 10, 0.99994012767121321, 0.99974326147411507, 0.99987485850649338, 0.970379288808072, 0.95027470619969945, 0.93683012616718919, 0.17314165365655804, 0.14726896648379706, 0.13576325670205261, 1, 1, 1, 15, 0.99994012767121321, 0.99974326147411507, 0.99987485850649338, 0.970379288808072, 0.95027470619969945, 0.93683012616718919, 0.3481506775773433, 0.29853487098289033, 0.27855988861884273, 1, 1, 1, 20, 0.99994012767121321, 0.99974326147411507, 0.99987485850649338, 0.970379288808072, 0.95027470619969945, 0.93683012616718919, 0.85719459330059711, 0.74767538224853725, 0.71410844002614948, 1, 1, 1, 25, 0.99998860165458137, 0.99998405506376475, 0.99994129487763628, 0.97834420606252215, 0.9592086954026382, 0.94639328546002388, 1.4397512132898596, 1.1099623611534934, 0.9289610766286478, 1, 1, 1, 30, 0.99997868773200105, 1.0000128744625323, 0.99987390166815182, 0.99986135777103136, 0.97846151925556812, 0.96428182700735732, 1.2885473732704416, 1.0097961360792755, 0.85212892165494525, 1, 1, 1, 35, 0.96932004909372826, 0.99995022500741981, 0.99997642865903091, 0.99212119137385479, 0.99999459906789145, 0.98587808938966315, 1.0516901234966409, 1.0033307681195913, 0.84986379353651254, 1, 1, 1, 40, 0.85681928090544024, 0.88648580427756085, 0.90755440545490784, 0.8967547597723905, 0.90655814175453875, 0.91360819421216066, 0.54638640488302492, 0.52332590711799631, 0.50841120332212841, 1, 1, 1, 45, 0.76192612755033284, 0.78992076166486025, 0.81009772614290565, 0.81887517513574226, 0.82799762047592962, 0.83504194672755783, 0.32667431104075889, 0.31463845368914112, 0.30664767313185981, 1, 1, 1 };
//	ssc_data_set_matrix(data, "sco2ud_T_amb_ind_od", p_sco2ud_T_amb_ind_od, 10, 13);
//	ssc_number_t p_sco2ud_m_dot_htf_ind_od[130] = { 0.5, 0.50000238394483887, 0.50002030400496988, 0.5000072744279479, 0.49996830468452913, 0.54311461140321193, 0.58949291750771449, 0.0023965079935530688, 0.050651289208334289, 0.085268991630535099, 1, 1, 1, 0.56111111111111112, 0.56107350053341665, 0.56109752841228611, 0.56110469253490813, 0.55257347648896971, 0.59674948077268875, 0.64372698634700765, 0.0035623149303332731, 0.076089576895326735, 0.12158112224599875, 1, 1, 1, 0.62222222222222223, 0.62224697121064898, 0.62224996267725707, 0.62219761804463092, 0.60616131300962228, 0.65145619071565242, 0.69969734374588888, 0.0052256619247860641, 0.11370919032872756, 0.17234788816884622, 1, 1, 1, 0.68333333333333335, 0.68334763160686485, 0.68332772806833242, 0.68334268666386189, 0.66063755666452395, 0.70667012650879169, 0.75616720875118448, 0.0076071355040999147, 0.16722335929420953, 0.24228904960439612, 1, 1, 1, 0.74444444444444446, 0.74441099641078146, 0.74443754704645426, 0.73579757426674453, 0.7154614610083625, 0.76238207877819242, 0.80635338615523044, 0.010993657103543011, 0.24183918938496599, 0.28923568019902324, 1, 1, 1, 0.80555555555555558, 0.80548586203577432, 0.80556625128549619, 0.7607325689541361, 0.7709115279333274, 0.81874600280477805, 0.81846324371170665, 0.015863026451133713, 0.34650772248743805, 0.32712367869313513, 1, 1, 1, 0.8666666666666667, 0.86665068706416915, 0.86667726624963815, 0.77310372269008887, 0.82666048629288214, 0.87575544351530044, 0.82250334117333646, 0.022915833888653158, 0.49058181757621305, 0.32138671984149092, 1, 1, 1, 0.92777777777777781, 0.92781567340041837, 0.92781937609673271, 0.78228939831004085, 0.88241774237123771, 0.93313651551826904, 0.82559824503018264, 0.033028022568911568, 0.6869968410833619, 0.31804744512991351, 1, 1, 1, 0.98888888888888893, 0.98860321834099996, 0.98888850977399578, 0.78897052574490523, 0.93832104872570143, 0.99049121752897784, 0.82770087400040337, 0.049618846148749549, 0.94725737476210103, 0.31520682775233339, 1, 1, 1, 1.05, 1.0487348778724641, 1.0062218585376423, 0.79376542360284852, 1.0039295318936428, 1.0043865029637598, 0.82928412460314016, 0.049807927876335194, 0.99339517007581468, 0.31300940662606114, 1, 1, 1 };
//	ssc_data_set_matrix(data, "sco2ud_m_dot_htf_ind_od", p_sco2ud_m_dot_htf_ind_od, 10, 13);
//
//	ssc_data_set_number(data, "_sco2_P_high_limit", 25);
//	ssc_data_set_number(data, "_sco2_P_ref", 115);
//	ssc_data_set_number(data, "_sco2_T_amb_des", 35);
//	ssc_data_set_number(data, "_sco2_T_approach", 10);
//	ssc_data_set_number(data, "_sco2_T_htf_hot_des", 574);
//	ssc_data_set_number(data, "_sco2_deltaT_PHX", 20);
//	ssc_data_set_number(data, "_sco2_design_eff", 0.41200000047683716);
//	ssc_data_set_number(data, "_sco2_eta_c", 0.88999998569488525);
//	ssc_data_set_number(data, "_sco2_eta_t", 0.89999997615814209);
//	ssc_data_set_number(data, "_sco2_recup_eff_max", 0.95999997854232788);
//
//	...

/// Test tcsmolten_salt with alternative power cycle: Super Critical CO2
/// Cycle Configuration alternative: Partial Cooling
/// Rest default configurations with respect to the single owner financial model
//TEST_F(CMTcsMoltenSalt, SCO2_Partial_Cooling_SingleOwner_cmod_tcsmolten_salt) {
//
//	ssc_data_t data = ssc_data_create();
//	ssc_data_set_number(data, "pc_config", 2);
//	ssc_data_set_number(data, "cycle_cutoff_frac", 0.5);
//	ssc_data_set_number(data, "sco2_cycle_config", 2);
//	ssc_data_set_number(data, "sco2ud_T_htf_cold_calc", 354.84);
//	ssc_data_set_number(data, "sco2ud_T_htf_low", 554);
//	ssc_data_set_number(data, "sco2ud_T_htf_high", 589);
//	ssc_data_set_number(data, "sco2ud_T_amb_high", 45);
//	ssc_data_set_number(data, "sco2ud_m_dot_htf_low", 0.5);
//	ssc_data_set_number(data, "sco2ud_m_dot_htf_high", 1.05);
//	ssc_number_t p_sco2ud_T_htf_ind_od[65] = { 554, 0.49999352886459142, 0.96525561209463806, 0.97453395561145839, 0.62911366508823352, 0.98998074672002789, 0.99261217903413035, 0.20247797682275048, 1.0571168076843547, 1.0432895732028127, 1, 1, 1, 562.75, 0.49999364663999546, 0.98061760093183636, 0.98945608039614341, 0.59967747034505758, 0.99410410545590111, 0.99692477104310284, 0.17800016270512209, 1.0348516156853151, 1.0211725729287384, 1, 1, 1, 571.5, 0.49999395161728027, 0.99563145139446119, 1.0038535082905322, 0.58306031118146817, 0.99832427864796236, 1.001449148486844, 0.16124320857207705, 1.0092594720500885, 1.0025470111145369, 1, 1, 1, 580.25, 0.49999438710690025, 1.0000368631016281, 1.0187996347167014, 0.57310398959762909, 0.99215602847837348, 1.0059800813551822, 0.14338401700982784, 0.9341583009417217, 0.98266060214587514, 1, 1, 1, 589, 0.50001605482790912, 0.99999012779922869, 1.0325220180423378, 0.56534529941933243, 0.9816011558820481, 1.0098950333460388, 0.12932201013498895, 0.84387453074601659, 0.96199109269179472, 1, 1, 1 };
//	ssc_data_set_matrix(data, "sco2ud_T_htf_ind_od", p_sco2ud_T_htf_ind_od, 5, 13);
//	ssc_number_t p_sco2ud_T_amb_ind_od[130] = { 0, 0.99997428799188914, 0.99999000874401356, 0.99999963664033542, 1.0096260890795448, 0.97647524655927898, 0.95539332954343525, 0.035964053686654389, 0.030460203665356428, 0.028191373083630726, 1, 1, 1, 5, 0.99997428799188914, 0.99999000874401356, 0.99999963664033542, 1.0096260890795448, 0.97647524655927898, 0.95539332954343525, 0.054169401117576316, 0.046164874738715223, 0.043090992060877832, 1, 1, 1, 10, 0.99997428799188914, 0.99999000874401356, 0.99999963664033542, 1.0096260890795448, 0.97647524655927898, 0.95539332954343525, 0.086054162974292386, 0.074011959990269383, 0.069988333857598825, 1, 1, 1, 15, 0.99997428799188914, 0.99999000874401356, 0.99999963664033542, 1.0096260890795448, 0.97647524655927898, 0.95539332954343525, 0.14717747411619553, 0.12834741606804084, 0.12359755317608473, 1, 1, 1, 20, 0.99997428799188914, 0.99999000874401356, 0.99999963664033542, 1.0096260890795448, 0.97647524655927898, 0.95539332954343525, 0.27882175950298732, 0.24810999613169693, 0.24587350287625651, 1, 1, 1, 25, 0.99998001476123333, 0.99998252902816598, 0.99991197104226259, 1.0098815301216097, 0.97647895095481074, 0.9554677877793778, 0.59120853683278496, 0.54403699709247888, 0.56813028411712974, 1, 1, 1, 30, 0.99998001476123333, 0.99998700539505325, 1.000003594937197, 1.0098815301216097, 0.97801555014110308, 0.95937811970467868, 1.6874663271788872, 1.1677785621376786, 0.92812586911858419, 1, 1, 1, 35, 0.96525561209463806, 0.99994365838368371, 0.99999012779922869, 0.98998074672002789, 1.0001162551906568, 0.9816011558820481, 1.0571168076843547, 1.0070553661423967, 0.84387453074601659, 1, 1, 1, 40, 0.88091956289263618, 0.91354408714037638, 0.93692898396657309, 0.92804590812890486, 0.93898338187710573, 0.94720156997882032, 0.77791598161892206, 0.7475571669584532, 0.72852820207159907, 1, 1, 1, 45, 0.80443081580069309, 0.83588054350931573, 0.85830802419616214, 0.87954062349121198, 0.89162501172294772, 0.90100255955577613, 0.71796263227634882, 0.69604875226586571, 0.68078331511457812, 1, 1, 1 };
//	ssc_data_set_matrix(data, "sco2ud_T_amb_ind_od", p_sco2ud_T_amb_ind_od, 10, 13);
//	ssc_number_t p_sco2ud_m_dot_htf_ind_od[130] = { 0.5, 0.50001158587629013, 0.49999335234425468, 0.500009458086906, 0.52092523582781203, 0.580310601704488, 0.67277741730869234, 0.0066297710364970602, 0.15612995926951076, 0.42538208998154886, 1, 1, 1, 0.56111111111111112, 0.56112632572922161, 0.56109347634533013, 0.56112858298132773, 0.5676475361894191, 0.62646030027815847, 0.70338416085113653, 0.0079531402579119982, 0.18764587089730347, 0.43103981940440877, 1, 1, 1, 0.62222222222222223, 0.62221611814605471, 0.62221274231112988, 0.62222151074161314, 0.61455395777278332, 0.67446637440230506, 0.74670855688273574, 0.0093884633943930561, 0.22842611147672137, 0.48833317891759392, 1, 1, 1, 0.68333333333333335, 0.68329224890757501, 0.68333522189257412, 0.68331145370876678, 0.66231318458618582, 0.72371156880387011, 0.7949045334243946, 0.011139413945746518, 0.28028443088130395, 0.57482477745595617, 1, 1, 1, 0.74444444444444446, 0.74445060439362043, 0.74444232493970608, 0.74442697568885108, 0.71280359548337557, 0.77393932493368434, 0.84599994004797685, 0.01370986733027353, 0.34683195143613776, 0.69088300755849497, 1, 1, 1, 0.80555555555555558, 0.80554205452706595, 0.80553118613158747, 0.78568541702380978, 0.76617945177385471, 0.82545035439296532, 0.87282832981226932, 0.018334310087357693, 0.43577215265376867, 0.73125604358596707, 1, 1, 1, 0.8666666666666667, 0.86667780683097451, 0.86669111505864649, 0.80779790168115828, 0.82501645350095432, 0.8784837343548566, 0.88083521806059473, 0.031013715053052799, 0.55720344212317163, 0.71564348223866958, 1, 1, 1, 0.92777777777777781, 0.92778930075110044, 0.92781165563938151, 0.82346900197673678, 0.89420782417679856, 0.9332817335240674, 0.88681855363756823, 0.030225663571636099, 0.72555609695067691, 0.70381015654357315, 1, 1, 1, 0.98888888888888893, 0.98891033574992371, 0.98886027855958214, 0.8342986400696234, 0.96400610586573099, 0.98968856703989938, 0.89098743214344123, 0.030057676976388854, 0.95810218483069376, 0.69690700803894567, 1, 1, 1, 1.05, 1.0499204736064125, 1.0082881287098067, 0.84167433259869806, 1.0322051570968707, 1.002564506765949, 0.89389762859965882, 0.034640376520654041, 0.99538596406341329, 0.69252556580813951, 1, 1, 1 };
//	ssc_data_set_matrix(data, "sco2ud_m_dot_htf_ind_od", p_sco2ud_m_dot_htf_ind_od, 10, 13);
//	ssc_data_set_number(data, "_sco2_P_high_limit", 25);
//	ssc_data_set_number(data, "_sco2_P_ref", 115);
//	ssc_data_set_number(data, "_sco2_T_amb_des", 35);
//	ssc_data_set_number(data, "_sco2_T_approach", 10);
//	ssc_data_set_number(data, "_sco2_T_htf_hot_des", 574);
//	ssc_data_set_number(data, "_sco2_deltaT_PHX", 20);
//	ssc_data_set_number(data, "_sco2_design_eff", 0.412);
//	ssc_data_set_number(data, "_sco2_eta_c", 0.89);
//	ssc_data_set_number(data, "_sco2_eta_t", 0.9);
//	ssc_data_set_number(data, "_sco2_recup_eff_max", 0.96);
//
//	...

/// Test tcsmolten_salt with alternative power cycle: Super Critical CO2
/// Materials and Flow alternative: Flow pattern 2 instead of 1
/// Rest default configurations with respect to the single owner financial model
//TEST_F(CMTcsMoltenSalt, SCO2_Flow_Pattern_Alternative_SingleOwner_cmod_tcsmolten_salt) {
//
//	ssc_data_t data = ssc_data_create();
//	ssc_data_set_number(data, "Flow_type", 2);
//	ssc_data_set_number(data, "pc_config", 2);
//	ssc_data_set_number(data, "cycle_cutoff_frac", 0.5);
//
//	// Start of super critical CO2 metrics
//	ssc_data_set_number(data, "sco2ud_T_htf_cold_calc", 406.04);
//	ssc_data_set_number(data, "sco2ud_T_htf_low", 554);
//	ssc_data_set_number(data, "sco2ud_T_htf_high", 589);
//	ssc_data_set_number(data, "sco2ud_T_amb_high", 45);
//	ssc_data_set_number(data, "sco2ud_m_dot_htf_low", 0.5);
//	ssc_data_set_number(data, "sco2ud_m_dot_htf_high", 1.05);
//	ssc_number_t p_sco2ud_T_htf_ind_od[65] = { 554, 0.50001432179685623, 0.96932004909372826, 0.97558944094670463, 0.56452945008574118, 0.99212119137385479, 0.9938261663184943, 0.071046760698792344, 1.0516901234966409, 1.0396677815546937, 1, 1, 1, 562.75, 0.50000677254611736, 0.98273822965810698, 0.98931005948737616, 0.55442312464765453, 0.99593839560635933, 0.99813862855158975, 0.060760248142716182, 1.0280675561194188, 1.0216094704370726, 1, 1, 1, 571.5, 0.49997035485832741, 0.99560036541912367, 1.0030665958141747, 0.54540906175814596, 0.99964495720997093, 1.0023929148428683, 0.052633463645261176, 1.0056543163255209, 1.0033307681195913, 1, 1, 1, 580.25, 0.50001678576431519, 0.99981330235493204, 1.0149012834308118, 0.53755371644427918, 0.99437061481748712, 1.0057187757624548, 0.046124783184180999, 0.93267305740289486, 0.97564080126247599, 1, 1, 1, 589, 0.49998766910986947, 0.99997642865903091, 1.0280965929880774, 0.53035396718324523, 0.98587808938966315, 1.0103635810622889, 0.040778740942652915, 0.84986379353651254, 0.96107738725047209, 1, 1, 1 };
//	ssc_data_set_matrix(data, "sco2ud_T_htf_ind_od", p_sco2ud_T_htf_ind_od, 5, 13);
//	ssc_number_t p_sco2ud_T_amb_ind_od[130] = { 0, 0.99994012767121321, 0.99974326147411507, 0.99987485850649338, 0.970379288808072, 0.95027470619969945, 0.93683012616718919, 0.059164848919520591, 0.049933864673572616, 0.04548467867978704, 1, 1, 1, 5, 0.99994012767121321, 0.99974326147411507, 0.99987485850649338, 0.970379288808072, 0.95027470619969945, 0.93683012616718919, 0.097133136927079936, 0.082248959886362452, 0.075269394493921518, 1, 1, 1, 10, 0.99994012767121321, 0.99974326147411507, 0.99987485850649338, 0.970379288808072, 0.95027470619969945, 0.93683012616718919, 0.17314165365655804, 0.14726896648379706, 0.13576325670205261, 1, 1, 1, 15, 0.99994012767121321, 0.99974326147411507, 0.99987485850649338, 0.970379288808072, 0.95027470619969945, 0.93683012616718919, 0.3481506775773433, 0.29853487098289033, 0.27855988861884273, 1, 1, 1, 20, 0.99994012767121321, 0.99974326147411507, 0.99987485850649338, 0.970379288808072, 0.95027470619969945, 0.93683012616718919, 0.85719459330059711, 0.74767538224853725, 0.71410844002614948, 1, 1, 1, 25, 0.99998860165458137, 0.99998405506376475, 0.99994129487763628, 0.97834420606252215, 0.9592086954026382, 0.94639328546002388, 1.4397512132898596, 1.1099623611534934, 0.9289610766286478, 1, 1, 1, 30, 0.99997868773200105, 1.0000128744625323, 0.99987390166815182, 0.99986135777103136, 0.97846151925556812, 0.96428182700735732, 1.2885473732704416, 1.0097961360792755, 0.85212892165494525, 1, 1, 1, 35, 0.96932004909372826, 0.99995022500741981, 0.99997642865903091, 0.99212119137385479, 0.99999459906789145, 0.98587808938966315, 1.0516901234966409, 1.0033307681195913, 0.84986379353651254, 1, 1, 1, 40, 0.85681928090544024, 0.88648580427756085, 0.90755440545490784, 0.8967547597723905, 0.90655814175453875, 0.91360819421216066, 0.54638640488302492, 0.52332590711799631, 0.50841120332212841, 1, 1, 1, 45, 0.76192612755033284, 0.78992076166486025, 0.81009772614290565, 0.81887517513574226, 0.82799762047592962, 0.83504194672755783, 0.32667431104075889, 0.31463845368914112, 0.30664767313185981, 1, 1, 1 };
//	ssc_data_set_matrix(data, "sco2ud_T_amb_ind_od", p_sco2ud_T_amb_ind_od, 10, 13);
//	ssc_number_t p_sco2ud_m_dot_htf_ind_od[130] = { 0.5, 0.50000238394483887, 0.50002030400496988, 0.5000072744279479, 0.49996830468452913, 0.54311461140321193, 0.58949291750771449, 0.0023965079935530688, 0.050651289208334289, 0.085268991630535099, 1, 1, 1, 0.56111111111111112, 0.56107350053341665, 0.56109752841228611, 0.56110469253490813, 0.55257347648896971, 0.59674948077268875, 0.64372698634700765, 0.0035623149303332731, 0.076089576895326735, 0.12158112224599875, 1, 1, 1, 0.62222222222222223, 0.62224697121064898, 0.62224996267725707, 0.62219761804463092, 0.60616131300962228, 0.65145619071565242, 0.69969734374588888, 0.0052256619247860641, 0.11370919032872756, 0.17234788816884622, 1, 1, 1, 0.68333333333333335, 0.68334763160686485, 0.68332772806833242, 0.68334268666386189, 0.66063755666452395, 0.70667012650879169, 0.75616720875118448, 0.0076071355040999147, 0.16722335929420953, 0.24228904960439612, 1, 1, 1, 0.74444444444444446, 0.74441099641078146, 0.74443754704645426, 0.73579757426674453, 0.7154614610083625, 0.76238207877819242, 0.80635338615523044, 0.010993657103543011, 0.24183918938496599, 0.28923568019902324, 1, 1, 1, 0.80555555555555558, 0.80548586203577432, 0.80556625128549619, 0.7607325689541361, 0.7709115279333274, 0.81874600280477805, 0.81846324371170665, 0.015863026451133713, 0.34650772248743805, 0.32712367869313513, 1, 1, 1, 0.8666666666666667, 0.86665068706416915, 0.86667726624963815, 0.77310372269008887, 0.82666048629288214, 0.87575544351530044, 0.82250334117333646, 0.022915833888653158, 0.49058181757621305, 0.32138671984149092, 1, 1, 1, 0.92777777777777781, 0.92781567340041837, 0.92781937609673271, 0.78228939831004085, 0.88241774237123771, 0.93313651551826904, 0.82559824503018264, 0.033028022568911568, 0.6869968410833619, 0.31804744512991351, 1, 1, 1, 0.98888888888888893, 0.98860321834099996, 0.98888850977399578, 0.78897052574490523, 0.93832104872570143, 0.99049121752897784, 0.82770087400040337, 0.049618846148749549, 0.94725737476210103, 0.31520682775233339, 1, 1, 1, 1.05, 1.0487348778724641, 1.0062218585376423, 0.79376542360284852, 1.0039295318936428, 1.0043865029637598, 0.82928412460314016, 0.049807927876335194, 0.99339517007581468, 0.31300940662606114, 1, 1, 1 };
//	ssc_data_set_matrix(data, "sco2ud_m_dot_htf_ind_od", p_sco2ud_m_dot_htf_ind_od, 10, 13);
//
//	...

/// Test series of Advanced Combinatorial Testing System (ACTS) runs
//TEST_F(CMTcsMoltenSalt, ACTS_sCO2_recompression) {
//
// Molten Salt power tower - Super-critical CO2 power cycle 
// ACTS framework driven testing
//int ACTS_sCO2_testing(ssc_data_t &data, int test_case)
//{
//	tcsmolten_salt_default(data);
//
//	//ssc_data_set_number(data, "T_htf_cold_des", 414.53549194335938);	// Different values each run ... But have no effect on outputs
//	
//	ssc_data_set_number(data, "pc_config", 2);
//	
//	ssc_data_set_number(data, "cycle_cutoff_frac", 0.5);
//	
//	//ssc_data_set_number(data, "sco2ud_T_htf_cold_calc", 414.53549194335938);	// Different values each run ... But have no effect on outputs
//	ssc_data_set_number(data, "sco2ud_T_htf_low", 554);
//	ssc_data_set_number(data, "sco2ud_T_htf_high", 589);
//	
//	ssc_data_set_number(data, "sco2ud_T_amb_high", 45);
//	ssc_data_set_number(data, "sco2ud_m_dot_htf_low", 0.5);
//	ssc_data_set_number(data, "sco2ud_m_dot_htf_high", 1.05);
//	ssc_number_t p_sco2ud_T_htf_ind_od[65] = { 554, 0.50001432179685623, 0.96932004909372826, 0.97558944094670463, 0.56452945008574118, 0.99212119137385479, 0.9938261663184943, 0.071046760698792344, 1.0516901234966409, 1.0396677815546937, 1, 1, 1, 562.75, 0.50000677254611736, 0.98273822965810698, 0.98931005948737616, 0.55442312464765453, 0.99593839560635933, 0.99813862855158975, 0.060760248142716182, 1.0280675561194188, 1.0216094704370726, 1, 1, 1, 571.5, 0.49997035485832741, 0.99560036541912367, 1.0030665958141747, 0.54540906175814596, 0.99964495720997093, 1.0023929148428683, 0.052633463645261176, 1.0056543163255209, 1.0033307681195913, 1, 1, 1, 580.25, 0.50001678576431519, 0.99981330235493204, 1.0149012834308118, 0.53755371644427918, 0.99437061481748712, 1.0057187757624548, 0.046124783184180999, 0.93267305740289486, 0.97564080126247599, 1, 1, 1, 589, 0.49998766910986947, 0.99997642865903091, 1.0280965929880774, 0.53035396718324523, 0.98587808938966315, 1.0103635810622889, 0.040778740942652915, 0.84986379353651254, 0.96107738725047209, 1, 1, 1 };
//	ssc_data_set_matrix(data, "sco2ud_T_htf_ind_od", p_sco2ud_T_htf_ind_od, 5, 13);
//	ssc_number_t p_sco2ud_T_amb_ind_od[130] = { 0, 0.99994012767121321, 0.99974326147411507, 0.99987485850649338, 0.970379288808072, 0.95027470619969945, 0.93683012616718919, 0.059164848919520591, 0.049933864673572616, 0.04548467867978704, 1, 1, 1, 5, 0.99994012767121321, 0.99974326147411507, 0.99987485850649338, 0.970379288808072, 0.95027470619969945, 0.93683012616718919, 0.097133136927079936, 0.082248959886362452, 0.075269394493921518, 1, 1, 1, 10, 0.99994012767121321, 0.99974326147411507, 0.99987485850649338, 0.970379288808072, 0.95027470619969945, 0.93683012616718919, 0.17314165365655804, 0.14726896648379706, 0.13576325670205261, 1, 1, 1, 15, 0.99994012767121321, 0.99974326147411507, 0.99987485850649338, 0.970379288808072, 0.95027470619969945, 0.93683012616718919, 0.3481506775773433, 0.29853487098289033, 0.27855988861884273, 1, 1, 1, 20, 0.99994012767121321, 0.99974326147411507, 0.99987485850649338, 0.970379288808072, 0.95027470619969945, 0.93683012616718919, 0.85719459330059711, 0.74767538224853725, 0.71410844002614948, 1, 1, 1, 25, 0.99998860165458137, 0.99998405506376475, 0.99994129487763628, 0.97834420606252215, 0.9592086954026382, 0.94639328546002388, 1.4397512132898596, 1.1099623611534934, 0.9289610766286478, 1, 1, 1, 30, 0.99997868773200105, 1.0000128744625323, 0.99987390166815182, 0.99986135777103136, 0.97846151925556812, 0.96428182700735732, 1.2885473732704416, 1.0097961360792755, 0.85212892165494525, 1, 1, 1, 35, 0.96932004909372826, 0.99995022500741981, 0.99997642865903091, 0.99212119137385479, 0.99999459906789145, 0.98587808938966315, 1.0516901234966409, 1.0033307681195913, 0.84986379353651254, 1, 1, 1, 40, 0.85681928090544024, 0.88648580427756085, 0.90755440545490784, 0.8967547597723905, 0.90655814175453875, 0.91360819421216066, 0.54638640488302492, 0.52332590711799631, 0.50841120332212841, 1, 1, 1, 45, 0.76192612755033284, 0.78992076166486025, 0.81009772614290565, 0.81887517513574226, 0.82799762047592962, 0.83504194672755783, 0.32667431104075889, 0.31463845368914112, 0.30664767313185981, 1, 1, 1 };
//	ssc_data_set_matrix(data, "sco2ud_T_amb_ind_od", p_sco2ud_T_amb_ind_od, 10, 13);
//	ssc_number_t p_sco2ud_m_dot_htf_ind_od[130] = { 0.5, 0.50000238394483887, 0.50002030400496988, 0.5000072744279479, 0.49996830468452913, 0.54311461140321193, 0.58949291750771449, 0.0023965079935530688, 0.050651289208334289, 0.085268991630535099, 1, 1, 1, 0.56111111111111112, 0.56107350053341665, 0.56109752841228611, 0.56110469253490813, 0.55257347648896971, 0.59674948077268875, 0.64372698634700765, 0.0035623149303332731, 0.076089576895326735, 0.12158112224599875, 1, 1, 1, 0.62222222222222223, 0.62224697121064898, 0.62224996267725707, 0.62219761804463092, 0.60616131300962228, 0.65145619071565242, 0.69969734374588888, 0.0052256619247860641, 0.11370919032872756, 0.17234788816884622, 1, 1, 1, 0.68333333333333335, 0.68334763160686485, 0.68332772806833242, 0.68334268666386189, 0.66063755666452395, 0.70667012650879169, 0.75616720875118448, 0.0076071355040999147, 0.16722335929420953, 0.24228904960439612, 1, 1, 1, 0.74444444444444446, 0.74441099641078146, 0.74443754704645426, 0.73579757426674453, 0.7154614610083625, 0.76238207877819242, 0.80635338615523044, 0.010993657103543011, 0.24183918938496599, 0.28923568019902324, 1, 1, 1, 0.80555555555555558, 0.80548586203577432, 0.80556625128549619, 0.7607325689541361, 0.7709115279333274, 0.81874600280477805, 0.81846324371170665, 0.015863026451133713, 0.34650772248743805, 0.32712367869313513, 1, 1, 1, 0.8666666666666667, 0.86665068706416915, 0.86667726624963815, 0.77310372269008887, 0.82666048629288214, 0.87575544351530044, 0.82250334117333646, 0.022915833888653158, 0.49058181757621305, 0.32138671984149092, 1, 1, 1, 0.92777777777777781, 0.92781567340041837, 0.92781937609673271, 0.78228939831004085, 0.88241774237123771, 0.93313651551826904, 0.82559824503018264, 0.033028022568911568, 0.6869968410833619, 0.31804744512991351, 1, 1, 1, 0.98888888888888893, 0.98860321834099996, 0.98888850977399578, 0.78897052574490523, 0.93832104872570143, 0.99049121752897784, 0.82770087400040337, 0.049618846148749549, 0.94725737476210103, 0.31520682775233339, 1, 1, 1, 1.05, 1.0487348778724641, 1.0062218585376423, 0.79376542360284852, 1.0039295318936428, 1.0043865029637598, 0.82928412460314016, 0.049807927876335194, 0.99339517007581468, 0.31300940662606114, 1, 1, 1 };
//	ssc_data_set_matrix(data, "sco2ud_m_dot_htf_ind_od", p_sco2ud_m_dot_htf_ind_od, 10, 13);
//	// sco2 default configuration preprocessing parameters
//	ssc_data_set_number(data, "_sco2_P_high_limit", 25);
//	ssc_data_set_number(data, "_sco2_P_ref", 115);
//	ssc_data_set_number(data, "_sco2_T_amb_des", 35);
//	ssc_data_set_number(data, "_sco2_T_approach", 10);
//	ssc_data_set_number(data, "_sco2_T_htf_hot_des", 574);
//	ssc_data_set_number(data, "_sco2_deltaT_PHX", 20);
//	ssc_data_set_number(data, "_sco2_design_eff", 0.412);
//	ssc_data_set_number(data, "_sco2_eta_c", 0.89);
//	ssc_data_set_number(data, "_sco2_eta_t", 0.9);
//	ssc_data_set_number(data, "_sco2_recup_eff_max", 0.96);
//	
//	// Testing level to vector index map
//	std::unordered_map<int, int> idx =
//	{
//		{-1, 0},
//		{ 0, 1},
//		{ 1, 2}
//	};
//
//	// Parameter test range values
//	std::vector<double> sco2_T_amb_des_vals{ 20.98, 35, 60 };		// Ambient temperature at design				// SAM SSC - "sco2_T_amb_des"
//	std::vector<double> sco2_T_approach_vals{ 1, 10, 30 };			// Air cooler aproach temperature 				// SAM SSC - "sco2_T_approach"
//	std::vector<double> deltaT_PHX_vals{ 1, 20, 50 };				// PHX approach temperature						// SAM SSC - "deltaT_PHX"
//	std::vector<double> eta_c_vals{ 0.70, 0.89, 1.0 };				// Compressor(s) Isentropic efficiency			// SAM SSC - "eta_c"
//	std::vector<double> eta_t_vals{ 0.70, 0.90, 1.0 };				// Turbine Isentropic efficiency				// SAM SSC - "eta_t"
//	std::vector<double> fan_power_perc_net_vals{ 0.25, 1.5, 5 };	// Cooling fan electricity consumption			// SAM SSC - "fan_power_perc_net"
//
//	// Full ACTS sCO2 framework - 15 total tests, where 6/15 tests are actually able to simulate on the SAM UI
//	
//	// Test case pass/fail summary                 1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
//	// P = Pass & F = Fail in SAM UI               F   P   F   P   P   F   P   F   F   F   F   P   F   F   P
//	//      std::vector<int> sco2_T_amb_des_lvls{ -1, -1, -1,  0,  0,  0,  1,  1,  1, -1,  0,  1,  1, -1,  1 };
//	//     std::vector<int> sco2_T_approach_lvls{ -1,  0,  1, -1,  0,  1, -1,  0,  1, -1,  0,  0,  1, -1,  1 };
//	//          std::vector<int> deltaT_PHX_lvls{  0,  1, -1,  1, -1,  0, -1,  0,  1,  0,  1, -1,  1, -1,  0 };
//	//               std::vector<int> eta_c_lvls{  0,  1, -1, -1,  0,  1,  1, -1,  0,  0,  1,  1, -1,  1,  1 };
//	//               std::vector<int> eta_t_lvls{  0,  1, -1,  0,  1, -1,  0,  1, -1, -1, -1,  0,  0,  1,  1 };
//	//  std::vector<int> fan_power_perc_net_lvls{  0,  1, -1,  1, -1,  0, -1,  0,  1, -1, -1 , 0,  0,  1,  1 };
//					
//
//	// Passing ACTS sco2 tests within the SAM UI
//	// NOTE:
//	// Certain tests are able to completely pass the unit tests
//	// using high tolerances while some tests still are
//	// unable to pass using the high tolerances when testing outputs
//	
//	// Unit test case pass/fall summary            2   4   5   7  12  15
//	// P = Pass & F = Fail for tested outputs      F   F   P   P   P   F
//	//      std::vector<int> sco2_T_amb_des_lvls{ -1,  0,  0,  1,  1,  1 };
//	//     std::vector<int> sco2_T_approach_lvls{  0, -1,  0, -1,  0,  1 };
//	//          std::vector<int> deltaT_PHX_lvls{  1,  1, -1, -1, -1,  0 };
//	//               std::vector<int> eta_c_lvls{  1, -1,  0,  1,  1,  1 };
//	//               std::vector<int> eta_t_lvls{  1,  0,  1,  0,  0,  1 };
//	//  std::vector<int> fan_power_perc_net_lvls{  1,  1,  1, -1,  0,  1 };
//
//
//	// Passing SAM UI configurations             5   7  12
//	//  P = Pass & F = Fail for tested outputs   P   P   P
//	      std::vector<int> sco2_T_amb_des_lvls{  0,  1,  1 };
//	     std::vector<int> sco2_T_approach_lvls{  0, -1,  0 };
//	          std::vector<int> deltaT_PHX_lvls{ -1, -1, -1 };
//	               std::vector<int> eta_c_lvls{  0,  1,  1 };
//	               std::vector<int> eta_t_lvls{  1,  0,  0 };
//	  std::vector<int> fan_power_perc_net_lvls{  1, -1,  0 };
//
//	// Get test case values from index
//	double sco2_T_amb_des_ACTS = sco2_T_amb_des_vals.at(idx.find(sco2_T_amb_des_lvls.at(test_case))->second);
//	double sco2_T_approach_ACTS = sco2_T_approach_vals.at(idx.find(sco2_T_approach_lvls.at(test_case))->second);
//	double deltaT_PHX_ACTS = deltaT_PHX_vals.at(idx.find(deltaT_PHX_lvls.at(test_case))->second);
//	double eta_c_ACTS = eta_c_vals.at(idx.find(eta_c_lvls.at(test_case))->second);
//	double eta_t_ACTS = eta_t_vals.at(idx.find(eta_t_lvls.at(test_case))->second);
//	double fan_power_perc_net_ACTS = fan_power_perc_net_vals.at(idx.find(fan_power_perc_net_lvls.at(test_case))->second);
//
//	// Assigning values to variables 
//	ssc_data_set_number(data, "sco2_T_amb_des", sco2_T_amb_des_ACTS);
//	ssc_data_set_number(data, "sco2_T_approach", sco2_T_approach_ACTS);
//	ssc_data_set_number(data, "deltaT_PHX", deltaT_PHX_ACTS);
//	ssc_data_set_number(data, "eta_c", eta_c_ACTS);
//	ssc_data_set_number(data, "eta_t", eta_t_ACTS);
//	ssc_data_set_number(data, "fan_power_perc_net", fan_power_perc_net_ACTS);
//
//	int status = run_module(data, "tcsmolten_salt");
//
//	return status;
//}
//	// Outputs of 6/15 total ACTS tests for the sCO2 model
//	// The other 9 test case scenarios were unable to simulate properly
//	// on the SAM UI.
//
//	//  // ACTS pass/fail summary				F          F          P         P          P          F
//	//  // sCO2 ACTS Test Cases                 2          4          5         7         12         15
//	//  std::vector<double> annual_energys{ 4.47253e8, 4.83719e8, 5.3984e8, 5.29801e8, 5.12115e8, 4.648e8 };
//	//  std::vector<double> land_area_bases{ 1847.04, 1847.04, 1847.04, 1847.04, 1847.04, 1847.04 };
//	//  std::vector<double> capacity_factors{ 49.3297, 53.3518, 59.5417, 58.4344, 56.4837, 51.2651 };
//	//  std::vector<double> annual_W_cycle_grosss{ 5.20554e8, 6.03866e8, 6.21568e8, 6.29351e8, 6.31792e8, 5.49548e8 };
//	//  std::vector<double> kwh_per_kws{ 4321.28, 4673.62, 5215.85, 5118.85, 4947.98, 4490.82 };
//	//  std::vector<double> conversion_factors{ 85.9187, 80.1037, 86.8513, 84.1821, 81.0576, 84.5787 };
//	//  std::vector<double> N_hels{ 8790, 8790, 8790, 8790, 8790, 8790 };
//	//  std::vector<double> rec_heights{ 21.6029, 21.6029, 21.6029, 21.6029, 21.6029, 21.6029 };
//	//  std::vector<double> A_sfs{ 1.26905e6, 1.26905e6, 1.26905e6, 1.26905e6, 1.26905e6, 1.26905e6 };
//	//  std::vector<double> D_recs{ 17.65, 17.65, 17.65, 17.65, 17.65, 17.65 };
//	//  std::vector<double> annual_total_water_uses{ 55965.3, 55965.3, 55965.3, 55965.3, 55965.3, 55965.3 };
//	//  std::vector<double> csp_pt_cost_total_land_areas{ 1892.04, 1892.04, 1892.04, 1892.04, 1892.04, 1892.04 };
//	//  std::vector<double> h_towers{ 193.458, 193.458, 193.458, 193.458, 193.458, 193.458 };
//
//	// Passing ACTS configurations
//	// sCO2 ACTS Test Cases                 5         7         12
//	std::vector<double> annual_energys{ 5.3984e8, 5.29801e8, 5.12115e8 };
//	std::vector<double> land_area_bases{ 1847.04, 1847.04, 1847.04 };
//	std::vector<double> capacity_factors{ 59.5417, 58.4344, 56.4837 };
//	std::vector<double> annual_W_cycle_grosss{ 6.21568e8, 6.29351e8, 6.31792e8 };
//	std::vector<double> kwh_per_kws{ 5215.85, 5118.85, 4947.98 };
//	std::vector<double> conversion_factors{ 86.8513, 84.1821, 81.0576 };
//	std::vector<double> N_hels{ 8790, 8790, 8790 };
//	std::vector<double> rec_heights{ 21.6029, 21.6029, 21.6029 };
//	std::vector<double> A_sfs{ 1.26905e6, 1.26905e6, 1.26905e6 };
//	std::vector<double> D_recs{ 17.65, 17.65, 17.65 };
//	std::vector<double> annual_total_water_uses{ 55965.3, 55965.3, 55965.3 };
//	std::vector<double> csp_pt_cost_total_land_areas{ 1892.04, 1892.04, 1892.04 };
//	std::vector<double> h_towers{ 193.458, 193.458, 193.458 };
//
//	ssc_data_t data = ssc_data_create();
//
//	for (std::vector<double>::size_type i = 0; i != annual_energys.size(); i++) {
//		int test_errors = ACTS_sCO2_testing(data, i);
//
//		EXPECT_FALSE(test_errors);
//		if (!test_errors)
//		{
//			ssc_number_t annual_energy;
//			ssc_data_get_number(data, "annual_energy", &annual_energy);
//			EXPECT_NEAR(annual_energy, annual_energys[i], annual_energys[i] * m_error_tolerance_hi) << "Annual Energy"; // choose m_error_tolerance_hi
//
//			ssc_number_t land_area_base;
//			ssc_data_get_number(data, "land_area_base", &land_area_base);
//			EXPECT_NEAR(land_area_base, land_area_bases[i], land_area_bases[i] * m_error_tolerance_hi) << "Land Area Base";
//
//			ssc_number_t capacity_factor;
//			ssc_data_get_number(data, "capacity_factor", &capacity_factor);
//			EXPECT_NEAR(capacity_factor, capacity_factors[i], capacity_factors[i] * m_error_tolerance_hi) << "Capacity Factor"; // choose m_error_tolerance_hi
//
//			ssc_number_t annual_W_cycle_gross;
//			ssc_data_get_number(data, "annual_W_cycle_gross", &annual_W_cycle_gross);
//			EXPECT_NEAR(annual_W_cycle_gross, annual_W_cycle_grosss[i], annual_W_cycle_grosss[i] * m_error_tolerance_hi) << "Annual W_cycle Gross"; // choose m_error_tolerance_hi
//
//			ssc_number_t kwh_per_kw;
//			ssc_data_get_number(data, "kwh_per_kw", &kwh_per_kw);
//			EXPECT_NEAR(kwh_per_kw, kwh_per_kws[i], kwh_per_kws[i] * m_error_tolerance_hi) << "kwh per kw"; // choose m_error_tolerance_hi
//
//			ssc_number_t conversion_factor;
//			ssc_data_get_number(data, "conversion_factor", &conversion_factor);
//			EXPECT_NEAR(conversion_factor, conversion_factors[i], conversion_factors[i] * m_error_tolerance_hi) << "Conversion Factor"; // choose m_error_tolerance_hi
//
//			ssc_number_t N_hel;
//			ssc_data_get_number(data, "N_hel", &N_hel);
//			EXPECT_NEAR(N_hel, N_hels[i], N_hels[i] * m_error_tolerance_hi) << "Number of Heliostats";
//
//			ssc_number_t rec_height;
//			ssc_data_get_number(data, "rec_height", &rec_height);
//			EXPECT_NEAR(rec_height, rec_heights[i], rec_heights[i] * m_error_tolerance_hi) << "Rec Height";
//
//			ssc_number_t A_sf;
//			ssc_data_get_number(data, "A_sf", &A_sf);
//			EXPECT_NEAR(A_sf, A_sfs[i], A_sfs[i] * m_error_tolerance_hi) << "Solar Field Area";
//
//			ssc_number_t D_rec;
//			ssc_data_get_number(data, "D_rec", &D_rec);
//			EXPECT_NEAR(D_rec, D_recs[i], D_recs[i] * m_error_tolerance_hi) << "Receiver Outer Diameter";
//
//			ssc_number_t annual_total_water_use;
//			ssc_data_get_number(data, "annual_total_water_use", &annual_total_water_use);
//			EXPECT_NEAR(annual_total_water_use, annual_total_water_uses[i], annual_total_water_uses[i] * m_error_tolerance_hi) << "Annual Total Water Use";
//
//			ssc_number_t csp_pt_cost_total_land_area;
//			ssc_data_get_number(data, "csp.pt.cost.total_land_area", &csp_pt_cost_total_land_area);
//			EXPECT_NEAR(csp_pt_cost_total_land_area, csp_pt_cost_total_land_areas[i], csp_pt_cost_total_land_areas[i] * m_error_tolerance_hi) << "Total Land Area";
//
//			ssc_number_t h_tower;
//			ssc_data_get_number(data, "h_tower", &h_tower);
//			EXPECT_NEAR(h_tower, h_towers[i], h_towers[i] * m_error_tolerance_hi) << "Tower Height";
//
//		}
//	}
//}
