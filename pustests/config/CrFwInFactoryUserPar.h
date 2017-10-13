/**
 * @file
 * @ingroup crConfigGroup
 * User-modifiable parameters for the InFactory component (see <code>CrFwInFactory.h</code>).
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

#ifndef CRFW_INFACTORY_USERPAR_H_
#define CRFW_INFACTORY_USERPAR_H_

#include "CrFwInCmdSample1.h"
#include "CrFwInRepSample1.h"

#include "Services/Test/InCmd/CrPsTestAreYouAliveConnection.h"
#include "Services/Test/InRep/CrPsTestAreYouAliveConnectInRep.h"
#include "Services/Test/InCmd/CrPsTestOnBoardConnection.h"

#include "Services/Housekeeping/InCmd/CrPsHkHkRepStruct.h"
#include "Services/Housekeeping/InCmd/CrPsHkHkOneShot.h"
#include "Services/Housekeeping/InCmd/CrPsHkEnablePeriodicHk.h"
#include "Services/Housekeeping/InCmd/CrPsHkEnablePeriodicDiag.h"
#include "Services/Housekeeping/InCmd/CrPsHkDisablePeriodicHk.h"
#include "Services/Housekeeping/InCmd/CrPsHkDisablePeriodicDiag.h"
#include "Services/Housekeeping/InCmd/CrPsHkDiagRepStruct.h"
#include "Services/Housekeeping/InCmd/CrPsHkDiagRepStruct.h"
#include "Services/Housekeeping/InCmd/CrPsHkDiagOneShot.h"
#include "Services/Housekeeping/InCmd/CrPsHkDeleteHkStruct.h"
#include "Services/Housekeeping/InCmd/CrPsHkDeleteDiagStruct.h"
#include "Services/Housekeeping/InCmd/CrPsHkCreateHkStruct.h"
#include "Services/Housekeeping/InCmd/CrPsHkCreateDiagStruct.h"

#include "UtilityFunctions/CrFwUtilityFunctions.h"
/**
 * The maximum number of components representing an incoming command which may be allocated
 * at any one time.
 * This constant must be a positive integer smaller than the range of
 * <code>::CrFwInFactoryPoolIndex_t</code>.
 */
#define CR_FW_INFACTORY_MAX_NOF_INCMD 5

/**
 * The maximum number of InReports which may be allocated at any one time.
 * This constant must be smaller than the range of <code>::CrFwInFactoryPoolIndex_t</code>.
 */
#define CR_FW_INFACTORY_MAX_NOF_INREP 5

/**
 * The total number of kinds of incoming commands supported by the application.
 * An incoming command kind is defined by the triplet: [service type, service sub-type,
 * discriminant value].
 * The value of this constant must be the same as the number of rows of the
 * initializer <code>#CR_FW_INCMD_INIT_KIND_DESC</code> and it must be smaller
 * than the range of the <code>::CrFwCmdRepKindIndex_t</code> type.
 *
 * This constant is used as the size of a statically declared array.
 * Hence, a value of zero may not be allowed by some compilers.
 */
#define CR_FW_INCMD_NKINDS 14

/**
 * The total number of kinds of incoming reports supported by the application.
 * An incoming report kind is defined by the triplet: [service type, service sub-type,
 * discriminant value].
 * The value of this constant must be the same as the number of rows of the
 * initializer <code>#CR_FW_INREP_INIT_KIND_DESC</code> and it must be smaller
 * than the range of the <code>::CrFwCmdRepKindIndex_t</code> type.
 *
 * This constant is used as the size of a statically declared array.
 * Hence, a value of zero may not be allowed by some compilers.
 */
#define CR_FW_INREP_NKINDS 2

/**
 * Definition of the incoming command kinds supported by an application.
 * An application supports a number of service types and, for each service type, it supports
 * a number of sub-types.
 * Each sub-type may support a range of discriminant values.
 * An incoming command kind is defined by the triplet: [service type, service sub-type,
 * discriminant].
 *
 * Each line in this initializer describes one incoming command kind.
 * The elements in each line are as follows:
 * - The service type.
 * - The service sub-type.
 * - The discriminant value. A value of zero indicates either that no discriminant is
 *   associated to commands/reports of that type and sub-type or else that all commands/reports of
 *   this type and sub-type have the same characteristics.
 * - The function implementing the Validity Check Operation for this kind of incoming command
 *   (this must be a function pointer of type <code>::CrFwInCmdValidityCheck_t</code>;
 *   function <code>::CrFwPrCheckAlwaysTrue</code> can be used as a default).
 * - The function implementing the Ready Check Operation for this kind of incoming command
 *   (this must be a function pointer of type <code>::CrFwInCmdReadyCheck_t</code>;
 *   function <code>::CrFwSmCheckAlwaysTrue</code> can be used as a default).
 * - The function implementing the Start Action Operation for this kind of incoming command
 *   (this must be a function pointer of type <code>::CrFwInCmdStartAction_t</code>;
 *   function <code>::CrFwSmEmptyAction</code> can be used as a default).
 * - The function implementing the Progress Action Operation for this kind of incoming command
 *   (this must be a function pointer of type <code>::CrFwInCmdProgressAction_t</code>;
 *   function <code>::CrFwSmEmptyAction</code> can be used as a default).
 * - The function implementing the Termination Action Operation for this kind of incoming command
 *   (this must be a function pointer of type <code>::CrFwInCmdTerminationAction_t</code>;
 *   function <code>::CrFwSmEmptyAction</code> can be used as a default).
 * - The function implementing the Abort Action Operation for this kind of incoming command
 *   (this must be a function pointer of type <code>::CrFwInCmdAbortAction_t</code>;
 *   function <code>::CrFwSmEmptyAction</code> can be used as a default).
 * .
 * The list of service descriptors must satisfy the following constraints:
 * - The number of lines must be the same as <code>::CR_FW_INCMD_NKINDS</code>.
 * - The values of the service types, sub-types and discriminant must be lower than
 * 	 <code>#CR_FW_MAX_SERV_TYPE</code>, <code>#CR_FW_MAX_SERV_SUBTYPE</code> and
 * 	 <code>#CR_FW_MAX_DISCRIMINANT</code>.
 * - The service types must be listed in increasing order.
 * - The service sub-types within a service type must be listed in increasing order.
 * - The discriminant values within a service type/sub-type must be listed in increasing order.
 * .
 * The last four constraints are checked by the auxiliary function
 * <code>::CrFwAuxInFactoryInCmdConfigCheck</code>.
 *
 * The initializer values defined below are those which are used for the framework Test Suite.
 * The function pointers in the last row are those of the Sample InCommand defined in
 * <code>CrFwInCommandSample1.h</code>.
 */
#define CR_FW_INCMD_INIT_KIND_DESC \
	{ \
      {3,  1, 1,   &CrFwPrCheckAlwaysTrue,           &CrPsHkCreateHkStructReadyCheck,            &CrPsHkCreateHkStructStartAction,            &CrPsHkCreateHkStructProgressAction,            &CrPsHkCreateHkStructTerminationAction,            &CrPsHkCreateHkStructAbortAction}, \
      {3,  2, 1,   &CrFwPrCheckAlwaysTrue,           &CrPsHkCreateDiagStructReadyCheck,          &CrPsHkCreateDiagStructStartAction,          &CrPsHkCreateDiagStructProgressAction,          &CrPsHkCreateDiagStructTerminationAction,          &CrPsHkCreateDiagStructAbortAction}, \
      {3,  3, 1,   &CrFwPrCheckAlwaysTrue,           &CrPsHkDeleteHkStructReadyCheck,            &CrPsHkDeleteHkStructStartAction,            &CrPsHkDeleteHkStructProgressAction,            &CrPsHkDeleteHkStructTerminationAction,            &CrPsHkDeleteHkStructAbortAction}, \
      {3,  4, 1,   &CrFwPrCheckAlwaysTrue,           &CrPsHkDeleteDiagStructReadyCheck,          &CrPsHkDeleteDiagStructStartAction,          &CrPsHkDeleteDiagStructProgressAction,          &CrPsHkDeleteDiagStructTerminationAction,          &CrPsHkDeleteDiagStructAbortAction}, \
      {3,  5, 1,   &CrFwPrCheckAlwaysTrue,           &CrPsHkEnablePeriodicHkReadyCheck,          &CrPsHkEnablePeriodicHkStartAction,          &CrPsHkEnablePeriodicHkProgressAction,          &CrPsHkEnablePeriodicHkTerminationAction,          &CrPsHkEnablePeriodicHkAbortAction}, \
      {3,  6, 1,   &CrFwPrCheckAlwaysTrue,           &CrPsHkDisablePeriodicHkReadyCheck,         &CrPsHkDisablePeriodicHkStartAction,         &CrPsHkDisablePeriodicHkProgressAction,         &CrPsHkDisablePeriodicHkTerminationAction,         &CrPsHkDisablePeriodicHkAbortAction}, \
      {3,  7, 1,   &CrFwPrCheckAlwaysTrue,           &CrPsHkEnablePeriodicDiagReadyCheck,        &CrPsHkEnablePeriodicDiagStartAction,        &CrPsHkEnablePeriodicDiagProgressAction,        &CrPsHkEnablePeriodicDiagTerminationAction,        &CrPsHkEnablePeriodicDiagAbortAction}, \
      {3,  8, 1,   &CrFwPrCheckAlwaysTrue,           &CrPsHkDisablePeriodicDiagReadyCheck,       &CrPsHkDisablePeriodicDiagStartAction,       &CrPsHkDisablePeriodicDiagProgressAction,       &CrPsHkDisablePeriodicDiagTerminationAction,       &CrPsHkDisablePeriodicDiagAbortAction}, \
      {3,  9, 1,   &CrFwPrCheckAlwaysTrue,           &CrPsHkHkRepStructReadyCheck,               &CrPsHkHkRepStructStartAction,               &CrPsHkHkRepStructProgressAction,               &CrPsHkHkRepStructTerminationAction,               &CrPsHkHkRepStructAbortAction}, \
      {3,  11, 1,  &CrFwPrCheckAlwaysTrue,           &CrPsHkDiagRepStructReadyCheck,             &CrPsHkDiagRepStructStartAction,             &CrPsHkDiagRepStructProgressAction,             &CrPsHkDiagRepStructTerminationAction,             &CrPsHkDiagRepStructAbortAction}, \
      {3,  27, 1,  &CrFwPrCheckAlwaysTrue,           &CrPsHkHkOneShotReadyCheck,                 &CrPsHkHkOneShotStartAction,                 &CrPsHkHkOneShotProgressAction,                 &CrPsHkHkOneShotTerminationAction,                 &CrPsHkHkOneShotAbortAction}, \
      {3,  28, 1,  &CrFwPrCheckAlwaysTrue,           &CrPsHkDiagOneShotReadyCheck,               &CrPsHkDiagOneShotStartAction,               &CrPsHkDiagOneShotProgressAction,               &CrPsHkDiagOneShotTerminationAction,               &CrPsHkDiagOneShotAbortAction}, \
      {17, 1, 0,   &CrFwPrCheckAlwaysTrue,           &CrPsTestAreYouAliveConnectionReadyCheck,   &CrPsTestAreYouAliveConnectionStartAction,   &CrPsTestAreYouAliveConnectionProgressAction,   &CrPsTestAreYouAliveConnectionTerminationAction,   &CrPsTestAreYouAliveConnectionAbortAction}, \
      {17, 3, 0,   &CrFwPrCheckAlwaysTrue,           &CrPsTestOnBoardConnectionReadyCheck,       &CrPsTestOnBoardConnectionStartAction,       &CrPsTestOnBoardConnectionProgressAction,       &CrPsTestOnBoardConnectionTerminationAction,       &CrPsTestOnBoardConnectionAbortAction    }  \
	}

/**
 * Definition of the incoming report kinds supported by an application.
 * An application supports a number of service types and, for each service type, it supports
 * a number of sub-types.
 * Each sub-type may support a range of discriminant values.
 * An incoming report kind is defined by the triplet: [service type, service sub-type,
 * discriminant].
 *
 * Each line in this initializer describes one incoming report kind.
 * The elements in each line are as follows:
 * - The service type.
 * - The service sub-type.
 * - The discriminant value. A value of zero indicates either that no discriminant is
 *   associated to commands/reports of that type and sub-type or else that all commands/reports of
 *   this type and sub-type have the same characteristics.
 * - The function implementing the Update Action Operation for this kind of incoming report
 *   (this must be a function pointer of type <code>::CrFwInRepUpdateAction_t</code>;
 *   function <code>::CrFwPrEmptyAction</code> can be used as a default).
 * - The function implementing the Validity Check Operation for this kind of incoming report
 *   (this must be a function pointer of type <code>::CrFwInRepValidityCheck_t</code>;
 *   function <code>::CrFwPrCheckAlwaysTrue</code> can be used as a default).
 * .
 * The list of service descriptors must satisfy the following constraints:
 * - The number of lines must be the same as <code>::CR_FW_INREP_NKINDS</code>.
 * - The values of the service types, sub-types and discriminant must be lower than
 * 	 <code>#CR_FW_MAX_SERV_TYPE</code>, <code>#CR_FW_MAX_SERV_SUBTYPE</code> and
 * 	 <code>#CR_FW_MAX_DISCRIMINANT</code>.
 *   maximum values defined in <code>CrFwUserConstants.h</code> (TBC).
 * - The service types must be listed in increasing order.
 * - The service sub-types within a service type must be listed in increasing order.
 * - The discriminant values within a service type/sub-type must be listed in increasing order.
 * .
 * The last four constraints are checked by the auxiliary function
 * <code>::CrFwAuxInFactoryInRepConfigCheck</code>.
 *
 * The initializer values defined below are those which are used for the framework Test Suite.
 * The function pointers in the last row are those of the Sample InReport defined in
 * <code>CrFwInReportSample1.h</code>.
 */
#define CR_FW_INREP_INIT_KIND_DESC \
	{ \
      {17, 2, 0, &CrPsTestAreYouAliveConnectInRepUpdateAction,   &CrPsTestAreYouAliveConnectInRepValidityCheck,   0}, \
      {17, 4, 0, &CrPsTestAreYouAliveConnectInRepUpdateAction,   &CrPsTestAreYouAliveConnectInRepValidityCheck,   0}  \
	}

#endif /* CRFW_INFACTORY_USERPAR_H_ */
