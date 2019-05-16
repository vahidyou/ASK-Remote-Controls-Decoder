/*
 * ASKRemoteControlDecoder.h
 *  ASK remote controls signal decoder header file. FixCode and LearningCode remote controls are supported.
 *
 * This program is published under the terms of the MIT License.
 * This program is free software and can be distributed by everyone.
 * You can modify this program and distribute it by your name and contact information as the author.
 * No warranty of any kind is expressed or implied. You use this program at your own risk.
 *
 *   Created: 9 May 2019 1:57:43 AM
 *    Author: Mohammad Yousefi (www.dihav.com - mohammad-yousefi.id.ir - vahidyou@gmail.com)
 * Last Edit: 16 May 2019
 */ 

#ifndef ASKRemoteControlDecoder_H_
#define ASKRemoteControlDecoder_H_

/* You must define the value of TCCR1B register for running Timer1 at 1MHz. The default 
   value for 1MHz internal RC oscillator is 1 that starts Timer1 with no prescaling.
*/
#define ASKRmt_TCCR1B 1

/* You must define EEPROM start address and end address for saving remote controls codes.
   Each remote control code occupies 3 bytes in the EEPROM. */
#define ASKRmt_EEPROM_START 0
#define ASKRmt_EEPROM_END  59

/* If this variable is true and the received data remote control code is not saved to
   the EEPROM, the data will be discarded automatically. The default value is true.*/
extern volatile bool ASKRmt_AutoDiscardUnsavedRemotes;

/* Returns true if valid data is received.
   This function will not pick the data. 
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_IsDataReceived(void);

/* Discards the received data.
   Note that while data is not picked or discarded, new data will not receive. */
void ASKRmt_DiscardData(void);

/* Reads the data and returns true if valid data is received.
   The received data (3 bytes) will be copied to the "data" array. 
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_GetData(uint8_t *data);

/* Picks the data and returns true if valid data is received.
   The received data (3 bytes) will be copied to the "data" array.    
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_PickData(uint8_t *data);

/* Returns the key number if valid data is received, otherwise returns -1.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
int8_t ASKRmt_GetKey(bool isFixCode);

/* Picks the data and returns the key number if valid data is received, otherwise returns -1.
   Note that while data is not picked or discarded, new data will not receive. */
int8_t ASKRmt_PickKey(bool isFixCode);

/* Returns the key number if valid data is received and remote control code has been
   saved to the EEPROM, otherwise returns -1. Type of remote control will be detected 
   automatically.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
int8_t ASKRmt_GetKeyIfRemoteSaved(void);

/* Picks the data and returns the key number if valid data is received and remote control code 
   has been saved to the EEPROM, otherwise returns -1. Type of remote control will be detected 
   automatically.
   Note that while data is not picked or discarded, new data will not receive. */
int8_t ASKRmt_PickKeyIfRemoteSaved(void);

/* Saves the remote control code to the EEPROM if valid data is received.
   This function returns false if no valid data is received or the code is already saved or
   the EEPROM is full.
   You must assign false to ASKRmt_AutoDiscardUnsavedRemotes before saving process.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_SaveRemote(bool isFixCode);

/* Picks the data and saves the remote control code to the EEPROM if valid data is received.
   This function returns false if no valid data is received or the code is already saved or
   the EEPROM is full.
   You must assign false to ASKRmt_AutoDiscardUnsavedRemotes before saving process.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_PickDataAndSaveRemote(bool isFixCode);

/* Saves the remote control code to the EEPROM if valid data is received. Type of remote 
   control will be detected automatically. The user must only press key 1 or A.
   This function returns false if no valid data is received or the code is already saved or
   the EEPROM is full or type detection fails due to pressing another key.
   You must assign false to ASKRmt_AutoDiscardUnsavedRemotes before saving process.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_SaveRemoteAutoDetectType(void);

/* Picks the data and saves the remote control code to the EEPROM if valid data is received. 
   Type of remote control will be detected automatically. The user must only press key 1 or A.
   This function returns false if no valid data is received or the code is already saved or
   the EEPROM is full or type detection fails due to pressing another key.
   You must assign false to ASKRmt_AutoDiscardUnsavedRemotes before saving process.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_PickDataAndSaveRemoteAutoDetectType(void);

/* Deletes the remote control code from the EEPROM if valid data is received. Type of remote control 
   will be detected automatically.
   This function returns false if no valid data is received or the code does not exist in the EEPROM.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_DeleteRemote(void);

/* Picks the data and deletes the remote control code from the EEPROM if valid data is received. 
   Type of remote control will be detected automatically.
   This function returns false if no valid data is received or the code does not exist in the EEPROM.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_PickDataAndDeleteRemote(void);

/* Deletes the remote control code from the EEPROM. Type of remote control will be detected 
   automatically. "code" is pointer to array of 3 bytes.
   This function returns false if the code does not exist in the EEPROM. */
bool ASKRmt_DeleteRemoteByCode(uint8_t *code);

/* Deletes all of the remote controls codes from the EEPROM. */
void ASKRmt_DeleteAllRemotes(void);

/* This function reads a remote control code from the EEPROM by index and copies 3 bytes of code
   to the "code" array. 
   This function returns false if the index is out of range. */
bool ASKRmt_GetRemoteCodeByIndex(uint8_t index, uint8_t *code);

#endif /* ASKRemoteControlDecoder_H_ */