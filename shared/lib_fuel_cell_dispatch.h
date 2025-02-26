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


#ifndef _LIB_FUEL_CELL_DISPATCH_
#define _LIB_FUEL_CELL_DISPATCH_

#include "lib_fuel_cell.h"
#include "lib_battery_powerflow.h"
#include "lib_util.h"

class FuelCellDispatch
{
public:
	/// Default constructor
	FuelCellDispatch() { /* Nothing to do */ };

	/// Construct with arguments
	FuelCellDispatch(FuelCell * fuelCell, size_t numberOfUnits, int dispatchOption, int shutdownOption, double dt_hour,
		double fixed_percent,
		std::vector<double> dispatchInput_kW,
		std::vector<bool> canCharge,
		std::vector<bool> canDischarge,
		std::map<size_t, double> discharge_percent, 
		std::map<size_t, size_t> discharge_units,
		util::matrix_t<size_t> scheduleWeekday,
		util::matrix_t<size_t> scheduleWeekend);

	/// Destructor
	~FuelCellDispatch();

	/// Run dispatch for single step
	void runSingleTimeStep(size_t hour_of_year, size_t year_idx, double powerSystem_kWac=0, double powerLoad_kWac=0);

	/// Update dispatch option (for testing)
	void setDispatchOption(int dispatchOption);

	/// Update the fixed percentage to dispatch
	void setFixedDischargePercentage(double discharge_percent);

	/// Update dispatch units (for testing)
	void setManualDispatchUnits(std::map<size_t, size_t> unitsByPeriod);

	/// Get the total fuel cell power output kW
	double getPower();

	/// Get the total fuel cell heat output kW
	double getPowerThermal();

	/// Get the maximum power percentage available
	double getPowerMaxPercent();

	/// Get the percent of the current power divided by max power (degraded  not original)
	double getPercentLoad();

	/// Get the electrical efficiency percent at the current step
	double getElectricalEfficiencyPercent();

	/// Get the fuel consumed during this time step MCf
	double getFuelConsumption();

	/// Return a pointer to the underlying calculated power quantities
	BatteryPower * getBatteryPower() { return m_batteryPower; };

	/// Dispatch option enumerations
	enum FC_DISPATCH_OPTION { FIXED, LOAD_FOLLOW, MANUAL, INPUT };

private:

	// allocated and managed internally
	std::unique_ptr<BatteryPowerFlow> m_batteryPowerFlow;

	// managed by BatteryPowerFlow
	BatteryPower * m_batteryPower;

	double m_powerTotal_kW;
	double m_loadAverage_percent;
	double m_powerMaxPercentAverage_percent;
	double m_efficiencyAverage_percent;
	double m_powerThermalTotal_kW;
	double m_fuelConsumedTotal_MCf;

	size_t m_numberOfUnits;
	int m_dispatchOption;
	int m_shutdownOption;
	double dt_hour;
	double m_fixed_percent;
	
	std::vector<double> m_dispatchInput_kW;
	std::vector< FuelCell *> m_fuelCellVector;
	std::vector<bool> m_canCharge;
	std::vector<bool> m_canDischarge;
	std::map<size_t, double> m_discharge_percent;
	std::map<size_t, size_t> m_discharge_units;
	util::matrix_t<size_t> m_scheduleWeekday;
	util::matrix_t<size_t> m_scheduleWeekend;
};

#endif
