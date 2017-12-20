/**
 * @file
 * This header file declares the function to create one instance of the CrPsLpt state machine.
 * The state machine is configured with a set of function pointers representing the non-default
 * actions and guards of the state machine. Some of these functions may also be declared in
 * this header file in accordance with the configuration of the state machine in the FW Profile
 * Editor. In the latter case, the user has to provide an implementation for these functions
 * in a user-supplied body file.
 *
 * This header file has been automatically generated by the FW Profile Editor.
 * The state machine created by this file is shown in the figure below.
 * @image html CrPsLpt.png
 *
 * @author FW Profile code generator version 5.01
 * @date Created on: Nov 26 2017 12:16:43
 */

/** Make sure to include this header file only once */
#ifndef CRPSLPT_H_
#define CRPSLPT_H_

/** FW Profile function definitions */
#include "FwSmConstants.h"

/** State identifiers */
#define CrPsLpt_DOWN_TRANSFER (1)		/* The identifier of state DOWN_TRANSFER in State Machine CrPsLpt */
#define CrPsLpt_INACTIVE (2)		/* The identifier of state INACTIVE in State Machine CrPsLpt */
#define CrPsLpt_UP_TRANSFER (3)		/* The identifier of state UP_TRANSFER in State Machine CrPsLpt */

/** The identifiers of transition commands (triggers) */
#define Execute (0)
#define Abort (18)
#define StartDownTransfer (19)
#define StartUpTransfer (20)
#define EndUpTransfer (21)

/**
 * Create a new state machine descriptor.
 * This interface creates the state machine descriptor dynamically.
 * @param smData the pointer to the state machine data.
 * A value of NULL is legal (note that the default value of the pointer
 * to the state machine data when the state machine is created is NULL).
 * @return the pointer to the state machine descriptor
 */
FwSmDesc_t CrPsLptCreate(void* smData);

/**
 * Entry Action for the state DOWN_TRANSFER.
 * <pre>
 * nOfDownlinks++;
 * partSeqNmb = 1;
 * lptRemSize = lptSize;
 * </pre>
 * @param smDesc the state machine descriptor
 */
void CrPsLptDownTransferEntryAction(FwSmDesc_t smDesc);

/**
 * Exit Action for the state DOWN_TRANSFER.
 * <pre>
 * Increment largeMsgTransId by LPT\_N\_BUF;
 * nOfDownlinks--;
 * </pre>
 * @param smDesc the state machine descriptor
 */
void CrPsLptDownTransferExitAction(FwSmDesc_t smDesc);

/**
 * Do Action for the state DOWN_TRANSFER.
 * <pre>
 * if (partSeqNmb == 0) 
 *   Load (13,1) report with lptDest as destination;
 * elseif (partSeqNmb == 1) 
 *   Load (13,2) report with lptDest as destination;
 * elseif (lptRemSize <= partSize) 
 *   Load (13,3) report with lptDest as destination;
 * </pre>
 * @param smDesc the state machine descriptor
 */
void CrPsLptDownTransferDoAction(FwSmDesc_t smDesc);

/**
 * Entry Action for the state UP_TRANSFER.
 * <pre>
 * nOfUplinks++;
 * lptFailCode = NO_FAIL;
 * </pre>
 * @param smDesc the state machine descriptor
 */
void CrPsLptUpTransferEntryAction(FwSmDesc_t smDesc);

/**
 * Exit Action for the state UP_TRANSFER.
 * nOfDownlinks--
 * @param smDesc the state machine descriptor
 */
void CrPsLptUpTransferExitAction(FwSmDesc_t smDesc);

/**
 * Do Action for the state UP_TRANSFER.
 * if (Flag_1) lptFailCode = TIME_OUT
 * @param smDesc the state machine descriptor
 */
void CrPsLptUpTransferDoAction(FwSmDesc_t smDesc);

/**
 * Action on the transition from DOWN_TRANSFER to INACTIVE.
 * <pre>
 * Generate event
 * EVT_DOWN_ABORT
 * </pre>
 * @param smDesc the state machine descriptor
 */
void CrPsLptGenerateAbortEvent(FwSmDesc_t smDesc);

/**
 * Guard on the transition from DOWN_TRANSFER to INACTIVE.
 *  Transfer is finished 
 * @param smDesc the state machine descriptor
 * @return 1 if the guard is fulfilled, otherwise 0.
 */
FwSmBool_t CrPsLptIsTransferFinished(FwSmDesc_t smDesc);

/**
 * Action on the transition from UP_TRANSFER to INACTIVE.
 * Load (13,16) report
 * @param smDesc the state machine descriptor
 */
void CrPsLptLoadReport(FwSmDesc_t smDesc);

/**
 * Guard on the transition from UP_TRANSFER to INACTIVE.
 * lptFailCode != NO_FAIL 
 * @param smDesc the state machine descriptor
 * @return 1 if the guard is fulfilled, otherwise 0.
 */
FwSmBool_t CrPsLptIsFailed(FwSmDesc_t smDesc);

/**
 * Action on the transition from Initial State to INACTIVE.
 * Initialize largeMsgTransId
 * @param smDesc the state machine descriptor
 */
void CrPsLptInitialization(FwSmDesc_t smDesc);

#endif /* CrPsLptCreate_H_ */
