/** 
 * File: device.h
 * Author: K M Masum Habib
 * All dimensions assumed to be in nano-meters (nm);
 */

#ifndef TMFSC_LIB_DEVICE_H
#define TMFSC_LIB_DEVICE_H

#include "utils/Printable.hpp"
#include "maths/arma.hpp"
#include "maths/constants.h"
#include "maths/geometry.hpp"
#include "potential/potential.h"
#include "potential/linearPot.h"
#include "segment.h"

#include <vector>
#include <map>
#include <memory>
#include <tuple>

namespace qmicad{ namespace tmfsc {
using utils::Printable;
using maths::armadillo::linspace;
using maths::constants::pi;
using maths::armadillo::dcmplx;
using maths::armadillo::vec;
using maths::geometry::squadrilateral;
using potential::Potential;
using potential::LinearPot;
using std::vector;
using std::map;
using std::shared_ptr;
using std::make_shared;
using std::tuple;
using std::make_tuple;
using std::complex;
using std::exp;
using std::acos;
using std::conj;

class Edge : public Segment {
public:
    Edge(const point &p, const point &q, int type = EDGE_REFLECT);

    void type(int type) { mType = type; }
    int type() { return mType; }

public:
    // edge types
    static constexpr int EDGE_REFLECT = 1;
    static constexpr int EDGE_ABSORB = 2;
    static constexpr int EDGE_TRANSMIT = 3;

protected:
    int mType;
};


class Device : public Printable {
public:
    typedef shared_ptr<Device> ptr;
    Device();
    /** Adds point to the device. The device has a closed polygon, so add 
     *  first point again. */
    int addPoint(const point &pt);
    void addPoints(const vector<point> &pts);
    int addEdge(int ipt1, int ipt2, int type = Edge::EDGE_REFLECT);
    void edgeType(int iEdge, int type);
    int intersects(const point &p, const point &q);
    int intersects(int iEdge, const point &p, const point &q);
    point intersection(int iEdge, const point &p, const point &q);
    vector<point> createPointsOnCont(int iCnt, int n);
    vector<point> createPointsOnCont(int iCnt, double dl);
    
    int contToEdgeIndx(int iCnt) const { return mCnts[iCnt]; };
    int edgeToContIndx(int iEdge) const { return mEdg2Cnt.at(iEdge); };
    int numEdges() { return mEdgs.size(); };
    int numConts() { return mCnts.size(); };

    int edgeType(int iEdge) { return mEdgs[iEdge].type(); };
    bool isReflectEdge(int iEdge);
    bool isAbsorbEdge(int iEdge);
    bool isTransmitEdge(int iEdge);
    const svec& edgeUnitVect(int indx) const { return mEdgs[indx].unitVect(); };
    svec edgeNormVect(int indx) const { return mEdgs[indx].normal(); };
    svec edgeVect(int indx) const { return mEdgs[indx].vect(); };
    const Edge& edge(int indx) const { return mEdgs[indx]; };
   
    svec contNormVect(int indx) const { 
        return edgeNormVect(contToEdgeIndx(indx)); };
    const svec& contUnitVect(int indx) const { 
        return edgeUnitVect(contToEdgeIndx(indx)); };
    double contDirctn(int iCnt) { return mEdgs[mCnts[iCnt]].angle(); }
    point contMidPoint(int iCnt) const { return mEdgs[mCnts[iCnt]].midPoint() + contNormVect(iCnt)/1E6; }
    double contWidth(int iCnt) const { return mEdgs[mCnts[iCnt]].length(); }

    int addGate(const point& lb, const point& rb, const point& rt, 
            const point& lt);
    int getNumGates() const { return mPot->NG(); };
    void setGatePotential(int igate, double V);
    double getPotAt(const point& position);

    double getSplitLen() { return splitLen; }
    void setSplitLen(double len);

    void setRefEdgRghnsEff( double efficiency );
    double getRefEdgRghnsEff() { return RefEdgRghnsEff; }

    void setTranEdgRghnsEff( double efficiency );
    double getTranEdgRghnsEff() { return TranEdgRghnsEff; }

    void setRefEdgRghnsOn( bool flagRefEdgRghnsOn );
    bool getRefEdgRghnsOn(){ return isRefEdgRghnsOn; }

    void setTranEdgRghnsOn( bool flagTranEdgRghnsOn );
    bool getTranEdgRghnsOn(){ return isTranEdgRghnsOn; }


    tuple<double, double, double, double> calcProbab(double V1, 
            double V2, const svec& vel, double En, int iEdge);
 
public:
    /*!
     * this number will be multiplied to the occupation of the particle each
     * time it reflects from a boundary or crosses a transmitting edge
     */

private:
    shared_ptr<LinearPot> mPot; //!< Potential solver.
    vector<point> mPts; //!< vertices.
    vector<Edge> mEdgs; //!< edges.
    vector<int> mCnts;  //!< Contact to edge map.
    map<int, int> mEdg2Cnt; //!< Egde to contact map.
    double splitLen = 0.0; //!< Split length between gates
    double RefEdgRghnsEff = 1.0;
    double TranEdgRghnsEff = 1.0;
    bool isRefEdgRghnsOn = false;
    bool isTranEdgRghnsOn = false;
};


}}

#endif


