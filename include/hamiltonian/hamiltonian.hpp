/* 
 * File:   hamiltonian.hpp
 * Copyright (C) 2013-2014  K M Masum Habib <masum.habib@gmail.com>
 *
 * Created on April 6, 2013, 5:52 PM
 * 
 * Description: Tight binding calculation logic and data.
 * 
 */

#ifndef HAMILTONIAN_HPP
#define	HAMILTONIAN_HPP

#include <boost/serialization/string.hpp>
#include <boost/serialization/access.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "maths/arma.hpp"
#include "atoms/AtomicStruct.h"
#include "utils/vout.h"

namespace qmicad{
namespace hamiltonian{

using boost::shared_ptr;
using boost::make_shared;
using namespace utils::stds;
using utils::Printable;
using namespace maths::armadillo;
using atoms::AtomicStruct;

template<class T>
class HamParams: public Printable{    
public:    
    HamParams(const string &prefix = ""):
        Printable(" " + prefix)
    {        
    }
    
    void   dtol(double dtol){ mdtol = dtol; update(); }
    double dtol(){ return mdtol; }
    
    void   orthogonal(bool orth){ mortho = orth; update(); }
    bool   orthogonal(){ return mortho; }

    //!< Generate Hamiltonian between two atoms.
    virtual T twoAtomHam(const AtomicStruct& atomi, 
                            const AtomicStruct& atomj) const { return T(); };
    //!< Generate Overlap matrix between two atoms.
    virtual T twoAtomOvl(const AtomicStruct& atomi, 
                            const AtomicStruct& atomj) const { return T(); };
    
protected:
    // Updates internal parameters. Call it after changing any of the 
    // public parameters.
    virtual void update(){}; 
    
protected:
    // Parameters required for all Hamiltonin
    double mdtol;         //!< Distance tolerance. 
    
    bool   mortho;         //!< Is this an orthogonal basis.
                            
};

//!< Generates the hamiltonaina and overlap matrices between atomc block
//!< i and atomic block j. 
template<class T>
void generateHamOvl(T &hmat, T&smat, const HamParams<T> &p, 
        const AtomicStruct &bi, const AtomicStruct &bj){
    // Just for easy reference
    int nai = bi.NumOfAtoms();
    int naj = bj.NumOfAtoms();
    int noi = bi.NumOfOrbitals();
    int noj = bj.NumOfOrbitals();

    // Most of the matrix elements are zeros. So, we'll only change the 
    // non zero elements below.
    hmat = zeros<T>(noi, noj);
    smat = zeros<T>(noi, noj);

    // Lets find the neighbors. 
    int io = 0;
    for(int ia = 0; ia != nai; ++ia){
        AtomicStruct atomi = bi(ia);        // extract atom ia
        int ni = atomi.NumOfOrbitals();     // number of orbitals in atom ia

        int jo = 0;
        for(int ja = 0; ja != naj; ++ja){
            AtomicStruct atomj = bj(ja);        // extract atom ja    
            int nj = atomj.NumOfOrbitals();     // number of orbitals in atom ja
            // generate Hamiltonian matrix between orbitals of
            // atom i and atom j
            hmat(span(io,io+ni-1), span(jo,jo+nj-1)) = p.twoAtomHam(atomi, atomj);
            smat(span(io,io+ni-1), span(jo,jo+nj-1)) = p.twoAtomOvl(atomi, atomj);
            jo += nj;
        }

        io += ni;
    }    
}   

typedef HamParams<cxmat>  cxhamparams;
typedef HamParams<mat>    hamparams;

}
}
#endif	/* HAMILTONIAN_HPP */

