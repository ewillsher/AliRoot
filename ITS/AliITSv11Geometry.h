#ifndef ALIITSV11GEOMETRY_H
#define ALIITSV11GEOMETRY_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*
  $Id$
 */

/*
  Base class for defining large parts of the ITS geometry, v11.
 */
#include <TObject.h>
class TGeoArb8;
class TGeoPcon;
class TGeoTube;
class TGeoTubeSeg;
class TGeoConeSeg;
class TGeoBBox;

class AliITSv11Geometry : public TObject {
  public:
    AliITSv11Geometry(){fDebug=kTRUE;};
    AliITSv11Geometry(Bool_t debug){fDebug=debug;};
    virtual ~AliITSv11Geometry(){};
    //
    // Sets the debug flag for debugging output
    void SetDebug(){fDebug=kTRUE;}
    // Clears the debug flag so no debugging output will be generated
    void SetNoDebug(){fDebug=kFALSE;}
    // Returns the debug flag value
    Bool_t GetDebug()const {return fDebug;}
    //
    // Static functions
    //
    // Define Trig functions for use with degrees (standerd TGeo angles).
    // Sine function
    static Double_t SinD(Double_t deg){return TMath::Sin(deg*TMath::DegToRad());}
    // Cosine function
    static Double_t CosD(Double_t deg){return TMath::Cos(deg*TMath::DegToRad());}
    // Tangent function
    static Double_t TanD(Double_t deg){return TMath::Tan(deg*TMath::DegToRad());}
    // Given the line, defined by the two points (x0,y0) and (x1,y1) and the
    // point x, return the value of y.
    static Double_t Yfrom2Points(Double_t x0,Double_t y0,
                                 Double_t x1,Double_t y1,Double_t x);
    // Given the line, defined by the two points (x0,y0) and (x1,y1) and the
    // point y, return the value of x.
    static Double_t Xfrom2Points(Double_t x0,Double_t y0,
                                 Double_t x1,Double_t y1,Double_t y);
    // Given 2 points from a TGeoPcon(z and Rmax) finds Rmax at given z
    static Double_t RmaxFrom2Points(const TGeoPcon *p,Int_t i1,Int_t i2,
                                    Double_t z);
    // Given 2 points from a TGeoPcon(z and Rmin) finds Rmin at given z
    static Double_t RminFrom2Points(const TGeoPcon *p,Int_t i1,Int_t i2,
                                    Double_t z);
    // Give two points in the array ar and az, returns the value r 
    // corresponding z along the line defined by those two points
    static Double_t RFrom2Points(const Double_t *ar,const Double_t *az,
                                 Int_t i1,Int_t i2,Double_t z);
    // Given 2 points from a TGeoPcon(z and Rmax) finds z at given Rmin
    static Double_t Zfrom2MinPoints(const TGeoPcon *p,Int_t i1,Int_t i2,
                                    Double_t r);
    // Given 2 points from a TGeoPcon(z and Rmax) finds z at given Rmax
    static Double_t Zfrom2MaxPoints(const TGeoPcon *p,Int_t i1,Int_t i2,
                                    Double_t r);
    // Give two points in the array ar and az, returns the value z 
    // corresponding r along the line defined by those two points
    static Double_t Zfrom2Points(const Double_t *az,const Double_t *ar,
                                 Int_t i1,Int_t i2,Double_t r);
    // Given 1 point from a TGeoPcon(z and Rmax) the angle tc returns r for 
    // a given z, an offset (distnace perpendicular to line at angle tc) of 
    // th may be applied.
    static Double_t RmaxFromZpCone(const TGeoPcon *p,int ip,Double_t tc,
                                   Double_t z,Double_t th=0.0);
    static Double_t RFromZpCone(const Double_t *ar,const Double_t *az,int ip,
                                Double_t tc,Double_t z,Double_t th=0.0);
    // Given 1 point from a TGeoPcon(z and Rmin) the angle tc returns r for 
    // a given z, an offset (distnace perpendicular to line at angle tc) of 
    // th may be applied.
    static Double_t RminFromZpCone(const TGeoPcon *p,Int_t ip,Double_t tc,
                                   Double_t z,Double_t th=0.0);
    // Given 1 point from a TGeoPcon(z and Rmax) the angle tc returns z for 
    // a given Rmax, an offset (distnace perpendicular to line at angle tc) of 
    // th may be applied.
    static Double_t ZFromRmaxpCone(const TGeoPcon *p,int ip,Double_t tc,
                                   Double_t r,Double_t th=0.0);
    // General Outer cone Surface equation for z.
    static Double_t ZFromRmaxpCone(const Double_t *ar,const Double_t *az,
                                   Int_t ip,Double_t tc,Double_t r,
                                   Double_t th=0.0);
    // Given 1 point from a TGeoPcon(z and Rmin) the angle tc returns z for 
    // a given Rmin, an offset (distnace perpendicular to line at angle tc) of 
    // th may be applied.
    static Double_t ZFromRminpCone(const TGeoPcon *p,int ip,Double_t tc,
                                   Double_t r,Double_t th=0.0);
    // Given two lines defined by the points i1, i2,i3 in the TGeoPcon 
    // class p that intersect at point p->GetZ(i2) return the point z,r 
    // that is Cthick away in the TGeoPcon class q. If points i1=i2
    // and max == kTRUE, then p->GetRmin(i1) and p->GetRmax(i2) are used.
    // if points i2=i3 and max=kTRUE then points p->GetRmax(i2) and
    // p->GetRmin(i3) are used. If i2=i3 and max=kFALSE, then p->GetRmin(i2)
    // and p->GetRmax(i3) are used.
    static void InsidePoint(const TGeoPcon *p,Int_t i1,Int_t i2,Int_t i3,
                        Double_t Cthick,TGeoPcon *q,Int_t j1,Bool_t max);
    // Given two intersecting lines defined by the points (x0,y0), (x1,y1) and
    // (x1,y1), (x1,y2) {intersecting at (x1,y1)} the point (x,y) a distance
    // c away is returned such that two lines a distance c away from the
    // lines defined above intersect at (x,y).
    static void InsidePoint(Double_t x0,Double_t y0,Double_t x1,Double_t y1,
                            Double_t x2,Double_t y2,Double_t c,
                            Double_t &x,Double_t &y);
    // Given a initial point z0,r0, the initial angle theta0, and the radius
    // of curvature, returns the point z1, r1 at the angle theta1. Theta
    // measured from the r axis in the clock wise direction [degrees].
    static void RadiusOfCurvature(Double_t rc,Double_t theta0,Double_t z0,
                                  Double_t r0,Double_t theta1,Double_t &z1,
                                  Double_t &r1);
    //
    // Output functions for debugging
    //
    // Prints out the contents of the TGeoArb8
    void PrintArb8(const TGeoArb8 *a) const;
    // Prints out the contents of the TGeoPcon
    void PrintPcon(const TGeoPcon *a) const;
    // Prints out the contents of the TGeoTube
    void PrintTube(const TGeoTube *a) const;
    // Prints out the contents of the TGeoTubeSeg
    void PrintTubeSeg(const TGeoTubeSeg *a) const;
    // Prints out the contents of the TGeoConeSeg
    void PrintConeSeg(const TGeoConeSeg *a) const;
    // Prints out the contents of the TGeoBBox
    void PrintBBox(const TGeoBBox *a) const;
    // Draws a 2D crossection of the TGeoPcon r,z section
    void DrawCrossSection(const TGeoPcon *p,Int_t fillc=7,Int_t fills=4050,
                          Int_t linec=3,Int_t lines=1,Int_t linew=4,
                          Int_t markc=2,Int_t marks=4,
                          Float_t marksize=1.0) const;
  private:
    Bool_t fDebug; //! Debug flag
    ClassDef(AliITSv11Geometry,1) // Base class for ITS v11 geometry
};

// Units, Convert from k?? to cm,degree,GeV,seconds,
static const Double_t kmm = 0.10; // Convert mm to TGeom's cm.
static const Double_t kcm = 1.00; // Convert cv to TGeom's cm.
static const Double_t kDegree = 1.0; // Convert degrees to TGeom's degrees
static const Double_t kRadian = TMath::DegToRad(); // conver to Radians

#endif
