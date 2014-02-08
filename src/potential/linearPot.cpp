/* 
 * File:   linearPot.cpp
 * Author: K M Masum Habib<masum.habib@virginia.edu>
 *
 * Created on January 27, 2014, 5:19 PM
 */

#include "linearPot.h"

namespace qmicad{
LinearRegion4::LinearRegion4(point lb, point rb, point rt, point lt, 
            double Vl, double Vr, double Vt, double Vb,
            const string &prefix):
            Terminal4(lb, rb, rt, lt, prefix), Vl(Vl), Vr(Vr), Vt(Vt), Vb(Vb)
{
    mTitle = "Quadrilateral Linear Voltage Region";
}

string LinearRegion4::toString() const { 
    stringstream ss;
    ss << Terminal4::toString() << endl;
    ss << mPrefix << " " << "Vl = " << Vl << " Vr = " << Vr << " Vt = " << Vt 
                  << " Vb = " << Vb;
    
    return ss.str();
};


LinearPot::LinearPot(const Atoms &atoms, const contact &source, 
const contact &drain, const vector<gate> &gates, 
const vector <linear_region> &linear, const string &prefix): 
Potential(atoms, source, drain, gates, prefix), 
mlr(linear){
    for (int it = 0; it < mlr.size() ; ++it){
        mlr[it].Title("Linear Region # " + dtos(it));
        mlr[it].Prefix(" " + prefix);
    }
}

/*
 * Calculates linear potential.
 */
void LinearPot::compute(){
    using namespace bg;
    using namespace bgi;
    using namespace std;
    
    // loop over all the atoms
    int na = ma.NumOfAtoms();
    for(int ia = 0; ia < na; ++ia){
        double V = 0;
        double x = ma.X(ia);
        double y = ma.Y(ia);
        
        // source
        if(ms.contains(x, y)){
            V = ms.V;
        // drain
        }else if (md.contains(x, y)){
            V = md.V;
        // gates
        }else{
            // search x,y inside the gates
            vector<gate>::iterator itg = find_if(mg.begin(), mg.end(), 
                    Contains(x,y));
            // found inside a gate
            if (itg != mg.end()){
                gate g = *itg;
                V = g.V;
            // not fount inside any gate, search it on linear regions
            }else{
                vector<linear_region>::iterator itl = find_if(mlr.begin(), mlr.end(), 
                        Contains(x,y));
                // found inside linear region
                if(itl  != mlr.end()){
                    // get four points: lb, rb, rt, lt
                    vector<point>::iterator it;
                    polyring points = itl->geom.outer();
                    double xlt, xlb, xrt, xrb;
                    double ylt, ylb, yrt, yrb;
                    for(it = points.begin(); it != points.end(); ++it){
                        double xp = it->get<0>();
                        double yp = it->get<1>();
                        if(xp < x && yp < y){
                            //plb = *it;
                            xlb = xp;
                            ylb = yp;
                        }else if(xp < x && yp > y){
                            //plt = *it;
                            xlt = xp;
                            ylt = yp;
                        }else if(xp > x && yp < y){
                            //prb = *it;
                            xrb = xp;
                            yrb = yp;
                        }else{
                            //prt = *it;
                            xrt = xp;
                            yrt = yp;
                        }
                    }
                    
                    double xl = xlb + (xlt - xlb)/(ylt - ylb)*(y-ylb);
                    double xr = xrb + (xrt - xrb)/(yrt - yrb)*(y-yrb);
                    double Vlr = itl->Vl + (itl->Vr - itl->Vl)/(xr - xl)*(x - xl);

                    double yb = ylb + (yrb - ylb)/(xrb - xlb)*(x-xlb);
                    double yt = ylt + (yrt - ylt)/(xrt - xlt)*(x-xlt);
                    double Vbt = itl->Vb + (itl->Vt - itl->Vb)/(yt - yb)*(y - yb);
                    
                    V = (Vlr + Vbt);
                }
            }
        }
        mV(ia) = V;
    }
}

void LinearPot::exportSvg(const string& path){
    using namespace std;
    using namespace bg;
    ofstream svg(path.c_str());
    
    svg_mapper<point> mapper(svg, 1024, 768);
    
    // add polygons to mapper
    mapper.add(ms.geom);
    for (int it = 0; it < mg.size(); ++it){
        mapper.add(mg[it].geom);
    }
    for (int it = 0; it < mlr.size() ; ++it){
        mapper.add(mlr[it].geom);
    }    
    mapper.add(md.geom);
    
    // draw polygons on mapper
    mapper.map(ms.geom, "fill-opacity:0.4;fill:rgb(10,10,255);stroke:rgb(10,10,255);stroke-width:2");
    for (int it = 0; it < mg.size(); ++it){
        mapper.map(mg[it].geom, "fill-opacity:0.4;fill:rgb(204,10,204);stroke:rgb(204,10,204);stroke-width:2");
    }
    for (int it = 0; it < mlr.size() ; ++it){
        mapper.map(mlr[it].geom, "fill-opacity:0.4;fill:rgb(10,100,204);stroke:rgb(10,100,204);stroke-width:2");

    }    
    mapper.map(md.geom, "fill-opacity:0.4;fill:rgb(255,10,10);stroke:rgb(255,10,10);stroke-width:2");

}

string LinearPot::toString() const{
    stringstream ss;
    ss << mPrefix << ms << endl;
    for (vector<gate>::const_iterator it = mg.begin(); it != mg.end(); ++it){
        ss << mPrefix << *it << endl;
    }
    for (vector<linear_region>::const_iterator it = mlr.begin(); it != mlr.end(); ++it){
        ss << mPrefix << *it << endl;
    }
    ss << mPrefix << md << endl;
   
    return ss.str();
}


/*
    bg::model::multi_polygon<polygon> tmp1, tmp3;
    bg::union_(mg[0].geom, mg[1].geom, tmp1);
    polygon linGate;
    bg::convex_hull(tmp1, linGate);
    cout << "DBG: conv " << wkt(linGate) << endl;
    bg::difference(linGate, tmp1, tmp3);
    cout << "DBG: diff1 " << wkt(tmp3) << endl;
    linGate.clear();
    bg::convex_hull(tmp3, linGate);
    bg::correct(linGate);
    cout << "DBG: linGate " << wkt(linGate) << endl;    
*/
/*// terminal polygons with ID's
    // source = 0, drain = N-1 etc.
    typedef pair<polygon, int> pids;            // pairing polygon with id
    rtree<pids, dynamic_linear> rt;             // rtree
    
    // create rtree, map id's and bookmark stating and ending of gates
    // and linear regions.
    int id = 0;
    int sid = id;                               // source id = 0;
    rt.insert(make_pair(ms.geom, id));
    int gstart = id + 1;                        // id of gate # 0
    for (int it = 0; it < mg.size(); ++it){     // gates
        rt.insert(make_pair(mg[it].geom, ++id));
    }
    int gend = id;                              // id of gate # N-1
    int lstart = id + 1;                        // id of linear region # 0
    for (int it = 0; it < mlr.size(); ++it){    // linear regions
        rt.insert(make_pair(mlr[it].geom, ++id));
    }
    int lstart = id;                            // id of linear region # N-1*/
    
}
