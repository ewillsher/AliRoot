#ifndef ALIHLTMUONCONSTANTS_H
#define ALIHLTMUONCONSTANTS_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

/**
 * @file   AliHLTMUONConstants.h
 * @author Indranil Das <indra.das@saha.ac.in>,
 *         Artur Szostak <artursz@iafrica.com>
 * @date   
 * @brief  Class containing various dimuon HLT constants used in the system.
 */

#include "AliHLTDataTypes.h"
#include "AliHLTMUONRecHitsBlockStruct.h"
#include "AliHLTMUONRecHitsDebugBlockStruct.h"

/**
 * AliHLTMUONConstants contains a list of global dimuon HLT specific constants
 * and constant structures used in the system.
 * Static methods are provided to access these values.
 */
class AliHLTMUONConstants
{
public:

	static const AliHLTMUONRecHitStruct& NilRecHitStruct()
	{
		return fgkNilRecHitStruct;
	}

	static const AliHLTMUONChannelInfoStruct& NilChannelInfoStruct()
	{
		return fgkNilChannelInfoStruct;
	}

	static const AliHLTMUONClusterInfoStruct& NilClusterInfoStruct()
	{
		return fgkNilClusterInfoStruct;
	}

	static const AliHLTComponentDataType& TriggerDDLStreamDataType()
	{
		return fgkTriggerDDLStreamDataType;
	}

	static const AliHLTComponentDataType& TrackingDDLStreamDataType()
	{
		return fgkTrackingDDLStreamDataType;
	}

	static const AliHLTComponentDataType& RecHitsBlockDataType()
	{
		return fgkRecHitsBlockDataType;
	}

	static const AliHLTComponentDataType& RecHitsDebugBlockDataType()
	{
		return fgkRecHitsDebugBlockDataType;
	}

	static const AliHLTComponentDataType& TriggerRecordsBlockDataType()
	{
		return fgkTriggerRecordsBlockDataType;
	}

	static const AliHLTComponentDataType& TriggerRecordsDebugBlockDataType()
	{
		return fgkTriggerRecordsDebugBlockDataType;
	}

	static const AliHLTComponentDataType& MansoTracksBlockDataType()
	{
		return fgkMansoTracksBlockDataType;
	}

	static const AliHLTComponentDataType& MansoTracksDebugBlockDataType()
	{
		return fgkMansoTracksDebugBlockDataType;
	}

	static const AliHLTComponentDataType& DecisionBlockDataType()
	{
		return fgkDecisionBlockDataType;
	}

	static const AliHLTComponentDataType& DecisionDebugBlockDataType()
	{
		return fgkDecisionDebugBlockDataType;
	}

private:
	// Should never have to create or destroy this object.
	AliHLTMUONConstants();
	~AliHLTMUONConstants();

	// Sentinel structure for a reconstructed hit.
	static const AliHLTMUONRecHitStruct fgkNilRecHitStruct;

	// Sentinel structure for channel information.
	static const AliHLTMUONChannelInfoStruct fgkNilChannelInfoStruct;

	// Sentinel structure for cluster information.
	static const AliHLTMUONClusterInfoStruct fgkNilClusterInfoStruct;

	// DDL packed data block type from dimuon trigger stations.
	static const AliHLTComponentDataType fgkTriggerDDLStreamDataType;

	// DDL packed data block type from dimuon tracking stations.
	static const AliHLTComponentDataType fgkTrackingDDLStreamDataType;

	// Reconstructed hits block type generated by hit reconstruction components.
	static const AliHLTComponentDataType fgkRecHitsBlockDataType;

	// Debugging information block type generated by hit reconstruction components.
	static const AliHLTComponentDataType fgkRecHitsDebugBlockDataType;

	// Trigger records block type generated by trigger DDL translation components.
	static const AliHLTComponentDataType fgkTriggerRecordsBlockDataType;

	// Debugging information block type generated by trigger DDL translation components.
	static const AliHLTComponentDataType fgkTriggerRecordsDebugBlockDataType;

	// Manso tracks block type generated by Manso tracker components.
	static const AliHLTComponentDataType fgkMansoTracksBlockDataType;

	// Debugging information block type generated by Manso tracker components.
	static const AliHLTComponentDataType fgkMansoTracksDebugBlockDataType;

	// Trigger decision block type generated by dimuon HLT trigger components.
	static const AliHLTComponentDataType fgkDecisionBlockDataType;

	// Debugging information block type generated by dimuon HLT trigger components.
	static const AliHLTComponentDataType fgkDecisionDebugBlockDataType;
};

#endif // ALIHLTMUONCONSTANTS_H
