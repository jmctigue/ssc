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

#include "cmod_mhk_tidal_test.h"

TEST_F(CM_MHKTidal, ComputeModuleTest_cmod_mhk_tidal) {
	int mhk_tidal_errors = run_module(data, "mhk_tidal");
	ASSERT_EQ( mhk_tidal_errors , 0 );

	ssc_number_t annual_energy, average_power, device_rated_capacity, capacity_factor, lcoe_fcr;

	ssc_data_get_number(data, "annual_energy", &annual_energy);

	EXPECT_NEAR(annual_energy, 2161517.37607, 0.1);

	ssc_data_get_number(data, "device_average_power", &average_power);
	EXPECT_NEAR(average_power, 265.321, 0.1);

	ssc_data_get_number(data, "device_rated_capacity", &device_rated_capacity);
	EXPECT_NEAR(device_rated_capacity, 1115.0, 0.1);

	ssc_data_get_number(data, "capacity_factor", &capacity_factor);
	EXPECT_NEAR(capacity_factor, 22.1299, 0.1);

	mhk_tidal_errors = run_module(data, "lcoefcr");
	ASSERT_EQ(mhk_tidal_errors, 0);

	ssc_data_get_number(data, "lcoe_fcr", &lcoe_fcr);
	EXPECT_NEAR(lcoe_fcr, 1.67476, 0.1);
}
