/**
 * @file
 * This header file declares the function to create one instance of the CrPsCmd5EidStart procedure.
 * The procedure is configured with a set of function pointers representing the non-default
 * actions and guards of the procedure. Some of these functions may also be declared in
 * this header file in accordance with the configuration of the procedure in the FW Profile
 * Editor. In the latter case, the user has to provide an implementation for these functions
 * in a user-supplied body file.
 *
 * This header file has been automatically generated by the FW Profile Editor.
 * The procedure created by this file is shown in the figure below.
 * @image html CrPsCmd5EidStart.png
 *
 * @author FW Profile code generator version 5.01
 * @date Created on: Sep 6 2017 17:17:43
 */

/** Make sure to include this header file only once */
#ifndef CRPSCMD5EIDSTART_H_
#define CRPSCMD5EIDSTART_H_

/** FW Profile function definitions */
#include "FwPrConstants.h"

/** Action node identifiers */
#define CrPsCmd5EidStart_N1 (1)		/* The identifier of action node N1 in procedure CrPsCmd5EidStart */
#define CrPsCmd5EidStart_N2 (2)		/* The identifier of action node N2 in procedure CrPsCmd5EidStart */
#define CrPsCmd5EidStart_N3 (3)		/* The identifier of action node N3 in procedure CrPsCmd5EidStart */
#define CrPsCmd5EidStart_N4 (4)		/* The identifier of action node N4 in procedure CrPsCmd5EidStart */
#define CrPsCmd5EidStart_N7 (5)		/* The identifier of action node N7 in procedure CrPsCmd5EidStart */
#define CrPsCmd5EidStart_N8 (6)		/* The identifier of action node N8 in procedure CrPsCmd5EidStart */

/**
 * Create a new procedure descriptor.
 * This interface creates the procedure descriptor dynamically.
 * @param prData the pointer to the procedure data.
 * A value of NULL is legal (note that the default value of the pointer
 * to the procedure data when the procedure is created is NULL).
 * @return the pointer to the procedure descriptor
 */
FwPrDesc_t CrPsCmd5EidStartCreate(void* prData);

/**
 * Action for node N4.
 * Increment i
 * @param smDesc the procedure descriptor
 */
void CrPsCmd5EidStartN4(FwPrDesc_t prDesc);

/**
 * Action for node N1.
 * Set i equal to 1
 * @param smDesc the procedure descriptor
 */
void CrPsCmd5EidStartN1(FwPrDesc_t prDesc);

/**
 * Action for node N2.
 * <pre>
 * Load invalid EID in
 * data pool item verFailData
 * </pre>
 * @param smDesc the procedure descriptor
 */
void CrPsCmd5EidStartN2(FwPrDesc_t prDesc);

/**
 * Action for node N3.
 * <pre>
 * Run Command Verification
 * Failure Procedure to generate (1,4) 
 * report with failure code VER_ILL_EID
 * </pre>
 * @param smDesc the procedure descriptor
 */
void CrPsCmd5EidStartN3(FwPrDesc_t prDesc);

/**
 * Action for node N7.
 * <pre>
 * Set action
 * outcome to 'success'
 * </pre>
 * @param smDesc the procedure descriptor
 */
void CrPsCmd5EidStartN7(FwPrDesc_t prDesc);

/**
 * Action for node N8.
 * <pre>
 * Set action outcome to 'failure'
 * with failure code VER_EID_START_FD
 * </pre>
 * @param smDesc the procedure descriptor
 */
void CrPsCmd5EidStartN8(FwPrDesc_t prDesc);

/**
 * Guard on the Control Flow from DECISION1 to N2.
 * <pre>
 *  The i-th EID is not
 * in EVT_EID 
 * </pre>
 * @param smDesc the procedure descriptor
 * @return 1 if the guard is fulfilled, otherwise 0.
 */
FwPrBool_t CrPsCmd5EidStartG1(FwPrDesc_t prDesc);

/**
 * Guard on the Control Flow from DECISION1 to DECISION3.
 *  Else 
 * @param smDesc the procedure descriptor
 * @return 1 if the guard is fulfilled, otherwise 0.
 */
FwPrBool_t CrPsCmd5EidStartG1E(FwPrDesc_t prDesc);

/**
 * Guard on the Control Flow from DECISION3 to DECISION4.
 * <pre>
 *  The i-th EID was the last
 * EID in the command 
 * </pre>
 * @param smDesc the procedure descriptor
 * @return 1 if the guard is fulfilled, otherwise 0.
 */
FwPrBool_t CrPsCmd5EidStartG2(FwPrDesc_t prDesc);

/**
 * Guard on the Control Flow from DECISION3 to N4.
 *  Else 
 * @param smDesc the procedure descriptor
 * @return 1 if the guard is fulfilled, otherwise 0.
 */
FwPrBool_t CrPsCmd5EidStartG2E(FwPrDesc_t prDesc);

/**
 * Guard on the Control Flow from DECISION4 to N8.
 * <pre>
 *  All EIDs in the
 * command are invalid 
 * </pre>
 * @param smDesc the procedure descriptor
 * @return 1 if the guard is fulfilled, otherwise 0.
 */
FwPrBool_t CrPsCmd5EidStartG3(FwPrDesc_t prDesc);

/**
 * Guard on the Control Flow from DECISION4 to N7.
 *  Else 
 * @param smDesc the procedure descriptor
 * @return 1 if the guard is fulfilled, otherwise 0.
 */
FwPrBool_t CrPsCmd5EidStartG3E(FwPrDesc_t prDesc);

#endif /* CrPsCmd5EidStartCreate_H_ */