/* 
 * File:   pysimulator.h
 * Author: K M Masum Habib <masum.habib@gmail.com>
 *
 * Created on May 31, 2015, 09:03 AM
 */

#ifndef TMFSC_PYLIB_PYSIMULATOR_H
#define	TMFSC_PYLIB_PYSIMULATOR_H

#include "boostpython.hpp"
#include "simulator.h"
#include "pydevice.h"
#include <vector>

namespace qmicad { namespace python {

using tmfsc::Simulator;
using tmfsc::Device;
using tmfsc::point;
using tmfsc::TrajectoryVect;
using tmfsc::Trajectory;
using maths::armadillo::mat;
using std::vector;

class PySimulator : public Simulator {
public:
    PySimulator(PyDevice &dev) : Simulator(dev){};
    mat calcTrajPy(point ri, double thi, double B, 
            double EF, double V, bool saveTraj = true);
    tuple calcTranPy(double B, double E, double V, 
            int injCont = 0, bool saveTraj = false);
    int getParticleTypePy();
    void setParticleTypePy(int type);
 
};

void export_Simulator();

}}

#endif // TMFSC_PYLIB_PYDEVICE_H 


