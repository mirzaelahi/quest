/* 
 * File:   Workers.h
 * Copyright (C) 2014  K M Masum Habib <masum.habib@gmail.com>
 *
 * Created on February 16, 2014, 6:03 PM
 */

#ifndef WORKERS_H
#define	WORKERS_H

#include "utils/vout.h"
#include "utils/std.hpp"
#include <boost/mpi.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

namespace qmicad{namespace parallel{
using namespace boost::mpi;
using namespace utils;
using namespace utils::stds;

class Workers {
public:
    Workers();
    Workers(const vector<string>& argv);
    Workers(int argc, char** argv);
    Workers(const communicator &workers);    

    int     MyId()          const {return mMyCpuId; };
    int     MasterId()      const { return mMasterId; };
    int     N()             const { return mNcpu; };
    bool    AmIMaster()     const { return mIAmMaster; };
    bool    IAmMaster()     const { return mIAmMaster; };
    
    void    assignCpus(long &myStart, long &myEnd, long &myN, long N) const;
    
    const communicator& Comm() const {return mWorkers; };
    
private:
    void init();
    
private:
    environment             menv;           //!< Our local MPI environment.
    communicator            mworld;         //!< Our local MPI world.
    const communicator      &mWorkers;      //!< MPI worker processes.
    int                     mMyCpuId;       //!< This process ID.
    int                     mNcpu;          //!< Total number of processes.
    bool                    mIAmMaster;     //!< Master process.
    const int               mMasterId;      //!< Master ID.

};


}}
#endif	/* WORKERS_H */

