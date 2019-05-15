# ASK RF Remote Controls Signal Decoder
This project is a C++ program written in Atmel Studio environment and compiled by GCC for ATmega8A microcontroller to decode common FixCode and LearningCode ASK RF remote controls signals. These remote controls usually encode data using PT2262, EV1527, HS1527, or RT1527 ICs. This program contains functions to read received data, extract key code, save the remote control to the EEPROM and etc. It is very useful for making a receiver circuit.
## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
## Author
* **Mohammad Yousefi** - *Initial work* - [vahidyou](https://github.com/vahidyou)
## Preparing for Usage
This program is written for ATmega8A microcontroller but you can use it for any AVR microcontroller just by little changes in the code.
The program uses INT0 extrenal interrupt pin as the input pin for the signals. So you must set this pin to input and enable its interrupt for both raing and falling edges.
```C++
DDRD  = 0bxxxxx0xx;
MCUCR = (1 << ISC00);
GICR  = (1 << INT0);
```C++
Timer1 is used for measuring the signals so you must enable its overflow interrupt.
```C++
TIMSK = (1 << TOIE1);
```C++
Open the file *ASKRemoteControlDecoder.h* and adjust EEPROM start and end address for saving remote controls if you want to use this featyre in your program. Not that each remote control requires 3 bytes.
```C++
#define ASKRmt_EEPROM_START 0
#define ASKRmt_EEPROM_END  59
```
Include *ASKRemoteControlDecoder.h* to your program and use variables and functions starting with `ASKRmt_`.
```C++
#include "PATH/ASKRemoteControlDecoder.h"
```
## Variables and Functions
/* If this variable is true and the received data remote control code is not saved to
   the EEPROM, the data will be discarded automatically. */
extern volatile bool ASKRmt_AutoDiscardUnsavedRemotes;

/* Returns true if valid data is received.
   This function will not pick the data. 
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_IsDataReceived(void);

/* Discards received data.
   Note that while data is not picked or discarded, new data will not receive. */
void ASKRmt_DiscardData(void);

/* Reads data and returns true if valid data is received.
   The received data (3 bytes) will be copied to the data array. 
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_GetData(uint8_t *data);

/* Picks data and returns true if valid data is received.
   The received data (3 bytes) will be copied to the data array.    
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_PickData(uint8_t *data);

/* Returns key number if valid data is received, otherwise returns -1.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
int8_t ASKRmt_GetKey(bool isFixCode);

/* Picks data and returns key number if valid data is received, otherwise returns -1.
   Note that while data is not picked or discarded, new data will not receive. */
int8_t ASKRmt_PickKey(bool isFixCode);

/* Returns key number if valid data is received and remote control code has been
   saved to the EEPROM, otherwise returns -1.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
int8_t ASKRmt_GetKeyIfRemoteSaved(void);

/* Picks data and returns key number if valid data is received and remote control code has been
   saved to the EEPROM, otherwise returns -1.
   Note that while data is not picked or discarded, new data will not receive. */
int8_t ASKRmt_PickKeyIfRemoteSaved(void);

/* Saves the remote control code to the EEPROM if valid data is received.
   This function returns false if no valid data is received or the code is already saved or
   EEPROM is full.
   You must assign false to ASKRmt_AutoDiscardUnsavedRemotes before saving process.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_SaveRemote(bool isFixCode);

/* Picks data and saves the remote control code to the EEPROM if valid data is received.
   This function returns false if no valid data is received or the code is already saved or
   EEPROM is full.
   You must assign false to ASKRmt_AutoDiscardUnsavedRemotes before saving process.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_PickDataAndSaveRemote(bool isFixCode);

/* Saves the remote control code to the EEPROM if valid data is received. Type of the remote 
   control will be detected automatically. The user must only press key 1 or A.
   This function returns false if no valid data is received or the code is already saved or
   EEPROM is full or type detection fails due to pressing another key.
   You must assign false to ASKRmt_AutoDiscardUnsavedRemotes before saving process.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_SaveRemoteAutoDetectType(void);

/* Picks data and saves the remote control code to the EEPROM if valid data is received. Type 
   of the remote control will be detected automatically. The user must only press key 1 or A.
   This function returns false if no valid data is received or the code is already saved or
   EEPROM is full or type detection fails due to pressing another key.
   You must assign false to ASKRmt_AutoDiscardUnsavedRemotes before saving process.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_PickDataAndSaveRemoteAutoDetectType(void);

/* Deletes the remote control code from the EEPROM if valid data is received. Type of the remote 
   control will be detected automatically.
   This function returns false if no valid data is received or the code does not exist in the EEPROM.
   This function will not pick the data.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_DeleteRemote(void);

/* Picks data and deletes the remote control code from the EEPROM if valid data is received. Type 
   of the remote control will be detected automatically.
   This function returns false if no valid data is received or the code does not exist in the EEPROM.
   Note that while data is not picked or discarded, new data will not receive. */
bool ASKRmt_PickDataAndDeleteRemote(void);

/* Deletes the remote control code from the EEPROM. Type of the remote control will be detected 
   automatically.
   This function returns false if the code does not exist in the EEPROM. */
bool ASKRmt_DeleteRemoteByCode(uint8_t *code);

/* Deletes all of the remote controls codes from the EEPROM. */
void ASKRmt_DeleteAllRemotes(void);

/* This function reads remote control code from the EEPROM and copies 3 bytes of code to the 
   code array. 
   This function returns false if the index is out of range. */
bool ASKRmt_GetRemoteCodeByIndex(uint8_t index, uint8_t *code);

## Test Project
I made a simple circuit to test this program.
![ASK Remote Controls Decoder](Test%20Circuit/ASKRmtCntrlDcdr_bb.png)
![ASK Remote Controls Decoder](Test%20Circuit/ASKRmtCntrlDcdr_schem.png)
The microcontroller is configured to run by 1MHz internal RC oscillator.
The *Test Project* works in 4 modes:
**1. Normal mode (PB0:H, PB1:H, PB2:H) [LED of PB3 is off]:** When pressing any key on the remote control, the data will be sent to the UART and if the remote control has already saved, the key code will be displayed by LEDs on pins PC0 to PC3.
**2. Add mode (PB0:L, PB1:H, PB2:H) [LED of PB3 is on]:** The remote control will be saved by pressing key 1 or A. After a successful operation LED on PB3 will blink fast 10 times. The data will be sent to the UART and the key code will be displayed by LEDs on pins PC0 to PC3.
**3. Remove mode (PB0:H, PB1:L, PB2:H) [LED of PB3 is blinking]:** The remote control will be removed by pressing any key. After a successful operation LED on PB3 will blink fast 10 times. The data will be sent to the UART.  
**4. Delete All mode (PB0: H, PB1: H , PB2: L):** All saved remote controls will be removed by making PB2 low for a short time. After a successful operation LED on PB3 will blink fast 10 times.
Modes 1-3 can be selected by 2 switches. Mode 4 is just a momentary mode.
