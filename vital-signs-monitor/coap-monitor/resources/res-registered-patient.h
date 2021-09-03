/**
 * \file
 *         Header file for the registered patient resource
 * \author
 *         Diego Casu
 */

/**
 * \defgroup res-registered-patient Registered patient resource
 * @{
 *
 * The res-registered-patient module provides the implementation of a CoAP resource
 * representing the patient ID currently registered in the vital signs monitor.
 */

#ifndef SMART_ICU_RES_REGISTERED_PATIENT_H
#define SMART_ICU_RES_REGISTERED_PATIENT_H

/**
 * \brief   Activate the registered patient resource.
 */
void res_registered_patient_activate(void);

/**
 * \brief              Update the registered patient resource.
 * \param patient_id   A pointer to the new patient ID.
 *
 *                     This function updates the registered patient
 *                     resource, triggering notifications to the observers.
 */
void res_registered_patient_update(char *patient_id);

#endif /* SMART_ICU_RES_REGISTERED_PATIENT_H */
/** @} */
