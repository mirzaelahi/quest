/* 
 * File:   CohRgfa.h
 * Copyright (C) 2013-2014  K M Masum Habib <masum.habib@gmail.com>
 *
 * Created on April 22, 2013, 8:05 PM
 */

#ifndef COHRGFA_H
#define	COHRGFA_H

#include "negf/computegs.h"

#include "utils/Printable.hpp"
#include "utils/myenums.hpp"
#include "utils/std.hpp"
#include "utils/vout.h"
#include "utils/std.hpp"
#include "maths/grid.hpp"
#include "maths/constants.h"
#include "maths/trace.hpp"
#include "maths/fermi.hpp"
#include "maths/arma.hpp"
#include "cache/cache.hpp"

#include <sys/types.h>
#include <sys/stat.h>

namespace qmicad{
namespace negf{

using std::shared_ptr;
using maths::trace;
using cache::CxMatCache;
using utils::Printable;
using namespace maths::armadillo;
using namespace maths::constants;
using namespace utils::stds;

/*
 * Device geometry:
 *
 *   ----------------------------------------
 *    ... |-1 | 0 | 1 |  ...  | N |N+1|N+2| ...
 *   ----------------------------------------
 *              ^  <------^------>  ^
 *            left      Device    right
 *          contact              contact
 */

/**
 * CohRgfa - Coherent RGF algorithm class. 
 * It implements the RGF algorithm for a single energy point.
 * It only works for coherent transport. It is not parallel right now.
 */
class CohRgfa: public Printable {

/*
 * Helper classes for the potential, Hamiltonian
 * and Green functions.
 */    
protected:
    
 /*
  * Cache for block mareices of NEGF
  */
    class NegfMatCache: public CxMatCache{
    public:
        NegfMatCache(CohRgfa *negf, int begin, int end, bool cache = true):
            CxMatCache(begin, end, cache), mnegf(negf){};
        virtual const cxmat& operator ()(int ib){
            return getAt(ib);
        }
            
    protected:
        CohRgfa *mnegf;
    };
/*
 * Diagonal blocks: Dii = [ESii - USii - Hii] for non orthogonal basis.
 * Diagonal blocks: Dii = [EI - USii - Hii] for orthogonal basis.
 */
    class Di:public NegfMatCache{
    public:
        Di(CohRgfa *negf, int begin, int end, bool cache = true):
            NegfMatCache(negf, begin, end, cache){};
        const cxmat& operator ()(int ib);
    protected:
        inline void computeDi(cxmat& M, int ib);
    }; // end of Di

/*
 * Lower diagonal blocks: Tij_tilde = [Hij + USij - ESij] for non-orthogonal basis.
 * Lower Diagonal blocks: Tij = [Hij] for orthogonal basis.
 */
    class Tl:public NegfMatCache{
    public:
        Tl(CohRgfa *negf, int begin, int end, bool cache = true):
            NegfMatCache(negf, begin, end, cache){};
        const cxmat& operator ()(int ib);
    protected:
        inline void computeTl(cxmat& Tl, int ib);    
    }; // end of Tl

/*
 * Right connected Green function:
 * class grc
 */
    class grc:public NegfMatCache{
    public:
        grc(CohRgfa *negf, int begin, int end, bool cache = true):
            NegfMatCache(negf, begin, end, cache){
            // Initally, cache is empty
            mIt = mEnd + 1;            
        };
        virtual void reset(){
            // reset iterator
            mIt = mEnd + 1;
            // reset members.
            mM.reset();
            if (mCacheEnabled == true){
                mM.set_size(mLength);
            }else{
                mM.set_size(1);
            }
        }
        const cxmat& operator ()(int ib);
    protected:
        inline void computegrc(cxmat& grci, const cxmat& grcip1, int ib);    
    }; // end of grc

/*
 * Left connected Green function:
 * class glc
 */
    class glc:public NegfMatCache{
    public:
        glc(CohRgfa *negf, int begin, int end, bool cache = true):
            NegfMatCache(negf, begin, end, cache){};
        const cxmat& operator ()(int ib);
    protected:
        inline void computeglc(cxmat& glci, const cxmat& glcim1, int ib);    
    }; // end of glc

 /*
 * Diagonal block of full Green function:
 * class Gii
 */
    class Gii:public NegfMatCache{
    public:
        Gii(CohRgfa *negf, int begin, int end, bool cache = true):
            NegfMatCache(negf, begin, end, cache){};
        const cxmat& operator ()(int ib);
    protected:
        inline void computeGii(cxmat& Gii, const cxmat& Gim1im1, int ib);    
    }; // end of Gii    
 
/*
 * Blocks along the first column of full Green function:
 * class Gi1
 */
    class Gi1:public NegfMatCache{
    public:
        Gi1(CohRgfa *negf, int begin, int end, bool cache = true):
            NegfMatCache(negf, begin, end, cache){};
        const cxmat& operator ()(int ib);
    protected:
        inline void computeGi1(cxmat& Gi1, const cxmat& Gim11, int ib);    
    }; // end of Gi1

/*
 * Blocks along the last column of full Green function:
 * class GiN
 */
    class GiN:public NegfMatCache{
    public:
        GiN(CohRgfa *negf, int begin, int end, bool cache = true):
            NegfMatCache(negf, begin, end, cache){};
        const cxmat& operator ()(int ib);
    protected:
        inline void computeGiN(cxmat& GiN, const cxmat& Gip1N, int ib);    
    }; // end of GiN

/*
 * Upper diagonal block of full Green function:
 * class Giip1
 */
    class Giip1:public NegfMatCache{
    public:
        Giip1(CohRgfa *negf, int begin, int end, bool cache = true):
            NegfMatCache(negf, begin, end, cache){};
        const cxmat& operator ()(int ib);
    protected:
        inline void computeGiip1(cxmat& Giip1, const cxmat& Gii, int ib);    
    }; // end of Giip1

/*
 * Upper diagonal block of full Green function:
 * class Giim1
 */
    class Giim1:public NegfMatCache{
    public:
        Giim1(CohRgfa *negf, int begin, int end, bool cache = true):
            NegfMatCache(negf, begin, end, cache){};
        const cxmat& operator ()(int ib);
    protected:
        inline void computeGiim1(cxmat& Giim1, const cxmat& Gim1im1, int ib);    
    }; // end of Giim1    

//------------------------------------------------------------------------------    
/*
 * NEGF class members
 */

// Methods    
public:
    CohRgfa(uint nb, double kT = 0.0259, dcmplx ieta = dcmplx(0,1E-3), bool orthogonal = true, string newprefix = "");

    uint        nb() { return mnb; };
    double      kT() { return mkT; };
    dcmplx      ieta(){ return mieta; };
    bool        OrthoBasis() { return morthogonal; };
    uint        N() { return mN; };
    
    void        mu(double muD = 0.0, double muS = 0.0);
    double      muS() { return mmuS; };
    double      muD() { return mmuD; };
    
    void        E(double E);
    void        H(const field<shared_ptr<cxmat> > &H0, const field<shared_ptr<cxmat> > &Hl);
    void        S(const field<shared_ptr<cxmat> > &S0, const field<shared_ptr<cxmat> > &Sl);
    void        V(const field<shared_ptr<vec> >  &V);   
    
    virtual string toString() const;
    
    cxmat       pOp(uint N = 1, int ib = -1, ucol *atomsTracedOver = 0); //!< hole density.
    cxmat       nOp(uint N = 1, int ib = -1, ucol *atomsTracedOver = 0); //!< electron density.
    cxmat       DOSop(uint N = 1, ucol *atomsTracedOver = 0); //!< density of states.
    cxmat       Aop(uint N = 1, uint ib = 1, ucol *atomsTracedOver = 0); //!< spectral function.    
    cxmat       Iop(uint N = 1, uint ib = 0, uint jb = 0, ucol *atomsTracedOver = 0); //!< Generic current operators: current from block i to block j.
    cxmat       TEop(uint N = 1, ucol *atomsTracedOver = 0); //!< Transmission operator
    
    
protected:
    inline cxmat          Ui(int i);
    inline cxmat          Ul(int i);
    
    inline void           computeSigL(cxmat& SigLii, const cxmat& Tiim1, const cxmat& glcim1);
    inline void           computeSigR(cxmat& SigRii, const cxmat& Tip1i, const cxmat& grcip1);
    inline const cxmat&   SigL11();
    inline const cxmat&   SigRNN();
    inline const cxmat&   GamL11();
    inline const cxmat&   GamRNN();
    
    inline cxmat Iijop(uint ib, uint jb); //!< Current from block i to block j.
    inline cxmat INop(); //!< Current injected from terminal # N to device.
    inline cxmat I0op(); //!< Current injected from terminal # 0 to device.

    inline cxmat Gn(uint ib, uint jb); //!< Correlation function.
    inline const cxmat& G(uint ib, uint jb); //!< Retarded green function.
    
    inline void  reset();
        
private:
    CohRgfa();

// Fields    
private:
    uint                mnb;     // Total number of blocks including contacts
    double              mkT;     // k*T
    dcmplx              mieta;   // small infinitesimal energy    
    bool                morthogonal; // orthognality.
    
    double              mmuS;     // Fermi function at the left contact
    double              mmuD;     // Fermi function at the right contact

    double              mE;      // Energy at which calculations are performed.
    double              mf0;     // Fermi function at contact 1
    double              mfNp1;   // Fermi function at contact N+1

    uint                mN;      // number of blocks without the contacts
    uint                miLc;    // index of left contact block
    uint                miRc;    // index of right contact block
    
    static constexpr double SurfGTolX = 1E-8;
    
    // Hamiltonian , overlap and potential
    field<shared_ptr<cxmat> >mH0;// Diagonal blocks of Hamiltonian: H0(i) = [H]_i,i
                                 // H0(0) is on the left contact and H0(N+1) is 
                                 // on the right contact.

    field<shared_ptr<cxmat> >mS0;// Diagonal blocks of overlap matrix: S0(i) = [S]_i,i
                                 // H0(0) is on the left contact and H0(N+1) is 
                                 // on the right contact.
    
    field<shared_ptr<cxmat> >mHl;// Lower diagonal blocks of Hamiltonian: 
                                 // Hl(i) = [H]_i,i-1. Hl(0) = [H]_0,-1 is the 
                                 // hopping between two left contact blocks.
                                 // H(1) = [H]_1,0 is the hopping 
                                 // between block # 1 and left contact. Hl(N+1) is
                                 // hopping between right contact and block # N.
                                 // Hl(N+2) is hopping between two right contact
                                 // blocks.
    
    field<shared_ptr<cxmat> >mSl;// Lower diagonal blocks of overlap matrix: 
                                 // Sl(i) = [S]_i,i-1. Sl(0) = [S]_0,-1 is the 
                                 // overlap between two left contact blocks.
                                 // S(1) = [S]_1,0 is the overlap between 
                                 // block # 1 and left contact. Hl(N+1) is
                                 // overlap between right contact and block # N.
                                 // Sl(N+2) is hopping between two right contact
                                 // blocks.
    
    field<shared_ptr<vec> >   mV;// Electrostatic potential of all the orbitals
                                 // for the entire device: from block#0 
                                 // to block#N+1.

    // Hamiltonian, Overlap and Potential matrices.
    // [U]ij = -(Vi+Vj)/2*Sij
    // Dii = ESii - Uii - Hii; 
    // Tij = Hij + Uij - ESij;          
    // Di(i) = Dii = D_i,i. Dii(0) and Dii(N+1) hold H for left and right contacts
    // Tl(i) = T_ij = T_i,i-1.
    Di                  mDi;   // block Hamiltonian: 0 to N+1
    Tl                  mTl;   // coupling matrix: T_i,i-1. e.g., T10. 0 to N+2
    // Bare Green functions
    grc                 mgrc;   // left connected Green function  grc: 1 to N+1
    glc                 mglc;   // right connected Green function glc: 0 to N
    // Full Green function
    Gii                 mGii;   // Green function along the diagonal Gi,i: 1 to N
    Gi1                 mGi1;   // Green function along the column 1 Gi,1: 2 to N
    GiN                 mGiN;   // Green function along the column N Gi,N: 1 to N-1
    Giip1               mGiip1; // Green function along the upper diagonal Gi,i+1: 1 to N-1
    Giim1               mGiim1; // Green function along the lower diagonal Gi,i-1: 2 to N
     
    cxmat               mSigL11; // Self energy of left contact
    cxmat               mSigRNN; // Self energy of right contact
    cxmat               mGamL11; // Broadening of left contact
    cxmat               mGamRNN; // Broadening of right contact
    
    
}; // end of CohRgfa
}  // end of namespace
}
#endif	/* COHRGFA_H */

