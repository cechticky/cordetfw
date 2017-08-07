/**
 * @file
 * @ingroup crConfigGroup
 * User-modifiable parameters for the OutManager components (see <code>CrFwOutManager.h</code>).
 * This file defines all the user-modifiable parameters for the OutManager.
 * Users will normally have to modify this file as part of the framework instantiation process.
 *
 * The parameters defined in this file determine the configuration of the OutManager Components.
 * The value of these parameters cannot be changed dynamically.
 *
 * The default values defined in this file are those used for the testing of the
 * CORDET Framework.
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

#ifndef CR_FW_OUTMANAGER_USERPAR_H_
#define CR_FW_OUTMANAGER_USERPAR_H_

/* Include framework files */
#include "OutStream/CrFwOutStream.h"
#include "BaseCmp/CrFwResetProc.h"
/* Include test suite files */
#include "CrFwOutStreamStub.h"

/**
 * The number of OutManager components in the application.
 * The value of this constant must be smaller than the range of the <code>::CrFwCounterU1_t</code>
 * integer type.
 *
 * The number of OutManager components defined in this file is the one used for the test cases
 * of <code>CrFwOutManagerTestCases.h</code>.
 */
#define CR_FW_NOF_OUTMANAGER 4

/**
 * The sizes of the Pending OutComponent List (POCL) of the OutManager components.
 * Each OutManager has one POCL.
 * This constant defines the size of the POCL of the i-th OutManager.
 * The size of a POCL must be a positive integer (i.e. it is not legal
 * to define a zero-size POCL) in the range of the <code>#CrFwCounterU2_t</code> type.
 *
 * The POCL sizes defined in this file are those used for the test cases
 * of <code>CrFwOutManagerTestCases.h</code>.
 */
#define CR_FW_OUTMANAGER_POCLSIZE {30,4,5,6}

#endif /* CR_FW_OUTMANAGER_USERPAR_H_ */
