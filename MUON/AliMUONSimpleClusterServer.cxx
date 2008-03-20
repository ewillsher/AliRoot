/**************************************************************************
* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
*                                                                        *
* Author: The ALICE Off-line Project.                                    *
* Contributors are mentioned in the code where appropriate.              *
*                                                                        *
* Permission to use, copy, modify and distribute this software and its   *
* documentation strictly for non-commercial purposes is hereby granted   *
* without fee, provided that the above copyright notice appears in all   *
* copies and that both the copyright notice and this permission notice   *
* appear in the supporting documentation. The authors make no claims     *
* about the suitability of this software for any purpose. It is          *
* provided "as is" without express or implied warranty.                  *
**************************************************************************/

// $Id$

#include "AliMUONSimpleClusterServer.h"

#include "AliMUONConstants.h"
#include "AliMUONCluster.h"
#include "AliMUONGeometryTransformer.h"
#include "AliMUONPad.h"
#include "AliMUONVCluster.h"
#include "AliMUONVClusterFinder.h"
#include "AliMUONVClusterStore.h"
#include "AliMUONVDigitStore.h"
#include "AliMpArea.h"
#include "AliMpDEIterator.h"
#include "AliMpDEManager.h"
#include "AliMpExMap.h"
#include "AliMpSegmentation.h"
#include "AliMpVPadIterator.h"
#include "AliMpVSegmentation.h"
#include "AliESDMuonPad.h"
#include "AliLog.h"
#include <float.h>
#include <Riostream.h>
#include <TClonesArray.h>
#include <TString.h>


/// \class AliMUONSimpleClusterServer
///
/// Implementation of AliMUONVClusterServer interface
/// 
/// 
/// \author Laurent Aphecetche, Subatech

/// \cond CLASSIMP  
ClassImp(AliMUONSimpleClusterServer)
/// \endcond

namespace
{
  TString AsString(const AliMpArea& area)
  {
    return Form("(X,Y)=(%7.3f,%7.3f) (DX,DY)=(%7.3f,%7.3f)",
                area.Position().X(),
                area.Position().Y(),
                area.Dimensions().Y(),
                area.Dimensions().Y());
  }
}

//_____________________________________________________________________________
AliMUONSimpleClusterServer::AliMUONSimpleClusterServer(AliMUONVClusterFinder& clusterFinder,
                                                       const AliMUONGeometryTransformer& transformer)
: AliMUONVClusterServer(), 
  fClusterFinder(clusterFinder),
  fTransformer(transformer),
  fPads(),
  fDEAreas(new AliMpExMap(true))
{
    /// Ctor
    
    fPads[0] = new AliMpExMap(true);
    fPads[1] = new AliMpExMap(true);
    
    AliMpDEIterator it;
    
    it.First();
    
    /// Generate the DE areas in global coordinates
    
    while ( !it.IsDone() )
    {
      Int_t detElemId = it.CurrentDEId();
      
      const AliMpVSegmentation* seg = AliMpSegmentation::Instance()->GetMpSegmentation(detElemId,AliMp::kCath0);
      
      Double_t xg,yg,zg;
      
      AliMp::StationType stationType = AliMpDEManager::GetStationType(detElemId);
      
      Double_t xl(0.0), yl(0.0), zl(0.0);
      Double_t dx(seg->Dimensions().X());
      Double_t dy(seg->Dimensions().Y());
      
      if ( stationType == AliMp::kStation1 || stationType == AliMp::kStation2 ) 
      {
        Double_t xmin(FLT_MAX);
        Double_t xmax(-FLT_MAX);
        Double_t ymin(FLT_MAX);
        Double_t ymax(-FLT_MAX);
        
        for ( Int_t icathode = 0; icathode < 2; ++icathode ) 
        {
          const AliMpVSegmentation* cathode 
            = AliMpSegmentation::Instance()->GetMpSegmentation(detElemId,AliMp::GetCathodType(icathode));
          
          AliMpVPadIterator* it = cathode->CreateIterator();
          
          it->First();
          
          while ( !it->IsDone() ) 
          {
            AliMpPad pad = it->CurrentItem();
            AliMpArea a(pad.Position(),pad.Dimensions());
            xmin = TMath::Min(xmin,a.LeftBorder());
            xmax = TMath::Max(xmax,a.RightBorder());
            ymin = TMath::Min(ymin,a.DownBorder());
            ymax = TMath::Max(ymax,a.UpBorder());
            it->Next();
          }
          
          delete it;
        }
        
        xl = (xmin+xmax)/2.0;
        yl = (ymin+ymax)/2.0;
        dx = (xmax-xmin)/2.0;
        dy = (ymax-ymin)/2.0;
        
        fTransformer.Local2Global(detElemId,xl,yl,zl,xg,yg,zg);
      }
      else
      {
        fTransformer.Local2Global(detElemId,xl,yl,zl,xg,yg,zg);
      }
      
      fDEAreas->Add(detElemId,new AliMpArea(TVector2(xg,yg),TVector2(dx,dy)));
     
      it.Next();
    }
}

//_____________________________________________________________________________
AliMUONSimpleClusterServer::~AliMUONSimpleClusterServer()
{
  /// Dtor
  delete &fClusterFinder;
  delete fPads[0];
  delete fPads[1];
  delete fDEAreas;
}

//_____________________________________________________________________________
Int_t 
AliMUONSimpleClusterServer::Clusterize(Int_t chamberId,
                                       AliMUONVClusterStore& clusterStore,
                                       const AliMpArea& area)
{
  /// Area is in absolute coordinate. If not valid, means clusterize all
  /// the chamber.
  ///
  /// We first find out the list of DE that have a non-zero overlap with area,
  /// and then use the clusterfinder to find clusters in those areas (and DE).
  
  AliMpDEIterator it;
  
  it.First(chamberId);
  
  Int_t nofAddedClusters(0);
  Int_t fNCluster = clusterStore.GetSize();

  AliDebug(1,Form("chamberId = %2d NofClusters before = %d searchArea=%s",
                  chamberId,fNCluster,AsString(area).Data()));
  
  while ( !it.IsDone() )
  {
    Int_t detElemId = it.CurrentDEId();
    
    TClonesArray* pads[2] = 
    { 
      static_cast<TClonesArray*>(fPads[0]->GetValue(detElemId)),
      static_cast<TClonesArray*>(fPads[1]->GetValue(detElemId)) 
    };
    
    if ( ( pads[0] && pads[0]->GetLast()>=0 ) || 
         ( pads[1] && pads[1]->GetLast()>=0 ) )
    {
      AliMpArea deArea; // area in DE-local-coordinates
      Bool_t ok(kTRUE);
      
      if ( area.IsValid() ) 
      {
        ok = Overlap(detElemId,area,deArea);
      }
      
      if ( ok ) 
      {      
        if ( fClusterFinder.NeedSegmentation() )
        {
          const AliMpVSegmentation* seg[2] = 
        { AliMpSegmentation::Instance()->GetMpSegmentation(detElemId,AliMp::kCath0),
          AliMpSegmentation::Instance()->GetMpSegmentation(detElemId,AliMp::kCath1)
        };
          fClusterFinder.Prepare(detElemId,pads,deArea,seg);
        }
        else
        {
          fClusterFinder.Prepare(detElemId,pads,deArea);
        }
        
        AliDebug(1,Form("Clusterizing DE %04d with %3d pads (cath0) and %3d pads (cath1)",
                        detElemId,
                        (pads[0] ? pads[0]->GetLast()+1 : 0),
                        (pads[1] ? pads[1]->GetLast()+1 : 0)));
        
        AliMUONCluster* cluster;
        
        while ( ( cluster = fClusterFinder.NextCluster() ) ) 
        {      
          // add new cluster to the store with information to build its ID
          // increment the number of clusters into the store
          AliMUONVCluster* rawCluster = clusterStore.Add(AliMpDEManager::GetChamberId(detElemId), detElemId, fNCluster++);
          
          ++nofAddedClusters;
          
          // fill array of Id of digits attached to this cluster
          Int_t nPad = cluster->Multiplicity();
          if (nPad < 1) AliWarning("no pad attached to the cluster");
          
          for (Int_t iPad=0; iPad<nPad; iPad++) 
          {
            AliMUONPad *pad = cluster->Pad(iPad);
	    
	    // skip virtual pads
	    if (!pad->IsReal()) continue;
            
	    rawCluster->AddDigitId(pad->GetUniqueID());
          }
          
          // fill charge and other cluster informations
          rawCluster->SetCharge(cluster->Charge());
          rawCluster->SetChi2(cluster->Chi2());
          
          Double_t xg, yg, zg;
          fTransformer.Local2Global(detElemId, 
                                    cluster->Position().X(), cluster->Position().Y(), 
                                    0, xg, yg, zg);
          rawCluster->SetXYZ(xg, yg, zg);
          
          AliDebug(1,Form("Adding RawCluster detElemId %4d mult %2d charge %e (xl,yl,zl)=(%e,%e,%e) (xg,yg,zg)=(%e,%e,%e)",
                          detElemId,rawCluster->GetNDigits(),rawCluster->GetCharge(),
                          cluster->Position().X(),cluster->Position().Y(),0.0,
                          xg,yg,zg));
        }
      }
    }
    it.Next();
  }
  
  AliDebug(1,Form("chamberId = %2d NofClusters after = %d",chamberId,fNCluster));

  return nofAddedClusters;
}

//_____________________________________________________________________________
void
AliMUONSimpleClusterServer::Global2Local(Int_t detElemId, const AliMpArea& globalArea,
                                         AliMpArea& localArea) const
{
  /// Convert a global area in local area for a given DE
  
  Double_t xl,yl,zl;
  
  Double_t zg = AliMUONConstants::DefaultChamberZ(AliMpDEManager::GetChamberId(detElemId));
  
  fTransformer.Global2Local(detElemId,
                             globalArea.Position().X(),globalArea.Position().Y(),zg,
                             xl,yl,zl);
  
  localArea = AliMpArea(TVector2(xl,yl), globalArea.Dimensions());
}

//_____________________________________________________________________________
Bool_t
AliMUONSimpleClusterServer::Overlap(Int_t detElemId,
                                    const AliMpArea& area,
                                    AliMpArea& deArea) const
{
  /// Check whether (global) area overlaps with the given DE.
  /// If it is, set deArea to the overlap region and convert it
  /// in the local coordinate system of that DE.
  
  Bool_t overlap(kFALSE);
  
  AliMpArea* globalDEArea = static_cast<AliMpArea*>(fDEAreas->GetValue(detElemId));
  
  AliMpArea overlapArea;
  
  if ( area.Overlap(*globalDEArea) )
  {
    overlapArea = area.Intersect(*globalDEArea);
    Global2Local(detElemId,overlapArea,deArea);
    overlap = kTRUE;
  }
  else
  {
    deArea = AliMpArea();
  }
  
  AliDebug(1,Form("DE %04d area %s globalDEArea %s overlapArea %s deArea %s overlap=%d",
                  detElemId,
                  AsString(area).Data(),
                  AsString(*globalDEArea).Data(),
                  AsString(overlapArea).Data(),
                  AsString(deArea).Data(),
                  overlap));
                  
  return overlap;
}

//_____________________________________________________________________________
TClonesArray* 
AliMUONSimpleClusterServer::PadArray(Int_t detElemId, Int_t cathode) const
{
  /// Return array for given cathode of given DE
  
  return static_cast<TClonesArray*>(fPads[cathode]->GetValue(detElemId));
}

//_____________________________________________________________________________
void 
AliMUONSimpleClusterServer::UseDigits(TIter& next)
{
  /// Convert digitStore into two arrays of AliMUONPads

  fPads[0]->Clear();
  fPads[1]->Clear();
  
  AliMUONVDigit* d;
  while ( ( d = static_cast<AliMUONVDigit*>(next()) ) )
  {
    if ( ! d->Charge() > 0 ) continue; // skip void digits.
    Int_t ix = d->PadX();
    Int_t iy = d->PadY();
    Int_t cathode = d->Cathode();
    Int_t detElemId = d->DetElemId();
    const AliMpVSegmentation* seg = AliMpSegmentation::Instance()->
      GetMpSegmentation(detElemId,AliMp::GetCathodType(cathode));
    AliMpPad pad = seg->PadByIndices(AliMpIntPair(ix,iy));
    
    TClonesArray* padArray = PadArray(detElemId,cathode);
    if (!padArray)
    {
      padArray = new TClonesArray("AliMUONPad",100);
      fPads[cathode]->Add(detElemId,padArray);
    }
    
    AliMUONPad mpad(detElemId,cathode,
                    ix,iy,pad.Position().X(),pad.Position().Y(),
                    pad.Dimensions().X(),pad.Dimensions().Y(),
                    d->Charge());
    if ( d->IsSaturated() ) mpad.SetSaturated(kTRUE);
    mpad.SetUniqueID(d->GetUniqueID());
    new ((*padArray)[padArray->GetLast()+1]) AliMUONPad(mpad);      
  }
}

//_____________________________________________________________________________
void 
AliMUONSimpleClusterServer::Print(Option_t*) const
{
  /// Printout for debug only
  
  AliMpDEIterator it;
  
  it.First();
  
  while ( !it.IsDone() )
  {
    Int_t detElemId = it.CurrentDEId();
    
    // printout the number of pads / de, and number of used pads / de
    
    if ( ( PadArray(detElemId,0) && PadArray(detElemId,0)->GetLast() >= 0 ) || 
         ( PadArray(detElemId,1) && PadArray(detElemId,1)->GetLast() >= 0 ) )
    {
      cout << Form("---- DE %04d",detElemId) << endl;
      
      for ( Int_t cathode = 0; cathode < 2; ++cathode ) 
      {
        cout << Form("  -- Cathode %1d",cathode) << endl;
        
        TClonesArray* padArray = PadArray(detElemId,cathode);
        
        if (!padArray)
        {
          cout << "no pad array" << endl;
        }
        else if ( padArray->GetLast() < 0 ) 
        {
          cout << "no pads" << endl;
        }
        else
        {
          TIter next(padArray);
          AliMUONPad* pad;
          while ( ( pad = static_cast<AliMUONPad*>(next()) ) )
          {
            pad->Print("full");
          }
        }
      }
    }
    it.Next();
  }
}  


