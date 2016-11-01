/**
 * @file
 *
 * Implementation of OutLoader State Machine.
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
#include "CrFwOutLoaderUserPar.h"
#include "CrFwCmpData.h"
/* Include framework files */
#include "CrFwConstants.h"
#include "CrFwRepErr.h"
#include "CrFwTime.h"
#include "CrFwOutLoader.h"
#include "BaseCmp/CrFwBaseCmp.h"
#include "BaseCmp/CrFwInitProc.h"
#include "BaseCmp/CrFwResetProc.h"
#include "BaseCmp/CrFwDummyExecProc.h"
#include "Pckt/CrFwPckt.h"
#include "Pckt/CrFwPcktQueue.h"
#include "OutManager/CrFwOutManager.h"
#include "UtilityFunctions/CrFwUtilityFunctions.h"
/* Include FW Profile files */
#include "FwSmConstants.h"
#include "FwSmDCreate.h"
#include "FwSmConfig.h"
#include "FwSmCore.h"
#include "FwPrConstants.h"
#include "FwPrDCreate.h"
#include "FwPrConfig.h"
#include "FwPrCore.h"

/** Descriptor of the OutLoader Singleton */
static FwSmDesc_t outLoader = NULL;

/** The data structure for the OutLoader Singleton */
static CrFwCmpData_t outLoaderData;

/** The function implementing the OutManager Selection Operation */
static CrFwOutManagerSelect_t outManagerSelect = CR_FW_OUTLOADER_OUTMANAGER_SELECT;

/** The function implementing the OutManager Activation Operation */
static CrFwOutManagerActivate_t outManagerActivate = CR_FW_OUTLOADER_OUTMANAGER_ACTIVATE;

/** The function implementing the Initialization Check */
static FwPrAction_t initCheck = CR_FW_OUTLOADER_INITCHECK;

/** The function implementing the Initialization Action */
static FwPrAction_t initAction = CR_FW_OUTLOADER_INITACTION;

/** The function implementing the Configuration Check */
static FwPrAction_t configCheck = CR_FW_OUTLOADER_CONFIGCHECK;

/** The function implementing the Configuration Action */
static FwPrAction_t configAction = CR_FW_OUTLOADER_CONFIGACTION;

/** The function implementing the Shutdown Action */
static FwSmAction_t shutdownAction = CR_FW_OUTLOADER_SHUTDOWNACTION;


/*-----------------------------------------------------------------------------------------*/
FwSmDesc_t CrFwOutLoaderMake() {
	FwPrDesc_t resetPr, execPr, initPr;

	if (outLoader != NULL) {
		return outLoader;	/* The requested SM has already been created */
	}

	/* Create the requested SM as an extension of the Base Component SM */
	outLoader = FwSmCreateDer(CrFwBaseCmpMake());

	/* Create the Reset Procedure for the OutLoader Component */
	resetPr = FwPrCreateDer(CrFwCmpGetResetProc());
	FwPrOverrideAction(resetPr, &CrFwBaseCmpDefConfigCheck, configCheck);
	FwPrOverrideAction(resetPr, &CrFwBaseCmpDefConfigAction, configAction);

	/* Create the Initialization Procedure for the OutLoader Component */
	initPr = FwPrCreateDer(CrFwCmpGetInitProc());
	FwPrOverrideAction(resetPr, &CrFwBaseCmpDefInitCheck, initCheck);
	FwPrOverrideAction(resetPr, &CrFwBaseCmpDefInitAction, initAction);

	/* Get the Dummy Execution Procedure for the OutLoader Component */
	execPr = CrFwBaseCmpGetDummyExecProc();

	/* Override the Shutdown Action for the OutStream Component */
	FwSmOverrideAction(outLoader, &CrFwBaseCmpDefShutdownAction, shutdownAction);

	/* Initialize the data for the requested SM */
	outLoaderData.outcome = 1;
	outLoaderData.initProc = initPr;
	outLoaderData.resetProc = resetPr;
	outLoaderData.execProc = execPr;
	outLoaderData.instanceId = 0;
	outLoaderData.typeId = CR_FW_OUTLOADER_TYPE;

	/* Attach the data to the OutLoader state machine and to its procedures. */
	FwSmSetData(outLoader, &outLoaderData);
	FwPrSetData(outLoaderData.initProc, &outLoaderData);
	FwPrSetData(outLoaderData.resetProc, &outLoaderData);

	/* Start the OutLoader */
	FwSmStart(outLoader);

	return outLoader;
}

/*-----------------------------------------------------------------------------------------*/
void CrFwOutLoaderLoad(FwSmDesc_t outCmp) {
	FwSmDesc_t selectedOutManager;

	selectedOutManager = outManagerSelect(outCmp);
	CrFwOutManagerLoad(selectedOutManager, outCmp);
	outManagerActivate(selectedOutManager);
}

/*-----------------------------------------------------------------------------------------*/
FwSmDesc_t CrFwOutLoaderDefOutManagerSelect(FwSmDesc_t outCmp) {
	CRFW_UNUSED(outCmp);
	return CrFwOutManagerMake(0);
}

/*-----------------------------------------------------------------------------------------*/
void CrFwOutLoadDefOutManagerActivate(FwSmDesc_t outManager) {
	CRFW_UNUSED(outManager);
	return;
}

