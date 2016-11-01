/**
 * @file
 *
 * Implementation of OutRegistry component.
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
/* Include configuration files */
#include "CrFwOutRegistryUserPar.h"
#include "CrFwCmpData.h"
/* Include framework files */
#include "CrFwConstants.h"
#include "UtilityFunctions/CrFwUtilityFunctions.h"
#include "OutCmp/CrFwOutCmp.h"
#include "BaseCmp/CrFwBaseCmp.h"
#include "BaseCmp/CrFwInitProc.h"
#include "BaseCmp/CrFwResetProc.h"
#include "BaseCmp/CrFwDummyExecProc.h"
#include "CrFwOutRegistry.h"
/* Include FW Profile files */
#include "FwPrConfig.h"
#include "FwPrDCreate.h"
#include "FwSmConfig.h"
#include "FwSmDCreate.h"
#include "FwPrCore.h"

/** Array of service descriptors (see <code>CrFwServDesc_t</code>). */
static CrFwServDesc_t servDesc[CR_FW_OUTREGISTRY_NSERV] = CR_FW_OUTREGISTRY_INIT_SERV_DESC;

/**
 * An instance of this type holds the information about an out-going command or report
 * which is being tracked by the OuRegistry.
 */
typedef struct {
	/** The identifier of the out-going command or report */
	CrFwInstanceId_t instanceId;
	/** The state of the out-going command or report */
	CrFwOutRegistryCmdRepState_t state;
} CrFwTrackedState_t;

/** Array to track the state of out-going commands or reports */
static CrFwTrackedState_t cmdRepState[CR_FW_OUTREGISTRY_N];

/** The index of the most recent entry in <code>cmdRepState</code>. */
static CrFwTrackingIndex_t cmdRepStateIndex = 0;

/** The OutRegistry singleton. */
static FwSmDesc_t outRegistry = NULL;

/** The data for the OutRegistry singleton. */
static CrFwCmpData_t outRegistryData;

/**
 * Initialization action for OutRegistry.
 * This function allocates the memory for the OutRegistry data structures and
 * initializes all data structures which depend on the set of services supported
 * by the application.
 * The outcome of the initialization action is always "success".
 * The situation where the memory allocation for the OutRegistry data structures
 * fails is not handled and will result in undefined behaviour (probably a run-time
 * exception).
 * @param initPr the Initialization Procedure of the OutRegistry
 */
static void OutRegistryInitAction(FwPrDesc_t initPr);

/**
 * Configuration action for OutRegistry.
 * This function sets the enable state of all service types, sub-types and discriminant
 * to "enabled" and resets the queue of tracked commands and reports.
 * The outcome of the configuration action is always: "successful"
 * @param resetPr the Configuration Procedure of the OutRegistry
 */
static void OutRegistryConfigAction(FwPrDesc_t resetPr);

/**
 * Shutdown action for OutRegistry.
 * This function sets the enable state of all service types, sub-types and discriminant
 * to "enabled" and resets the queue of tracked commands and reports.
 * This function also releases the memory allocated when the OutRegistry was initialized.
 * @param smDesc the OutRegistry state machine
 */
static void OutRegistryShutdownAction(FwSmDesc_t smDesc);

/*------------------------------------------------------------------------------------*/
FwSmDesc_t CrFwOutRegistryMake() {
	FwPrDesc_t resetPr, execPr, initPr;

	if (outRegistry != NULL) {
		return outRegistry;
	}

	/* Extend the Base Component */
	outRegistry = FwSmCreateDer(CrFwBaseCmpMake());

	/* Create the Reset Procedure for the OuRegistry Component */
	resetPr = FwPrCreateDer(CrFwCmpGetResetProc());
	FwPrOverrideAction(resetPr, &CrFwBaseCmpDefConfigAction, &OutRegistryConfigAction);

	/* Create the Initialization Procedure for the OuRegistry Component */
	initPr = FwPrCreateDer(CrFwCmpGetInitProc());
	FwPrOverrideAction(initPr, &CrFwBaseCmpDefInitAction, &OutRegistryInitAction);

	/* Override the Shutdown Action for the InStream Component */
	FwSmOverrideAction(outRegistry, &CrFwBaseCmpDefShutdownAction, &OutRegistryShutdownAction);

	/* Get the Dummy Execution Procedure for the OutRegistry Component */
	execPr = CrFwBaseCmpGetDummyExecProc();

	/* Initialize the data for the requested SM */
	outRegistryData.outcome = 1;
	outRegistryData.initProc = initPr;
	outRegistryData.resetProc = resetPr;
	outRegistryData.execProc = execPr;
	outRegistryData.instanceId = 0;
	outRegistryData.typeId = CR_FW_OUTREGISTRY_TYPE;

	/* Attach the data to the OutRegistry state machine and to its procedures. */
	FwSmSetData(outRegistry, &outRegistryData);
	FwPrSetData(outRegistryData.initProc, &outRegistryData);
	FwPrSetData(outRegistryData.resetProc, &outRegistryData);

	/* Start the OutRegistry */
	FwSmStart(outRegistry);

	return outRegistry;
}

/*------------------------------------------------------------------------------------*/
CrFwServType_t CrFwOutRegistryGetServType(CrFwCmdRepIndex_t cmdRepIndex) {
	return servDesc[cmdRepIndex].servType;
}

/*------------------------------------------------------------------------------------*/
CrFwServSubType_t CrFwOutRegistryGetServSubType(CrFwCmdRepIndex_t cmdRepIndex) {
	return servDesc[cmdRepIndex].servSubType;
}

/*------------------------------------------------------------------------------------*/
CrFwCmdRepIndex_t CrFwOutRegistryGetCmdRepIndex(CrFwServType_t servType, CrFwServSubType_t servSubType) {
	CrFwCmdRepIndex_t i = 0;

	while (servDesc[i].nextServType != 0)
		if (servDesc[i].servType < servType)
			i = servDesc[i].nextServType;
		else if (servDesc[i].servType > servType)
			return CR_FW_OUTREGISTRY_NSERV;	/* The specified type does not exist */
		else
			break;

	if (servDesc[i].servType != servType)
		return CR_FW_OUTREGISTRY_NSERV;	/* The specified type does not exist */

	while (servDesc[i].servSubType <= servSubType) {
		if (servDesc[i].servSubType == servSubType)
			return i;	/* Cmd-Rep index found! */
		i++;
		if (servDesc[i].servType != servType)
			return CR_FW_OUTREGISTRY_NSERV; /* The specified sub-type does not exist in the specified type */
	}

	return CR_FW_OUTREGISTRY_NSERV; /* The specified sub-type does not exist in the specified type */
}

/*------------------------------------------------------------------------------------*/
void CrFwOutRegistrySetEnable(CrFwServType_t servType, CrFwServSubType_t servSubType,
                              CrFwDiscriminant_t discriminant, CrFwBool_t isEnabled) {
	CrFwCmdRepIndex_t i = 0;
	CrFwDiscriminant_t byteIndex, bitPos;
	unsigned char temp;

	while (servDesc[i].nextServType != 0)
		if (servDesc[i].servType < servType)
			i = servDesc[i].nextServType;
		else if (servDesc[i].servType > servType) {
			CrFwSetAppErrCode(crIllServType);
			return;
		} else
			break;

	if (servDesc[i].servType != servType) {
		CrFwSetAppErrCode(crIllServType);
		return;
	}

	if (servSubType == 0) {
		while (servDesc[i].servType == servType) {
			servDesc[i].isServTypeEnabled = isEnabled;
			i++;
			if (i == CR_FW_OUTREGISTRY_NSERV)
				return;
		}
		return;
	}

	while (servDesc[i].servType == servType) {
		if (servDesc[i].servSubType < servSubType)
			i++;
		else if (servDesc[i].servSubType > servSubType) {
			CrFwSetAppErrCode(crIllServSubType);
			return;
		} else
			break;
	}

	if (servDesc[i].servSubType < servSubType) {
		CrFwSetAppErrCode(crIllServSubType);
		return;
	}

	if (discriminant == 0) {
		servDesc[i].isServSubTypeEnabled = isEnabled;
		return;
	}

	if (discriminant > servDesc[i].maxDiscriminant)
		CrFwSetAppErrCode(crIllDiscriminant);
	else {
		byteIndex = discriminant/(8*sizeof(char));
		bitPos = (CrFwDiscriminant_t)(discriminant - byteIndex*8*(CrFwDiscriminant_t)sizeof(char));
		if (isEnabled == 1)
			servDesc[i].isDiscriminantEnabled[byteIndex] = servDesc[i].isDiscriminantEnabled[byteIndex] | (unsigned char)(1 << (bitPos-1));
		else {
			temp = (unsigned char)(~(1 << (bitPos-1)));
			servDesc[i].isDiscriminantEnabled[byteIndex] = servDesc[i].isDiscriminantEnabled[byteIndex] & temp;
		}
	}
}

/*------------------------------------------------------------------------------------*/
CrFwBool_t CrFwOutRegistryIsEnabled(FwSmDesc_t outCmp) {
	CrFwCmpData_t* cmpData = (CrFwCmpData_t*)FwSmGetData(outCmp);
	CrFwOutCmpData_t* cmpSpecificData = (CrFwOutCmpData_t*)cmpData->cmpSpecificData;
	CrFwCmdRepIndex_t cmdRepIndex;
	CrFwDiscriminant_t byteIndex, bitPos, discriminant;
	unsigned char enableByte;
	/*CrFwOutCmpData_t* outCmpData = &(((CrFwCmpData_t*)FwSmGetData(outCmp))->cmpSpecificData.outCmpData);*/

	cmdRepIndex = cmpSpecificData->index;
	if (servDesc[cmdRepIndex].isServTypeEnabled == 0)
		return 0;

	if (servDesc[cmdRepIndex].isServSubTypeEnabled == 0)
		return 0;

	discriminant = CrFwOutCmpGetDiscriminant(outCmp);
	if (discriminant == 0)
		return 1;
	byteIndex = discriminant/(8*sizeof(char));
	bitPos = (CrFwDiscriminant_t)(discriminant - byteIndex*8*(CrFwDiscriminant_t)sizeof(char));
	enableByte = servDesc[cmdRepIndex].isDiscriminantEnabled[byteIndex];
	if ((enableByte & (1 << (bitPos-1))) == 0)
		return 0;
	else
		return 1;
}

/*------------------------------------------------------------------------------------*/
void CrFwOutRegistryStartTracking(FwSmDesc_t outCmp) {
	CrFwCmpData_t* outCmpData = (CrFwCmpData_t*)FwSmGetData(outCmp);
	CrFwOutCmpData_t* outCmpSpecificData = (CrFwOutCmpData_t*)outCmpData->cmpSpecificData;

	cmdRepState[cmdRepStateIndex].instanceId = outCmpData->instanceId;
	cmdRepState[cmdRepStateIndex].state = crOutRegistryPending;
	outCmpSpecificData->trackingIndex = cmdRepStateIndex;
	/*outCmpData->cmpSpecificData.outCmpData.trackingIndex = cmdRepStateIndex;*/

	if (cmdRepStateIndex == (CR_FW_OUTREGISTRY_N-1))
		cmdRepStateIndex = 0;
	else
		cmdRepStateIndex++;
}

/*------------------------------------------------------------------------------------*/
void CrFwOutRegistryUpdateState(FwSmDesc_t outCmp, CrFwOutRegistryCmdRepState_t newState) {
	CrFwCmpData_t* outCmpData = (CrFwCmpData_t*)FwSmGetData(outCmp);
	CrFwOutCmpData_t* outCmpSpecificData = (CrFwOutCmpData_t*)outCmpData->cmpSpecificData;
	CrFwTrackingIndex_t i;

	i = outCmpSpecificData->trackingIndex;
	/*i = outCmpData->cmpSpecificData.outCmpData.trackingIndex;*/
	if (cmdRepState[i].instanceId == outCmpData->instanceId) {
		cmdRepState[i].state = newState;
	}
}

/*------------------------------------------------------------------------------------*/
CrFwOutRegistryCmdRepState_t CrFwOutRegistryGetState(CrFwInstanceId_t cmdRepId) {
	CrFwTrackingIndex_t i;
	CrFwTrackingIndex_t j;

	if (cmdRepStateIndex > 0)
		i = (CrFwTrackingIndex_t)(cmdRepStateIndex-1);
	else
		i = (CR_FW_OUTREGISTRY_N-1);

	for (j=0; j<CR_FW_OUTREGISTRY_N; j++) {
		if (cmdRepState[i].state == crOutRegistryNoEntry)
			break;
		if (cmdRepState[i].instanceId == cmdRepId) {
			return cmdRepState[i].state;
		} else if (i == 0)
			i = (CR_FW_OUTREGISTRY_N-1);
		else
			i--;
	}

	return crOutRegistryNotTracked;
}

/*------------------------------------------------------------------------------------*/
static void OutRegistryInitAction(FwPrDesc_t initPr) {
	CrFwCmdRepIndex_t i, nextServType;
	CrFwDiscriminant_t nOfBytes;
	CrFwCmpData_t* cmpData = (CrFwCmpData_t*)FwPrGetData(initPr);

	for (i=0; i<CR_FW_OUTREGISTRY_NSERV; i++) {
		if (servDesc[i].maxDiscriminant == 0)
			servDesc[i].isDiscriminantEnabled = NULL;
		else {
			nOfBytes = (CrFwDiscriminant_t)(servDesc[i].maxDiscriminant/sizeof(char)+1);
			servDesc[i].isDiscriminantEnabled = malloc(sizeof(char)*nOfBytes);
		}
	}

	nextServType = 0;
	for (i=CR_FW_OUTREGISTRY_NSERV-1; i>0; i--) {
		servDesc[i].nextServType = nextServType;
		if (servDesc[i-1].servType != servDesc[i].servType)
			nextServType = i;
	}
	servDesc[0].nextServType = nextServType;

	cmpData->outcome = 1;
}

/*------------------------------------------------------------------------------------*/
static void OutRegistryConfigAction(FwPrDesc_t initPr) {
	CrFwCmdRepIndex_t i;
	CrFwDiscriminant_t j;
	CrFwCmpData_t* cmpData = (CrFwCmpData_t*)FwPrGetData(initPr);
	CrFwCounterU2_t k;

	for (i=0; i<CR_FW_OUTREGISTRY_NSERV; i++) {
		servDesc[i].isServTypeEnabled = 1;
		servDesc[i].isServSubTypeEnabled = 1;
		if (servDesc[i].maxDiscriminant != 0)
			for (j=0; j<(servDesc[i].maxDiscriminant/(8*sizeof(char))+1); j++)
				servDesc[i].isDiscriminantEnabled[j] = (unsigned char)(~0);
	}

	for (k=0; k<CR_FW_OUTREGISTRY_N; k++)
		cmdRepState[k].state = crOutRegistryNoEntry;

	cmdRepStateIndex = 0;

	cmpData->outcome = 1;
}

/*------------------------------------------------------------------------------------*/
static void OutRegistryShutdownAction(FwSmDesc_t smDesc) {
	CrFwCmdRepIndex_t i;
	CrFwCounterU2_t k;
	CRFW_UNUSED(smDesc);

	for (i=0; i<CR_FW_OUTREGISTRY_NSERV; i++) {
		servDesc[i].isServTypeEnabled = 1;
		servDesc[i].isServSubTypeEnabled = 1;
		if (servDesc[i].maxDiscriminant != 0)
			free(servDesc[i].isDiscriminantEnabled);
	}

	for (k=0; k<CR_FW_OUTREGISTRY_N; k++)
		cmdRepState[k].state = crOutRegistryNoEntry;

	cmdRepStateIndex = 0;
}
