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

#ifndef _CSP_COMMON_TEST_H_
#define _CSP_COMMON_TEST_H_

#include <gtest/gtest.h>
#include<vector>
#include "../ssc/common.h"
//#include "csp_financial_defaults.h"
#include "../input_cases/code_generator_utilities.h"

#define EXPECT_NEAR_FRAC(val1, val2, frac_error) EXPECT_NEAR(val1, val2, val2 * frac_error)
#define ASSERT_NEAR_FRAC(val1, val2, frac_error) ASSERT_NEAR(val1, val2, val2 * frac_error)

const double kErrorToleranceLo = 0.001;    // 0.1%
const double kErrorToleranceHi = 0.01;     // 1.0%

class CmodUnderTest {
public:
    CmodUnderTest(std::string module_name, ssc_data_t defaults)
        : module_name_{ module_name }, data_{ defaults } {}
    ~CmodUnderTest() {
        if (data_) {
            ssc_data_free(data_);
            data_ = nullptr;
        }
    }
    int RunModule() {
        int errors = run_module(this->data_, this->module_name_);
        return errors;
    }
    void SetInput(std::string name, ssc_number_t value) {
        ssc_data_set_number(this->data_, name.c_str(), value);
    }
    void SetInput(std::string name, std::string value) {
        ssc_data_set_string(this->data_, name.c_str(), value.c_str());
    }
    void SetInput(std::string name, ssc_number_t values[], int size) {
        // deprecated, replaced by the following that uses initializer_list
        ssc_data_set_array(this->data_, name.c_str(), values, size);
    }
    void SetInput(std::string name, const std::initializer_list<ssc_number_t>& values) {
        ssc_data_set_array(this->data_, name.c_str(), (ssc_number_t*)(values.begin()), values.size());
    }
    ssc_number_t GetOutput(std::string name) const {
        ssc_number_t output;
        ssc_data_get_number(this->data_, name.c_str(), &output);
        return output;
    }
private:
    const std::string module_name_;
    ssc_data_t data_;
};


ssc_data_t singleowner_defaults();

#endif
