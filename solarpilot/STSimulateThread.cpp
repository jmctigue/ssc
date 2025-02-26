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

#include "STSimulateThread.h"
#include "definitions.h"
#include "exceptions.hpp"

using namespace std;

#ifdef SP_USE_SOLTRACE
using namespace std;

int STSimThread::GetResultCode(){ return ResultCode; }

st_context_t STSimThread::GetContextId() { return ContextId; }

void STSimThread::Setup( st_context_t spcxt, int thd_num, int seed, bool is_load_st0, bool is_save_st0 )
{
	ThreadNum = thd_num;
	CancelFlag = false;
	Finished = false;
	SeedVal = seed;
	ContextId = spcxt;
    LoadStage0Data = is_load_st0;
    SaveStage0Data = is_save_st0;
	ResultCode = -1;
	NToTrace = 0;
	NTraced = 0; 
	NTraceTotal = 0;
	CurStage = 0;
	NStages = 0;
}

void STSimThread::CopyStageRayData( vector<vector<double> > &src, int which_stage /*0 or 1*/, int istart, int iend )
{
    vector<vector<double> > *which_raydat;
    if(which_stage==0)
        which_raydat = &raydata_st0;
    else
        which_raydat = &raydata_st1;

    which_raydat->clear();
    try{
        which_raydat->reserve(iend-istart);
    }
    catch(std::exception &e)
    {
        string msg = e.what();
        msg.append(": Error resizing raytrace data array");
        throw spexception(msg.c_str());
    }

    for(int i=istart; i<iend; i++)
        which_raydat->push_back(src.at(i));

}

vector<vector< double > > *STSimThread::GetStage0RayDataObject()
{
    return &raydata_st0;
}

vector<vector< double > > *STSimThread::GetStage1RayDataObject()
{
    return &raydata_st1;
}

STSimThread::~STSimThread()
{
	::st_free_context( ContextId );
}	


void STSimThread::CancelTrace()
{
	CancelLock.lock();
	CancelFlag = true;
	CancelLock.unlock();
}

bool STSimThread::IsTraceCancelled()
{
	bool r;
	CancelLock.lock();
	r = CancelFlag;
	CancelLock.unlock();
	return r;
}

bool STSimThread::IsFinished()
{
	bool f;
	FinishedLock.lock();
	f = Finished;
	FinishedLock.unlock();
	return f;
}

void STSimThread::UpdateStatus(int ntracedtotal, int ntraced, int ntotrace, int curstage, int nstages)
{
	StatusLock.lock();
	this->NTraceTotal = ntracedtotal;
	this->NTraced = ntraced;
	this->NToTrace = ntotrace;
	this->CurStage = curstage;
	this->NStages = nstages;
	StatusLock.unlock();
}

void STSimThread::GetStatus(int *total, int *traced, int *ntotrace, int *stage, int *nstages)
{
	StatusLock.lock();
	*total = this->NTraceTotal;
	*traced = this->NTraced;
	*ntotrace = this->NToTrace;
	*stage = this->CurStage;
	*nstages = this->NStages;
	StatusLock.unlock();
}

//void *STSimThread::Entry()
//{
//	ResultCode = st_sim_run( ContextId, (unsigned int)SeedVal, STCallback_MT, (void*) this );
//	FinishedLock.lock();
//	Finished = true;
//	FinishedLock.unlock();
//	return NULL;
//
//};

void STSimThread::StartThread()
{
    //if(LoadStage0Data)
        //ResultCode = st_sim_run_data(ContextId, (unsigned int)SeedVal, &raydata_st0, &raydata_st1, false, STCallback_MT, (void*) this);
    //else if(SaveStage0Data)
        //ResultCode = st_sim_run_data(ContextId, (unsigned int)SeedVal, &raydata_st0, &raydata_st1, true, STCallback_MT, (void*) this);
    //else
	    ResultCode = st_sim_run( ContextId, (unsigned int)SeedVal, true, STCallback_thread_update, (void*) this );

	FinishedLock.lock();
	Finished = true;
	FinishedLock.unlock();
	//	return NULL;

};	

int STCallback_thread_update(st_uint_t ntracedtotal, st_uint_t ntraced, st_uint_t ntotrace, st_uint_t curstage, st_uint_t nstages, void *data)
{
	STSimThread *t = static_cast<STSimThread*>(data);
	t->UpdateStatus(ntracedtotal, ntraced, ntotrace, curstage, nstages);
	return t->IsTraceCancelled() ? 0 : 1;
};


#endif
