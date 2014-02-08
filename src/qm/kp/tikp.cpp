/* 
 * File:   ti.cpp
 * Author: K M Masum Habib <khabib@ee.ucr.edu>
 *
 * Created on January 25, 2014, 10:16 AM
 */

#include "tikp.h"

namespace qmicad{
cx_mat TISurfHamGen::operator()(const Atoms& atomi, const Atoms& atomj) const{
    
    if (atomi.NumOfAtoms() > 1 || atomj.NumOfAtoms() > 1){
        throw invalid_argument("TISurfHamGen(): atomi and atomj must should contain one atom each.");
    }

    int noi = atomi.NumOfOrbitals();
    int noj = atomj.NumOfOrbitals();

    cx_mat hmat =  zeros<cx_mat>(noi, noj);

    // calculate distance between atom i and atom j
    double xi = atomi.X(0);
    double yi = atomi.Y(0);
    double xj = atomj.X(0);
    double yj = atomj.Y(0);

    double dx = abs(xi - xj);
    double dy = abs(yi - yj);           
    double d = sqrt(dx*dx + dy*dy);

    // Assign the the matrix elements based on the distance between 
    // the lattice points.
    if (atomi.Symbol(0) == "D" && atomj.Symbol(0) == "D"){
        // site energy
        if (d <= p.dtol){
            hmat = p.eps;
        // nearest neighbor in x
        }else if(abs(d - p.ax) <= p.dtol && abs(dx - p.ax) <= p.dtol){ 
            if (xi > xj){
                hmat = p.t10x;
            }else{
                hmat = p.t01x;
            }
        //nearest neighbor y
        }else if (abs(d - p.ay) <= p.dtol && abs(dy - p.ay) <= p.dtol){
            if(yi > yj){
                hmat = p.t10y;
            }else{
                hmat = p.t01y;
            }
        }            
    }
    
    return hmat;
};

}
