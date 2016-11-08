/**
 * @file
 * @ingroup crTestSuiteGroup
 * Implementation of the Sample 1 InReport.
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
#include "CrFwCmpData.h"
/* Include framework files */
#include "CrFwConstants.h"
#include "InRep/CrFwInRep.h"
#include "BaseCmp/CrFwBaseCmp.h"
#include "BaseCmp/CrFwInitProc.h"
#include "BaseCmp/CrFwResetProc.h"
#include "BaseCmp/CrFwDummyExecProc.h"
#include "OutFactory/CrFwOutFactory.h"
#include "Pckt/CrFwPckt.h"
/* Include FW Profile files */
#include "FwPrConfig.h"
#include "FwPrDCreate.h"
#include "FwSmConfig.h"
#include "FwSmDCreate.h"
#include "FwPrCore.h"

/** The Validity Flag. */
static CrFwBool_t validityFlag;

/** The Update Action Outcome. */
static CrFwOutcome_t updateOutcome;

/** The Update Action Counter. */
static CrFwCounterU1_t updateCounter = 0;

/*-----------------------------------------------------------------------------------------*/
CrFwBool_t CrFwInRepSample1ValidityCheck(FwPrDesc_t prDesc) {
	(void)(prDesc);
	return validityFlag;
}

/*-----------------------------------------------------------------------------------------*/
void CrFwInRepSample1SetValidityFlag(CrFwBool_t flag) {
	validityFlag = flag;
}

/*-----------------------------------------------------------------------------------------*/
void CrFwInRepSample1UpdateAction(FwPrDesc_t prDesc) {
	CrFwCmpData_t* cmpData = (CrFwCmpData_t*)FwPrGetData(prDesc);
	cmpData->outcome = updateOutcome;
	updateCounter++;
}

/*-----------------------------------------------------------------------------------------*/
void CrFwInRepSample1SetUpdateActionOutcome(CrFwOutcome_t outcome) {
	updateOutcome = outcome;
}

/*-----------------------------------------------------------------------------------------*/
CrFwCounterU1_t CrFwInRepSample1GetUpdateActionCounter() {
	return updateCounter;
}
