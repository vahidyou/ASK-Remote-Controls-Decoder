/*
 * ASKRemoteControlDecoder.h
 *  ASK RF remote controls signal decoder header file. FixCode and LearningCode remote controls are supported.
 *
 * This program is published under the terms of the MIT License.
 * This program is free software and can be distributed by everyone.
 * You can modify this program and distribute it with your name and contact information as the author.
 * No warranty of any kind is expressed or implied. You use this program at your own risk.
 *
 *   Created: 09 May 2019 01:57 AM
 *    Author: Mohammad Yousefi (www.dihav.com - mohammad-yousefi.id.ir - vahidyou@gmail.com)
 * Last Edit: 08 Jun 2021
 */ 

#ifndef ASKRemoteControlDecoder_H_
#define ASKRemoteControlDecoder_H_

/* You must define a 2-byte timer that runs at 1MHz. The default value of 
   TCCR1B register for running Timer1 at 1MHz with 1MHz internal RC oscillator 
   is 1 that starts Timer1 with no prescaling.                                 */
#define ASKRmt_2BYTE1MHZTIMER_START        TCCR1B = 1
#define ASKRmt_2BYTE1MHZTIMER_STOP         TCCR1B = 0
#define ASKRmt_2BYTE1MHZTIMER_COUNTERVALUE TCNT1
#define ASKRmt_2BYTE1MHZTIMER_RESETCOUNTER TCNT1 = 0

/* Comment below definition to reduce program size if you don't want to save 
   and detect remote controls automatically.                                   */
#define ASKRmt_SAVEREMOTECONTROLSTOEEPROM

/* Comment below definition to reduce program size if you don't want to save 
   and detect keys automatically.                                              */
//#define ASKRmt_SAVEKEYCODESTOEEPROM

/* You must define EEPROM start address and end address for saving remote 
   controls or keys codes. Each remote control code or key code occupies 3 
   bytes in the EEPROM.                                                        */
#define ASKRmt_EEPROM_START 0
#define ASKRmt_EEPROM_END  59

/* Only one of save remotes or save keys modes are allowed.                    */
#if defined(ASKRmt_SAVEREMOTECONTROLSTOEEPROM) && defined(ASKRmt_SAVEKEYCODESTOEEPROM)
#error "Only one of save remotes or save keys modes are allowed."
#endif

/* Call this subroutine on any change of RF signal pin value.                  */
void ASKRmt_RFSignalPinChanged(uint8_t pinValue);

/* Call this subroutine on 2-byte 1MHz timer overflow interrupt.               */
void ASKRmt_TwoByte1MHzTimerOverflowInterrupt(void);

/* Returns true if valid data is received.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_IsDataReceived(void);

/* Discards the received data.
   Note that while data is not picked or discarded, new data will not receive. */
void ASKRmt_DiscardData(void);

/* Reads the data and returns true if valid data is received. The received data 
   (3 bytes) will be copied to the "data" array.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_GetData(uint8_t *data);

/* Picks the data and returns true if valid data is received. The received data 
   (3 bytes) will be copied to the "data" array.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_PickData(uint8_t *data);

/* Returns the key number if valid data is received, otherwise returns -1.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
int8_t ASKRmt_GetKey(bool isFixCode);

/* Picks the data and returns the key number if valid data is received, 
   otherwise returns -1.
   Note that while data is not picked or discarded, new data will not receive. */
int8_t ASKRmt_PickKey(bool isFixCode);

#ifdef ASKRmt_SAVEREMOTECONTROLSTOEEPROM

/* If this variable is true and the received data remote control code is not 
   saved to the EEPROM, the data will be discarded automatically. The default 
   value is true.                                                              */
extern volatile bool ASKRmt_AutoDiscardUnsavedRemotes;

/* Returns the key number if valid data is received and remote control code has 
   been saved to the EEPROM, otherwise returns -1. The type of remote control 
   will be detected automatically.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
int8_t ASKRmt_GetKeyIfRemoteSaved(void);

/* Picks the data and returns the key number if valid data is received and 
   remote control code has been saved to the EEPROM, otherwise returns -1. The 
   type of remote control will be detected automatically.
   Note that while data is not picked or discarded, new data will not receive. */
int8_t ASKRmt_PickKeyIfRemoteSaved(void);

/* Saves the remote control code to the EEPROM if valid data is received. This 
   function returns false if no valid data is received or the code is already 
   saved or the EEPROM is full.
   You must assign false to ASKRmt_AutoDiscardUnsavedRemotes before the saving 
   process. 
   This function will not pick the data. 
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_SaveRemote(bool isFixCode);

/* Picks the data and saves the remote control code to the EEPROM if valid data 
   is received. This function returns false if no valid data is received or the 
   code is already saved or the EEPROM is full.
   You must assign false to ASKRmt_AutoDiscardUnsavedRemotes before the saving 
   process.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_PickDataAndSaveRemote(bool isFixCode);

/* Saves the remote control code to the EEPROM if valid data is received. The 
   type of remote control will be detected automatically. The user must only 
   press key 1 or A. This function returns false if no valid data is received 
   or the code is already saved or the EEPROM is full or type detection fails 
   due to pressing another key.
   You must assign false to ASKRmt_AutoDiscardUnsavedRemotes before the saving 
   process.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_SaveRemoteAutoDetectType(void);

/* Picks the data and saves the remote control code to the EEPROM if valid data 
   is received. The type of remote control will be detected automatically. The 
   user must only press key 1 or A. This function returns false if no valid 
   data is received or the code is already saved or the EEPROM is full or type 
   detection fails due to pressing another key.
   You must assign false to ASKRmt_AutoDiscardUnsavedRemotes before the saving 
   process.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_PickDataAndSaveRemoteAutoDetectType(void);

/* Deletes the remote control code from the EEPROM if valid data is received. 
   The type of remote control will be detected automatically. This function 
   returns false if no valid data is received or the code does not exist in the 
   EEPROM.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_DeleteRemote(void);

/* Picks the data and deletes the remote control code from the EEPROM if valid 
   data is received. The type of remote control will be detected automatically. 
   This function returns false if no valid data is received or the code does 
   not exist in the EEPROM.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_PickDataAndDeleteRemote(void);

/* Deletes the remote control code from the EEPROM. The type of remote control 
   will be detected automatically. "code" the is the pointer to an array of 3 
   bytes. This function returns false if the code does not exist in the EEPROM.*/
bool ASKRmt_DeleteRemoteByCode(uint8_t *code);

/* Deletes all of the saved remote controls from the EEPROM.                   */
void ASKRmt_DeleteAllRemotes(void);

/* This function reads a remote control code from the EEPROM by index and 
   copies 3 bytes of code to the "code" array. This function returns false if 
   the index is out of range.                                                  */
bool ASKRmt_GetRemoteCodeByIndex(uint8_t index, uint8_t *code);

#endif

#ifdef ASKRmt_SAVEKEYCODESTOEEPROM

/* If this variable is true and the received key code is not saved to the 
   EEPROM, the data will be discarded automatically. The default value is true.*/
extern volatile bool ASKRmt_AutoDiscardUnsavedKeys;

/* Returns true if valid data is received and key code has been saved to the 
   EEPROM, otherwise returns false. The key number will be stored to the 
   variable pointed by the "key" parameter. The type of remote control will not 
   be detected automatically.
   This function will not pick the data. 
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_GetKeyIfKeySaved(uint8_t *key);

/* Picks the data and returns true if valid data is received and key code has 
   been saved to the EEPROM, otherwise returns false. The key number will be 
   stored to the variable pointed by the "key" parameter. The type of remote 
   control will not be detected automatically.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_PickKeyIfKeySaved(uint8_t *key);

/* Saves the key code to the EEPROM if valid data is received. This function 
   returns false if no valid data is received or the code is already saved or 
   the EEPROM is full.
   You must assign false to ASKRmt_AutoDiscardUnsavedKeys before the saving 
   process.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_SaveKey(void);

/* Picks the data and saves the key code to the EEPROM if valid data is 
   received. This function returns false if no valid data is received or the 
   code is already saved or the EEPROM is full.
   You must assign false to ASKRmt_AutoDiscardUnsavedKeys before the saving 
   process.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_PickDataAndSaveKey(void);

/* Deletes the key code from the EEPROM if valid data is received. This 
   function returns false if no valid data is received or the code does not 
   exist in the EEPROM.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_DeleteKey(void);

/* Picks the data and deletes the key code from the EEPROM if valid data is 
   received. This function returns false if no valid data is received or the 
   code does not exist in the EEPROM.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_PickDataAndDeleteKey(void);

/* Deletes the key code from the EEPROM. "code" is the pointer to an array of 3 
   bytes. This function returns false if the code does not exist in the EEPROM.*/
bool ASKRmt_DeleteKeyByCode(uint8_t *code);

/* Deletes all of the key codes from the EEPROM.                               */
void ASKRmt_DeleteAllKeys(void);

/* This function reads a key code from the EEPROM by index and copies 3 bytes 
   of code to the "code" array. This function returns false if the index is out 
   of range. */
bool ASKRmt_GetKeyCodeByIndex(uint8_t index, uint8_t *code);

#endif

#endif /* ASKRemoteControlDecoder_H_ */