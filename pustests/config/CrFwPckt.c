/**
 * @file
 * @ingroup CrTestSuiteGroup
 *
 * Default implementation of the packet interface of <code>CrFwPckt.h</code>.
 * The implementation of this interface is one of the adaptation points of the
 * CORDET Framework.
 * This file provides a default implementation which is primarily intended to
 * support testing.
 * It is expected that applications will provide their own implementation.
 * Application will therefore normally replace this file with their own file
 * providing their application-specific implementation.
 *
 * This implementation pre-allocates the memory for a predefined number of packets
 * of fixed size.
 * Packets can be either "in use" or "not in use".
 * A packet is in use if it has been requested through a call to <code>::CrFwPcktMake</code>
 * and has not yet been released through a call to <code>::CrFwPcktRelease</code>.
 *
 * The implementation provided in this file uses global data structures to hold
 * the pool of pre-allocated packets.
 *
 * A packet encapsulates a command or a report and it holds all the attributes of the
 * command or report.
 * The layout of a packet is defined by the value of the <code>offsetYyy</code> constants
 * which defines the offset within a packet at which attribute "Yyy" is stored.
 *
 * The setter functions for the packet attributes assume that the packet length is
 * adequate to hold the attributes.
 * Compliance with this constraint is not checked by the setter functions.
 * Its violation may result in memory corruption.
 *
 * @author Vaclav Cechticky <vaclav.cechticky@pnp-software.com>
 * @author Alessandro Pasetti <pasetti@pnp-software.com>
 * @copyright P&P Software GmbH, 2013, All Rights Reserved
 *
 * This file is part of CORDET Framework.
 *
 * CORDET Framework is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CORDET Framework is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with CORDET Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 * For information on alternative licensing, please contact P&P Software GmbH.
 */

#include <stdlib.h>
#include "CrFwConstants.h"
#include "UtilityFunctions/CrFwUtilityFunctions.h"
#include "Pckt/CrFwPckt.h"
#include "BaseCmp/CrFwBaseCmp.h"
#include <Services/General/CrPsPktServReqVerif.h>
#include <Services/General/CrPsPktHk.h>
#include <Services/General/CrPsPktEvt.h>
#include <Services/General/CrPsPktServTest.h>
#include "stdio.h"

/**
 * Maximum length of a packet expressed in number of bytes (see <code>CrFwPacket.h</code>).
 * The value of this constant must be smaller than the range of the <code>::CrFwPcktLength_t</code>
 * integer type.
 */
#define CR_FW_MAX_PACKET_LENGTH 100

/** The maximum size in number of bytes of a packet */
#define CR_FW_MAX_PCKT_LENGTH 100

/**
 * The array holding the packets.
 * Packets are stored in this array in blocks of size CR_FW_MAX_PCKT_LENGTH.
 */
static char pcktArray[CR_FW_MAX_NOF_PCKTS*CR_FW_MAX_PCKT_LENGTH];

/**
 * The array holding the "in use" status of the packets.
 * A packet is in use if it has been requested through a call to the "make" function
 * and has not yet been released through a call to the "release" function.
 */
static CrFwBool_t pcktInUse[CR_FW_MAX_NOF_PCKTS] = {0};

/** The number of currently allocated packets. */
static CrFwCounterU2_t nOfAllocatedPckts = 0;


#if 0
/** Offset of the length field in a packet */
static const CrFwPcktLength_t offsetLength = 0;

/** Offset of the flag defining the type of packet (1 for a command, 2 for a report) */
static const CrFwPcktLength_t offsetCmdRepType = 4;

/** Offset of the time stamp field in a packet */
static const CrFwPcktLength_t offsetTimeStamp = 8;

/** Offset of the service type field in a packet */
static const CrFwPcktLength_t offsetServType = 12;

/** Offset of the service sub-type field in a packet */
static const CrFwPcktLength_t offsetServSubType = 16;

/** Offset of the destination field in a packet */
static const CrFwPcktLength_t offsetDest = 20;

/** Offset of the source field in a packet */
static const CrFwPcktLength_t offsetSrc = 24;

/** Offset of the discriminant field in a packet */
static const CrFwPcktLength_t offsetDiscriminant = 28;

/** Offset of the sequence counter field in a packet */
static const CrFwPcktLength_t offsetSeqCnt = 32;

/** Offset of the command or report identifier in a packet */
static const CrFwPcktLength_t offsetCmdRepId = 36;

/** Offset of the acceptance acknowledge level field in a packet */
static const CrFwPcktLength_t offsetAcceptAckLev = 40;

/** Offset of the start acknowledge level field in a packet */
static const CrFwPcktLength_t offsetStartAckLev = 44;

/** Offset of the progress acknowledge level field in a packet */
static const CrFwPcktLength_t offsetProgressAckLev = 48;

/** Offset of the termination acknowledge level field in a packet */
static const CrFwPcktLength_t offsetTermAckLev = 52;

/** Offset of the group in a packet */
static const CrFwPcktLength_t offsetGroup = 56;

/** Offset of the parameter area in a packet */
static const CrFwPcktLength_t offsetPar = 60;



#endif
/*static const CrFwPcktLength_t offsetPar = sizeof(TmHeader_t);*/
/*internal functions*/
/*-----------------------------------------------------------------------------------------*/

CrFwGroup_t CrFwPcktGetPcat(CrFwPckt_t pckt) {
	unsigned short apid;
	if (CrFwPcktGetCmdRepType(pckt) == crRepType)
	{
		apid =  getTmHeaderAPID(pckt);
	}
	else
	{
		apid =  getTcHeaderAPID(pckt);
	}
	return (apid & 0x000f);
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetPcat(CrFwPckt_t pckt, CrFwGroup_t pcat) {
	unsigned short apid;
	if (CrFwPcktGetCmdRepType(pckt) == crRepType)
	{
		apid =  getTmHeaderAPID(pckt);
		setTmHeaderAPID (pckt, (apid & 0x07f0)|(pcat & 0x000f));
	}
	else
	{
		apid =  getTcHeaderAPID(pckt);
		setTcHeaderAPID (pckt, (apid & 0x07f0)|(pcat & 0x000f));
	}
}

/*-----------------------------------------------------------------------------------------*/
CrFwDestSrc_t CrFwPcktGetPid(CrFwPckt_t pckt)
{
	/*
	unsigned short apid, pckttype, secheaderflag;
	if (CrFwPcktGetCmdRepType(pckt) == crRepType)
	{
		apid = getTmHeaderAPID(pckt);
		pckttype = getTmHeaderPcktType(pckt);
	    secheaderflag = getTmHeaderSecHeaderFlag(pckt);
	}
	else
	{
		apid = getTcHeaderAPID(pckt);
		pckttype = getTcHeaderPcktType(pckt);
	    secheaderflag = getTcHeaderSecHeaderFlag(pckt);
	}
	return ((apid & 0x07f0) | (secheaderflag<<12 & 0x1000) | (pckttype<<13 & 0x0800));
	*/

	/*SO WIE IN CHEOPS (PID ist nur ein TEIL der Appid)*/
	/* maske ist 0x07f0 ( also 0000011111110000 ) */
	if (CrFwPcktGetCmdRepType(pckt) == crRepType)
	{
		return (getTmHeaderAPID(pckt) & 0x07f0) >> 4;
	}
	else
	{
		return (getTcHeaderAPID(pckt) & 0x07f0) >> 4;
	}
}


/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetPid(CrFwPckt_t pckt, unsigned short pid)
{
	/*SO WIE IN CHEOPS (PID ist nur ein TEIL der Appid)*/
	/* maske ist 0x07f0 ( also 0000011111110000 ) */
	unsigned short apid;
	if (CrFwPcktGetCmdRepType(pckt) == crRepType)
	{
		apid = getTmHeaderAPID(pckt);
		apid = (apid & 0xf80f) | ((pid<<4) & 0x07f0);
		setTmHeaderAPID(pckt, apid);
	}
	else
	{
		apid = getTcHeaderAPID(pckt);
		apid = (apid & 0xf80f) | ((pid<<4) & 0x07f0);
		setTcHeaderAPID(pckt, apid);
	}
}

/*CrFwPckt.h Functions*/
/*-----------------------------------------------------------------------------------------*/
CrFwPckt_t CrFwPcktMake(CrFwPcktLength_t pcktLength) {
	CrFwCounterU2_t i;
	printf("pcktLength%d\n",pcktLength);
	if (pcktLength > CR_FW_MAX_PCKT_LENGTH) {
		CrFwSetAppErrCode(crPcktAllocationFail);
		return NULL;
	}

	if (pcktLength < 1) {
		CrFwSetAppErrCode(crPcktAllocationFail);
		return NULL;
	}

	for (i=0; i<CR_FW_MAX_NOF_PCKTS; i++) {
		if (pcktInUse[i] == 0) {
			pcktInUse[i] = 1;
			setTmHeaderPcktDataLen((&pcktArray[i*CR_FW_MAX_PCKT_LENGTH]), pcktLength);
			nOfAllocatedPckts++;
			return (&pcktArray[i*CR_FW_MAX_PCKT_LENGTH]);
		}
	}
	CrFwSetAppErrCode(crPcktAllocationFail);
	return NULL;
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktRelease(CrFwPckt_t pckt) {
	CrFwCounterU2_t i;

	for (i=0; i<CR_FW_MAX_NOF_PCKTS; i++) {
		if (pckt == &pcktArray[i*CR_FW_MAX_PCKT_LENGTH]) {
			if (pcktInUse[i] == 0) {
				CrFwSetAppErrCode(crPcktRelErr);
			} else {
				nOfAllocatedPckts--;
				pcktInUse[i] = 0;
			}
			return;
		}
	}

	CrFwSetAppErrCode(crPcktRelErr);
	return;
}

/*-----------------------------------------------------------------------------------------*/
CrFwBool_t CrFwPcktIsAvail(CrFwPcktLength_t pcktLength) {
	CrFwCounterU2_t i;

	if (pcktLength > CR_FW_MAX_PCKT_LENGTH)
		return 0;

	if (pcktLength < 1)
		return 0;

	for (i=0; i<CR_FW_MAX_NOF_PCKTS; i++) {
		if (pcktInUse[i] == 0)
			return 1;
	}
	return 0;
}



/*-----------------------------------------------------------------------------------------*/
CrFwCounterU2_t CrFwPcktGetNOfAllocated() {
	return nOfAllocatedPckts;
}

/*-----------------------------------------------------------------------------------------*/
CrFwPcktLength_t CrFwPcktGetMaxLength() {
	return CR_FW_MAX_PCKT_LENGTH;
}

/*-----------------------------------------------------------------------------------------*/
CrFwPcktLength_t CrFwPcktGetLength(CrFwPckt_t pckt) {
	if (CrFwPcktGetCmdRepType(pckt) == crRepType)
	{
		return (CrFwPcktLength_t) getTmHeaderPcktDataLen(pckt);
	}
	else
	{
		return (CrFwPcktLength_t) getTcHeaderPcktDataLen(pckt);
	}
}

/*-----------------------------------------------------------------------------------------*/
CrFwCmdRepType_t CrFwPcktGetCmdRepType(CrFwPckt_t p){
	
	if (getTcHeaderPcktType(p) == 0)
	{
		return crRepType;
	}
	else
	{
		return crCmdType;
	}
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetCmdRepType(CrFwPckt_t pckt, CrFwCmdRepType_t type) {
	if (type == crRepType)
	{
		setTmHeaderPcktType(pckt, 0);
	}
	else
	{
		setTcHeaderPcktType(pckt, 1);
	}
}

CrFwInstanceId_t CrFwPcktGetCmdRepId(CrFwPckt_t pckt) {
	/* Field not present */
	(void)(pckt);
	return 0;
}

void CrFwPcktSetCmdRepId(CrFwPckt_t pckt, CrFwInstanceId_t id) {
	/* Field not present */
	(void)(pckt);
	(void)(id);
}

/*-----------------------------------------------------------------------------------------*/
CrFwSeqCnt_t CrFwPcktGetSeqCnt(CrFwPckt_t pckt) {
	if (CrFwPcktGetCmdRepType(pckt) == crRepType)
	{
		return (CrFwSeqCnt_t) getTmHeaderSeqCount(pckt);
	}
	else
	{
		return (CrFwSeqCnt_t) getTcHeaderSeqCount(pckt);
	}
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetSeqCnt(CrFwPckt_t pckt, CrFwSeqCnt_t seqCnt) {
	if (CrFwPcktGetCmdRepType(pckt) == crRepType)
	{
		setTmHeaderSeqCount(pckt, seqCnt);
	}
	else
	{
		setTcHeaderSeqCount(pckt, seqCnt);
	}	
}

/*-----------------------------------------------------------------------------------------*/
CrFwTimeStamp_t CrFwPcktGetTimeStamp(CrFwPckt_t pckt) {
	CrFwTimeStamp_t time;

	getTmHeaderTime(pckt, &time);

	return time;
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetTimeStamp(CrFwPckt_t pckt, CrFwTimeStamp_t timeStamp) {

	setTmHeaderTime(pckt, &timeStamp);
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetServType(CrFwPckt_t pckt, CrFwServType_t servType) {
	if (CrFwPcktGetCmdRepType(pckt) == crRepType)
	{
		setTmHeaderServType(pckt, servType);
	}
	else
	{
		setTcHeaderServType(pckt, servType);
	}
}

/*-----------------------------------------------------------------------------------------*/
CrFwServType_t CrFwPcktGetServType(CrFwPckt_t pckt) {
	if (CrFwPcktGetCmdRepType(pckt) == crRepType)
	{
		return (CrFwServSubType_t) getTmHeaderServType(pckt);
	}
	else
	{
		return (CrFwServSubType_t) getTcHeaderServType(pckt);
	}
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetServSubType(CrFwPckt_t pckt, CrFwServSubType_t servSubType) {
	if (CrFwPcktGetCmdRepType(pckt) == crRepType)
	{
		setTmHeaderServSubType(pckt, servSubType);
	}
	else
	{
		setTcHeaderServSubType(pckt, servSubType);
	}
}

/*-----------------------------------------------------------------------------------------*/
CrFwServSubType_t CrFwPcktGetServSubType(CrFwPckt_t pckt) {
	if (CrFwPcktGetCmdRepType(pckt) == crRepType)
	{
		return (CrFwServSubType_t) getTmHeaderServSubType(pckt);
	}
	else
	{
		return (CrFwServSubType_t) getTcHeaderServSubType(pckt);
	}
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetDiscriminant(CrFwPckt_t pckt, CrFwDiscriminant_t discriminant) {


	if (CrFwPcktGetServType(pckt) == 1)
	{
		if(CrFwPcktGetServSubType(pckt) == 2)
		{
			/*Packet 1.2 - Failed Acceptance Verification Report*/
			setVerFailedAccRepTcFailureCode(pckt, discriminant);			
		}
		if(CrFwPcktGetServSubType(pckt) == 4)
		{
			/*Packet 1.4 - Failed Start of Execution Verification Report*/
			setVerFailedStartRepTcFailureCode(pckt, discriminant);			
		}	
		if(CrFwPcktGetServSubType(pckt) == 6)
		{
			/*Packet 1.6 - Failed Progress of Execution Verification Report*/
			setVerFailedPrgrRepTcFailureCode(pckt, discriminant);			
		}
		if(CrFwPcktGetServSubType(pckt) == 8)
		{
			/*Packet 1.8 - Failed Completion of Execution Verification Report*/
			setVerFailedTermRepTcFailureCode(pckt, discriminant);			
		}
		if(CrFwPcktGetServSubType(pckt) == 10)
		{
			/*Packet 1.10 - Failed Routing Verification Report*/
			setVerFailedRoutingRepTcFailureCode(pckt, discriminant);
		}
	}
	/*Service 3*/
	if (CrFwPcktGetServType(pckt) == 3)
	{
		if(CrFwPcktGetServSubType(pckt) == 10)
		{
			/*Packet 3.10 - Housekeeping Parameter Report Structure Report*/
			setHkRepStructHkParRepHKRepStrucID(pckt, discriminant);
		}
		if(CrFwPcktGetServSubType(pckt) == 12)
		{
			/*Packet 3.12 - Diagnostic Parameter Report Structure Report*/
			setHkRepStructDiagParRepDiagRepStrucID(pckt, discriminant);
		}
		if(CrFwPcktGetServSubType(pckt) == 25)
		{
			/*Packet 3.25 - Housekeeping Parameter Report*/
			setHkOneShotHkRepHKRepStrucID(pckt, discriminant);
		}
		if(CrFwPcktGetServSubType(pckt) == 26)
		{
			/*Packet 3.26 - Diagnostic Parameter Report*/
			setHkOneShotDiagRepDiagRepStrucID(pckt, discriminant);
		}
		if(CrFwPcktGetServSubType(pckt) == 27)
		{
			/*Packet 3.27 - Generate One-Shot Report for Housekeeping Parameters*/
			setHkOneShotHkCmdHKRepStrucID(pckt, discriminant);
		}
		if(CrFwPcktGetServSubType(pckt) == 28)
		{
			/*Packet 3.28 - Generate One-Shot Report for Diagnostic Parameters*/
			setHkOneShotDiagCmdDiagRepStrucID(pckt, discriminant);
		}
	}

/*Service 5 */
	if (CrFwPcktGetServType(pckt) == 5)
	{
		if(CrFwPcktGetServSubType(pckt) == 1)
		{
			/*Packet 5.1 - Informative Event Report (Level 1)*/
			setEvtRep1EventD(pckt, discriminant);
		}
		if(CrFwPcktGetServSubType(pckt) == 2)
		{
			/*Packet 5.2 - Low Severity Event Report (Level 2)*/
			setEvtRep2EventD(pckt, discriminant);			
		}
		if(CrFwPcktGetServSubType(pckt) == 3)
		{
			/*Packet 5.3 - Medium Severity Event Report (Level 3)*/
			setEvtRep3EventD(pckt, discriminant);
		}
		if(CrFwPcktGetServSubType(pckt) == 4)
		{
			/*Packet 5.4 - High Severity Event Report (Level 4)*/
			setEvtRep4EventD(pckt, discriminant);			
		}	
	}

/*TODO add all IF'S (service 1 .. the failcode acts as discriminant*/
	
}

/*-----------------------------------------------------------------------------------------*/
CrFwDiscriminant_t CrFwPcktGetDiscriminant(CrFwPckt_t pckt) {

/*TODO add all IF'S (service 1 .. the failcode acts as discriminant*/
	if (CrFwPcktGetServType(pckt) == 1)
	{
		if(CrFwPcktGetServSubType(pckt) == 2)
		{
			/*Packet 1.2 - Failed Acceptance Verification Report*/
			return (CrFwDiscriminant_t) getVerFailedAccRepTcFailureCode(pckt);			
		}
		if(CrFwPcktGetServSubType(pckt) == 4)
		{
			/*Packet 1.4 - Failed Start of Execution Verification Report*/
			return (CrFwDiscriminant_t) getVerFailedStartRepTcFailureCode(pckt);			
		}	
		if(CrFwPcktGetServSubType(pckt) == 6)
		{
			/*Packet 1.6 - Failed Progress of Execution Verification Report*/
			return (CrFwDiscriminant_t) getVerFailedPrgrRepTcFailureCode(pckt);			
		}
		if(CrFwPcktGetServSubType(pckt) == 8)
		{
			/*Packet 1.8 - Failed Completion of Execution Verification Report*/
			return (CrFwDiscriminant_t) getVerFailedTermRepTcFailureCode(pckt);			
		}
		if(CrFwPcktGetServSubType(pckt) == 10)
		{
			/*Packet 1.10 - Failed Routing Verification Report*/
			return (CrFwDiscriminant_t) getVerFailedRoutingRepTcFailureCode(pckt);
		}
	}

/*Service 3*/
	if (CrFwPcktGetServType(pckt) == 3)
	{
		if(CrFwPcktGetServSubType(pckt) == 10)
		{
			/*Packet 3.10 - Housekeeping Parameter Report Structure Report*/
			return (CrFwDiscriminant_t) getHkRepStructHkParRepHKRepStrucID(pckt);
		}
		if(CrFwPcktGetServSubType(pckt) == 12)
		{
			/*Packet 3.12 - Diagnostic Parameter Report Structure Report*/
			return (CrFwDiscriminant_t) getHkRepStructDiagParRepDiagRepStrucID(pckt);
		}
		if(CrFwPcktGetServSubType(pckt) == 25)
		{
			/*Packet 3.25 - Housekeeping Parameter Report*/
			return (CrFwDiscriminant_t) getHkOneShotHkRepHKRepStrucID(pckt);
		}
		if(CrFwPcktGetServSubType(pckt) == 26)
		{
			/*Packet 3.26 - Diagnostic Parameter Report*/
			return (CrFwDiscriminant_t) getHkOneShotDiagRepDiagRepStrucID(pckt);
		}
		if(CrFwPcktGetServSubType(pckt) == 27)
		{
			/*Packet 3.27 - Generate One-Shot Report for Housekeeping Parameters*/
			return (CrFwDiscriminant_t) getHkOneShotHkCmdHKRepStrucID(pckt);
		}
		if(CrFwPcktGetServSubType(pckt) == 28)
		{
			/*Packet 3.28 - Generate One-Shot Report for Diagnostic Parameters*/
			return (CrFwDiscriminant_t) getHkOneShotDiagCmdDiagRepStrucID(pckt);
		}
	}

/*Service 5 */
	if (CrFwPcktGetServType(pckt) == 5)
	{
		if(CrFwPcktGetServSubType(pckt) == 1)
		{
			/*Packet 5.1 - Informative Event Report (Level 1)*/
			return (CrFwDiscriminant_t) getEvtRep1EventD(pckt);
		}
		if(CrFwPcktGetServSubType(pckt) == 2)
		{
			/*Packet 5.2 - Low Severity Event Report (Level 2)*/
			return (CrFwDiscriminant_t) getEvtRep2EventD(pckt);			
		}
		if(CrFwPcktGetServSubType(pckt) == 3)
		{
			/*Packet 5.3 - Medium Severity Event Report (Level 3)*/
			return (CrFwDiscriminant_t) getEvtRep3EventD(pckt);
		}
		if(CrFwPcktGetServSubType(pckt) == 4)
		{
			/*Packet 5.4 - High Severity Event Report (Level 4)*/
			return (CrFwDiscriminant_t) getEvtRep4EventD(pckt);			
		}	
	}

	return 0;
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetDest(CrFwPckt_t pckt, CrFwDestSrc_t dest) {
	if (CrFwPcktGetCmdRepType(pckt) == crRepType)
	{
		setTmHeaderDestId(pckt, dest);
	}
	else
	{
		CrFwPcktSetPid(pckt, dest);
	}
}

/*-----------------------------------------------------------------------------------------*/
CrFwDestSrc_t CrFwPcktGetDest(CrFwPckt_t pckt) {
	if (CrFwPcktGetCmdRepType(pckt) == crRepType)
	{
		return (CrFwDestSrc_t) getTmHeaderDestId(pckt);
	}
	else
	{
		return (CrFwDestSrc_t) CrFwPcktGetPid(pckt);
	}
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetSrc(CrFwPckt_t pckt, CrFwDestSrc_t src) {
	if (CrFwPcktGetCmdRepType(pckt) == crCmdType)
	{
		setTcHeaderSrcId(pckt, src);
	}
	else
	{
		CrFwPcktSetPid(pckt, src);
	}
}

/*-----------------------------------------------------------------------------------------*/
CrFwDestSrc_t CrFwPcktGetSrc(CrFwPckt_t pckt) {
	if (CrFwPcktGetCmdRepType(pckt) == crCmdType)
	{
		return (CrFwDestSrc_t) getTcHeaderSrcId(pckt);
	}
	else
	{
		return (CrFwDestSrc_t) CrFwPcktGetPid(pckt);
	}
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetAckLevel(CrFwPckt_t pckt, CrFwBool_t accept, CrFwBool_t start,
                         CrFwBool_t progress, CrFwBool_t term) {
	if (CrFwPcktGetCmdRepType(pckt) == crCmdType)
	{
		setTcHeaderAckAccFlag(pckt, accept);
		setTcHeaderAckStartFlag(pckt, start);
		setTcHeaderAckProgFlag(pckt, progress);
		setTcHeaderAckTermFlag(pckt, term);
	}
}

/*-----------------------------------------------------------------------------------------*/
CrFwBool_t CrFwPcktIsAcceptAck(CrFwPckt_t pckt) {
	if (CrFwPcktGetCmdRepType(pckt) == crCmdType)
	{
		return (CrFwBool_t) getTcHeaderAckAccFlag(pckt);
	}
	return 0;
}

/*-----------------------------------------------------------------------------------------*/
CrFwBool_t CrFwPcktIsStartAck(CrFwPckt_t pckt) {
	if (CrFwPcktGetCmdRepType(pckt) == crCmdType)
	{
		return (CrFwBool_t) getTcHeaderAckStartFlag(pckt);
	}
	return 0;
}

/*-----------------------------------------------------------------------------------------*/
CrFwBool_t CrFwPcktIsProgressAck(CrFwPckt_t pckt) {
	if (CrFwPcktGetCmdRepType(pckt) == crCmdType)
	{
		return (CrFwBool_t) getTcHeaderAckProgFlag(pckt);
	}
	return 0;
}

/*-----------------------------------------------------------------------------------------*/
CrFwBool_t CrFwPcktIsTermAck(CrFwPckt_t pckt) {
	if (CrFwPcktGetCmdRepType(pckt) == crCmdType)
	{
		return (CrFwBool_t) getTcHeaderAckTermFlag(pckt);
	}
	return 0;
}

/*-----------------------------------------------------------------------------------------*/
char* CrFwPcktGetParStart(CrFwPckt_t pckt) {
	if (CrFwPcktGetCmdRepType(pckt) == crRepType)
	{
		return (char *) &pckt[sizeof(TmHeader_t)];
	}
	else
	{
		return (char *) &pckt[sizeof(TcHeader_t)];
	}
}

/*-----------------------------------------------------------------------------------------*/
CrFwPcktLength_t CrFwPcktGetParLength(CrFwPckt_t pckt) {
	if (CrFwPcktGetCmdRepType(pckt) == crRepType)
	{
		return (CrFwPcktLength_t) (CrFwPcktGetLength(pckt)-sizeof(TmHeader_t));
	}
	else
	{
		return (CrFwPcktLength_t) (CrFwPcktGetLength(pckt)-sizeof(TcHeader_t));
	}
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetGroup(CrFwPckt_t pckt, CrFwGroup_t group) {
	CrFwPcktSetPcat(pckt, group+1);
}

/*-----------------------------------------------------------------------------------------*/
CrFwGroup_t CrFwPcktGetGroup(CrFwPckt_t pckt) {
	return CrFwPcktGetPcat(pckt)-1;
}




