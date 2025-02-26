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

#include "core.h"

// for adjustment factors
#include "common.h"

// solarpilot header files
#include "AutoPilot_API.h"
#include "SolarField.h"
#include "IOUtil.h"
#include "csp_common.h"

// Can probably delete these headers later...
#include "csp_solver_util.h"
#include "csp_solver_core.h"
#include "csp_solver_pt_sf_perf_interp.h"
#include "csp_solver_mspt_receiver_222.h"
#include "csp_solver_mspt_receiver.h"
#include "csp_solver_mspt_collector_receiver.h"
#include "csp_solver_pc_Rankine_indirect_224.h"
#include "csp_solver_two_tank_tes.h"
#include "csp_solver_tou_block_schedules.h"

#include "csp_dispatch.h"

#include "csp_solver_cr_electric_resistance.h"
#include "csp_solver_cavity_receiver.h"

#include "csp_system_costs.h"

#include <ctime>

static var_info _cm_vtab_tcsmolten_salt[] = {
    // VARTYPE       DATATYPE    NAME                                  LABEL                                                                                                                                      UNITS           META                                 GROUP                                       REQUIRED_IF                                                         CONSTRAINTS      UI_HINTS
    { SSC_INPUT,     SSC_STRING, "solar_resource_file",                "Local weather file path",                                                                                                                 "",             "",                                  "Solar Resource",                    "?",                                                                "LOCAL_FILE",    ""},
    { SSC_INPUT,     SSC_TABLE,  "solar_resource_data",                "Weather resource data in memory",                                                                                                         "",             "",                                  "Solar Resource",                    "?",                                                                "",              ""},

    { SSC_INPUT,     SSC_NUMBER, "is_parallel_htr",                    "Does plant include a HTF heater parallel to solar field?",                                                                                "",             "",                                  "System Control",                           "?=0",                                                              "",              ""},

    { SSC_INPUT,     SSC_NUMBER, "csp_financial_model",                "",                                                                                                                                        "1-8",          "",                                  "Financial Model",                          "?=1",                                                              "INTEGER,MIN=0", ""},
    { SSC_INPUT,     SSC_NUMBER, "ppa_multiplier_model",               "PPA multiplier model 0: dispatch factors dispatch_factorX, 1: hourly multipliers dispatch_factors_ts",                                    "0/1",          "0=diurnal,1=timestep",              "Time of Delivery Factors",                 "?=0", /*need a default so this var works in required_if*/          "INTEGER,MIN=0", ""},
    { SSC_INPUT,     SSC_ARRAY,  "dispatch_factors_ts",                "Dispatch payment factor array",                                                                                                           "",             "",                                  "Time of Delivery Factors",                 "ppa_multiplier_model=1&csp_financial_model<5&is_dispatch=1",                     "",              ""},
    { SSC_INPUT,     SSC_ARRAY,  "timestep_load_fractions",            "Turbine load fraction for each timestep, alternative to block dispatch",                                                                  "",             "",                                  "System Control",                           "?",                                                                "",              ""},

    { SSC_INPUT,     SSC_NUMBER, "field_model_type",                   "0=design field and tower/receiver geometry, 1=design field, 2=user specified field, 3=user performance maps vs solar position",           "",             "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "gross_net_conversion_factor",        "Estimated gross to net conversion factor",                                                                                                "",             "",                                  "System Design",                            "*",                                                                "",              ""},

    { SSC_INPUT,     SSC_NUMBER, "receiver_type",                      "0: external (default), 1; cavity",                                                                                                        "",             "",                                  "Heliostat Field",                          "?=0",                                                              "",              ""},

    // Cavity height and width can be reset by solarpilot optimization
    { SSC_INOUT,     SSC_NUMBER, "cav_rec_height",                     "Cavity receiver height",                                                                                                                  "m",            "",                                  "Tower and Receiver",                       "receiver_type=1",                                                  "",              ""},
    { SSC_INOUT,     SSC_NUMBER, "cav_rec_width",                      "Cavity receiver width",                                                                                                                   "m",            "",                                  "Tower and Receiver",                       "receiver_type=1",                                                  "",              ""},
    // Cavity inputs that should *not* be reset during call to this cmod
    { SSC_INPUT,     SSC_NUMBER, "n_cav_rec_panels",                   "Cavity receiver number of panels",                                                                                                        "",             "",                                  "Tower and Receiver",                       "receiver_type=1",                                                  "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "cav_rec_span",                       "Cavity receiver span angle",                                                                                                              "deg",          "",                                  "Tower and Receiver",                       "receiver_type=1",                                                  "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "cav_rec_passive_abs",                "Cavity receiver passive surface solar absorptance",                                                                                       "",             "",                                  "Tower and Receiver",                       "receiver_type=1",                                                  "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "cav_rec_passive_eps",                "Cavity receiver passive surface thermal emissivity",                                                                                      "",             "",                                  "Tower and Receiver",                       "receiver_type=1",                                                  "",              ""},


    { SSC_INPUT,     SSC_NUMBER, "helio_width",                        "Heliostat width",                                                                                                                         "m",            "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "helio_height",                       "Heliostat height",                                                                                                                        "m",            "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "helio_optical_error_mrad",           "Heliostat optical error",                                                                                                                 "mrad",         "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "helio_active_fraction",              "Heliostat active fraction",                                                                                                               "",             "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "dens_mirror",                        "Ratio of heliostat reflective area to profile",                                                                                           "",             "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "helio_reflectance",                  "Heliostat reflectance",                                                                                                                   "",             "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "rec_absorptance",                    "Receiver absorptance",                                                                                                                    "",             "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "rec_hl_perm2",                       "Receiver design heatloss",                                                                                                                "kW/m2",        "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "land_max",                           "Land max boundary",                                                                                                                       "-ORm",         "",                                  "Heliostat Field",                          "?=7.5",                                                            "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "land_min",                           "Land min boundary",                                                                                                                       "-ORm",         "",                                  "Heliostat Field",                          "?=0.75",                                                           "",              ""},
    { SSC_INPUT,     SSC_MATRIX, "land_bound_table",                   "Land boundary table",                                                                                                                     "m",            "",                                  "Heliostat Field",                          "?",                                                                "",              ""},
    { SSC_INPUT,     SSC_ARRAY,  "land_bound_list",                    "Land boundary table listing",                                                                                                             "",             "",                                  "Heliostat Field",                          "?",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "dni_des",                            "Design-point DNI",                                                                                                                        "W/m2",         "",                                  "System Design",                            "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "p_start",                            "Heliostat startup energy",                                                                                                                "kWe-hr",       "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "p_track",                            "Heliostat tracking energy",                                                                                                               "kWe",          "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "hel_stow_deploy",                    "Stow/deploy elevation angle",                                                                                                             "deg",          "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "v_wind_max",                         "Heliostat max wind velocity",                                                                                                             "m/s",          "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "interp_nug",                         "Interpolation nugget",                                                                                                                    "-",            "",                                  "Heliostat Field",                          "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "interp_beta",                        "Interpolation beta coef.",                                                                                                                "-",            "",                                  "Heliostat Field",                          "?=1.99",                                                           "",              ""},
    { SSC_INPUT,     SSC_MATRIX, "helio_aim_points",                   "Heliostat aim point table",                                                                                                               "m",            "",                                  "Heliostat Field",                          "?",                                                                "",              ""},
    { SSC_INPUT,     SSC_MATRIX, "eta_map",                            "Field efficiency array",                                                                                                                  "",             "",                                  "Heliostat Field",                          "?",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "eta_map_aod_format",                 "Use 3D AOD format field efficiency array",                                                                                                "",             "heliostat",                         "Heliostat Field",                          "",                                                                 "",              ""},
    { SSC_INPUT,     SSC_MATRIX, "flux_maps",                          "Flux map intensities",                                                                                                                    "",             "",                                  "Heliostat Field",                          "?",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "c_atm_0",                            "Attenuation coefficient 0",                                                                                                               "",             "",                                  "Heliostat Field",                          "?=0.006789",                                                       "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "c_atm_1",                            "Attenuation coefficient 1",                                                                                                               "",             "",                                  "Heliostat Field",                          "?=0.1046",                                                         "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "c_atm_2",                            "Attenuation coefficient 2",                                                                                                               "",             "",                                  "Heliostat Field",                          "?=-0.0107",                                                        "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "c_atm_3",                            "Attenuation coefficient 3",                                                                                                               "",             "",                                  "Heliostat Field",                          "?=0.002845",                                                       "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "n_facet_x",                          "Number of heliostat facets - X",                                                                                                          "",             "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "n_facet_y",                          "Number of heliostat facets - Y",                                                                                                          "",             "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "focus_type",                         "Heliostat focus method",                                                                                                                  "",             "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "cant_type",                          "Heliostat canting method",                                                                                                                "",             "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "n_flux_days",                        "Number of days in flux map lookup",                                                                                                       "",             "",                                  "Tower and Receiver",                       "?=8",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "delta_flux_hrs",                     "Hourly frequency in flux map lookup",                                                                                                     "",             "",                                  "Tower and Receiver",                       "?=1",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "water_usage_per_wash",               "Water usage per wash",                                                                                                                    "L/m2_aper",    "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "washing_frequency",                  "Mirror washing frequency",                                                                                                                "none",         "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "check_max_flux",                     "Check max flux at design point",                                                                                                          "",             "",                                  "Heliostat Field",                          "?=0",                                                              "",              ""},


    { SSC_INPUT,     SSC_NUMBER, "sf_excess",                          "Heliostat field multiple",                                                                                                                "",             "",                                  "System Design",                            "?=1.0",                                                            "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "calc_fluxmaps",                      "Include fluxmap calculations",                                                                                                            "",             "",                                  "Heliostat Field",                          "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "tower_fixed_cost",                   "Tower fixed cost",                                                                                                                        "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "tower_exp",                          "Tower cost scaling exponent",                                                                                                             "",             "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "rec_ref_cost",                       "Receiver reference cost",                                                                                                                 "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "rec_ref_area",                       "Receiver reference area for cost scale",                                                                                                  "",             "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "rec_cost_exp",                       "Receiver cost scaling exponent",                                                                                                          "",             "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "site_spec_cost",                     "Site improvement cost",                                                                                                                   "$/m2",         "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "heliostat_spec_cost",                "Heliostat field cost",                                                                                                                    "$/m2",         "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "plant_spec_cost",                    "Power cycle specific cost",                                                                                                               "$/kWe",        "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "bop_spec_cost",                      "BOS specific cost",                                                                                                                       "$/kWe",        "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "tes_spec_cost",                      "Thermal energy storage cost",                                                                                                             "$/kWht",       "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "land_spec_cost",                     "Total land area cost",                                                                                                                    "$/acre",       "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "contingency_rate",                   "Contingency for cost overrun",                                                                                                            "%",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "sales_tax_rate",                     "Sales tax rate",                                                                                                                          "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "sales_tax_frac",                     "Percent of cost to which sales tax applies",                                                                                              "%",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "cost_sf_fixed",                      "Solar field fixed cost",                                                                                                                  "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "fossil_spec_cost",                   "Fossil system specific cost",                                                                                                             "$/kWe",        "",                                  "System Costs",                             "*",                                                                "",              ""},

    { SSC_INPUT,     SSC_NUMBER, "flux_max",                           "Maximum allowable flux",                                                                                                                  "",             "",                                  "Tower and Receiver",                       "?=1000",                                                           "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "opt_init_step",                      "Optimization initial step size",                                                                                                          "",             "",                                  "Heliostat Field",                          "?=0.05",                                                           "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "opt_max_iter",                       "Max number iteration steps",                                                                                                              "",             "",                                  "Heliostat Field",                          "?=200",                                                            "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "opt_conv_tol",                       "Optimization convergence tolerance",                                                                                                      "",             "",                                  "Heliostat Field",                          "?=0.001",                                                          "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "opt_flux_penalty",                   "Optimization flux overage penalty",                                                                                                       "",             "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "opt_algorithm",                      "Optimization algorithm",                                                                                                                  "",             "",                                  "Heliostat Field",                          "?=1",                                                              "",              ""},

    //other costs needed for optimization update
    { SSC_INPUT,     SSC_NUMBER, "csp.pt.cost.epc.per_acre",           "EPC cost per acre",                                                                                                                       "$/acre",       "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "csp.pt.cost.epc.percent",            "EPC cost percent of direct",                                                                                                              "%",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "csp.pt.cost.epc.per_watt",           "EPC cost per watt",                                                                                                                       "$/W",          "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "csp.pt.cost.epc.fixed",              "EPC fixed",                                                                                                                               "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "csp.pt.cost.plm.percent",            "PLM cost percent of direct",                                                                                                              "%",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "csp.pt.cost.plm.per_watt",           "PLM cost per watt",                                                                                                                       "$/W",          "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "csp.pt.cost.plm.fixed",              "PLM fixed",                                                                                                                               "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "csp.pt.sf.fixed_land_area",          "Fixed land area",                                                                                                                         "acre",         "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "csp.pt.sf.land_overhead_factor",     "Land overhead factor",                                                                                                                    "",             "",                                  "Heliostat Field",                          "*",                                                                "",              ""},





    // System Design
    { SSC_INPUT,     SSC_NUMBER, "T_htf_cold_des",                     "Cold HTF inlet temperature at design conditions",                                                                                         "C",            "",                                  "System Design",                            "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "T_htf_hot_des",                      "Hot HTF outlet temperature at design conditions",                                                                                         "C",            "",                                  "System Design",                            "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "P_ref",                              "Reference output electric power at design condition",                                                                                     "MW",           "",                                  "System Design",                            "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "design_eff",                         "Power cycle efficiency at design",                                                                                                        "none",         "",                                  "System Design",                            "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "tshours",                            "Equivalent full-load thermal storage hours",                                                                                              "hr",           "",                                  "System Design",                            "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "solarm",                             "Solar multiple",                                                                                                                          "-",            "",                                  "System Design",                            "*",                                                                "",              ""},

    // Receiver (type 222) parameters
    { SSC_INPUT,     SSC_NUMBER, "N_panels",                           "Number of individual panels on the receiver",                                                                                             "",             "",                                  "Tower and Receiver",                       "*",                                                                "INTEGER",       ""},
    { SSC_INPUT,     SSC_NUMBER, "d_tube_out",                         "The outer diameter of an individual receiver tube",                                                                                       "mm",           "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "th_tube",                            "The wall thickness of a single receiver tube",                                                                                            "mm",           "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "mat_tube",                           "Receiver tube material, 2=Stainless AISI316",                                                                                             "",             "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "rec_htf",                            "Receiver HTF, 17=Salt (60% NaNO3, 40% KNO3) 10=Salt (46.5% LiF 11.5% NaF 42% KF) 50=Lookup tables",                                       "",             "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_MATRIX, "field_fl_props",                     "User defined field fluid property data",                                                                                                  "-",            "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "Flow_type",                          "Receiver flow pattern: see figure on SAM Receiver page",                                                                                  "",             "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "crossover_shift",                    "Number of panels shift in receiver crossover position",                                                                                   "",             "",                                  "Tower and Receiver",                       "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "epsilon",                            "The emissivity of the receiver surface coating",                                                                                          "",             "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "hl_ffact",                           "The heat loss factor (thermal loss fudge factor)",                                                                                        "",             "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "f_rec_min",                          "Minimum receiver mass flow rate turn down fraction",                                                                                      "",             "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "rec_su_delay",                       "Fixed startup delay time for the receiver",                                                                                               "hr",           "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "rec_qf_delay",                       "Energy-based receiver startup delay (fraction of rated thermal power)",                                                                   "",             "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "csp.pt.rec.max_oper_frac",           "Maximum receiver mass flow rate fraction",                                                                                                "",             "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "eta_pump",                           "Receiver HTF pump efficiency",                                                                                                            "",             "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "piping_length_mult",                 "Piping length multiplier",                                                                                                                "",             "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "piping_length_const",                "Piping constant length",                                                                                                                  "m",            "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},

    // New variables replacing deprecated variable "piping_loss". Variable currently not required so exec() can check if assigned and throw a more detailed error
    { SSC_INPUT,     SSC_NUMBER, "piping_loss_coefficient",            "Thermal loss per meter of piping",                                                                                                        "Wt/m2-K",      "",                                  "Tower and Receiver",                       "",                                                                 "",              ""},

	{ SSC_INPUT,     SSC_NUMBER, "rec_clearsky_model",				   "Clearsky model: None = -1, User-defined data = 0, Meinel = 1; Hottel = 2; Allen = 3; Moon = 4",											  "",             "",                                  "Tower and Receiver",                       "?=-1",															   "",              ""},
	{ SSC_INPUT,     SSC_ARRAY,  "rec_clearsky_dni",					"User-defined clear-sky DNI",																											  "W/m2",         "",                                  "Tower and Receiver",                       "rec_clearsky_model=0",											   "",              ""},
	{ SSC_INPUT,     SSC_NUMBER, "rec_clearsky_fraction",               "Weighting fraction on clear-sky DNI for receiver flow control",                                                                          "",             "",                                  "Tower and Receiver",                       "?=0.0",                                                            "",              ""},

    // Transient receiver parameters
	{ SSC_INPUT,     SSC_NUMBER, "is_rec_model_trans",                 "Formulate receiver model as transient?",                                                                                                  "",             "",                                  "Tower and Receiver",                       "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "is_rec_startup_trans",               "Formulate receiver startup model as transient?",                                                                                          "",             "",                                  "Tower and Receiver",                       "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "rec_tm_mult",                        "Receiver thermal mass multiplier",                                                                                                        "",             "",                                  "Tower and Receiver",                       "?=1.0",                                                            "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "riser_tm_mult",                      "Riser thermal mass multiplier",                                                                                                           "",             "",                                  "Tower and Receiver",                       "?=1.0",                                                            "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "downc_tm_mult",                      "Downcomer thermal mass multiplier",                                                                                                       "",             "",                                  "Tower and Receiver",                       "?=1.0",                                                            "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "u_riser",                            "Design point HTF velocity in riser",                                                                                                      "m/s",          "",                                  "Tower and Receiver",                       "?=4.0",                                                            "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "th_riser",                           "Riser or downcomer tube wall thickness",                                                                                                  "mm",           "",                                  "Tower and Receiver",                       "?=15.0",                                                           "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "heat_trace_power",                   "Riser/downcomer heat trace power during startup",                                                                                         "kW/m",         "",                                  "Tower and Receiver",                       "?=500.0",                                                          "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "preheat_flux",                       "Tube absorbed solar flux during preheat",                                                                                                 "kW/m2",        "",                                  "Tower and Receiver",                       "?=50.0",                                                           "",              ""},
	{ SSC_INPUT,     SSC_NUMBER, "min_preheat_time",                   "Minimum time required in preheat startup stage",                                                                                          "hr",			  "",                                  "Tower and Receiver",                       "?=0.0",                                                         "",              ""},
	{ SSC_INPUT,     SSC_NUMBER, "min_fill_time",                      "Startup time delay for filling the receiver/piping",                                                                                      "hr",			  "",                                  "Tower and Receiver",                       "?=0.1333",                                                         "",              ""},	
	{ SSC_INPUT,     SSC_NUMBER, "startup_ramp_time",                  "Time required to reach full flux during receiver startup",                                                                                "hr",           "",                                  "Tower and Receiver",                       "?=0.1333",                                                            "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "startup_target_Tdiff",               "Target HTF T at end of startup - steady state hot HTF temperature",                                                                          "C",            "",                                  "Tower and Receiver",                       "?=-5.0",                                                           "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "is_rec_startup_from_T_soln",         "Begin receiver startup from solved temperature profiles?",                                                                                "",             "",                                  "Tower and Receiver",                       "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "is_rec_enforce_min_startup",         "Always enforce minimum startup time",                                                                                                     "",             "",                                  "Tower and Receiver",                       "?=1",                                                              "",              ""},

    // Parallel heater parameters
    { SSC_INPUT,     SSC_NUMBER, "heater_mult",                        "Heater multiple relative to design cycle thermal power",                                                                                  "-",             "",                                 "Parallel Heater",                          "is_parallel_htr=1",                                                "",              "" },
    { SSC_INPUT,     SSC_NUMBER, "f_q_dot_des_allowable_su",           "Fraction of design power allowed during startup",                                                                                         "-",            "",                                  "Parallel Heater",                          "is_parallel_htr=1",                                                "",              "" },
    { SSC_INPUT,     SSC_NUMBER, "hrs_startup_at_max_rate",            "Duration of startup at max startup power",                                                                                                "hr",           "",                                  "Parallel Heater",                          "is_parallel_htr=1",                                                "",              "" },
    { SSC_INPUT,     SSC_NUMBER, "f_q_dot_heater_min",                 "Minimum allowable heater output as fraction of design",                                                                                   "",             "",                                  "Parallel Heater",                          "is_parallel_htr=1",                                                "",              "" },


    // TES parameters - general
    { SSC_INPUT,     SSC_NUMBER, "csp.pt.tes.init_hot_htf_percent",    "Initial fraction of available volume that is hot",                                                                                        "%",            "",                                  "Thermal Storage",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "h_tank",                             "Total height of tank (height of HTF when tank is full)",                                                                                  "m",            "",                                  "Thermal Storage",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "cold_tank_max_heat",                 "Rated heater capacity for cold tank heating",                                                                                             "MW",           "",                                  "Thermal Storage",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "u_tank",                             "Loss coefficient from the tank",                                                                                                          "W/m2-K",       "",                                  "Thermal Storage",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "tank_pairs",                         "Number of equivalent tank pairs",                                                                                                         "",             "",                                  "Thermal Storage",                          "*",                                                                "INTEGER",       ""},
    { SSC_INPUT,     SSC_NUMBER, "cold_tank_Thtr",                     "Minimum allowable cold tank HTF temperature",                                                                                             "C",            "",                                  "Thermal Storage",                          "*",                                                                "",              ""},
    // TES parameters - 2 tank
    { SSC_INPUT,     SSC_NUMBER, "h_tank_min",                         "Minimum allowable HTF height in storage tank",                                                                                            "m",            "",                                  "Thermal Storage",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "hot_tank_Thtr",                      "Minimum allowable hot tank HTF temperature",                                                                                              "C",            "",                                  "Thermal Storage",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "hot_tank_max_heat",                  "Rated heater capacity for hot tank heating",                                                                                              "MW",           "",                                  "Thermal Storage",                          "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "tanks_in_parallel",                  "Tanks are in parallel, not in series, with solar field",                                                                                  "-",            "",                                  "Thermal Storage",                          "*",                                                                "",              "" },


        //RADIATIVE COOLING WITH COLD STORAGE
    { SSC_INPUT,     SSC_NUMBER, "h_ctes_tank_min",                    "Minimum allowable water height in storage tank",                                                                                          "m",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "ctes_tshours",                       "Equivalent full load storage hours",                                                                                                      "hr",           "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "ctes_field_fl",                      "Fluid in radiator field. 3=liquid water. Other = Glycol.",                                                                                "-",            "",                                  "RADCOOL",                                  "?=3",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "h_ctes_tank",                        "Total height of cold storage tank when full",                                                                                             "m",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "u_ctes_tank",                        "Loss coefficient from cold storage tank",                                                                                                 "W/m2-K",       "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "ctes_tankpairs",                     "Number of equivalent tank pairs",                                                                                                         "-",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "T_ctes_cold_design",                 "Design value of cooled water to power block",                                                                                             "C",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "T_ctes_warm_design",                 "Design value of warm water returning from power block",                                                                                   "C",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "T_ctes_warm_ini",                    "Initial value of warm tank",                                                                                                              "C",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "T_ctes_cold_ini",                    "Initial value of cold tank",                                                                                                              "C",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "f_ctes_warm_ini",                    "Initial fraction of avail. volume that is warm",                                                                                          "-",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "rad_multiplier",                     "Ratio of radiator field area to solar aperature area",                                                                                    "-",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "m_dot_radpanel",                     "Mass flow rate through single radiator panel",                                                                                            "kg/sec",       "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "n_rad_tubes",                        "Number of parallel tubes in single radiator panel",                                                                                       "-",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "W_rad_tubes",                        "Center-to-center distance between tubes in radiator panel",                                                                               "m",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "L_rad",                              "Length of radiator panel row",                                                                                                            "m",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "th_rad_panel",                       "Thickness of radiator panel",                                                                                                             "m",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "D_rad_tubes",                        "Inner diameter of tubes in radiator panel",                                                                                               "m",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "k_panel",                            "Thermal conductivity of radiator panel material",                                                                                         "W/m-K",        "",                                  "RADCOOL",                                  "?=235",                                                            "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "epsilon_radtop",                     "Emmissivity of top of radiator panel",                                                                                                    "-",            "",                                  "RADCOOL",                                  "?=.95",                                                            "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "epsilon_radbot",                     "Emmissivity of top of radiator panel bottom (facing ground)",                                                                             "-",            "",                                  "RADCOOL",                                  "?=.07",                                                            "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "epsilon_radgrnd",                    "Emmissivity of ground underneath radiator panel",                                                                                         "-",            "",                                  "RADCOOL",                                  "?=.90",                                                            "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "L_rad_sections",                     "Length of individual radiator panel",                                                                                                     "m",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "epsilon_radHX",                      "Effectiveness of HX between radiative field and cold storage",                                                                            "-",            "",                                  "RADCOOL",                                  "?=.8",                                                             "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "ctes_type",                          "Type of cold storage (2=two tank, 3= three node)",                                                                                        "-",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "helio_area_tot",                     "Heliostat total reflective area",                                                                                                         "-",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "radiator_unitcost",                  "Cost of radiative panels",                                                                                                                "$/m^2",        "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "radiator_installcost",               "Installation cost of radiative panels",                                                                                                   "$/m^2",        "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "radiator_fluidcost",                 "Cost of circulating fluid in radiative panels",                                                                                           "$/L",          "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "radfluid_vol_ratio",                 "Ratio of fluid in distribution to fluid in panels",                                                                                       "-",            "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "ctes_cost",                          "Cost of cold storage construction",                                                                                                       "$/L",          "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "rad_pressuredrop",                   "Average pressure drop through a radiative panel & distribution",                                                                          "kPa",          "",                                  "RADCOOL",                                  "?=0",                                                              "",              ""},

    // Power Cycle Inputs
    { SSC_INPUT,     SSC_NUMBER, "pc_config",                          "PC configuration 0=Steam Rankine (224), 1=user defined",                                                                                  "",             "",                                  "Power Cycle",                              "?=0",                                                              "INTEGER",       ""},
    { SSC_INPUT,     SSC_NUMBER, "pb_pump_coef",                       "Pumping power to move 1kg of HTF through PB loop",                                                                                        "kW/kg",        "",                                  "Power Cycle",                              "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "startup_time",                       "Time needed for power block startup",                                                                                                     "hr",           "",                                  "Power Cycle",                              "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "startup_frac",                       "Fraction of design thermal power needed for startup",                                                                                     "none",         "",                                  "Power Cycle",                              "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "cycle_max_frac",                     "Maximum turbine over design operation fraction",                                                                                          "",             "",                                  "Power Cycle",                              "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "cycle_cutoff_frac",                  "Minimum turbine operation fraction before shutdown",                                                                                      "",             "",                                  "Power Cycle",                              "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "q_sby_frac",                         "Fraction of thermal power required for standby",                                                                                          "",             "",                                  "Power Cycle",                              "*",                                                                "",              ""},

    // Steam Rankine cycle
    { SSC_INPUT,     SSC_NUMBER, "dT_cw_ref",                          "Reference condenser cooling water inlet/outlet temperature difference",                                                                   "C",            "",                                  "Rankine Cycle",                            "pc_config=0",                                                      "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "T_amb_des",                          "Reference ambient temperature at design point",                                                                                           "C",            "",                                  "Rankine Cycle",                            "pc_config=0",                                                      "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "P_boil",                             "Boiler operating pressure",                                                                                                               "bar",          "",                                  "Rankine Cycle",                            "pc_config=0",                                                      "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "CT",                                 "Condensor type: 1=evaporative, 2=air, 3=hybrid",                                                                                          "",             "",                                  "Rankine Cycle",                            "pc_config=0",                                                      "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "T_approach",                         "Cooling tower approach temperature",                                                                                                      "C",            "",                                  "Rankine Cycle",                            "pc_config=0",                                                      "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "T_ITD_des",                          "ITD at design for dry system",                                                                                                            "C",            "",                                  "Rankine Cycle",                            "pc_config=0",                                                      "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "P_cond_ratio",                       "Condenser pressure ratio",                                                                                                                "",             "",                                  "Rankine Cycle",                            "pc_config=0",                                                      "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "pb_bd_frac",                         "Power block blowdown steam fraction",                                                                                                     "",             "",                                  "Rankine Cycle",                            "pc_config=0",                                                      "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "P_cond_min",                         "Minimum condenser pressure",                                                                                                              "inHg",         "",                                  "Rankine Cycle",                            "pc_config=0",                                                      "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "n_pl_inc",                           "Number of part-load increments for the heat rejection system",                                                                            "none",         "",                                  "Rankine Cycle",                            "pc_config=0",                                                      "INTEGER",       ""},
    { SSC_INPUT,     SSC_ARRAY,  "F_wc",                               "TOU array of fractions indicating wet cooling share for hybrid cooling",                                                                  "",             "",                                  "System Control",                           "pc_config=0",                                                      "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "tech_type",                          "Turbine inlet pressure control 1=Fixed, 3=Sliding",                                                                                       "",             "",                                  "Rankine Cycle",                            "pc_config=0",                                                      "",              ""},

    // User Defined cycle
    { SSC_INPUT,     SSC_NUMBER, "ud_f_W_dot_cool_des",                "Percent of user-defined power cycle design gross output consumed by cooling",                                                             "%",            "",                                  "User Defined Power Cycle",                 "pc_config=1",                                                      "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "ud_m_dot_water_cool_des",            "Mass flow rate of water required at user-defined power cycle design point",                                                               "kg/s",         "",                                  "User Defined Power Cycle",                 "pc_config=1",                                                      "",              ""},
    { SSC_INPUT,     SSC_MATRIX, "ud_ind_od",                          "Off design user-defined power cycle performance as function of T_htf, m_dot_htf [ND], and T_amb",                                         "",             "",                                  "User Defined Power Cycle",                 "pc_config=1",                                                      "",              ""},

    // System Control
    { SSC_INPUT,     SSC_NUMBER, "time_start",                         "Simulation start time",                                                                                                                   "s",            "",                                  "System Control",                           "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "time_stop",                          "Simulation stop time",                                                                                                                    "s",            "",                                  "System Control",                           "?=31536000",                                                       "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "time_steps_per_hour",                "Number of simulation time steps per hour",                                                                                                "",             "",                                  "System Control",                           "?=-1",                                                             "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "vacuum_arrays",                      "Allocate arrays for only the required number of steps",                                                                                   "",             "",                                  "System Control",                           "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "pb_fixed_par",                       "Fixed parasitic load - runs at all times",                                                                                                "MWe/MWcap",    "",                                  "System Control",                           "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "aux_par",                            "Aux heater, boiler parasitic",                                                                                                            "MWe/MWcap",    "",                                  "System Control",                           "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "aux_par_f",                          "Aux heater, boiler parasitic - multiplying fraction",                                                                                     "",             "",                                  "System Control",                           "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "aux_par_0",                          "Aux heater, boiler parasitic - constant coefficient",                                                                                     "",             "",                                  "System Control",                           "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "aux_par_1",                          "Aux heater, boiler parasitic - linear coefficient",                                                                                       "",             "",                                  "System Control",                           "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "aux_par_2",                          "Aux heater, boiler parasitic - quadratic coefficient",                                                                                    "",             "",                                  "System Control",                           "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "bop_par",                            "Balance of plant parasitic power fraction",                                                                                               "MWe/MWcap",    "",                                  "System Control",                           "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "bop_par_f",                          "Balance of plant parasitic power fraction - mult frac",                                                                                   "",             "",                                  "System Control",                           "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "bop_par_0",                          "Balance of plant parasitic power fraction - const coeff",                                                                                 "",             "",                                  "System Control",                           "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "bop_par_1",                          "Balance of plant parasitic power fraction - linear coeff",                                                                                "",             "",                                  "System Control",                           "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "bop_par_2",                          "Balance of plant parasitic power fraction - quadratic coeff",                                                                             "",             "",                                  "System Control",                           "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_ARRAY,  "f_turb_tou_periods",                 "Dispatch logic for turbine load fraction",                                                                                                "",             "",                                  "System Control",                           "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_MATRIX, "weekday_schedule",                   "12x24 CSP operation Time-of-Use Weekday schedule",                                                                                        "",             "",                                  "System Control",                           "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_MATRIX, "weekend_schedule",                   "12x24 CSP operation Time-of-Use Weekend schedule",                                                                                        "",             "",                                  "System Control",                           "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "is_tod_pc_target_also_pc_max",       "Is the TOD target cycle heat input also the max cycle heat input?",                                                                       "",             "",                                  "System Control",                           "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "is_dispatch",                        "Allow dispatch optimization?",                                                                                                            "",             "",                                  "System Control",                           "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "disp_horizon",                       "Time horizon for dispatch optimization",                                                                                                  "hour",         "",                                  "System Control",                           "is_dispatch=1",                                                    "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "disp_frequency",                     "Frequency for dispatch optimization calculations",                                                                                        "hour",         "",                                  "System Control",                           "is_dispatch=1",                                                    "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "disp_steps_per_hour",                "Time steps per hour for dispatch optimization calculations",                                                                              "",             "",                                  "System Control",                           "?=1",                                                              "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "disp_max_iter",                      "Max number of dispatch optimization iterations",                                                                                          "",             "",                                  "System Control",                           "is_dispatch=1",                                                    "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "disp_timeout",                       "Max dispatch optimization solve duration",                                                                                                "s",            "",                                  "System Control",                           "is_dispatch=1",                                                    "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "disp_mip_gap",                       "Dispatch optimization solution tolerance",                                                                                                "",             "",                                  "System Control",                           "is_dispatch=1",                                                    "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "disp_spec_bb",                       "Dispatch optimization B&B heuristic",                                                                                                     "",             "",                                  "System Control",                           "?=-1",                                                             "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "disp_reporting",                     "Dispatch optimization reporting level",                                                                                                   "",             "",                                  "System Control",                           "?=-1",                                                             "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "disp_spec_presolve",                 "Dispatch optimization presolve heuristic",                                                                                                "",             "",                                  "System Control",                           "?=-1",                                                             "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "disp_spec_scaling",                  "Dispatch optimization scaling heuristic",                                                                                                 "",             "",                                  "System Control",                           "?=-1",                                                             "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "disp_time_weighting",                "Dispatch optimization future time discounting factor",                                                                                    "",             "",                                  "System Control",                           "?=0.99",                                                           "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "is_write_ampl_dat",                  "Write AMPL data files for dispatch run",                                                                                                  "",             "",                                  "System Control",                           "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_STRING, "ampl_data_dir",                      "AMPL data file directory",                                                                                                                "",             "",                                  "System Control",                           "?=''",                                                             "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "is_ampl_engine",                     "Run dispatch optimization with external AMPL engine",                                                                                     "",             "",                                  "System Control",                           "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_STRING, "ampl_exec_call",                     "System command to run AMPL code",                                                                                                         "",             "",                                  "System Control",                           "?='ampl sdk_solution.run'",                                        "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "disp_rsu_cost",                      "Receiver startup cost",                                                                                                                   "$",            "",                                  "System Control",                           "is_dispatch=1",                                                    "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "disp_csu_cost",                      "Cycle startup cost",                                                                                                                      "$",            "",                                  "System Control",                           "is_dispatch=1",                                                    "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "disp_pen_delta_w",                   "Dispatch cycle production change penalty",                                                                                                "$/kWe-change", "",                                  "System Control",                           "is_dispatch=1",                                                    "",              "" },
    { SSC_INPUT,     SSC_NUMBER, "disp_inventory_incentive",           "Dispatch storage terminal inventory incentive multiplier",                                                                                "",             "",                                  "System Control",                           "?=0.0",                                                            "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "q_rec_standby",                      "Receiver standby energy consumption",                                                                                                     "kWt",          "",                                  "System Control",                           "?=9e99",                                                           "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "q_rec_heattrace",                    "Receiver heat trace energy consumption during startup",                                                                                   "kWe-hr",       "",                                  "System Control",                           "?=0.0",                                                            "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "is_wlim_series",                     "Use time-series net electricity generation limits",                                                                                       "",             "",                                  "System Control",                           "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_ARRAY,  "wlim_series",                        "Time series net electicity generation limits",                                                                                            "kWe",          "",                                  "System Control",                           "is_wlim_series=1",                                                 "",              ""},


    // Financial inputs
    { SSC_INPUT,     SSC_NUMBER, "ppa_soln_mode",                      "PPA solution mode (0=Specify IRR target, 1=Specify PPA price)",                                                                           "",             "",                                  "Financial Solution Mode",                  "ppa_multiplier_model=0&csp_financial_model<5&is_dispatch=1",       "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "en_electricity_rates",               "Enable electricity rates for grid purchase",                                                                                              "0/1",          "",                                  "Electricity Rates",                        "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_MATRIX, "dispatch_sched_weekday",             "PPA pricing weekday schedule, 12x24",                                                                                                     "",             "",                                  "Time of Delivery Factors",                 "ppa_multiplier_model=0&csp_financial_model<5&is_dispatch=1",       "",              ""},
    { SSC_INPUT,     SSC_MATRIX, "dispatch_sched_weekend",             "PPA pricing weekend schedule, 12x24",                                                                                                     "",             "",                                  "Time of Delivery Factors",                 "ppa_multiplier_model=0&csp_financial_model<5&is_dispatch=1",       "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "dispatch_factor1",                   "Dispatch payment factor 1",                                                                                                               "",             "",                                  "Time of Delivery Factors",                 "ppa_multiplier_model=0&csp_financial_model<5&is_dispatch=1",       "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "dispatch_factor2",                   "Dispatch payment factor 2",                                                                                                               "",             "",                                  "Time of Delivery Factors",                 "ppa_multiplier_model=0&csp_financial_model<5&is_dispatch=1",       "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "dispatch_factor3",                   "Dispatch payment factor 3",                                                                                                               "",             "",                                  "Time of Delivery Factors",                 "ppa_multiplier_model=0&csp_financial_model<5&is_dispatch=1",       "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "dispatch_factor4",                   "Dispatch payment factor 4",                                                                                                               "",             "",                                  "Time of Delivery Factors",                 "ppa_multiplier_model=0&csp_financial_model<5&is_dispatch=1",       "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "dispatch_factor5",                   "Dispatch payment factor 5",                                                                                                               "",             "",                                  "Time of Delivery Factors",                 "ppa_multiplier_model=0&csp_financial_model<5&is_dispatch=1",       "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "dispatch_factor6",                   "Dispatch payment factor 6",                                                                                                               "",             "",                                  "Time of Delivery Factors",                 "ppa_multiplier_model=0&csp_financial_model<5&is_dispatch=1",       "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "dispatch_factor7",                   "Dispatch payment factor 7",                                                                                                               "",             "",                                  "Time of Delivery Factors",                 "ppa_multiplier_model=0&csp_financial_model<5&is_dispatch=1",       "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "dispatch_factor8",                   "Dispatch payment factor 8",                                                                                                               "",             "",                                  "Time of Delivery Factors",                 "ppa_multiplier_model=0&csp_financial_model<5&is_dispatch=1",       "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "dispatch_factor9",                   "Dispatch payment factor 9",                                                                                                               "",             "",                                  "Time of Delivery Factors",                 "ppa_multiplier_model=0&csp_financial_model<5&is_dispatch=1",       "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "is_dispatch_series",                 "Use time-series dispatch factors",                                                                                                        "",             "",                                  "System Control",                           "?=0",                                                              "",              ""},
    { SSC_INPUT,     SSC_ARRAY,  "dispatch_series",                    "Time series dispatch factors",                                                                                                            "",             "",                                  "System Control",                           "",                                                                 "",              ""},
    { SSC_INPUT,     SSC_MATRIX, "mp_energy_market_revenue",           "Energy market revenue input",                                                                                                             "",             "Lifetime x 2[Cleared Capacity(MW),Price($/MWh)]", "Revenue",                    "csp_financial_model=6&is_dispatch=1",                              "",              ""},

    // Inputs required for user defined SF performance
    { SSC_INPUT,     SSC_NUMBER, "A_sf_in",                            "Solar field area",                                                                                                                        "m^2",          "",                                  "Heliostat Field",                          "",                                                                 "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "A_sf",                               "Solar field area",                                                                                                                        "m^2",          "",                                  "",                                         "*",                                                                "",              ""},


    // optimized outputs updated depending on run type 
    { SSC_INOUT,     SSC_NUMBER, "rec_height",                         "Receiver height",                                                                                                                         "m",            "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INOUT,     SSC_NUMBER, "D_rec",                              "The overall outer diameter of the receiver",                                                                                              "m",            "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INOUT,     SSC_NUMBER, "h_tower",                            "Tower height",                                                                                                                            "m",            "",                                  "Tower and Receiver",                       "*",                                                                "",              ""},
    { SSC_INOUT,     SSC_NUMBER, "N_hel",                              "Number of heliostats",                                                                                                                    "",             "",                                  "Heliostat Field",                          "",                                                                 "",              ""},
    { SSC_INOUT,     SSC_MATRIX, "helio_positions",                    "Heliostat position table",                                                                                                                "",             "",                                  "Heliostat Field",                          "*",                                                                "",              "COL_LABEL=XY_POSITION"},
    { SSC_INOUT,     SSC_NUMBER, "land_area_base",                     "Base land area occupied by heliostats",                                                                                                   "acre",         "",                                  "Heliostat Field",                          "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.total_land_area",        "Total land area",                                                                                                                         "acre",         "",                                  "System Costs",                             "*",                                                                "",              ""},

    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.site_improvements",      "Site improvement cost",                                                                                                                   "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.heliostats",             "Heliostat cost",                                                                                                                          "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.tower",                  "Tower cost",                                                                                                                              "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.receiver",               "Receiver cost",                                                                                                                           "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.storage",                "TES cost",                                                                                                                                "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.power_block",            "Power cycle cost",                                                                                                                        "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.rad_field",              "Radiative field cost"                                                                                                                     "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.rad_fluid",              "Radiative fluid cost"                                                                                                                     "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.rad_storage",            "Cold storage cost"                                                                                                                        "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.bop",                    "BOP cost",                                                                                                                                "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.fossil",                 "Fossil backup cost",                                                                                                                      "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "ui_direct_subtotal",                 "Direct capital precontingency cost",                                                                                                      "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.contingency",            "Contingency cost",                                                                                                                        "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "total_direct_cost",                  "Total direct cost",                                                                                                                       "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.epc.total",              "EPC and owner cost",                                                                                                                      "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.plm.total",              "Total land cost",                                                                                                                         "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.sales_tax.total",        "Sales tax cost",                                                                                                                          "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "total_indirect_cost",                "Total indirect cost",                                                                                                                     "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "total_installed_cost",               "Total installed cost",                                                                                                                    "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "csp.pt.cost.installed_per_capacity", "Estimated installed cost per cap",                                                                                                        "$",            "",                                  "System Costs",                             "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "system_capacity",                    "System capacity",                                                                                                                         "kWe",          "",                                  "System Costs",                             "*",                                                                "",              "" },
        // land area with variable name required by downstream financial model
    { SSC_OUTPUT,    SSC_NUMBER, "total_land_area",                    "Total land area",                                                                                                                         "acre",         "",                                  "System Costs",                             "*",                                                                "",              "" },

        // Construction financing inputs/outputs (SSC variable table from cmod_cb_construction_financing)
    { SSC_INPUT,     SSC_NUMBER, "const_per_interest_rate1",           "Interest rate, loan 1",                                                                                                                   "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_interest_rate2",           "Interest rate, loan 2",                                                                                                                   "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_interest_rate3",           "Interest rate, loan 3",                                                                                                                   "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_interest_rate4",           "Interest rate, loan 4",                                                                                                                   "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_interest_rate5",           "Interest rate, loan 5",                                                                                                                   "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_months1",                  "Months prior to operation, loan 1",                                                                                                       "",             "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_months2",                  "Months prior to operation, loan 2",                                                                                                       "",             "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_months3",                  "Months prior to operation, loan 3",                                                                                                       "",             "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_months4",                  "Months prior to operation, loan 4",                                                                                                       "",             "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_months5",                  "Months prior to operation, loan 5",                                                                                                       "",             "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_percent1",                 "Percent of total installed cost, loan 1",                                                                                                 "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_percent2",                 "Percent of total installed cost, loan 2",                                                                                                 "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_percent3",                 "Percent of total installed cost, loan 3",                                                                                                 "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_percent4",                 "Percent of total installed cost, loan 4",                                                                                                 "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_percent5",                 "Percent of total installed cost, loan 5",                                                                                                 "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_upfront_rate1",            "Upfront fee on principal, loan 1",                                                                                                        "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_upfront_rate2",            "Upfront fee on principal, loan 2",                                                                                                        "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_upfront_rate3",            "Upfront fee on principal, loan 3",                                                                                                        "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_upfront_rate4",            "Upfront fee on principal, loan 4",                                                                                                        "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_INPUT,     SSC_NUMBER, "const_per_upfront_rate5",            "Upfront fee on principal, loan 5",                                                                                                        "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_principal1",               "Principal, loan 1",                                                                                                                       "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_principal2",               "Principal, loan 2",                                                                                                                       "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_principal3",               "Principal, loan 3",                                                                                                                       "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_principal4",               "Principal, loan 4",                                                                                                                       "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_principal5",               "Principal, loan 5",                                                                                                                       "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_interest1",                "Interest cost, loan 1",                                                                                                                   "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_interest2",                "Interest cost, loan 2",                                                                                                                   "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_interest3",                "Interest cost, loan 3",                                                                                                                   "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_interest4",                "Interest cost, loan 4",                                                                                                                   "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_interest5",                "Interest cost, loan 5",                                                                                                                   "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_total1",                   "Total financing cost, loan 1",                                                                                                            "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_total2",                   "Total financing cost, loan 2",                                                                                                            "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_total3",                   "Total financing cost, loan 3",                                                                                                            "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_total4",                   "Total financing cost, loan 4",                                                                                                            "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_total5",                   "Total financing cost, loan 5",                                                                                                            "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_percent_total",            "Total percent of installed costs, all loans",                                                                                             "%",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_principal_total",          "Total principal, all loans",                                                                                                              "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "const_per_interest_total",           "Total interest costs, all loans",                                                                                                         "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "construction_financing_cost",        "Total construction financing cost",                                                                                                       "$",            "",                                  "Financial Parameters",                     "*",                                                                "",              ""},

    // ****************************************************************************************************************************************
    //     DEPRECATED INPUTS -- exec() checks if a) variable is assigned and b) if replacement variable is assigned. throws exception if a=true and b=false
    // ****************************************************************************************************************************************
    { SSC_INPUT,     SSC_NUMBER, "piping_loss",                        "Thermal loss per meter of piping",                                                                                                        "Wt/m",         "",                                  "Tower and Receiver",                       "",                                                                 "",              "" },



    // ****************************************************************************************************************************************
    // Outputs here:
    // ****************************************************************************************************************************************
        // Simulation outputs
    { SSC_OUTPUT,    SSC_ARRAY,  "time_hr",                            "Time at end of timestep",                                                                                                                 "hr",           "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "solzen",                             "Resource solar zenith",                                                                                                                   "deg",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "solaz",                              "Resource solar azimuth",                                                                                                                  "deg",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "beam",                               "Resource beam normal irradiance",                                                                                                         "W/m2",         "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "tdry",                               "Resource dry Bulb temperature",                                                                                                           "C",            "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "twet",                               "Resource wet Bulb temperature",                                                                                                           "C",            "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "rh",                                 "Resource relative humidity",                                                                                                              "%",            "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "wspd",                               "Resource wind velocity",                                                                                                                  "m/s",          "",                                  "",                                         "*",                                                                "",              ""},

        // Collector-receiver outputs
            // Eventually want to make this INOUT, but will have to add 'eta_map' to UI...
    { SSC_OUTPUT,    SSC_MATRIX, "eta_map_out",                        "Solar field optical efficiencies",                                                                                                        "",             "",                                  "",                                         "*",                                                                "",              "COL_LABEL=OPTICAL_EFFICIENCY,ROW_LABEL=NO_ROW_LABEL"},
    { SSC_OUTPUT,    SSC_MATRIX, "flux_maps_out",                      "Flux map intensities",                                                                                                                    "",             "",                                  "",                                         "*",                                                                "",              "COL_LABEL=FLUX_MAPS,ROW_LABEL=NO_ROW_LABEL"},
    { SSC_OUTPUT,    SSC_MATRIX, "flux_maps_for_import",               "Flux map for import",                                                                                                                     "",             "",                                  "",                                         "*",                                                                "",              "COL_LABEL=FLUX_MAPS,ROW_LABEL=NO_ROW_LABEL" },

    { SSC_OUTPUT,    SSC_ARRAY,  "q_sf_inc",                           "Field incident thermal power",                                                                                                            "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "eta_field",                          "Field optical efficiency",                                                                                                                "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "defocus",                            "Field optical focus fraction",                                                                                                            "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "sf_adjust_out",                      "Field availability adjustment factor",                                                                                                    "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "q_dot_rec_inc",                      "Receiver incident thermal power",                                                                                                         "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "eta_therm",                          "Receiver thermal efficiency",                                                                                                             "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "Q_thermal",                          "Receiver thermal power to HTF less piping loss",                                                                                          "MWt",          "",                                  "",                                         "*",                                                                "",              ""},

    { SSC_OUTPUT,    SSC_ARRAY,  "m_dot_rec",                          "Receiver mass flow rate",                                                                                                                 "kg/s",         "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "q_startup",                          "Receiver startup thermal energy consumed",                                                                                                "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "T_rec_in",                           "Receiver HTF inlet temperature",                                                                                                          "C",            "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "T_rec_out",                          "Receiver HTF outlet temperature",                                                                                                         "C",            "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "q_piping_losses",                    "Receiver header/tower piping losses",                                                                                                     "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "q_thermal_loss",                     "Receiver convection and emission losses",                                                                                                 "MWt",          "",                                  "",                                         "*",                                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "q_dot_reflection_loss",              "Receiver reflection losses",                                                                                                              "MWt",          "",                                  "",                                         "receiver_type=1",                                                  "",              ""},
    
    { SSC_OUTPUT,    SSC_ARRAY,  "T_rec_out_end",                      "Receiver HTF outlet temperature at end of timestep",                                                                                      "C",            "",								   "CR",                                       "is_rec_model_trans=1",                                             "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "T_rec_out_max",                      "Receiver maximum HTF outlet temperature during timestep",                                                                                 "C",            "",                                  "CR",                                       "is_rec_model_trans=1",                                             "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "T_panel_out_max",                    "Receiver panel maximum HTF outlet temperature during timestep",                                                                           "C",            "",                                  "CR",                                       "is_rec_model_trans=1",                                             "",              ""},    
    { SSC_OUTPUT,    SSC_ARRAY,  "T_wall_rec_inlet",                   "Receiver inlet panel wall temperature at end of timestep",                                                                                "C",            "",                                  "CR",                                       "is_rec_model_trans=1",                                             "",              ""},    
    { SSC_OUTPUT,    SSC_ARRAY,  "T_wall_rec_outlet",                  "Receiver outlet panel wall temperature at end of timestep",                                                                               "C",            "",                                  "CR",                                       "is_rec_model_trans=1",                                             "",              ""},    
    { SSC_OUTPUT,    SSC_ARRAY,  "T_wall_riser",                       "Receiver riser wall temperature at end of timestep",                                                                                      "C",            "",                                  "CR",                                       "is_rec_model_trans=1",                                             "",              ""},    
    { SSC_OUTPUT,    SSC_ARRAY,  "T_wall_downcomer",                   "Receiver downcomer wall temperature at end of timestep",                                                                                  "C",            "",                                  "CR",                                       "is_rec_model_trans=1",                                             "",              ""},    
                                                                                                                                                                                                                                                                                                                    
	{ SSC_OUTPUT,    SSC_ARRAY,  "clearsky",						   "Predicted clear-sky beam normal irradiance",																							  "W/m2",         "",                                  "CR",                                       "is_rec_model_trans=1",                                             "",              "" },
	{ SSC_OUTPUT,    SSC_ARRAY,  "Q_thermal_ss",					   "Receiver thermal power to HTF less piping loss (steady state)",																			  "MWt",          "",                                  "CR",                                       "is_rec_model_trans=1",                                             "",              "" },
	{ SSC_OUTPUT,    SSC_ARRAY,  "Q_thermal_ss_csky",				   "Receiver thermal power to HTF less piping loss under clear-sky conditions (steady state)",												  "MWt",          "",                                  "CR",                                       "is_rec_model_trans=1",                                             "",              "" },

        // Heater outputs is_parallel_htr
    { SSC_OUTPUT,    SSC_ARRAY,  "W_dot_heater",                       "Parallel heater electricity consumption",                                                                                                 "MWe",          "",                                  "Parallel Heater",                          "is_parallel_htr=1",                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "q_dot_heater_to_htf",                "Parallel heater thermal power to HTF",                                                                                                    "MWt",          "",                                  "Parallel Heater",                          "is_parallel_htr=1",                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "q_dot_heater_startup",               "Parallel heater thermal power consumed during startup",                                                                                   "MWt",          "",                                  "Parallel Heater",                          "is_parallel_htr=1",                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "m_dot_htf_heater",                   "Parallel heater HTF mass flow rate",                                                                                                      "kg/s",         "",                                  "Parallel Heater",                          "is_parallel_htr=1",                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "T_htf_heater_in",                    "Parallel heater HTF inlet temperature",                                                                                                   "C",            "",                                  "Parallel Heater",                          "is_parallel_htr=1",                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "T_htf_heater_out",                   "Parallel heater HTF outlet temperature",                                                                                                  "C",            "",                                  "Parallel Heater",                          "is_parallel_htr=1",                                                "",              "" },

        // Power cycle outputs
    { SSC_OUTPUT,    SSC_ARRAY,  "eta",                                "PC efficiency, gross",                                                                                                                    "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "q_pb",                               "PC input energy",                                                                                                                         "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "m_dot_pc",                           "PC HTF mass flow rate",                                                                                                                   "kg/s",         "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "q_pc_startup",                       "PC startup thermal energy",                                                                                                               "MWht",         "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "q_dot_pc_startup",                   "PC startup thermal power",                                                                                                                "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "P_cycle",                            "PC electrical power output, gross",                                                                                                       "MWe",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "T_pc_in",                            "PC HTF inlet temperature",                                                                                                                "C",            "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "T_pc_out",                           "PC HTF outlet temperature",                                                                                                               "C",            "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "m_dot_water_pc",                     "PC water consumption, makeup + cooling",                                                                                                  "kg/s",         "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "T_cond_out",                         "PC condenser water outlet temperature",                                                                                                   "C",            "",                                  "PC",                                       "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "T_cold",                             "Cold storage cold temperature",                                                                                                           "C",            "",                                  "PC",                                       "?",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "m_cold",                             "Cold storage cold tank mass",                                                                                                             "kg",           "",                                  "PC",                                       "?",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "m_warm",                             "Cold storage warm tank mass",                                                                                                             "kg",           "",                                  "PC",                                       "?",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "T_warm",                             "Cold storage warm tank temperature",                                                                                                      "C",            "",                                  "PC",                                       "?",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "T_rad_out",                          "Radiator outlet temperature",                                                                                                             "C",            "",                                  "PC",                                       "?",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "A_radfield",                         "Radiator field surface area",                                                                                                             "m^2",          "",                                  "PC",                                       "?",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "P_cond",                             "PC condensing presssure",                                                                                                                 "Pa",           "",                                  "PC",                                       "?",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "radcool_control",                    "Radiative cooling status code",                                                                                                           "-",            "",                                  "PC",                                       "?",                                                                "",              ""},


        // Thermal energy storage outputs
    { SSC_OUTPUT,    SSC_ARRAY,  "tank_losses",                        "TES thermal losses",                                                                                                                      "MWt",          "",                                  "",                                         "*",                                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "q_heater",                           "TES freeze protection power",                                                                                                             "MWe",          "",                                  "",                                         "*",                                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "T_tes_hot",                          "TES hot temperature",                                                                                                                     "C",            "",                                  "",                                         "*",                                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "T_tes_cold",                         "TES cold temperature",                                                                                                                    "C",            "",                                  "",                                         "*",                                                                "",              "" },
    { SSC_OUTPUT,   SSC_ARRAY,   "mass_tes_cold",                      "TES cold tank mass (end)",                                                                                                                "kg",           "",                                  "",                                         "*",                                                                "",              "" },
    { SSC_OUTPUT,   SSC_ARRAY,   "mass_tes_hot",                       "TES hot tank mass (end)",                                                                                                                 "kg",           "",                                  "",                                         "*",                                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "q_dc_tes",                           "TES discharge thermal power",                                                                                                             "MWt",          "",                                  "",                                         "*",                                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "q_ch_tes",                           "TES charge thermal power",                                                                                                                "MWt",          "",                                  "",                                         "*",                                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "e_ch_tes",                           "TES charge state",                                                                                                                        "MWht",         "",                                  "",                                         "*",                                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "m_dot_cr_to_tes_hot",                "Mass flow: field to hot TES",                                                                                                             "kg/s",         "",                                  "",                                         "*",                                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "m_dot_tes_hot_out",                  "Mass flow: TES hot out",                                                                                                                  "kg/s",         "",                                  "",                                         "*",                                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "m_dot_pc_to_tes_cold",               "Mass flow: cycle to cold TES",                                                                                                            "kg/s",         "",                                  "",                                         "*",                                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "m_dot_tes_cold_out",                 "Mass flow: TES cold out",                                                                                                                 "kg/s",         "",                                  "",                                         "*",                                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "m_dot_field_to_cycle",               "Mass flow: field to cycle",                                                                                                               "kg/s",         "",                                  "",                                         "*",                                                                "",              "" },
    { SSC_OUTPUT,    SSC_ARRAY,  "m_dot_cycle_to_field",               "Mass flow: cycle to field",                                                                                                               "kg/s",         "",                                  "",                                         "*",                                                                "",              "" },
    //{ SSC_OUTPUT,    SSC_ARRAY,  "m_dot_tes_dc",                       "TES discharge mass flow rate",                                                                                                          "kg/s",         "",                                  "",                                         "*",                                                                "",              "" },
    //{ SSC_OUTPUT,    SSC_ARRAY,  "m_dot_tes_ch",                       "TES charge mass flow rate",                                                                                                             "kg/s",         "",                                  "",                                         "*",                                                                "",              "" },


        // Parasitics outputs
    { SSC_OUTPUT,    SSC_ARRAY,  "pparasi",                            "Parasitic power heliostat drives",                                                                                                        "MWe",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "P_tower_pump",                       "Parasitic power receiver/tower HTF pump",                                                                                                 "MWe",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "htf_pump_power",                     "Parasitic power TES and cycle HTF pump",                                                                                                  "MWe",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "P_cooling_tower_tot",                "Parasitic power condenser operation",                                                                                                     "MWe",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "P_fixed",                            "Parasitic power fixed load",                                                                                                              "MWe",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "P_plant_balance_tot",                "Parasitic power generation-dependent load",                                                                                               "MWe",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "P_rec_heattrace",                    "Receiver heat trace parasitic load",                                                                                                      "MWe",          "",                                  "System",                                   "is_rec_model_trans=1",                                             "",              ""},

        // System outputs
    { SSC_OUTPUT,    SSC_ARRAY,  "P_out_net",                          "Total electric power to grid",                                                                                                            "MWe",          "",                                  "",                                         "*",                                                                "",              ""},

        // Controller outputs
    { SSC_OUTPUT,    SSC_ARRAY,  "tou_value",                          "CSP operating time-of-use value",                                                                                                         "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "pricing_mult",                       "PPA price multiplier",                                                                                                                    "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "n_op_modes",                         "Operating modes in reporting timestep",                                                                                                   "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "op_mode_1",                          "1st operating mode",                                                                                                                      "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "op_mode_2",                          "2nd operating mode, if applicable",                                                                                                       "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "op_mode_3",                          "3rd operating mode, if applicable",                                                                                                       "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "m_dot_balance",                      "Relative mass flow balance error",                                                                                                        "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "q_balance",                          "Relative energy balance error",                                                                                                           "",             "",                                  "",                                         "*",                                                                "",              ""},

        // Dispatch outputs
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_rel_mip_gap",                   "Dispatch relative MIP gap",                                                                                                               "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_solve_state",                   "Dispatch solver state",                                                                                                                   "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_subopt_flag",                   "Dispatch suboptimal solution flag",                                                                                                       "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_solve_iter",                    "Dispatch iterations count",                                                                                                               "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_objective",                     "Dispatch objective function value",                                                                                                       "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_obj_relax",                     "Dispatch objective function - relaxed max",                                                                                               "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_qsf_expected",                  "Dispatch expected solar field available energy",                                                                                          "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_qsfprod_expected",              "Dispatch expected solar field generation",                                                                                                "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_qsfsu_expected",                "Dispatch expected solar field startup enegy",                                                                                             "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_tes_expected",                  "Dispatch expected TES charge level",                                                                                                      "MWht",         "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_pceff_expected",                "Dispatch expected power cycle efficiency adj.",                                                                                           "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_thermeff_expected",             "Dispatch expected SF thermal efficiency adj.",                                                                                            "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_qpbsu_expected",                "Dispatch expected power cycle startup energy",                                                                                            "MWht",         "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_wpb_expected",                  "Dispatch expected power generation",                                                                                                      "MWe",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_rev_expected",                  "Dispatch expected revenue factor",                                                                                                        "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_presolve_nconstr",              "Dispatch number of constraints in problem",                                                                                               "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_presolve_nvar",                 "Dispatch number of variables in problem",                                                                                                 "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "disp_solve_time",                    "Dispatch solver time",                                                                                                                    "sec",          "",                                  "",                                         "*",                                                                "",              ""},


        // These outputs correspond to the first csp-solver timestep in the reporting timestep.
        //     Subsequent csp-solver timesteps within the same reporting timestep are not tracked
    { SSC_OUTPUT,    SSC_ARRAY,  "q_dot_pc_sb",                        "Thermal power for PC standby",                                                                                                            "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "q_dot_pc_min",                       "Thermal power for PC min operation",                                                                                                      "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "q_dot_pc_max",                       "Max thermal power to PC",                                                                                                                 "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "q_dot_pc_target",                    "Target thermal power to PC",                                                                                                              "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "is_rec_su_allowed",                  "Is receiver startup allowed",                                                                                                             "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "is_pc_su_allowed",                   "Is power cycle startup allowed",                                                                                                          "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "is_pc_sb_allowed",                   "Is power cycle standby allowed",                                                                                                          "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "q_dot_est_cr_su",                    "Estimated receiver startup thermal power",                                                                                                "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "q_dot_est_cr_on",                    "Estimated receiver thermal power TO HTF",                                                                                                 "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "q_dot_est_tes_dc",                   "Estimated max TES discharge thermal power",                                                                                               "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "q_dot_est_tes_ch",                   "Estimated max TES charge thermal power",                                                                                                  "MWt",          "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "operating_modes_a",                  "First 3 operating modes tried",                                                                                                           "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "operating_modes_b",                  "Next 3 operating modes tried",                                                                                                            "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_ARRAY,  "operating_modes_c",                  "Final 3 operating modes tried",                                                                                                           "",             "",                                  "",                                         "*",                                                                "",              ""},

    { SSC_OUTPUT,    SSC_ARRAY,  "gen",                                "Total electric power to grid with available derate",                                                                                      "kWe",          "",                                  "",                                         "*",                                                                "",              ""},

    // Annual single-value outputs
    { SSC_OUTPUT,    SSC_NUMBER, "annual_energy",                      "Annual total electric power to grid",                                                                                                     "kWhe",         "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "annual_W_cycle_gross",               "Electrical source - power cycle gross output",                                                                                            "kWhe",         "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "annual_W_cooling_tower",             "Total of condenser operation parasitics",                                                                                                 "kWhe",         "",                                  "PC",                                       "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "annual_q_rec_inc",                   "Annual receiver incident thermal power after reflective losses",                                                                          "MWt-hr",       "",                                  "Tower and Receiver",                       "*",                                                                "",              "" },
    { SSC_OUTPUT,    SSC_NUMBER, "annual_q_rec_loss",                  "Annual receiver convective and radiative losses",                                                                                         "MWt-hr",       "",                                  "Tower and Receiver",                       "*",                                                                "",              "" },
    { SSC_OUTPUT,    SSC_NUMBER, "annual_eta_rec_th",                  "Annual receiver thermal efficiency ignoring rec reflective loss",                                                                         "",             "",                                  "Tower and Receiver",                       "*",                                                                "",              "" },
    { SSC_OUTPUT,    SSC_NUMBER, "annual_eta_rec_th_incl_refl",        "Annual receiver thermal efficiency including reflective loss",                                                                            "",             "",                                  "Tower and Receiver",                       "*",                                                                "",              "" },

    { SSC_OUTPUT,    SSC_NUMBER, "conversion_factor",                  "Gross to net conversion factor",                                                                                                          "%",            "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "capacity_factor",                    "Capacity factor",                                                                                                                         "%",            "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "kwh_per_kw",                         "First year kWh/kW",                                                                                                                       "kWh/kW",       "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "annual_total_water_use",             "Total annual water usage, cycle + mirror washing",                                                                                        "m3",           "",                                  "",                                         "*",                                                                "",              ""},

    { SSC_OUTPUT,    SSC_NUMBER, "disp_objective_ann",                 "Annual sum of dispatch objective function value",                                                                                         "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "disp_iter_ann",                      "Annual sum of dispatch solver iterations",                                                                                                "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "disp_presolve_nconstr_ann",          "Annual sum of dispatch problem constraint count",                                                                                         "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "disp_presolve_nvar_ann",             "Annual sum of dispatch problem variable count",                                                                                           "",             "",                                  "",                                         "*",                                                                "",              ""},
    { SSC_OUTPUT,    SSC_NUMBER, "disp_solve_time_ann",                "Annual sum of dispatch solver time",                                                                                                      "",             "",                                  "",                                         "*",                                                                "",              ""},

    { SSC_OUTPUT,    SSC_NUMBER, "sim_cpu_run_time",                   "Simulation duration clock time",                                                                                                         "s",             "",                                  "",                                         "*",                                                                "",              "" },

    var_info_invalid };

class cm_tcsmolten_salt : public compute_module
{
public:

    cm_tcsmolten_salt()
    {
        add_var_info(_cm_vtab_tcsmolten_salt);
        add_var_info(vtab_adjustment_factors);
        add_var_info(vtab_sf_adjustment_factors);
    } 

    bool relay_message(string &msg, double percent)
    {
        log(msg);
        return update(msg, (float)percent);
    }

	void exec() override
	{
        std::clock_t clock_start = std::clock();

        // *****************************************************
        // Check deprecated variables
        bool is_piping_loss_assigned = is_assigned("piping_loss");
        bool is_piping_loss_coefficient_assigned = is_assigned("piping_loss_coefficient");

        if (is_piping_loss_assigned && is_piping_loss_coefficient_assigned) {
            log("We replaced the functionality of input variable piping_loss with new input variable piping_loss_coefficient,"
                " so the model does not use your piping_loss input.");
        }
        else if (is_piping_loss_assigned) {
            throw exec_error("tcsmolten_salt", "We replaced the functionality of input variable piping_loss [Wt/m] with new input variable piping_loss_coefficient [Wt/m2-K]."
                " The new input scales piping thermal losses as a function of receiver thermal power and design-point temperatures."
                " Please define piping_loss_coefficient in your script.");
        }

        // *****************************************************
        // System Design Parameters
        double T_htf_cold_des = as_double("T_htf_cold_des");    //[C]
        double T_htf_hot_des = as_double("T_htf_hot_des");      //[C]
        double W_dot_cycle_des = as_double("P_ref");            //[MWe]
        double eta_cycle = as_double("design_eff");             //[-]
        double tshours = as_double("tshours");                  //[-]

        // System Design Calcs
        double q_dot_pc_des = W_dot_cycle_des / eta_cycle;      //[MWt]
        double Q_tes = q_dot_pc_des * tshours;                  //[MWt-hr]
        double q_dot_rec_des = q_dot_pc_des * as_number("solarm");  //[MWt]

        // Weather reader
		C_csp_weatherreader weather_reader;
		if (is_assigned("solar_resource_file")){
			weather_reader.m_weather_data_provider = make_shared<weatherfile>(as_string("solar_resource_file"));
			if (weather_reader.m_weather_data_provider->has_message()) log(weather_reader.m_weather_data_provider->message(), SSC_WARNING);
		}
		if (is_assigned("solar_resource_data")){
			weather_reader.m_weather_data_provider = make_shared<weatherdata>(lookup("solar_resource_data"));
			if (weather_reader.m_weather_data_provider->has_message()) log(weather_reader.m_weather_data_provider->message(), SSC_WARNING);
		}

        weather_reader.m_trackmode = 0;
        weather_reader.m_tilt = 0.0;
        weather_reader.m_azimuth = 0.0;
        // Initialize to get weather file info
        weather_reader.init();
        if (weather_reader.has_error()) throw exec_error("tcsmolten_salt", weather_reader.get_error());

        // Get info from the weather reader initialization
        double site_elevation = weather_reader.ms_solved_params.m_elev;     //[m]


        int tes_type = 1;

        assign("q_design", q_dot_rec_des);  //[MWt]

        // Set up "cmod_solarpilot.cpp" conversions as necessary
        assign("helio_optical_error", (ssc_number_t)(as_number("helio_optical_error_mrad")*1.E-3));

        // Set 'n_flux_x' and 'n_flux_y' here, for now
        assign("n_flux_y", 1);

        int rec_type = as_integer("receiver_type");
        if (rec_type == 0) {
            assign("rec_aspect", as_number("rec_height") / as_number("D_rec"));
            int n_ext_rec_panels = as_integer("N_panels");
            assign("n_flux_x", (ssc_number_t)max(12, n_ext_rec_panels));
        }
        else if (rec_type == 1) {
            assign("n_flux_x", 2);  // n_flux_x represents *per panel* the number subsurfaces in x direction 
        }
        else {
            throw exec_error("tcsmolten_salt", "receiver_type must be 1 (external) or 0 (cavity)");
        }

        // Calculate system capacity instead of pass in
        double system_capacity = as_double("P_ref") * as_double("gross_net_conversion_factor") *1.E3;       //[kWe]
        assign("system_capacity", system_capacity);     //[kWe]
        
        // 'sf_model_type'
        // 0 = design field and tower/receiver geometry
        // 1 = design field
        // 2 = user field, calculate performance
        // 3 = user performance maps vs solar position
        int field_model_type = as_integer("field_model_type");

        // Run solarpilot right away to update values as needed
        solarpilot_invoke spi(this);
        util::matrix_t<double> mt_eta_map;
        util::matrix_t<double> mt_flux_maps;

        assign("is_optimize", 0);
        bool is_optimize = false;

        if (field_model_type == 0 || field_model_type == 1) // Auto-design. Generate a new system (is_optimize = true) or field layout
        {
            if (field_model_type == 0)
            {
                assign("is_optimize", 1);
                is_optimize = true;
            }

            assign("calc_fluxmaps", 1);

            spi.run(weather_reader.m_weather_data_provider);

            if (is_optimize)
            {
                //Optimization iteration history
                vector<vector<double> > steps;
                vector<double> obj, flux;
                spi.getOptimizationSimulationHistory(steps, obj, flux);
                size_t nr = steps.size();
                if (nr > 0)
                {
                    size_t nc = steps.front().size() + 2;
                    ssc_number_t *ssc_hist = allocate("opt_history", nr, nc);
                    for (size_t i = 0; i<nr; i++){

                        for (size_t j = 0; j<steps.front().size(); j++)
                            ssc_hist[i*nc + j] = (ssc_number_t)steps.at(i).at(j);
                        ssc_hist[i*nc + nc - 2] = (ssc_number_t)obj.at(i);
                        ssc_hist[i*nc + nc - 1] = (ssc_number_t)flux.at(i);
                    }
                }
            }

            // receiver calculations
            double H_rec = spi.recs.front().rec_height.val;
            double THT = spi.sf.tht.val;

            int nr = (int)spi.layout.heliostat_positions.size();
            assign("N_hel", (ssc_number_t)nr);

            double A_sf = as_double("helio_height") * as_double("helio_width") * as_double("dens_mirror") * (double)nr;

            //update assignments for cost model
            if (rec_type == 0) {
                assign("rec_height", var_data((ssc_number_t)H_rec));
                double rec_aspect = spi.recs.front().rec_aspect.Val();
                assign("rec_aspect", var_data((ssc_number_t)rec_aspect));
                assign("D_rec", var_data((ssc_number_t)(H_rec / rec_aspect)));
            }
            else if (rec_type == 1) {

                // copied from cmod_solarpilot, would be nice to consolidate there
                double cav_rec_height_spout, cav_radius_spout, f_offset_spout;
                cav_rec_height_spout = cav_radius_spout = f_offset_spout = std::numeric_limits<double>::quiet_NaN();
                int n_panels_spout = -1;

                cav_rec_height_spout = spi.recs.front().rec_height.val;   //[m]
                cav_radius_spout = spi.recs.front().rec_cav_rad.val;      //[m]
                f_offset_spout = spi.recs.front().rec_cav_cdepth.val;       //[-]
                n_panels_spout = spi.recs.front().n_panels.val;           //[-]

                double theta0_calc, panelSpan_calc, panel_width_calc, rec_area_calc, rec_width_calc,
                    rec_span_calc, offset_calc;

                cavity_receiver_helpers::calc_receiver_macro_geometry_sp_inputs(cav_rec_height_spout, cav_radius_spout,
                    f_offset_spout, n_panels_spout,
                    theta0_calc, panelSpan_calc, panel_width_calc,
                    rec_area_calc, rec_width_calc, rec_span_calc, offset_calc);

                assign("cav_rec_width", (ssc_number_t)rec_width_calc);      //[m]
                assign("cav_rec_height", (ssc_number_t)H_rec);  //[m]
            }
            assign("h_tower", var_data((ssc_number_t)THT));
            assign("A_sf", var_data((ssc_number_t)A_sf));

            double land_area_base = spi.land.land_area.Val();       //[acres] Land area occupied by heliostats
            assign("land_area_base", (ssc_number_t)land_area_base);

            ssc_number_t *ssc_hl = allocate("helio_positions", nr, 2);
            for (int i = 0; i<nr; i++)
            {
                ssc_hl[i * 2] = (ssc_number_t)spi.layout.heliostat_positions.at(i).location.x;
                ssc_hl[i * 2 + 1] = (ssc_number_t)spi.layout.heliostat_positions.at(i).location.y;
            }

            //collect the optical efficiency data and sun positions
            if (spi.fluxtab.zeniths.size() > 0 && spi.fluxtab.azimuths.size() > 0
                && spi.fluxtab.efficiency.size() > 0)
            {
                size_t nvals = spi.fluxtab.efficiency.size();
                mt_eta_map.resize(nvals, 3);

                for (size_t i = 0; i<nvals; i++)
                {
                    mt_eta_map(i, 0) = spi.fluxtab.azimuths[i] * 180. / CSP::pi;      //Convention is usually S=0, E<0, W>0 
                    mt_eta_map(i, 1) = spi.fluxtab.zeniths[i] * 180. / CSP::pi;     //Provide zenith angle
                    mt_eta_map(i, 2) = spi.fluxtab.efficiency[i];
                }
            }
            else
                throw exec_error("solarpilot", "failed to calculate a correct optical efficiency table");

            //collect the flux map data
            block_t<double>* flux_data = &spi.fluxtab.flux_surfaces.front().flux_data;  //there should be only one flux stack for SAM
            if (flux_data->ncols() > 0 && flux_data->nlayers() > 0)
            {
                if (rec_type == 0) {

                    int nflux_y = (int)flux_data->nrows();
                    int nflux_x = (int)flux_data->ncols();

                    mt_flux_maps.resize(nflux_y * flux_data->nlayers(), nflux_x);

                    int cur_row = 0;

                    for (size_t i = 0; i < flux_data->nlayers(); i++)
                    {
                        for (int j = 0; j < nflux_y; j++)
                        {
                            for (int k = 0; k < nflux_x; k++)
                            {
                                mt_flux_maps(cur_row, k) = flux_data->at(j, k, i);
                                //fluxdata[cur_row * nflux_x + k] = (float)flux_data->at(j, k, i);
                            }
                            cur_row++;
                        }
                    }
                }
                else if (rec_type == 1) {

                    int nflux_y = (int)flux_data->nrows();
                    int nflux_x = (int)flux_data->ncols();

                    int n_panels_cav = as_integer("n_cav_rec_panels");  //[-]
                    int n_sp_surfaces = spi.fluxtab.flux_surfaces.size();
                    int n_panels_cav_sp = n_sp_surfaces - 1;

                    if (nflux_y > 1) {
                        throw exec_error("solarpilot", "cavity flux maps currently only work for nflux_y = 1");
                    }

                    mt_flux_maps.resize(nflux_y * flux_data->nlayers(), n_panels_cav_sp);

                    int cur_row = 0;

                    // nlayers is number of solar positions (i.e. flux maps)
                    for (size_t i = 0; i < flux_data->nlayers(); i++) {

                        int j = 0;

                        double flux_receiver = 0.0;

                        // Start at k=1 because the first surface in flux_surfaces is the aperture, which we don't want
                        for (int k = 1; k <= n_panels_cav_sp; k++) {

                            block_t<double>* flux_data = &spi.fluxtab.flux_surfaces[k].flux_data; //.front().flux_data;  //there should be only one flux stack for SAM

                            double flux_local = 0.0;
                            for (int l = 0; l < nflux_x; l++) {
                                //double flux_local0 = flux_data->at(j, 0, i);
                                //double flux_local1 = flux_data->at(j, 1, i);
                                //double flux_local2 = flux_data->at(j, 2, i);
                                //double flux_local3 = flux_data->at(j, 3, i);

                                flux_local += flux_data->at(j, l, i);
                            }

                            // Adjust k to start flux maps with first receiver surface
                            mt_flux_maps(cur_row, k - 1) = flux_local;
                            flux_receiver += flux_local;
                            double abc = 1.23;
                        }

                        cur_row++;
                    }
                }
            }
            else
                throw exec_error("solarpilot", "failed to calculate a correct flux map table");
        }
        else if (field_model_type == 2)
        {
            // only calculates a flux map, so need to "assign" 'helio_positions_in'
            util::matrix_t<double> helio_pos_temp = as_matrix("helio_positions");
            size_t n_h_rows = helio_pos_temp.nrows();
            ssc_number_t *p_helio_positions_in = allocate("helio_positions_in", n_h_rows, 2);

            // Try to determine whether heliostat positions represent surround or cavity field
            double y_h_min = 1.E5;
            double y_h_max = -1.E5;
            for (size_t i = 0; i < n_h_rows; i++)
            {
                p_helio_positions_in[i * 2] = (ssc_number_t)helio_pos_temp(i, 0);       //[m] x
                p_helio_positions_in[i * 2 + 1] = (ssc_number_t)helio_pos_temp(i, 1);   //[m] y

                y_h_min = min(y_h_min, p_helio_positions_in[i*2+1]);
                y_h_max = max(y_h_max, p_helio_positions_in[i*2+1]);
            }

            bool is_cavity_field = false;
            if ((y_h_max - y_h_min) / max(abs(y_h_max), abs(y_h_min)) < 1.25 ) {
                is_cavity_field = true;
            }

            // Check determined field type against user-specified receiver type
            if (is_cavity_field && rec_type == 0) {
                throw exec_error("mspt compute module", "\nExternal receiver specified, but cavity field detected. Try one of the following options:\n"
                    "1) Run field layout macro on Heliostat Field page\n"
                    "2) Select option for simulation to layout field and tower/receiver design\n"
                    "3) Enter new heliostat positions\n");
            }

            if (!is_cavity_field && rec_type == 1) {
                throw exec_error("mspt compute module", "\nCavity receiver specified, but surround field detected. Try one of the following options:\n"
                    "1) Run field layout macro on Heliostat Field page\n"
                    "2) Select option for simulation to layout field and tower/receiver design\n"
                    "3) Enter new heliostat positions\n");
            }

            assign("N_hel", (ssc_number_t)n_h_rows);
            // 'calc_fluxmaps' should be true
            assign("calc_fluxmaps", 1);

            spi.run(weather_reader.m_weather_data_provider);

            //collect the optical efficiency data and sun positions
            if (spi.fluxtab.zeniths.size() > 0 && spi.fluxtab.azimuths.size() > 0
                && spi.fluxtab.efficiency.size() > 0)
            {
                size_t nvals = spi.fluxtab.efficiency.size();
                mt_eta_map.resize(nvals, 3);

                for (size_t i = 0; i<nvals; i++)
                {
                    mt_eta_map(i, 0) = spi.fluxtab.azimuths[i] * 180. / CSP::pi;    //Convention is usually S=0, E<0, W>0 
                    mt_eta_map(i, 1) = spi.fluxtab.zeniths[i] * 180. / CSP::pi;     //Provide zenith angle
                    mt_eta_map(i, 2) = spi.fluxtab.efficiency[i];
                }
            }
            else
                throw exec_error("solarpilot", "failed to calculate a correct optical efficiency table");

            //collect the flux map data
            block_t<double> *flux_data = &spi.fluxtab.flux_surfaces.front().flux_data;  //there should be only one flux stack for SAM
            if (flux_data->ncols() > 0 && flux_data->nlayers() > 0)
            {
                if (rec_type == 0) {

                    int nflux_y = (int)flux_data->nrows();
                    int nflux_x = (int)flux_data->ncols();

                    mt_flux_maps.resize(nflux_y * flux_data->nlayers(), nflux_x);

                    int cur_row = 0;

                    for (size_t i = 0; i < flux_data->nlayers(); i++)
                    {
                        for (int j = 0; j < nflux_y; j++)
                        {
                            for (int k = 0; k < nflux_x; k++)
                            {
                                mt_flux_maps(cur_row, k) = flux_data->at(j, k, i);
                                //fluxdata[cur_row * nflux_x + k] = (float)flux_data->at(j, k, i);
                            }
                            cur_row++;
                        }
                    }
                }
                else if (rec_type == 1) {

                    int nflux_y = (int)flux_data->nrows();
                    int nflux_x = (int)flux_data->ncols();

                    int n_panels_cav = as_integer("n_cav_rec_panels"); //[-]
                    int n_sp_surfaces = spi.fluxtab.flux_surfaces.size();
                    int n_panels_cav_sp = n_sp_surfaces - 1;

                    if (nflux_y > 1) {
                        throw exec_error("solarpilot", "cavity flux maps currently only work for nflux_y = 1");
                    }

                    mt_flux_maps.resize(nflux_y* flux_data->nlayers(), n_panels_cav_sp);

                    int cur_row = 0;

                    // nlayers is number of solar positions (i.e. flux maps)
                    for (size_t i = 0; i < flux_data->nlayers(); i++) {

                        int j = 0;

                        double flux_receiver = 0.0;

                        // Start at k=1 because the first surface in flux_surfaces is the aperture, which we don't want
                        for (int k = 1; k <= n_panels_cav_sp; k++) {

                            block_t<double>* flux_data = &spi.fluxtab.flux_surfaces[k].flux_data; //.front().flux_data;  //there should be only one flux stack for SAM

                            double flux_local = 0.0;
                            for (int l = 0; l < nflux_x; l++) {
                                flux_local += flux_data->at(j, l, i);
                            }

                            // Adjust k to start flux maps with first receiver surface
                            mt_flux_maps(cur_row, k-1) = flux_local;
                            flux_receiver += flux_local;
                        }
                        // flux_receiver should equal 1 after each panel is added

                        cur_row++;
                    }
                }
            }
            else
                throw exec_error("solarpilot", "failed to calculate a correct flux map table");

            int nr = as_integer("N_hel");
            double A_sf = as_double("helio_height") * as_double("helio_width") * as_double("dens_mirror") * (double)nr;
            assign("A_sf", (ssc_number_t)A_sf);
        }
        else if (field_model_type == 3)
        {
            assign("calc_fluxmaps", 0);

            // The following optional inputs must be set here:
            assign("A_sf", as_number("A_sf_in"));
        }
        else
        {
            string msg = util::format("One field performance modeling option must be set to True");

            throw exec_error("MSPT CSP Solver", msg);
        }

        if( tes_type != 1 )
        {
            throw exec_error("MSPT CSP Solver", "Thermocline thermal energy storage is not yet supported by the new CSP Solver and Dispatch Optimization models.\n");
        }

        
        // Set steps per hour
        C_csp_solver::S_sim_setup sim_setup;
        sim_setup.m_sim_time_start = as_double("time_start");       //[s] time at beginning of first time step
        sim_setup.m_sim_time_end = as_double("time_stop");          //[s] time at end of last time step
        
        int steps_per_hour = (int)as_double("time_steps_per_hour");     //[-]

        //if the number of steps per hour is not provided (=-1), then assign it based on the weather file step
        if( steps_per_hour < 0 )
        {
            double sph_d = 3600. / weather_reader.m_weather_data_provider->step_sec();
            steps_per_hour = (int)( sph_d + 1.e-5 );
            if( (double)steps_per_hour != sph_d )
                throw spexception("The time step duration must be evenly divisible within an hour.");
        }

        size_t n_steps_fixed = (size_t)steps_per_hour * 8760;   //[-]
        if( as_boolean("vacuum_arrays") )
        {
            n_steps_fixed = steps_per_hour * (size_t)( (sim_setup.m_sim_time_end - sim_setup.m_sim_time_start)/3600. );
        }
        //int n_steps_fixed = (int)( (sim_setup.m_sim_time_end - sim_setup.m_sim_time_start) * steps_per_hour / 3600. ) ; 
        sim_setup.m_report_step = 3600.0 / (double)steps_per_hour;  //[s]

        // ***********************************************
        // ***********************************************
        // Power cycle
        // ***********************************************
        // ***********************************************
        C_csp_power_cycle * p_csp_power_cycle;
        // Steam Rankine and User Defined power cycle classes
        C_pc_Rankine_indirect_224 rankine_pc;

        // Check power block type
        int pb_tech_type = as_integer("pc_config");
        if (pb_tech_type == 0 || pb_tech_type == 1)     // Rankine or User Defined
        {
            C_pc_Rankine_indirect_224::S_params *pc = &rankine_pc.ms_params;
            pc->m_P_ref = as_double("P_ref");
            pc->m_eta_ref = as_double("design_eff");
            pc->m_T_htf_hot_ref = as_double("T_htf_hot_des");
            pc->m_T_htf_cold_ref = as_double("T_htf_cold_des");
            pc->m_cycle_max_frac = as_double("cycle_max_frac");
            pc->m_cycle_cutoff_frac = as_double("cycle_cutoff_frac");
            pc->m_q_sby_frac = as_double("q_sby_frac");
            pc->m_startup_time = as_double("startup_time");
            pc->m_startup_frac = as_double("startup_frac");
            pc->m_htf_pump_coef = as_double("pb_pump_coef");
            pc->m_pc_fl = as_integer("rec_htf");                            // power cycle HTF is same as receiver HTF
            pc->m_pc_fl_props = as_matrix("field_fl_props");

            if (pb_tech_type == 0)
            {
                pc->m_dT_cw_ref = as_double("dT_cw_ref");
                pc->m_T_amb_des = as_double("T_amb_des");
                pc->m_P_boil = as_double("P_boil");
                pc->m_CT = as_integer("CT");                    // cooling tech type: 1=evaporative, 2=air, 3=hybrid    , 5= custom for rad cool, 6= custom for rad cool
                pc->m_tech_type = as_integer("tech_type");      // 1: Fixed, 3: Sliding
                if (pc->m_tech_type == 2) { pc->m_tech_type = 1; }; // changing fixed pressure for the trough to fixed pressure for the tower
                //if (pc->m_tech_type == 8) { pc->m_tech_type = 3; }; // changing sliding pressure for the trough to sliding pressure for the tower  ->  don't, this disallows the use of the old tower sliding curves
                
                if (!(pc->m_tech_type == 1 || pc->m_tech_type == 3 || pc->m_tech_type ==5 || pc->m_tech_type==6 || pc->m_tech_type == 7 || pc->m_tech_type == 8))
                {
                    std::string tech_msg = util::format("tech_type must be either 1 (fixed pressure) or 3 (sliding). Input was %d."
                        " Simulation proceeded with fixed pressure", pc->m_tech_type);
                    pc->m_tech_type = 1;
                }
                pc->m_T_approach = as_double("T_approach");
                pc->m_T_ITD_des = as_double("T_ITD_des");
                pc->m_P_cond_ratio = as_double("P_cond_ratio");
                pc->m_pb_bd_frac = as_double("pb_bd_frac");
                pc->m_P_cond_min = as_double("P_cond_min");
                pc->m_n_pl_inc = as_integer("n_pl_inc");

                //parameters for radiative cooling with cold storage
                C_csp_cold_tes *two_tank = &rankine_pc.mc_two_tank_ctes;    //pointer for two tank
                C_csp_stratified_tes *stratified = &rankine_pc.mc_stratified_ctes; //pointer for stratified
            
                two_tank->ms_params.m_ctes_type = as_integer("ctes_type");
                stratified->ms_params.m_ctes_type = as_integer("ctes_type");

                if (rankine_pc.ms_params.m_CT == 4)
                {
                    if (two_tank->ms_params.m_ctes_type == 2)
                    {
                        two_tank->ms_params.m_h_tank_min = as_double("h_ctes_tank_min");
                        two_tank->ms_params.m_ts_hours = as_double("ctes_tshours");
                        two_tank->ms_params.m_h_tank = as_double("h_ctes_tank");
                        two_tank->ms_params.m_u_tank = as_double("u_ctes_tank");
                        two_tank->ms_params.m_tank_pairs = as_integer("ctes_tankpairs");
                        two_tank->ms_params.m_T_field_in_des = as_double("T_ctes_cold_design");
                        two_tank->ms_params.m_T_field_out_des = as_double("T_ctes_warm_design");
                        two_tank->ms_params.m_T_tank_hot_ini = as_double("T_ctes_warm_ini");
                        two_tank->ms_params.m_T_tank_cold_ini = as_double("T_ctes_cold_ini");
                        two_tank->ms_params.m_f_V_hot_ini = as_double("f_ctes_warm_ini");
                        two_tank->ms_params.m_lat = weather_reader.ms_solved_params.m_lat;
                    }
                    if (two_tank->ms_params.m_ctes_type > 2)
                    {
                        stratified->ms_params.m_h_tank_min = 0;                             //hardcode zero minimum height for stratified tanks.
                        stratified->ms_params.m_ts_hours = as_double("ctes_tshours");
                        stratified->ms_params.m_h_tank = as_double("h_ctes_tank");
                        stratified->ms_params.m_u_tank = as_double("u_ctes_tank");
                        stratified->ms_params.m_tank_pairs = as_integer("ctes_tankpairs");
                        stratified->ms_params.m_T_field_in_des = as_double("T_ctes_cold_design");
                        stratified->ms_params.m_T_field_out_des = as_double("T_ctes_warm_design");
                        stratified->ms_params.m_T_tank_hot_ini = as_double("T_ctes_warm_ini");
                        stratified->ms_params.m_T_tank_cold_ini = as_double("T_ctes_cold_ini");
                        stratified->ms_params.m_f_V_hot_ini = as_double("f_ctes_warm_ini");
                        stratified->ms_params.m_lat = weather_reader.ms_solved_params.m_lat;

                    }
                    rankine_pc.mc_radiator.ms_params.m_field_fl = as_integer("ctes_field_fl");
                    rankine_pc.mc_radiator.ms_params.RM = as_double("rad_multiplier");
                    rankine_pc.mc_radiator.ms_params.Asolar_refl = as_double("helio_area_tot");
                    rankine_pc.mc_radiator.ms_params.m_dot_panel = as_double("m_dot_radpanel");
                    rankine_pc.mc_radiator.ms_params.n = as_integer("n_rad_tubes");
                    rankine_pc.mc_radiator.ms_params.W = as_double("W_rad_tubes");
                    rankine_pc.mc_radiator.ms_params.L = as_double("L_rad");
                    rankine_pc.mc_radiator.ms_params.th = as_double("th_rad_panel");
                    rankine_pc.mc_radiator.ms_params.D = as_double("D_rad_tubes");
                    rankine_pc.mc_radiator.ms_params.k_panel = as_double("k_panel");
                    rankine_pc.mc_radiator.ms_params.epsilon = as_double("epsilon_radtop");
                    rankine_pc.mc_radiator.ms_params.epsilonb = as_double("epsilon_radbot");
                    rankine_pc.mc_radiator.ms_params.epsilong = as_double("epsilon_radgrnd");
                    rankine_pc.mc_radiator.ms_params.Lsec = as_double("L_rad_sections");
                    rankine_pc.mc_radiator.ms_params.epsilon_HX = as_double("epsilon_radHX");
                    rankine_pc.mc_radiator.ms_params.radfield_dp = as_double("rad_pressuredrop");
                }

                size_t n_F_wc = 0;
                ssc_number_t *p_F_wc = as_array("F_wc", &n_F_wc);
                pc->m_F_wc.resize(n_F_wc, 0.0);
                for (size_t i = 0; i < n_F_wc; i++)
                    pc->m_F_wc[i] = (double)p_F_wc[i];

                // Set User Defined cycle parameters to appropriate values
                pc->m_is_user_defined_pc = false;
                pc->m_W_dot_cooling_des = std::numeric_limits<double>::quiet_NaN();
            }
            else if (pb_tech_type == 1)
            {
                pc->m_is_user_defined_pc = true;

                // User-Defined Cycle Parameters
                pc->m_W_dot_cooling_des = as_double("ud_f_W_dot_cool_des") / 100.0*as_double("P_ref");  //[MWe]
                pc->m_m_dot_water_des = as_double("ud_m_dot_water_cool_des");       //[kg/s]

                // User-Defined Cycle Off-Design Tables 
                pc->mc_combined_ind = as_matrix("ud_ind_od");
            }

            // Set pointer to parent class
            p_csp_power_cycle = &rankine_pc;
        }
        else
        {
            std::string err_msg = util::format("The specified power cycle configuration, %d, does not exist. See SSC Input Table for help.\n", pb_tech_type);
            log(err_msg, SSC_WARNING);
            return;
        }

        // Set power cycle outputs common to all power cycle technologies
        p_csp_power_cycle->assign(C_pc_Rankine_indirect_224::E_Q_DOT_HTF, allocate("q_pb", n_steps_fixed), n_steps_fixed);
        p_csp_power_cycle->assign(C_pc_Rankine_indirect_224::E_M_DOT_HTF, allocate("m_dot_pc", n_steps_fixed), n_steps_fixed);
        p_csp_power_cycle->assign(C_pc_Rankine_indirect_224::E_Q_DOT_STARTUP, allocate("q_dot_pc_startup", n_steps_fixed), n_steps_fixed);
        p_csp_power_cycle->assign(C_pc_Rankine_indirect_224::E_W_DOT, allocate("P_cycle", n_steps_fixed), n_steps_fixed);
        p_csp_power_cycle->assign(C_pc_Rankine_indirect_224::E_T_HTF_IN, allocate("T_pc_in", n_steps_fixed), n_steps_fixed);
        p_csp_power_cycle->assign(C_pc_Rankine_indirect_224::E_T_HTF_OUT, allocate("T_pc_out", n_steps_fixed), n_steps_fixed);
        p_csp_power_cycle->assign(C_pc_Rankine_indirect_224::E_M_DOT_WATER, allocate("m_dot_water_pc", n_steps_fixed), n_steps_fixed);
        p_csp_power_cycle->assign(C_pc_Rankine_indirect_224::E_T_COND_OUT, allocate("T_cond_out", n_steps_fixed), n_steps_fixed);

        if (pb_tech_type == 0) {
            if (rankine_pc.ms_params.m_CT == 4) {
                p_csp_power_cycle->assign(C_pc_Rankine_indirect_224::E_T_COLD, allocate("T_cold", n_steps_fixed), n_steps_fixed);
                p_csp_power_cycle->assign(C_pc_Rankine_indirect_224::E_M_COLD, allocate("m_cold", n_steps_fixed), n_steps_fixed);
                p_csp_power_cycle->assign(C_pc_Rankine_indirect_224::E_M_WARM, allocate("m_warm", n_steps_fixed), n_steps_fixed);
                p_csp_power_cycle->assign(C_pc_Rankine_indirect_224::E_T_WARM, allocate("T_warm", n_steps_fixed), n_steps_fixed);
                p_csp_power_cycle->assign(C_pc_Rankine_indirect_224::E_T_RADOUT, allocate("T_rad_out", n_steps_fixed), n_steps_fixed);
                p_csp_power_cycle->assign(C_pc_Rankine_indirect_224::E_P_COND, allocate("P_cond", n_steps_fixed), n_steps_fixed);
                p_csp_power_cycle->assign(C_pc_Rankine_indirect_224::E_RADCOOL_CNTRL, allocate("radcool_control", n_steps_fixed), n_steps_fixed);
            }
        }

        //heliostat field class
        C_pt_sf_perf_interp heliostatfield;

        heliostatfield.ms_params.m_p_start = as_double("p_start");      //[kWe-hr] Heliostat startup energy
        heliostatfield.ms_params.m_p_track = as_double("p_track");      //[kWe] Heliostat tracking power
        heliostatfield.ms_params.m_hel_stow_deploy = as_double("hel_stow_deploy");  // N/A
        heliostatfield.ms_params.m_v_wind_max = as_double("v_wind_max");            // N/A
        if (rec_type == 0) {
            heliostatfield.ms_params.m_n_flux_x = (int)as_double("n_flux_x");      // sp match
        }
        else if (rec_type == 1) {
            heliostatfield.ms_params.m_n_flux_x = mt_flux_maps.ncols();     // for multi-surface cav receiver, these values need to match
        }
        heliostatfield.ms_params.m_n_flux_y = (int) as_double("n_flux_y");      // sp match

        if (field_model_type != 3)
        {
            heliostatfield.ms_params.m_eta_map = mt_eta_map;
            heliostatfield.ms_params.m_eta_map_aod_format = false;
            heliostatfield.ms_params.m_flux_maps = mt_flux_maps;
            heliostatfield.ms_params.m_N_hel = as_integer("N_hel");
            heliostatfield.ms_params.m_A_sf = as_double("A_sf");        //[m2]
        }
        else
        {
            heliostatfield.ms_params.m_eta_map = as_matrix("eta_map");
            heliostatfield.ms_params.m_eta_map_aod_format = as_boolean("eta_map_aod_format");
            heliostatfield.ms_params.m_flux_maps = as_matrix("flux_maps");
            heliostatfield.ms_params.m_N_hel = as_integer("N_hel");
            heliostatfield.ms_params.m_A_sf = as_double("A_sf");        //[m2]
        }



        //Load the solar field adjustment factors
        sf_adjustment_factors sf_haf(this);
        size_t n_steps_full = weather_reader.m_weather_data_provider->nrecords(); //steps_per_hour * 8760;
        if (!sf_haf.setup((int)n_steps_full))
            throw exec_error("tcsmolten_salt", "failed to setup sf adjustment factors: " + sf_haf.error());
        //allocate array to pass to tcs
        heliostatfield.ms_params.m_sf_adjust.resize( sf_haf.size() );
        for( int i=0; i<sf_haf.size(); i++)     
            heliostatfield.ms_params.m_sf_adjust.at(i) = sf_haf(i);

        // Set callback information
        heliostatfield.mf_callback = ssc_cmod_solarpilot_callback;
        heliostatfield.m_cdata = (void*)this;

        // Try running pt heliostat init() call just for funsies
            // What happens when no callback to reference?
        //heliostatfield.init();


        //// *********************************************************
        //// *********************************************************
        //// *********************************************************
        ////      Now set Type 222 parameters
        //// *********************************************************
        //// *********************************************************
        //// *********************************************************

        double H_rec = std::numeric_limits<double>::quiet_NaN();
        double D_rec = std::numeric_limits<double>::quiet_NaN();
        double A_rec = std::numeric_limits<double>::quiet_NaN();

        // Calculate external receiver area, height, diameter here
        // Calculate cavity receiver area and height below. Don't set diameter or aspect ratio for cavity receiver
        if(rec_type == 0){
            H_rec = as_double("rec_height");
            double rec_aspect = as_double("rec_aspect");
            D_rec = H_rec / rec_aspect;
            A_rec = H_rec * D_rec * 3.1415926;
        }

        std::unique_ptr<C_pt_receiver> receiver;
        bool is_rec_model_trans = as_boolean("is_rec_model_trans");

        if (rec_type == 1) {    // Cavity receiver

            // No transient model available for cavity receiver
            is_rec_model_trans = false;

            double hel_stow_deploy = as_double("hel_stow_deploy");          //[deg]

            double od_rec_tube = as_double("d_tube_out")*1.E-3;         //[m] convert from cmod units of mm
            double th_rec_tube = as_double("th_tube") * 1.E-3;          //[m] convert from cmod units of mm
            double receiverHeight = as_double("cav_rec_height");        //[m]
            double receiverWidth = as_double("cav_rec_width");          //[m]
            double rec_span = as_double("cav_rec_span")*CSP::pi/180.0;  //[rad] convert from cmod units of deg

            // lips must be 0 for now due to solarpilot limitations
            size_t nPanels = as_integer("n_cav_rec_panels");    //[-]
            double topLipHeight = 0;        //[m] Height of top lip in meters
            double botLipHeight = 0;        //[m] Height of bottom lip in meters

            // surface radiative properties
            double e_act_sol = as_double("rec_absorptance");        //[-] Absorptivity in short wave - active surfaces
            double e_act_therm = as_double("epsilon");              //[-] Emissivity in long wave - active surfaces
            double e_pass_sol = as_double("cav_rec_passive_abs");   //[-] Absorptivity in short wave (solar) - passive surfaces
            double e_pass_therm = as_double("cav_rec_passive_eps"); //[-] Emissivity in long wave - passive surfaces

            C_cavity_receiver::E_mesh_types active_surface_mesh_type = C_cavity_receiver::E_mesh_types::no_mesh;    // quad;   // no_mesh;    // quad;
            C_cavity_receiver::E_mesh_types floor_and_cover_mesh_type = C_cavity_receiver::E_mesh_types::no_mesh;
            C_cavity_receiver::E_mesh_types lips_mesh_type = C_cavity_receiver::E_mesh_types::no_mesh;  // quad;

            // Default values to test against matlab code
            // *************************************************************************************
            if (false) {
                // Method sets up, initializes, and runs the steady state model w/ inputs corresponding to matlab code
                cavity_receiver_helpers::test_cavity_case();
            }
            // ***************************************************************************************
            // ***************************************************************************************

            std::unique_ptr<C_cavity_receiver> c_cav_rec = std::unique_ptr<C_cavity_receiver>(new C_cavity_receiver(as_double("dni_des"), hel_stow_deploy,
                as_integer("rec_htf"), as_matrix("field_fl_props"),
                od_rec_tube, th_rec_tube, as_integer("mat_tube"),
                nPanels, receiverHeight, receiverWidth,
                rec_span, topLipHeight, botLipHeight,
                e_act_sol, e_pass_sol, e_act_therm, e_pass_therm,
                active_surface_mesh_type, floor_and_cover_mesh_type, lips_mesh_type,
                as_double("piping_loss_coefficient"), as_double("piping_length_const"), as_double("piping_length_mult"),
                as_double("A_sf"), as_double("h_tower"), as_double("T_htf_hot_des"),
                as_double("T_htf_cold_des"), as_double("f_rec_min"), q_dot_rec_des,
                as_double("rec_su_delay"), as_double("rec_qf_delay"), as_double("csp.pt.rec.max_oper_frac"),
                as_double("eta_pump") ));

            receiver = std::move(c_cav_rec);

            // Calculate receiver area
            double theta0, panelspan, panelwidth, radius, offset;
            theta0 = panelspan = panelwidth = radius = offset = A_rec = std::numeric_limits<double>::quiet_NaN();
            cavity_receiver_helpers::calc_receiver_macro_geometry(receiverHeight, receiverWidth,
                                            rec_span, nPanels,
                                            theta0, panelspan, panelwidth, A_rec, radius, offset );

            H_rec = receiverHeight;     //[m]
        }
        else if (rec_type == 0){
            if (!as_boolean("is_rec_model_trans") && !as_boolean("is_rec_startup_trans")) {
                //std::unique_ptr<C_mspt_receiver_222> ss_receiver = std::make_unique<C_mspt_receiver_222>();   // new to C++14
                std::unique_ptr<C_mspt_receiver_222> ss_receiver = std::unique_ptr<C_mspt_receiver_222>(new C_mspt_receiver_222());   // steady-state receiver

                ss_receiver->m_n_panels = as_integer("N_panels");
                ss_receiver->m_d_rec = D_rec;
                ss_receiver->m_h_rec = H_rec;
                ss_receiver->m_od_tube = as_double("d_tube_out");
                ss_receiver->m_th_tube = as_double("th_tube");
                ss_receiver->m_mat_tube = as_integer("mat_tube");
                ss_receiver->m_field_fl = as_integer("rec_htf");
                ss_receiver->m_field_fl_props = as_matrix("field_fl_props");
                ss_receiver->m_flow_type = as_integer("Flow_type");
                ss_receiver->m_crossover_shift = as_integer("crossover_shift");
                ss_receiver->m_hl_ffact = as_double("hl_ffact");
                ss_receiver->m_A_sf = as_double("A_sf");
                ss_receiver->m_piping_loss_coefficient = as_double("piping_loss_coefficient");
                ss_receiver->m_pipe_length_add = as_double("piping_length_const");  //[m]
                ss_receiver->m_pipe_length_mult = as_double("piping_length_mult");      //[-]
                ss_receiver->m_n_flux_x = as_integer("n_flux_x");
                ss_receiver->m_n_flux_y = as_integer("n_flux_y");
                ss_receiver->m_T_salt_hot_target = as_double("T_htf_hot_des");
                ss_receiver->m_hel_stow_deploy = as_double("hel_stow_deploy");
                ss_receiver->m_is_iscc = false;    // Set parameters that were set with TCS defaults
                ss_receiver->m_csky_frac = as_double("rec_clearsky_fraction");

                receiver = std::move(ss_receiver);
            }
            else {
                //std::unique_ptr<C_mspt_receiver> trans_receiver = std::make_unique<C_mspt_receiver>();    // new to C++14
                std::unique_ptr<C_mspt_receiver> trans_receiver = std::unique_ptr<C_mspt_receiver>(new C_mspt_receiver());    // transient receiver

                trans_receiver->m_n_panels = as_integer("N_panels");
                trans_receiver->m_d_rec = D_rec;
                trans_receiver->m_h_rec = H_rec;
                trans_receiver->m_od_tube = as_double("d_tube_out");
                trans_receiver->m_th_tube = as_double("th_tube");
                trans_receiver->m_mat_tube = as_integer("mat_tube");
                trans_receiver->m_field_fl = as_integer("rec_htf");
                trans_receiver->m_field_fl_props = as_matrix("field_fl_props");
                trans_receiver->m_flow_type = as_integer("Flow_type");
                trans_receiver->m_crossover_shift = as_integer("crossover_shift");
                trans_receiver->m_hl_ffact = as_double("hl_ffact");
                trans_receiver->m_A_sf = as_double("A_sf");
                trans_receiver->m_piping_loss_coeff = as_double("piping_loss_coefficient");                       //[Wt/m]
                trans_receiver->m_pipe_length_add = as_double("piping_length_const");   //[m]
                trans_receiver->m_pipe_length_mult = as_double("piping_length_mult");       //[-]
                trans_receiver->m_n_flux_x = as_integer("n_flux_x");
                trans_receiver->m_n_flux_y = as_integer("n_flux_y");
                trans_receiver->m_T_salt_hot_target = as_double("T_htf_hot_des");
                trans_receiver->m_hel_stow_deploy = as_double("hel_stow_deploy");
                trans_receiver->m_is_iscc = false;    // Set parameters that were set with TCS defaults
                trans_receiver->m_csky_frac = as_double("rec_clearsky_fraction");


                // Inputs for transient receiver model
                trans_receiver->m_is_transient = as_boolean("is_rec_model_trans");
                trans_receiver->m_is_startup_transient = as_boolean("is_rec_startup_trans");
                trans_receiver->m_u_riser = as_double("u_riser");                       //[m/s]
                trans_receiver->m_th_riser = as_double("th_riser");                 //[mm]
                trans_receiver->m_rec_tm_mult = as_double("rec_tm_mult");
                trans_receiver->m_riser_tm_mult = as_double("riser_tm_mult");
                trans_receiver->m_downc_tm_mult = as_double("downc_tm_mult");
                trans_receiver->m_heat_trace_power = as_double("heat_trace_power");		//[kW/m]
                trans_receiver->m_tube_flux_preheat = as_double("preheat_flux");        //[kW/m2]
                trans_receiver->m_min_preheat_time = as_double("min_preheat_time");		//[hr]
                trans_receiver->m_fill_time = as_double("min_fill_time");				//[hr]
                trans_receiver->m_flux_ramp_time = as_double("startup_ramp_time");      //[hr]
                trans_receiver->m_preheat_target = as_double("T_htf_cold_des");
                trans_receiver->m_startup_target_delta = min(0.0, as_double("startup_target_Tdiff"));
                trans_receiver->m_initial_temperature = 5.0; //[C]

                trans_receiver->m_is_startup_from_solved_profile = as_boolean("is_rec_startup_from_T_soln");
                if (!trans_receiver->m_is_startup_transient && trans_receiver->m_is_startup_from_solved_profile)
                    throw exec_error("tcsmolten_salt", "Receiver startup from solved temperature profiles is only available when receiver transient startup model is enabled");

                trans_receiver->m_is_enforce_min_startup = as_boolean("is_rec_enforce_min_startup");
                if (as_boolean("is_rec_startup_trans") && !trans_receiver->m_is_startup_from_solved_profile && !trans_receiver->m_is_enforce_min_startup)
                {
                    log("Both 'is_rec_enforce_min_startup' and 'is_rec_startup_from_T_soln' were set to 'false'. Minimum startup time will always be enforced unless 'is_rec_startup_from_T_soln' is set to 'true'", SSC_WARNING);
                    trans_receiver->m_is_enforce_min_startup = 1;
                }

                receiver = std::move(trans_receiver);
            }

            // Parent class member data
            receiver->m_h_tower = as_double("h_tower");                 //[m]
            receiver->m_epsilon = as_double("epsilon");                 //[-]
            receiver->m_T_htf_hot_des = as_double("T_htf_hot_des");     //[C]
            receiver->m_T_htf_cold_des = as_double("T_htf_cold_des");   //[C]
            receiver->m_f_rec_min = as_double("f_rec_min");             //[-]
            receiver->m_q_rec_des = q_dot_rec_des;                      //[MWt] 
            receiver->m_rec_su_delay = as_double("rec_su_delay");
            receiver->m_rec_qf_delay = as_double("rec_qf_delay");
            receiver->m_m_dot_htf_max_frac = as_double("csp.pt.rec.max_oper_frac");
            receiver->m_eta_pump = as_double("eta_pump");
            receiver->m_night_recirc = 0;

            receiver->m_clearsky_model = as_integer("rec_clearsky_model");
            if (receiver->m_clearsky_model > 4)
                throw exec_error("tcsmolten_salt", "Invalid specification for 'rec_clearsky_model'");
            if (receiver->m_clearsky_model == -1 && as_double("rec_clearsky_fraction") >= 0.0001)
                throw exec_error("tcsmolten_salt", "'rec_clearsky_model' must be specified when 'rec_clearsky_fraction' > 0.0.");

            if (receiver->m_clearsky_model == 0)
            {
                size_t n_csky = 0;
                ssc_number_t* csky = as_array("rec_clearsky_dni", &n_csky);
                if (n_csky != n_steps_full)
                    throw exec_error("tcsmolten_salt", "Invalid clear-sky DNI data. Array must have " + util::to_string((int)n_steps_full) + " rows.");

                receiver->m_clearsky_data.resize(n_steps_full);
                for (size_t i = 0; i < n_steps_full; i++)
                    receiver->m_clearsky_data.at(i) = (double)csky[i];
            }
        }        

        // Test mspt_receiver initialization
        //receiver.init();

        // Now try to instantiate mspt_collector_receiver
        C_csp_mspt_collector_receiver collector_receiver(heliostatfield, *receiver);
        // Then try init() call here, which should call inits from both classes
        //collector_receiver.init();

        // *******************************************************
        // *******************************************************
        // Set receiver outputs
        //float *p_q_thermal_copy = allocate("Q_thermal_123", n_steps_fixed);
        collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_FIELD_Q_DOT_INC, allocate("q_sf_inc", n_steps_fixed), n_steps_fixed);
        collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_FIELD_ETA_OPT, allocate("eta_field", n_steps_fixed), n_steps_fixed);
        collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_FIELD_ADJUST, allocate("sf_adjust_out", n_steps_fixed), n_steps_fixed);

        collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_Q_DOT_INC, allocate("q_dot_rec_inc", n_steps_fixed), n_steps_fixed);
        collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_ETA_THERMAL, allocate("eta_therm", n_steps_fixed), n_steps_fixed);
        collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_Q_DOT_THERMAL, allocate("Q_thermal", n_steps_fixed), n_steps_fixed);
        collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_M_DOT_HTF, allocate("m_dot_rec", n_steps_fixed), n_steps_fixed);
        collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_Q_DOT_STARTUP, allocate("q_startup", n_steps_fixed), n_steps_fixed);
        collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_T_HTF_IN, allocate("T_rec_in", n_steps_fixed), n_steps_fixed);
        collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_T_HTF_OUT, allocate("T_rec_out", n_steps_fixed), n_steps_fixed);
        collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_Q_DOT_PIPE_LOSS, allocate("q_piping_losses", n_steps_fixed), n_steps_fixed);
        collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_Q_DOT_LOSS, allocate("q_thermal_loss", n_steps_fixed), n_steps_fixed);
            // Cavity-specific outputs
        if (rec_type == 1) {
            collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_Q_DOT_REFL_LOSS, allocate("q_dot_reflection_loss", n_steps_fixed), n_steps_fixed);
        }

            // Transient model specific outputs
        if (is_rec_model_trans) {
            collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_P_HEATTRACE, allocate("P_rec_heattrace", n_steps_fixed), n_steps_fixed);
            collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_T_HTF_OUT_END, allocate("T_rec_out_end", n_steps_fixed), n_steps_fixed);
            collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_T_HTF_OUT_MAX, allocate("T_rec_out_max", n_steps_fixed), n_steps_fixed);
            collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_T_HTF_PANEL_OUT_MAX, allocate("T_panel_out_max", n_steps_fixed), n_steps_fixed);

            collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_T_WALL_INLET, allocate("T_wall_rec_inlet", n_steps_fixed), n_steps_fixed);
            collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_T_WALL_OUTLET, allocate("T_wall_rec_outlet", n_steps_fixed), n_steps_fixed);
            collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_T_RISER, allocate("T_wall_riser", n_steps_fixed), n_steps_fixed);
            collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_T_DOWNC, allocate("T_wall_downcomer", n_steps_fixed), n_steps_fixed);

            collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_CLEARSKY, allocate("clearsky", n_steps_fixed), n_steps_fixed);
            collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_Q_DOT_THERMAL_CSKY_SS, allocate("Q_thermal_ss_csky", n_steps_fixed), n_steps_fixed);
            collector_receiver.mc_reported_outputs.assign(C_csp_mspt_collector_receiver::E_Q_DOT_THERMAL_SS, allocate("Q_thermal_ss", n_steps_fixed), n_steps_fixed);
        }

        // Check if system configuration includes a heater parallel to primary collector receiver
        C_csp_collector_receiver* p_heater;
        C_csp_cr_electric_resistance* p_electric_resistance = NULL;
        bool is_parallel_heater = as_boolean("is_parallel_htr");    // defaults to false
        if (is_parallel_heater) {

            double heater_mult = as_double("heater_mult");          //[-]
            double q_dot_heater_des = q_dot_pc_des*heater_mult;     //[MWt]
            //double q_dot_heater_des = receiver->m_q_rec_des * 2.0;  // / 4.0;      //[MWt]

            double f_q_dot_des_allowable_su = as_double("f_q_dot_des_allowable_su");    //[-] fraction of design power allowed during startup
            double hrs_startup_at_max_rate = as_double("hrs_startup_at_max_rate");      //[hr] duration of startup at max startup power
            double f_heater_min = as_double("f_q_dot_heater_min");                      //[-] minimum allowable heater output as fraction of design

            p_electric_resistance = new C_csp_cr_electric_resistance(receiver->m_T_htf_cold_des, receiver->m_T_htf_hot_des,
                q_dot_heater_des, f_heater_min,
                f_q_dot_des_allowable_su, hrs_startup_at_max_rate,
                as_integer("rec_htf"), as_matrix("field_fl_props"), C_csp_cr_electric_resistance::E_elec_resist_startup_mode::INSTANTANEOUS_NO_MAX_ELEC_IN);

            p_electric_resistance->mc_reported_outputs.assign(C_csp_cr_electric_resistance::E_W_DOT_HEATER, allocate("W_dot_heater", n_steps_fixed), n_steps_fixed);
            p_electric_resistance->mc_reported_outputs.assign(C_csp_cr_electric_resistance::E_Q_DOT_HTF, allocate("q_dot_heater_to_htf", n_steps_fixed), n_steps_fixed);
            p_electric_resistance->mc_reported_outputs.assign(C_csp_cr_electric_resistance::E_Q_DOT_STARTUP, allocate("q_dot_heater_startup", n_steps_fixed), n_steps_fixed);
            p_electric_resistance->mc_reported_outputs.assign(C_csp_cr_electric_resistance::E_M_DOT_HTF, allocate("m_dot_htf_heater", n_steps_fixed), n_steps_fixed);
            p_electric_resistance->mc_reported_outputs.assign(C_csp_cr_electric_resistance::E_T_HTF_IN, allocate("T_htf_heater_in", n_steps_fixed), n_steps_fixed);
            p_electric_resistance->mc_reported_outputs.assign(C_csp_cr_electric_resistance::E_T_HTF_OUT, allocate("T_htf_heater_out", n_steps_fixed), n_steps_fixed);
        }
        p_heater = p_electric_resistance;        

        // Thermal energy storage 
        C_csp_two_tank_tes storage;
        C_csp_two_tank_tes::S_params *tes = &storage.ms_params;
        tes->m_field_fl = as_integer("rec_htf");
        tes->m_field_fl_props = as_matrix("field_fl_props");
        tes->m_tes_fl = as_integer("rec_htf");
        tes->m_tes_fl_props = as_matrix("field_fl_props");
        tes->m_W_dot_pc_design = as_double("P_ref");        //[MWe]
        tes->m_eta_pc = as_double("design_eff");                //[-]
        tes->m_solarm = as_double("solarm");
        tes->m_ts_hours = as_double("tshours");
        tes->m_h_tank = as_double("h_tank");
        tes->m_u_tank = as_double("u_tank");
        tes->m_tank_pairs = as_integer("tank_pairs");
        tes->m_hot_tank_Thtr = as_double("hot_tank_Thtr");
        tes->m_hot_tank_max_heat = as_double("hot_tank_max_heat");
        tes->m_cold_tank_Thtr = as_double("cold_tank_Thtr");
        tes->m_cold_tank_max_heat = as_double("cold_tank_max_heat");
        tes->m_dt_hot = 0.0;                                // MSPT assumes direct storage, so no user input here: hardcode = 0.0
        tes->m_T_field_in_des = as_double("T_htf_cold_des");
        tes->m_T_field_out_des = as_double("T_htf_hot_des");
        tes->m_T_tank_hot_ini = as_double("T_htf_hot_des");
        tes->m_T_tank_cold_ini = as_double("T_htf_cold_des");
        tes->m_h_tank_min = as_double("h_tank_min");
        tes->m_f_V_hot_ini = as_double("csp.pt.tes.init_hot_htf_percent");
        tes->m_htf_pump_coef = as_double("pb_pump_coef");


        tes->tanks_in_parallel = as_boolean("tanks_in_parallel");        //[-]
        //tes->tanks_in_parallel = false; // true;      //[-] False: Field HTF always goes to TES. PC HTF always comes from TES
        
        tes->V_tes_des = 1.85;  //[m/s]
        tes->calc_design_pipe_vals = false; // for now, to get 'tanks_in_parallel' to work
        
        
        // Set storage outputs
        storage.mc_reported_outputs.assign(C_csp_two_tank_tes::E_Q_DOT_LOSS, allocate("tank_losses", n_steps_fixed), n_steps_fixed);
        storage.mc_reported_outputs.assign(C_csp_two_tank_tes::E_W_DOT_HEATER, allocate("q_heater", n_steps_fixed), n_steps_fixed);
        storage.mc_reported_outputs.assign(C_csp_two_tank_tes::E_TES_T_HOT, allocate("T_tes_hot", n_steps_fixed), n_steps_fixed);
        storage.mc_reported_outputs.assign(C_csp_two_tank_tes::E_TES_T_COLD, allocate("T_tes_cold", n_steps_fixed), n_steps_fixed);
        storage.mc_reported_outputs.assign(C_csp_two_tank_tes::E_MASS_COLD_TANK, allocate("mass_tes_cold", n_steps_fixed), n_steps_fixed);
        storage.mc_reported_outputs.assign(C_csp_two_tank_tes::E_MASS_HOT_TANK, allocate("mass_tes_hot", n_steps_fixed), n_steps_fixed);



        // TOU parameters
        C_csp_tou_block_schedules tou;
        C_csp_tou_block_schedules::S_params *tou_params = &tou.ms_params;
        tou_params->mc_csp_ops.mc_weekdays = as_matrix("weekday_schedule");
        tou_params->mc_csp_ops.mc_weekends = as_matrix("weekend_schedule");

        if (true)
        {

            if (as_boolean("is_wlim_series"))
            {
                size_t n_wlim_series = 0;
                ssc_number_t* wlim_series = as_array("wlim_series", &n_wlim_series);
                if (n_wlim_series != n_steps_full)
                    throw exec_error("tcsmolten_salt", "Invalid net electricity generation limit series dimension. Matrix must have " + util::to_string((int)n_steps_full) + " rows.");
                for (size_t i = 0; i < n_steps_full; i++)
                    tou.mc_dispatch_params.m_w_lim_full.at(i) = (double)wlim_series[i];
            }
        }

        tou.mc_dispatch_params.m_is_tod_pc_target_also_pc_max = as_boolean("is_tod_pc_target_also_pc_max");
        tou.mc_dispatch_params.m_is_block_dispatch = ! as_boolean("is_dispatch");      //mw
        tou.mc_dispatch_params.m_use_rule_1 = true;
        tou.mc_dispatch_params.m_standby_off_buffer = 2.0;
        tou.mc_dispatch_params.m_use_rule_2 = false;
        tou.mc_dispatch_params.m_q_dot_rec_des_mult = -1.23;
        tou.mc_dispatch_params.m_f_q_dot_pc_overwrite = -1.23;

        size_t n_f_turbine = 0;
        ssc_number_t *p_f_turbine = as_array("f_turb_tou_periods", &n_f_turbine);
        tou_params->mc_csp_ops.mvv_tou_arrays[C_block_schedule_csp_ops::TURB_FRAC].resize(n_f_turbine,0.0);
        //tou_params->mv_t_frac.resize(n_f_turbine, 0.0);
        for( size_t i = 0; i < n_f_turbine; i++ )
            tou_params->mc_csp_ops.mvv_tou_arrays[C_block_schedule_csp_ops::TURB_FRAC][i] = (double)p_f_turbine[i];

        // Load fraction by time step:
        bool is_load_fraction_by_timestep = is_assigned("timestep_load_fractions");
        tou_params->mc_csp_ops.mv_is_diurnal = !(is_load_fraction_by_timestep);
        if (is_load_fraction_by_timestep) {
            size_t N_load_fractions;
            ssc_number_t* load_fractions = as_array("timestep_load_fractions", &N_load_fractions);
            std::copy(load_fractions, load_fractions + N_load_fractions, std::back_inserter(tou_params->mc_csp_ops.timestep_load_fractions));
        }

        int csp_financial_model = as_integer("csp_financial_model");
        bool is_dispatch = as_boolean("is_dispatch");
        if (csp_financial_model > 0 && csp_financial_model < 5) {   // Single Owner financial models

            int ppa_soln_mode = as_integer("ppa_soln_mode");    // PPA solution mode (0=Specify IRR target, 1=Specify PPA price)
            if (ppa_soln_mode == 0 && is_dispatch) {
                throw exec_error("tcsmolten_salt", "\n\nYou selected dispatch optimization and the Specify IRR Target financial solution mode, "
                    "but dispatch optimization requires known absolute electricity prices. Dispatch optimization requires "
                    "the Specify PPA Price financial solution mode. You can continue using dispatch optimization and iteratively "
                    "solve for the PPA that results in a target IRR by running a SAM Parametric analysis or script.\n");
            }

            int en_electricity_rates = as_integer("en_electricity_rates");  // 0 = Use PPA, 1 = Use Retail
            if (en_electricity_rates == 1 && is_dispatch) {
                throw exec_error("tcsmolten_salt", "\n\nYou selected dispatch optimization and the option to Use Retail Electricity Rates on the Electricity Purchases page, "
                    "but the dispatch optimization model currently does not accept separate buy and sell prices. Please use the Use PPA or Market Prices option "
                    "on the Electricity Purchases page.\n");
            }

            // Time-of-Delivery factors by time step:
            int ppa_mult_model = as_integer("ppa_multiplier_model");
            if (ppa_mult_model == 1)        // use dispatch_ts input
            {
                tou_params->mc_pricing.mv_is_diurnal = false;

                if (is_assigned("dispatch_factors_ts") || is_dispatch) {
                    size_t nmultipliers;
                    ssc_number_t* multipliers = as_array("dispatch_factors_ts", &nmultipliers);
                    tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE].resize(nmultipliers, 0.0);
                    for (size_t ii = 0; ii < nmultipliers; ii++)
                        tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE][ii] = multipliers[ii];
                }
                else { // if no dispatch optimization, don't need an input pricing schedule
                    tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE].resize(n_steps_fixed, -1.0);
                }
            }
            else if (ppa_mult_model == 0) // standard diurnal input
            {
                tou_params->mc_pricing.mv_is_diurnal = true;

                bool are_all_assigned = is_assigned("dispatch_sched_weekday") || is_assigned("dispatch_sched_weekend")
                    || is_assigned("dispatch_factor1") || is_assigned("dispatch_factor2") || is_assigned("dispatch_factor3")
                    || is_assigned("dispatch_factor4") || is_assigned("dispatch_factor5") || is_assigned("dispatch_factor6")
                    || is_assigned("dispatch_factor7") || is_assigned("dispatch_factor8") || is_assigned("dispatch_factor9");

                if (are_all_assigned || is_dispatch) {

                    tou_params->mc_pricing.mc_weekdays = as_matrix("dispatch_sched_weekday");
                    if (tou_params->mc_pricing.mc_weekdays.ncells() == 1) { tou_params->mc_pricing.mc_weekdays.resize_fill(12, 24, 1.); };
                    tou_params->mc_pricing.mc_weekends = as_matrix("dispatch_sched_weekend");
                    if (tou_params->mc_pricing.mc_weekends.ncells() == 1) { tou_params->mc_pricing.mc_weekends.resize_fill(12, 24, 1.); };

                    tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE].resize(9, 0.0);
                    tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE][0] = as_double("dispatch_factor1");
                    tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE][1] = as_double("dispatch_factor2");
                    tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE][2] = as_double("dispatch_factor3");
                    tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE][3] = as_double("dispatch_factor4");
                    tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE][4] = as_double("dispatch_factor5");
                    tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE][5] = as_double("dispatch_factor6");
                    tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE][6] = as_double("dispatch_factor7");
                    tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE][7] = as_double("dispatch_factor8");
                    tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE][8] = as_double("dispatch_factor9");
                }
                else {
                    // If electricity pricing data is not available, then dispatch to a uniform schedule
                    tou_params->mc_pricing.mc_weekdays.resize_fill(12, 24, 1.);
                    tou_params->mc_pricing.mc_weekends.resize_fill(12, 24, 1.);
                    tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE].resize(9, -1.0);
                }
            }
        }
        else if (csp_financial_model == 6) {     // use 'mp_energy_market_revenue' -> from Merchant Plant model

            tou_params->mc_pricing.mv_is_diurnal = false;

            if (is_dispatch) {
                util::matrix_t<double> mp_energy_market_revenue = as_matrix("mp_energy_market_revenue"); // col 0 = cleared capacity, col 1 = $/MWh
                size_t n_rows = mp_energy_market_revenue.nrows();
                if (n_rows < n_steps_fixed) {
                    string ppa_msg = util::format("mp_energy_market_revenue input has %d rows but there are %d number of timesteps", n_rows, n_steps_fixed);
                    throw exec_error("tcsmolten_salt", ppa_msg);
                }

                double conv_dolmwh_to_centkwh = 0.1;
                tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE].resize(n_steps_fixed, 0.0);
                for (size_t ii = 0; ii < n_steps_fixed; ii++) {
                    tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE][ii] = mp_energy_market_revenue(ii, 1) * conv_dolmwh_to_centkwh; //[cents/kWh]
                }
            }
            else { // if no dispatch optimization, don't need an input pricing schedule
                tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE].resize(n_steps_fixed, -1.0);
            }
        }
        else {
            throw exec_error("tcsmolten_salt", "csp_financial_model must be 1, 2, 3, 4, or 6");
        }


        // System parameters
        C_csp_solver::S_csp_system_params system;
        system.m_pb_fixed_par = as_double("pb_fixed_par");
        system.m_bop_par = as_double("bop_par");
        system.m_bop_par_f = as_double("bop_par_f");
        system.m_bop_par_0 = as_double("bop_par_0");
        system.m_bop_par_1 = as_double("bop_par_1");
        system.m_bop_par_2 = as_double("bop_par_2");

        // *****************************************************
        // System dispatch
        csp_dispatch_opt dispatch;

        if (as_boolean("is_dispatch")){
            dispatch.solver_params.set_user_inputs(as_boolean("is_dispatch"), as_integer("disp_steps_per_hour"), as_integer("disp_frequency"), as_integer("disp_horizon"),
                as_integer("disp_max_iter"), as_double("disp_mip_gap"), as_double("disp_timeout"),
                as_integer("disp_spec_presolve"), as_integer("disp_spec_bb"), as_integer("disp_spec_scaling"), as_integer("disp_reporting"),
                as_boolean("is_write_ampl_dat"), as_boolean("is_ampl_engine"), as_string("ampl_data_dir"), as_string("ampl_exec_call"));
            dispatch.params.set_user_params(as_double("disp_time_weighting"),
                as_double("disp_rsu_cost"), as_double("disp_csu_cost"), as_double("disp_pen_delta_w"), as_double("disp_inventory_incentive"),
                as_double("q_rec_standby"), as_double("q_rec_heattrace"));            
        }
        else {
            dispatch.solver_params.dispatch_optimize = false;
        }

        // Instantiate Solver       
        C_csp_solver csp_solver(weather_reader, 
                        collector_receiver, 
                        *p_csp_power_cycle, 
                        storage, 
                        tou,
                        dispatch,
                        system,
                        p_heater,
                        ssc_cmod_update,
                        (void*)(this));


        // Set solver reporting outputs
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::TIME_FINAL, allocate("time_hr", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::ERR_M_DOT, allocate("m_dot_balance", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::ERR_Q_DOT, allocate("q_balance", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::N_OP_MODES, allocate("n_op_modes", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::OP_MODE_1, allocate("op_mode_1", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::OP_MODE_2, allocate("op_mode_2", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::OP_MODE_3, allocate("op_mode_3", n_steps_fixed), n_steps_fixed);


        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::TOU_PERIOD, allocate("tou_value", n_steps_fixed), n_steps_fixed);            
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::PRICING_MULT, allocate("pricing_mult", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::PC_Q_DOT_SB, allocate("q_dot_pc_sb", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::PC_Q_DOT_MIN, allocate("q_dot_pc_min", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::PC_Q_DOT_TARGET, allocate("q_dot_pc_target", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::PC_Q_DOT_MAX, allocate("q_dot_pc_max", n_steps_fixed), n_steps_fixed);
        
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::CTRL_IS_REC_SU, allocate("is_rec_su_allowed", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::CTRL_IS_PC_SU, allocate("is_pc_su_allowed", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::CTRL_IS_PC_SB, allocate("is_pc_sb_allowed", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::EST_Q_DOT_CR_SU, allocate("q_dot_est_cr_su", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::EST_Q_DOT_CR_ON, allocate("q_dot_est_cr_on", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::EST_Q_DOT_DC, allocate("q_dot_est_tes_dc", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::EST_Q_DOT_CH, allocate("q_dot_est_tes_ch", n_steps_fixed), n_steps_fixed);
        
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::CTRL_OP_MODE_SEQ_A, allocate("operating_modes_a", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::CTRL_OP_MODE_SEQ_B, allocate("operating_modes_b", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::CTRL_OP_MODE_SEQ_C, allocate("operating_modes_c", n_steps_fixed), n_steps_fixed);

        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_REL_MIP_GAP, allocate("disp_rel_mip_gap", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_SOLVE_STATE, allocate("disp_solve_state", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_SUBOPT_FLAG, allocate("disp_subopt_flag", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_SOLVE_ITER, allocate("disp_solve_iter", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_SOLVE_OBJ, allocate("disp_objective", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_SOLVE_OBJ_RELAX, allocate("disp_obj_relax", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_QSF_EXPECT, allocate("disp_qsf_expected", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_QSFPROD_EXPECT, allocate("disp_qsfprod_expected", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_QSFSU_EXPECT, allocate("disp_qsfsu_expected", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_TES_EXPECT, allocate("disp_tes_expected", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_PCEFF_EXPECT, allocate("disp_pceff_expected", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_SFEFF_EXPECT, allocate("disp_thermeff_expected", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_QPBSU_EXPECT, allocate("disp_qpbsu_expected", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_WPB_EXPECT, allocate("disp_wpb_expected", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_REV_EXPECT, allocate("disp_rev_expected", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_PRES_NCONSTR, allocate("disp_presolve_nconstr", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_PRES_NVAR, allocate("disp_presolve_nvar", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::DISPATCH_SOLVE_TIME, allocate("disp_solve_time", n_steps_fixed), n_steps_fixed);

        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::SOLZEN, allocate("solzen", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::SOLAZ, allocate("solaz", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::BEAM, allocate("beam", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::TDRY, allocate("tdry", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::TWET, allocate("twet", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::RH, allocate("RH", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::WSPD, allocate("wspd", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::CR_DEFOCUS, allocate("defocus", n_steps_fixed), n_steps_fixed);

        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::TES_Q_DOT_DC, allocate("q_dc_tes", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::TES_Q_DOT_CH, allocate("q_ch_tes", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::TES_E_CH_STATE, allocate("e_ch_tes", n_steps_fixed), n_steps_fixed);
       
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::M_DOT_CR_TO_TES_HOT, allocate("m_dot_cr_to_tes_hot", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::M_DOT_TES_HOT_OUT, allocate("m_dot_tes_hot_out", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::M_DOT_PC_TO_TES_COLD, allocate("m_dot_pc_to_tes_cold", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::M_DOT_TES_COLD_OUT, allocate("m_dot_tes_cold_out", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::M_DOT_FIELD_TO_CYCLE, allocate("m_dot_field_to_cycle", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::M_DOT_CYCLE_TO_FIELD, allocate("m_dot_cycle_to_field", n_steps_fixed), n_steps_fixed);

        //csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::TES_M_DOT_DC, allocate("m_dot_tes_dc", n_steps_fixed), n_steps_fixed);
        //csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::TES_M_DOT_CH, allocate("m_dot_tes_ch", n_steps_fixed), n_steps_fixed);

        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::COL_W_DOT_TRACK, allocate("pparasi", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::CR_W_DOT_PUMP, allocate("P_tower_pump", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::SYS_W_DOT_PUMP, allocate("htf_pump_power", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::PC_W_DOT_COOLING, allocate("P_cooling_tower_tot", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::SYS_W_DOT_FIXED, allocate("P_fixed", n_steps_fixed), n_steps_fixed);
        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::SYS_W_DOT_BOP, allocate("P_plant_balance_tot", n_steps_fixed), n_steps_fixed);

        csp_solver.mc_reported_outputs.assign(C_csp_solver::C_solver_outputs::W_DOT_NET, allocate("P_out_net", n_steps_fixed), n_steps_fixed);



        update("Initialize MSPT model...", 0.0);

        int out_type = -1;
        std::string out_msg = "";
        try
        {
            // Initialize Solver
            csp_solver.init();
        }
        catch( C_csp_exception &csp_exception )
        {
            // Report warning before exiting with error
            while( csp_solver.mc_csp_messages.get_message(&out_type, &out_msg) )
            {
                log(out_msg, out_type);
            }

            throw exec_error("tcsmolten_salt", csp_exception.m_error_message);
        }

        // If no exception, then report messages
        while (csp_solver.mc_csp_messages.get_message(&out_type, &out_msg))
        {
            log(out_msg, out_type);
        }


        //if the pricing schedule is provided as hourly, overwrite the tou schedule
        if( as_boolean("is_dispatch_series") )
        {
            size_t n_dispatch_series;
            ssc_number_t *dispatch_series = as_array("dispatch_series", &n_dispatch_series);

       //     if( n_dispatch_series != n_steps_fixed)
                //throw exec_error("tcsmolten_salt", "Invalid dispatch pricing series dimension. Array length must match number of simulation time steps ("+my_to_string(n_steps_fixed)+").");
                
            //resize the m_hr_tou array
            if( tou_params->mc_pricing.m_hr_tou != 0 )
                delete [] tou_params->mc_pricing.m_hr_tou;
            tou_params->mc_pricing.m_hr_tou = new double[n_steps_fixed];
            //set the tou period as unique for each time step
            for(size_t i=0; i<n_steps_fixed; i++)
                tou_params->mc_pricing.m_hr_tou[i] = i+1;
            //allocate reported arrays
            tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE].resize(n_steps_fixed);
            for( size_t i=0; i<n_steps_fixed; i++)
                tou_params->mc_pricing.mvv_tou_arrays[C_block_schedule_pricing::MULT_PRICE][i] = dispatch_series[i];
        }

        update("Begin timeseries simulation...", 0.0);

        try
        {
            // Simulate !
            csp_solver.Ssimulate(sim_setup);
        }
        catch(C_csp_exception &csp_exception)
        {
            // Report warning before exiting with error
            while( csp_solver.mc_csp_messages.get_message(&out_type, &out_msg) )
            {
                log(out_msg);
            }

            throw exec_error("tcsmolten_salt", csp_exception.m_error_message);
        }

        // If no exception, then report messages
        while (csp_solver.mc_csp_messages.get_message(&out_type, &out_msg))
        {
            log(out_msg, out_type);
        }

        // ******* Re-calculate system costs here ************

        double A_sf_refl = as_double("A_sf");
        double site_improv_spec_cost = as_double("site_spec_cost");
        double heliostat_spec_cost = as_double("heliostat_spec_cost");
        double heliostat_fixed_cost = as_double("cost_sf_fixed");

        double h_tower = as_double("h_tower");
        double h_rec = H_rec;
        double h_helio = as_double("helio_height");
        double tower_fixed_cost = as_double("tower_fixed_cost");
        double tower_cost_scaling_exp = as_double("tower_exp");

        double rec_ref_cost = as_double("rec_ref_cost");
        double A_rec_ref = as_double("rec_ref_area");
        double rec_cost_scaling_exp = as_double("rec_cost_exp");

        double Q_storage = as_double("P_ref") / as_double("design_eff")*as_double("tshours");
        double tes_spec_cost = as_double("tes_spec_cost");

        double W_dot_design = as_double("P_ref");
        double power_cycle_spec_cost = as_double("plant_spec_cost");

        double q_dot_heater_design = 0.0;
        double heater_spec_cost = 0.0;

        double rad_fluidcost = 0.0;
        double rad_installcost = 0.0;
        double rad_unitcost = 0.0;
        double rad_volmulti = 0.0;
        double coldstorage_unitcost = 0.0;
        double radfield_area = 0.0;
        double coldstorage_vol = 0.0;
        double radfield_vol = 0.0;

        if (rankine_pc.ms_params.m_CT == 4) {
            radfield_area = rankine_pc.mc_radiator.ms_params.Afield;
            radfield_vol = rankine_pc.mc_radiator.ms_params.D*rankine_pc.mc_radiator.ms_params.D / 4 * PI*rankine_pc.mc_radiator.ms_params.n*rankine_pc.mc_radiator.ms_params.Np*rankine_pc.mc_radiator.ms_params.L; //Calculate volume in radiator panel tubes = pi/4*d^2*L*n*Np
            if (rankine_pc.mc_two_tank_ctes.ms_params.m_ctes_type == 2) //If two tank
            {
                coldstorage_vol = rankine_pc.mc_two_tank_ctes.get_physical_volume();
            }
            if (rankine_pc.mc_two_tank_ctes.ms_params.m_ctes_type > 2) //If stratified 
            {
                coldstorage_vol = rankine_pc.mc_stratified_ctes.get_physical_volume();

            }

            rad_unitcost = as_double("radiator_unitcost");
            rad_installcost = as_double("radiator_installcost");
            rad_fluidcost = as_double("radiator_fluidcost");
            rad_volmulti = as_double("radfluid_vol_ratio");
            coldstorage_unitcost = as_double("ctes_cost");
        }

        double bop_spec_cost = as_double("bop_spec_cost");

        double fossil_backup_spec_cost = as_double("fossil_spec_cost");

        double contingency_rate = as_double("contingency_rate");

        //land area
        double total_land_area = as_double("land_area_base") * as_double("csp.pt.sf.land_overhead_factor") + as_double("csp.pt.sf.fixed_land_area")+ radfield_area/4046.86 /*acres/m^2*/ ;
        assign("csp.pt.cost.total_land_area", (ssc_number_t)total_land_area);
        assign("total_land_area", (ssc_number_t)total_land_area);

        double plant_net_capacity = system_capacity / 1000.0;         //[MWe], convert from kWe
        double EPC_land_spec_cost = as_double("csp.pt.cost.epc.per_acre");
        double EPC_land_perc_direct_cost = as_double("csp.pt.cost.epc.percent");
        double EPC_land_per_power_cost = as_double("csp.pt.cost.epc.per_watt");
        double EPC_land_fixed_cost = as_double("csp.pt.cost.epc.fixed");
        double total_land_spec_cost = as_double("land_spec_cost");
        double total_land_perc_direct_cost = as_double("csp.pt.cost.plm.percent");
        double total_land_per_power_cost = as_double("csp.pt.cost.plm.per_watt");
        double total_land_fixed_cost = as_double("csp.pt.cost.plm.fixed");
        double sales_tax_basis = as_double("sales_tax_frac");
        double sales_tax_rate = as_double("sales_tax_rate");

        double site_improvement_cost, heliostat_cost, tower_cost, receiver_cost, tes_cost, power_cycle_cost,
        heater_cost, rad_field_totcost, rad_fluid_totcost, rad_storage_totcost, bop_cost, fossil_backup_cost,
        direct_capital_precontingency_cost, contingency_cost, total_direct_cost, epc_and_owner_cost, total_land_cost,
        sales_tax_cost, total_indirect_cost, total_installed_cost, estimated_installed_cost_per_cap;

        site_improvement_cost = heliostat_cost = tower_cost = receiver_cost = tes_cost = power_cycle_cost =
            heater_cost = rad_field_totcost = rad_fluid_totcost = rad_storage_totcost = bop_cost = fossil_backup_cost =
            direct_capital_precontingency_cost = contingency_cost = total_direct_cost = epc_and_owner_cost = total_land_cost =
            sales_tax_cost = total_indirect_cost = total_installed_cost = estimated_installed_cost_per_cap = std::numeric_limits<double>::quiet_NaN();

        N_mspt::calculate_mspt_etes_costs(
            A_sf_refl,
            site_improv_spec_cost,
            heliostat_spec_cost,
            heliostat_fixed_cost,

            h_tower,
            h_rec,
            h_helio,
            tower_fixed_cost,
            tower_cost_scaling_exp,

            A_rec,
            rec_ref_cost,
            A_rec_ref,
            rec_cost_scaling_exp,

            Q_storage,
            tes_spec_cost,

            W_dot_design,
            power_cycle_spec_cost,

            q_dot_heater_design,
            heater_spec_cost,

            radfield_area,
            coldstorage_vol,
            radfield_vol,
            rad_unitcost,
            rad_installcost,
            rad_volmulti,
            rad_fluidcost,
            coldstorage_unitcost,

            bop_spec_cost,

            fossil_backup_spec_cost,

            contingency_rate,

            total_land_area,
            plant_net_capacity,
            EPC_land_spec_cost,
            EPC_land_perc_direct_cost,
            EPC_land_per_power_cost,
            EPC_land_fixed_cost,
            total_land_spec_cost,
            total_land_perc_direct_cost,
            total_land_per_power_cost,
            total_land_fixed_cost,
            sales_tax_basis,
            sales_tax_rate,

            site_improvement_cost,
            heliostat_cost,
            tower_cost,
            receiver_cost,
            tes_cost,
            power_cycle_cost,
            heater_cost,
            rad_field_totcost,
            rad_fluid_totcost,
            rad_storage_totcost,
            bop_cost,
            fossil_backup_cost,
            direct_capital_precontingency_cost,
            contingency_cost,
            total_direct_cost,
            total_land_cost,
            epc_and_owner_cost,
            sales_tax_cost,
            total_indirect_cost,
            total_installed_cost,
            estimated_installed_cost_per_cap
        );

        // 1.5.2016 twn: financial model needs an updated total_installed_cost, remaining are for reporting only
        assign("total_installed_cost", (ssc_number_t)total_installed_cost);

        assign("csp.pt.cost.site_improvements", (ssc_number_t)site_improvement_cost);
        assign("csp.pt.cost.heliostats", (ssc_number_t)heliostat_cost);
        assign("csp.pt.cost.tower", (ssc_number_t)tower_cost);
        assign("csp.pt.cost.receiver", (ssc_number_t)receiver_cost);
        assign("csp.pt.cost.storage", (ssc_number_t)tes_cost);
        assign("csp.pt.cost.power_block", (ssc_number_t)power_cycle_cost);
        
        if (pb_tech_type == 0) {
            if (rankine_pc.ms_params.m_CT == 4) {
                assign("csp.pt.cost.rad_field", (ssc_number_t)rad_field_totcost);
                assign("csp.pt.cost.rad_fluid", (ssc_number_t)rad_fluid_totcost);
                assign("csp.pt.cost.rad_storage", (ssc_number_t)rad_storage_totcost);
            }
        }
        assign("csp.pt.cost.bop", (ssc_number_t)bop_cost);
        assign("csp.pt.cost.fossil", (ssc_number_t)fossil_backup_cost);
        assign("ui_direct_subtotal", (ssc_number_t)direct_capital_precontingency_cost);
        assign("csp.pt.cost.contingency", (ssc_number_t)contingency_cost);
        assign("total_direct_cost", (ssc_number_t)total_direct_cost);
        assign("csp.pt.cost.epc.total", (ssc_number_t)epc_and_owner_cost);
        assign("csp.pt.cost.plm.total", (ssc_number_t)total_land_cost);
        assign("csp.pt.cost.sales_tax.total", (ssc_number_t)sales_tax_cost);
        assign("total_indirect_cost", (ssc_number_t)total_indirect_cost);
        assign("csp.pt.cost.installed_per_capacity", (ssc_number_t)estimated_installed_cost_per_cap);

        // Update construction financing costs, specifically, update: "construction_financing_cost"
        double const_per_interest_rate1 = as_double("const_per_interest_rate1");
        double const_per_interest_rate2 = as_double("const_per_interest_rate2");
        double const_per_interest_rate3 = as_double("const_per_interest_rate3");
        double const_per_interest_rate4 = as_double("const_per_interest_rate4");
        double const_per_interest_rate5 = as_double("const_per_interest_rate5");
        double const_per_months1 = as_double("const_per_months1");
        double const_per_months2 = as_double("const_per_months2");
        double const_per_months3 = as_double("const_per_months3");
        double const_per_months4 = as_double("const_per_months4");
        double const_per_months5 = as_double("const_per_months5");
        double const_per_percent1 = as_double("const_per_percent1");
        double const_per_percent2 = as_double("const_per_percent2");
        double const_per_percent3 = as_double("const_per_percent3");
        double const_per_percent4 = as_double("const_per_percent4");
        double const_per_percent5 = as_double("const_per_percent5");
        double const_per_upfront_rate1 = as_double("const_per_upfront_rate1");
        double const_per_upfront_rate2 = as_double("const_per_upfront_rate2");
        double const_per_upfront_rate3 = as_double("const_per_upfront_rate3");
        double const_per_upfront_rate4 = as_double("const_per_upfront_rate4");
        double const_per_upfront_rate5 = as_double("const_per_upfront_rate5");

        double const_per_principal1, const_per_principal2, const_per_principal3, const_per_principal4, const_per_principal5;
        double const_per_interest1, const_per_interest2, const_per_interest3, const_per_interest4, const_per_interest5;
        double const_per_total1, const_per_total2, const_per_total3, const_per_total4, const_per_total5;
        double const_per_percent_total, const_per_principal_total, const_per_interest_total, construction_financing_cost;

        const_per_principal1 = const_per_principal2 = const_per_principal3 = const_per_principal4 = const_per_principal5 =
            const_per_interest1 = const_per_interest2 = const_per_interest3 = const_per_interest4 = const_per_interest5 =
            const_per_total1 = const_per_total2 = const_per_total3 = const_per_total4 = const_per_total5 =
            const_per_percent_total = const_per_principal_total = const_per_interest_total = construction_financing_cost =
            std::numeric_limits<double>::quiet_NaN();

        N_financial_parameters::construction_financing_total_cost(total_installed_cost,
            const_per_interest_rate1, const_per_interest_rate2, const_per_interest_rate3, const_per_interest_rate4, const_per_interest_rate5,
            const_per_months1, const_per_months2, const_per_months3, const_per_months4, const_per_months5,
            const_per_percent1, const_per_percent2, const_per_percent3, const_per_percent4, const_per_percent5,
            const_per_upfront_rate1, const_per_upfront_rate2, const_per_upfront_rate3, const_per_upfront_rate4, const_per_upfront_rate5,
            const_per_principal1, const_per_principal2, const_per_principal3, const_per_principal4, const_per_principal5,
            const_per_interest1, const_per_interest2, const_per_interest3, const_per_interest4, const_per_interest5,
            const_per_total1, const_per_total2, const_per_total3, const_per_total4, const_per_total5,
            const_per_percent_total, const_per_principal_total, const_per_interest_total, construction_financing_cost);

        assign("const_per_principal1", (ssc_number_t)const_per_principal1);
        assign("const_per_principal2", (ssc_number_t)const_per_principal2);
        assign("const_per_principal3", (ssc_number_t)const_per_principal3);
        assign("const_per_principal4", (ssc_number_t)const_per_principal4);
        assign("const_per_principal5", (ssc_number_t)const_per_principal5);
        assign("const_per_interest1", (ssc_number_t)const_per_interest1);
        assign("const_per_interest2", (ssc_number_t)const_per_interest2);
        assign("const_per_interest3", (ssc_number_t)const_per_interest3);
        assign("const_per_interest4", (ssc_number_t)const_per_interest4);
        assign("const_per_interest5", (ssc_number_t)const_per_interest5);
        assign("const_per_total1", (ssc_number_t)const_per_total1);
        assign("const_per_total2", (ssc_number_t)const_per_total2);
        assign("const_per_total3", (ssc_number_t)const_per_total3);
        assign("const_per_total4", (ssc_number_t)const_per_total4);
        assign("const_per_total5", (ssc_number_t)const_per_total5);
        assign("const_per_percent_total", (ssc_number_t)const_per_percent_total);
        assign("const_per_principal_total", (ssc_number_t)const_per_principal_total);
        assign("const_per_interest_total", (ssc_number_t)const_per_interest_total);
        assign("construction_financing_cost", (ssc_number_t)construction_financing_cost);

        // Do unit post-processing here
        double *p_q_pc_startup = allocate("q_pc_startup", n_steps_fixed);
        double* p_q_pc_eta = allocate("eta", n_steps_fixed);
        size_t count_pc_su = 0;
        size_t count_pc_q_dot = 0;
        size_t count_pc_W_dot_gross = 0;
        ssc_number_t *p_q_dot_pc_startup = as_array("q_dot_pc_startup", &count_pc_su);
        ssc_number_t* p_q_dot = as_array("q_pb", &count_pc_q_dot);
        ssc_number_t* p_W_dot_cycle = as_array("P_cycle", &count_pc_W_dot_gross);
        if( count_pc_su != n_steps_fixed || (int)count_pc_q_dot != n_steps_fixed || (int)count_pc_W_dot_gross != n_steps_fixed)
        {
            log("q_dot_pc_startup array is a different length than 'n_steps_fixed'.", SSC_WARNING);
            return;
        }
        for( size_t i = 0; i < n_steps_fixed; i++ )
        {
            p_q_pc_startup[i] = (float)(p_q_dot_pc_startup[i] * (sim_setup.m_report_step / 3600.0));    //[MWh]
            if (p_q_dot[i] > 0.0) {
                p_q_pc_eta[i] = (float)(p_W_dot_cycle[i] / p_q_dot[i]);   //[-]
            }
            else {
                p_q_pc_eta[i] = 0.0;    //[-]
            }
        }

        // Convert mass flow rates from [kg/hr] to [kg/s]
        size_t count_m_dot_pc, count_m_dot_rec, count_m_dot_water_pc; // , count_m_dot_tes_dc, count_m_dot_tes_ch;
        count_m_dot_pc = count_m_dot_rec = count_m_dot_water_pc = 0;    // = count_m_dot_tes_dc = count_m_dot_tes_ch = 0;
        ssc_number_t *p_m_dot_rec = as_array("m_dot_rec", &count_m_dot_rec);
        ssc_number_t *p_m_dot_pc = as_array("m_dot_pc", &count_m_dot_pc);
        ssc_number_t *p_m_dot_water_pc = as_array("m_dot_water_pc", &count_m_dot_water_pc);
        //ssc_number_t *p_m_dot_tes_dc = as_array("m_dot_tes_dc", &count_m_dot_tes_dc);
        //ssc_number_t *p_m_dot_tes_ch = as_array("m_dot_tes_ch", &count_m_dot_tes_ch);
        if (count_m_dot_rec != n_steps_fixed || count_m_dot_pc != n_steps_fixed || count_m_dot_water_pc != n_steps_fixed)
            //|| count_m_dot_tes_dc != n_steps_fixed || count_m_dot_tes_ch != n_steps_fixed)
        {
            log("At least one m_dot array is a different length than 'n_steps_fixed'.", SSC_WARNING);
            return;
        }
        for (size_t i = 0; i < n_steps_fixed; i++)
        {
            p_m_dot_rec[i] = (ssc_number_t)(p_m_dot_rec[i] / 3600.0);   //[kg/s] convert from kg/hr
            p_m_dot_pc[i] = (ssc_number_t)(p_m_dot_pc[i] / 3600.0);     //[kg/s] convert from kg/hr
            p_m_dot_water_pc[i] = (ssc_number_t)(p_m_dot_water_pc[i] / 3600.0); //[kg/s] convert from kg/hr
            //p_m_dot_tes_dc[i] = (ssc_number_t)(p_m_dot_tes_dc[i] / 3600.0);     //[kg/s] convert from kg/hr
            //p_m_dot_tes_ch[i] = (ssc_number_t)(p_m_dot_tes_ch[i] / 3600.0);     //[kg/s] convert from kg/hr
        }       

        // Set output data from heliostat class
        size_t n_rows_eta_map = heliostatfield.ms_params.m_eta_map.nrows();
        ssc_number_t *eta_map_out = allocate("eta_map_out", n_rows_eta_map, 3);
        size_t n_rows_flux_maps = heliostatfield.ms_params.m_flux_maps.nrows();
        size_t n_cols_flux_maps = heliostatfield.ms_params.m_flux_maps.ncols() + 2;
        ssc_number_t *flux_maps_out = allocate("flux_maps_out", n_rows_eta_map, n_cols_flux_maps);
        ssc_number_t *flux_maps_for_import = allocate("flux_maps_for_import", n_rows_eta_map, n_cols_flux_maps);

        if(n_rows_eta_map != n_rows_flux_maps)
        {
            log("The number of rows in the field efficiency and receiver flux map matrices are not equal. This is unexpected, and the flux maps may be inaccurate.");
        }

        // [W/m2 * m2 / (m2_per_panel?)]
        double flux_scaling_mult = as_double("dni_des")*heliostatfield.ms_params.m_A_sf / 1000.0 /
            (A_rec / double(heliostatfield.ms_params.m_n_flux_x));

        for( size_t i = 0; i < n_rows_eta_map; i++ )
        {
            flux_maps_out[n_cols_flux_maps*i] = eta_map_out[3 * i] = (ssc_number_t)heliostatfield.ms_params.m_eta_map(i, 0);        //[deg] Solar azimuth angle
            flux_maps_out[n_cols_flux_maps*i + 1] = eta_map_out[3 * i + 1] = (ssc_number_t)heliostatfield.ms_params.m_eta_map(i, 1);    //[deg] Solar zenith angle
            flux_maps_for_import[n_cols_flux_maps*i] = eta_map_out[3 * i] = (ssc_number_t)heliostatfield.ms_params.m_eta_map(i, 0);        //[deg] Solar azimuth angle
            flux_maps_for_import[n_cols_flux_maps*i + 1] = eta_map_out[3 * i + 1] = (ssc_number_t)heliostatfield.ms_params.m_eta_map(i, 1);    //[deg] Solar zenith angle
            eta_map_out[3 * i + 2] = (ssc_number_t)heliostatfield.ms_params.m_eta_map(i, 2);                            //[deg] Solar field optical efficiency
            for( size_t j = 2; j < n_cols_flux_maps; j++ )
            {
                flux_maps_out[n_cols_flux_maps*i + j] = (ssc_number_t)(heliostatfield.ms_params.m_flux_maps(i, j - 2)*heliostatfield.ms_params.m_eta_map(i, 2)*flux_scaling_mult);      //[kW/m^2]
                flux_maps_for_import[n_cols_flux_maps*i + j] = (ssc_number_t)heliostatfield.ms_params.m_flux_maps(i, j - 2);
            }
        }

        size_t count;
        ssc_number_t *p_W_dot_net = as_array("P_out_net", &count);
        ssc_number_t *p_time_final_hr = as_array("time_hr", &count);

        // 'adjustment_factors' class stores factors in hourly array, so need to index as such
        adjustment_factors haf(this, "adjust");
        if( !haf.setup() )
            throw exec_error("tcsmolten_salt", "failed to setup adjustment factors: " + haf.error());

        ssc_number_t *p_gen = allocate("gen", count);
        for( size_t i = 0; i < count; i++ )
        {
            size_t hour = (size_t)ceil(p_time_final_hr[i]);
            p_gen[i] = (ssc_number_t)(p_W_dot_net[i] * 1.E3 * haf(hour));           //[kWe]
        }
        ssc_number_t* p_annual_energy_dist_time = gen_heatmap(this, steps_per_hour);
        accumulate_annual_for_year("gen", "annual_energy", sim_setup.m_report_step / 3600.0, steps_per_hour, 1, n_steps_fixed/steps_per_hour);
        
        accumulate_annual_for_year("P_cycle", "annual_W_cycle_gross", 1000.0*sim_setup.m_report_step / 3600.0, steps_per_hour, 1, n_steps_fixed/steps_per_hour);        //[kWe-hr]
        accumulate_annual_for_year("P_cooling_tower_tot", "annual_W_cooling_tower", 1000.0*sim_setup.m_report_step / 3600.0, steps_per_hour, 1, n_steps_fixed / steps_per_hour);        //[kWe-hr]

        accumulate_annual_for_year("q_dot_rec_inc", "annual_q_rec_inc", sim_setup.m_report_step / 3600.0, steps_per_hour, 1, n_steps_fixed / steps_per_hour);           //[MWt-hr]
        accumulate_annual_for_year("q_thermal_loss", "annual_q_rec_loss", sim_setup.m_report_step / 3600.0, steps_per_hour, 1, n_steps_fixed / steps_per_hour);

        assign("annual_eta_rec_th", (ssc_number_t)(1.0 - as_number("annual_q_rec_loss") / as_number("annual_q_rec_inc")));
        assign("annual_eta_rec_th_incl_refl", (ssc_number_t)(as_number("rec_absorptance")*as_number("annual_eta_rec_th")));

        accumulate_annual_for_year("disp_objective", "disp_objective_ann", 1000.0*sim_setup.m_report_step / 3600.0, steps_per_hour, 1, n_steps_fixed/steps_per_hour);
        accumulate_annual_for_year("disp_solve_iter", "disp_iter_ann", 1000.0*sim_setup.m_report_step / 3600.0, steps_per_hour, 1, n_steps_fixed/steps_per_hour);
        accumulate_annual_for_year("disp_presolve_nconstr", "disp_presolve_nconstr_ann", sim_setup.m_report_step / 3600.0/ as_double("disp_frequency"), steps_per_hour, 1, n_steps_fixed/steps_per_hour);
        accumulate_annual_for_year("disp_presolve_nvar", "disp_presolve_nvar_ann", sim_setup.m_report_step / 3600.0/ as_double("disp_frequency"), steps_per_hour, 1, n_steps_fixed/steps_per_hour);
        accumulate_annual_for_year("disp_solve_time", "disp_solve_time_ann", sim_setup.m_report_step/3600. / as_double("disp_frequency"), steps_per_hour, 1, n_steps_fixed/steps_per_hour );

        // Calculated Outputs
            // First, sum power cycle water consumption timeseries outputs
        accumulate_annual_for_year("m_dot_water_pc", "annual_total_water_use", sim_setup.m_report_step / 1000.0, steps_per_hour, 1, n_steps_fixed/steps_per_hour); //[m^3], convert from kg
            // Then, add water usage from mirror cleaning
        ssc_number_t V_water_cycle = as_number("annual_total_water_use");
        double V_water_mirrors = as_double("water_usage_per_wash") / 1000.0*as_double("A_sf")*as_double("washing_frequency");
        assign("annual_total_water_use", (ssc_number_t)(V_water_cycle + V_water_mirrors));

        ssc_number_t ae = as_number("annual_energy");
        ssc_number_t pg = as_number("annual_W_cycle_gross");
        ssc_number_t convfactor = (pg != 0) ? 100 * ae / pg : (ssc_number_t)0.0;
        assign("conversion_factor", convfactor);

        double kWh_per_kW = 0.0;
        double nameplate = system_capacity;     //[kWe]
        if(nameplate > 0.0)
            kWh_per_kW = ae / nameplate;

        assign("capacity_factor", (ssc_number_t)(kWh_per_kW / ((double)n_steps_fixed / (double)steps_per_hour)*100.));
        assign("kwh_per_kw", (ssc_number_t)kWh_per_kW);
         
        if (pb_tech_type == 0) {
            if (rankine_pc.ms_params.m_CT == 4) {
                double A_radfield = rankine_pc.mc_radiator.ms_params.Afield;
                assign("A_radfield", (ssc_number_t)A_radfield);
            }
        }
        //Single value outputs from radiative cooling system

        if (p_electric_resistance != NULL) {
            delete p_electric_resistance;
        }

        std::clock_t clock_end = std::clock();
        double sim_cpu_run_time = (clock_end - clock_start) / (double)CLOCKS_PER_SEC;		//[s]
        assign("sim_cpu_run_time", sim_cpu_run_time);   //[s]

    }
};

DEFINE_MODULE_ENTRY(tcsmolten_salt, "CSP molten salt power tower with hierarchical controller and dispatch optimization", 1)
