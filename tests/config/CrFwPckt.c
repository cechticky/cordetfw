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

/*-----------------------------------------------------------------------------------------*/
CrFwPckt_t CrFwPcktMake(CrFwPcktLength_t pcktLength) {
	CrFwCounterU2_t i;

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
			pcktArray[i*CR_FW_MAX_PCKT_LENGTH] = (char)pcktLength;
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
	return (CrFwPcktLength_t)pckt[offsetLength];
}

/*-----------------------------------------------------------------------------------------*/
CrFwCmdRepType_t CrFwPcktGetCmdRepType(CrFwPckt_t pckt) {
	CrFwBool_t* loc = (CrFwBool_t*)(pckt+offsetCmdRepType);
	return (*loc);
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetCmdRepType(CrFwPckt_t pckt, CrFwCmdRepType_t type) {
	CrFwBool_t* loc = (CrFwBool_t*)(pckt+offsetCmdRepType);
	(*loc) = type;
}

/*-----------------------------------------------------------------------------------------*/
CrFwSeqCnt_t CrFwPcktGetSeqCnt(CrFwPckt_t pckt) {
	CrFwSeqCnt_t* loc = (CrFwSeqCnt_t*)(pckt+offsetSeqCnt);
	return (*loc);
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetSeqCnt(CrFwPckt_t pckt, CrFwSeqCnt_t seqCnt) {
	CrFwSeqCnt_t* loc = (CrFwSeqCnt_t*)(pckt+offsetSeqCnt);
	(*loc) = seqCnt;
}

/*-----------------------------------------------------------------------------------------*/
CrFwTimeStamp_t CrFwPcktGetTimeStamp(CrFwPckt_t pckt) {
	CrFwTimeStamp_t* loc = (CrFwTimeStamp_t*)(pckt+offsetTimeStamp);
	return (*loc);
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetTimeStamp(CrFwPckt_t pckt, CrFwTimeStamp_t timeStamp) {
	CrFwTimeStamp_t* loc = (CrFwTimeStamp_t*)(pckt+offsetTimeStamp);
	(*loc) = timeStamp;
}

/*-----------------------------------------------------------------------------------------*/
CrFwDiscriminant_t CrFwPcktGetDiscriminant(CrFwPckt_t pckt) {
	CrFwDiscriminant_t* loc = (CrFwDiscriminant_t*)(pckt+offsetDiscriminant);
	return (*loc);
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetDiscriminant(CrFwPckt_t pckt, CrFwDiscriminant_t discriminant) {
	CrFwDiscriminant_t* loc = (CrFwDiscriminant_t*)(pckt+offsetDiscriminant);
	(*loc) = discriminant;
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetServType(CrFwPckt_t pckt, CrFwServType_t servType) {
	CrFwServType_t* loc = (CrFwServType_t*)(pckt+offsetServType);
	(*loc) = servType;
}

/*-----------------------------------------------------------------------------------------*/
CrFwServType_t CrFwPcktGetServType(CrFwPckt_t pckt) {
	CrFwServSubType_t* loc = (CrFwServSubType_t*)(pckt+offsetServType);
	return (*loc);
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetServSubType(CrFwPckt_t pckt, CrFwServSubType_t servSubType) {
	CrFwServSubType_t* loc = (CrFwServSubType_t*)(pckt+offsetServSubType);
	(*loc) = servSubType;
}

/*-----------------------------------------------------------------------------------------*/
CrFwServSubType_t CrFwPcktGetServSubType(CrFwPckt_t pckt) {
	CrFwServSubType_t* loc = (CrFwServSubType_t*)(pckt+offsetServSubType);
	return (*loc);
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetDest(CrFwPckt_t pckt, CrFwDestSrc_t dest) {
	CrFwDestSrc_t* loc = (CrFwDestSrc_t*)(pckt+offsetDest);
	(*loc) = dest;
}

/*-----------------------------------------------------------------------------------------*/
CrFwDestSrc_t CrFwPcktGetDest(CrFwPckt_t pckt) {
	CrFwDestSrc_t* loc = (CrFwDestSrc_t*)(pckt+offsetDest);
	return (*loc);
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetSrc(CrFwPckt_t pckt, CrFwDestSrc_t src) {
	CrFwDestSrc_t* loc = (CrFwDestSrc_t*)(pckt+offsetSrc);
	(*loc) = src;
}

/*-----------------------------------------------------------------------------------------*/
CrFwDestSrc_t CrFwPcktGetSrc(CrFwPckt_t pckt) {
	CrFwDestSrc_t* loc = (CrFwDestSrc_t*)(pckt+offsetSrc);
	return (*loc);
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetCmdRepId(CrFwPckt_t pckt, CrFwInstanceId_t id) {
	CrFwInstanceId_t* loc = (CrFwInstanceId_t*)(pckt+offsetCmdRepId);
	(*loc) = id;
}

/*-----------------------------------------------------------------------------------------*/
CrFwInstanceId_t CrFwPcktGetCmdRepId(CrFwPckt_t pckt) {
	CrFwInstanceId_t* loc = (CrFwInstanceId_t*)(pckt+offsetCmdRepId);
	return (*loc);
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetAckLevel(CrFwPckt_t pckt, CrFwBool_t accept, CrFwBool_t start,
                         CrFwBool_t progress, CrFwBool_t term) {
	CrFwBool_t* loc = (CrFwBool_t*)(pckt+offsetAcceptAckLev);
	(*loc) = accept;
	loc = (CrFwBool_t*)(pckt+offsetStartAckLev);
	(*loc) = start;
	loc = (CrFwBool_t*)(pckt+offsetProgressAckLev);
	(*loc) = progress;
	loc = (CrFwBool_t*)(pckt+offsetTermAckLev);
	(*loc) = term;
}

/*-----------------------------------------------------------------------------------------*/
CrFwBool_t CrFwPcktIsAcceptAck(CrFwPckt_t pckt) {
	CrFwBool_t* loc = (CrFwBool_t*)(pckt+offsetAcceptAckLev);
	return (*loc);
}

/*-----------------------------------------------------------------------------------------*/
CrFwBool_t CrFwPcktIsStartAck(CrFwPckt_t pckt) {
	CrFwBool_t* loc = (CrFwBool_t*)(pckt+offsetStartAckLev);
	return (*loc);
}

/*-----------------------------------------------------------------------------------------*/
CrFwBool_t CrFwPcktIsProgressAck(CrFwPckt_t pckt) {
	CrFwBool_t* loc = (CrFwBool_t*)(pckt+offsetProgressAckLev);
	return (*loc);
}

/*-----------------------------------------------------------------------------------------*/
CrFwBool_t CrFwPcktIsTermAck(CrFwPckt_t pckt) {
	CrFwBool_t* loc = (CrFwBool_t*)(pckt+offsetTermAckLev);
	return (*loc);
}

/*-----------------------------------------------------------------------------------------*/
char* CrFwPcktGetParStart(CrFwPckt_t pckt) {
	return (char*)(pckt+offsetPar);
}

/*-----------------------------------------------------------------------------------------*/
CrFwPcktLength_t CrFwPcktGetParLength(CrFwPckt_t pckt) {
	return (CrFwPcktLength_t)(CrFwPcktGetLength(pckt)-offsetPar);
}

/*-----------------------------------------------------------------------------------------*/
void CrFwPcktSetGroup(CrFwPckt_t pckt, CrFwGroup_t group) {
	CrFwGroup_t* loc = (CrFwGroup_t*)(pckt+offsetGroup);
	(*loc) = group;
}

/*-----------------------------------------------------------------------------------------*/
CrFwGroup_t CrFwPcktGetGroup(CrFwPckt_t pckt) {
	CrFwGroup_t* loc = (CrFwGroup_t*)(pckt+offsetGroup);
	return (*loc);
}
