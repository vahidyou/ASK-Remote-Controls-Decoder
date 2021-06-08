# ASK RF Remote Controls Signal Decoder
This project is a program written in the Atmel Studio environment and compiled by GCC for ATmega8A microcontroller to decode common FixCode and LearningCode ASK RF remote controls signals. These remote controls usually encode data using PT2262, EV1527, HS1527, or RT1527 ICs. This program contains functions to decode received data, extract key code, save the remote controls or keys to the EEPROM, etc, and is very useful for making a receiver circuit. The code is written in C++, but it is also compatible with C.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Author
* **Mohammad Yousefi** - *Initial work* - [vahidyou](https://github.com/vahidyou)

## Preparing for Usage
This program is written for the ATmega8A microcontroller but you can use it for any AVR microcontroller just by little changes in the code. The microcontroller clock must be 1MHz or more.

Include *ASKRemoteControlDecoder.h* to your program.
```C++
#include "PATH/ASKRemoteControlDecoder.h"
```
Call `ASKRmt_RFSignalPinChanged` and `ASKRmt_TwoByte1MHzTimerOverflowInterrupt` on ISRs.
```C++
ISR(INT0_vect)
{
	ASKRmt_RFSignalPinChanged(PIND & (1 << PIND2));
}

ISR(TIMER1_OVF_vect)
{
	ASKRmt_TwoByte1MHzTimerOverflowInterrupt();
}
```
The above code uses INT0 external interrupt pin as the input pin for the signals. So you must configure this pin as input and INT0 interrupt for both rising and falling edges.
```C++
DDRD  = 0bxxxxx0xx;
MCUCR = (1 << ISC00);
GICR  = (1 << INT0);
```
Timer1 is used for measuring the length of the signals. So you must enable its overflow interrupt.
```C++
TIMSK = (1 << TOIE1);
```
Don't forget to enable global interrupts by setting the `I` bit of the `SREG`.
```C++
sei();
```
Open the file *ASKRemoteControlDecoder.h* and adjust Timer1 start, stop, read counter and reset counter codes. It is better to run the timer at 1MHz. The default value for TCCR1B register to start timer is 1 (no prescaling).
```C++
#define ASKRmt_2BYTE1MHZTIMER_START        TCCR1B = 1
#define ASKRmt_2BYTE1MHZTIMER_STOP         TCCR1B = 0
#define ASKRmt_2BYTE1MHZTIMER_COUNTERVALUE TCNT1
#define ASKRmt_2BYTE1MHZTIMER_RESETCOUNTER TCNT1 = 0
```
Open the file *ASKRemoteControlDecoder.h* and adjust EEPROM start and end positions for saving remote controls or keys if you want to use this feature in your program, otherwise comment `ASKRmt_SAVEREMOTECONTROLSTOEEPROM` and `ASKRmt_SAVEKEYCODESTOEEPROM` definitions to reduce the program size and speed it up. You can only have one of the above options. Note that each remote control or key code requires 3 bytes. By the default values, 20 remote controls or keys can be saved into the EEPROM from addresses 0 to 59.
```C++
#define ASKRmt_SAVEREMOTECONTROLSTOEEPROM
#define ASKRmt_SAVEKEYCODESTOEEPROM
#define ASKRmt_EEPROM_START 0
#define ASKRmt_EEPROM_END  59
```

## Variables and Functions
Variables and functions of this library start with `ASKRmt_` prefix. Some functions will pick (read and discard) the received data. Note that while data is not picked or discarded, new data will not receive.

```C++
void ASKRmt_RFSignalPinChanged(uint8_t pinValue);
```
Call this subroutine on any change of signal pin.

```C++
void ASKRmt_TwoByte1MHzTimerOverflowInterrupt(void);
```
Call this subroutine on the 2-byte timer overflow interrupt.

```C++
bool ASKRmt_IsDataReceived(void);
```
Returns true if valid data is received. This function will not pick the data. 

```C++
void ASKRmt_DiscardData(void);
```
Discards the received data.

```C++
bool ASKRmt_GetData(uint8_t *data);
```
Reads the data and returns true if valid data is received. The received data (3 bytes) will be copied to the `data` array. This function will not pick the data.

```C++
bool ASKRmt_PickData(uint8_t *data);
```
Picks the data and returns true if valid data is received. The received data (3 bytes) will be copied to the `data` array.    

```C++
int8_t ASKRmt_GetKey(bool isFixCode);
```
Returns the key number if valid data is received, otherwise returns -1. This function will not pick the data.

```C++
int8_t ASKRmt_PickKey(bool isFixCode);
```
Picks the data and returns the key number if valid data is received, otherwise returns -1.



```C++
extern volatile bool ASKRmt_AutoDiscardUnsavedRemotes;
```

If this variable is true and the received data remote control code is not saved to the EEPROM, the data will be discarded automatically. The default value is true.

```C++
int8_t ASKRmt_GetKeyIfRemoteSaved(void);
```
Returns the key number if valid data is received and remote control code has been saved to the EEPROM, otherwise returns -1. The type of remote control will be detected automatically. This function will not pick the data.

```C++
int8_t ASKRmt_PickKeyIfRemoteSaved(void);
```
Picks the data and returns the key number if valid data is received and remote control code has been saved to the EEPROM, otherwise returns -1. The type of remote control will be detected automatically.

```C++
bool ASKRmt_SaveRemote(bool isFixCode);
```
Saves the remote control code to the EEPROM if valid data is received. This function returns false if no valid data is received or the code is already saved or the EEPROM is full. You must assign false to `ASKRmt_AutoDiscardUnsavedRemotes` before the saving process. This function will not pick the data.

```C++
bool ASKRmt_PickDataAndSaveRemote(bool isFixCode);
```
Picks the data and saves the remote control code to the EEPROM if valid data is received. This function returns false if no valid data is received or the code is already saved or the EEPROM is full. You must assign false to `ASKRmt_AutoDiscardUnsavedRemotes` before the saving process.

```C++
bool ASKRmt_SaveRemoteAutoDetectType(void);
```
Saves the remote control code to the EEPROM if valid data is received. The type of remote control will be detected automatically. The user must only press key 1 or A. This function returns false if no valid data is received or the code is already saved or the EEPROM is full or type detection fails due to pressing another key. You must assign false to `ASKRmt_AutoDiscardUnsavedRemotes` before the saving process. This function will not pick the data.

```C++
bool ASKRmt_PickDataAndSaveRemoteAutoDetectType(void);
```
Picks the data and saves the remote control code to the EEPROM if valid data is received. The type of remote control will be detected automatically. The user must only press key 1 or A. This function returns false if no valid data is received or the code is already saved or the EEPROM is full or type detection fails due to pressing another key. You must assign false to `ASKRmt_AutoDiscardUnsavedRemotes` before the saving process.

```C++
bool ASKRmt_DeleteRemote(void);
```
Deletes the remote control code from the EEPROM if valid data is received. The type of remote control will be detected automatically. This function returns false if no valid data is received or the code does not exist in the EEPROM. This function will not pick the data.

```C++
bool ASKRmt_PickDataAndDeleteRemote(void);
```
Picks the data and deletes the remote control code from the EEPROM if valid data is received. The type of remote control will be detected automatically. This function returns false if no valid data is received or the code does not exist in the EEPROM.

```C++
bool ASKRmt_DeleteRemoteByCode(uint8_t *code);
```
Deletes the remote control code from the EEPROM. The type of remote control will be detected automatically. `code` is the pointer to an array of 3 bytes.

```C++
void ASKRmt_DeleteAllRemotes(void);
```
Deletes all of the saved remote controls from the EEPROM.

```C++
bool ASKRmt_GetRemoteCodeByIndex(uint8_t index, uint8_t *code);
```
This function reads a remote control code from the EEPROM by `index` and copies 3 bytes of code to the `code` array. 



```C++
extern volatile bool ASKRmt_AutoDiscardUnsavedKeys;
```

If this variable is true and the received key code is not saved to the EEPROM, the data will be discarded automatically. The default value is true.

```C++
bool ASKRmt_GetKeyIfKeySaved(uint8_t *key);
```
Returns true if valid data is received and key code has been saved to the EEPROM, otherwise returns false. The key number will be stored to the variable pointed by the `key` parameter. *The type of remote control will not be detected automatically.* This function will not pick the data.  Note that while data is not picked or discarded, new data will not receive.

```C++
bool ASKRmt_PickKeyIfKeySaved(uint8_t *key);
```
Picks the data and returns true if valid data is been saved to the EEPROM, otherwise returns false. The key number will be stored to the variable pointed by the `key` parameter. *The type of remote control will not be detected automatically.* Note that while data is not picked or discarded, new data will not receive.
```C++
bool ASKRmt_SaveKey(void);
```
Saves the key code to the EEPROM if valid data is received. This function returns false if no valid data is received or the code is already saved or the EEPROM is full. You must assign false to ASKRmt_AutoDiscardUnsavedKeys before the saving process. This function will not pick the data. Note that while data is not picked or discarded, new data will not receive.
```C++
bool ASKRmt_PickDataAndSaveKey(void);
```
Picks the data and saves the key code to the EEPROM if valid data is received. This function returns false if no valid data is received or the code is already saved or the EEPROM is full. You must assign false to ASKRmt_AutoDiscardUnsavedKeys before the saving process. Note that while data is not picked or discarded, new data will not receive.
```C++
bool ASKRmt_DeleteKey(void);
```
Deletes the key code from the EEPROM if valid data is received. This function returns false if no valid data is received or the code does not exist in the EEPROM. This function will not pick the data. Note that while data is not picked or discarded, new data will not receive.

```C++
bool ASKRmt_PickDataAndDeleteKey(void);
```
Picks the data and deletes the key code from the EEPROM if valid data is received. This function returns false if no valid data is received or the code does not exist in the EEPROM. Note that while data is not picked or discarded, new data will not receive.
```C++
bool ASKRmt_DeleteKeyByCode(uint8_t *code);
```
Deletes the key code from the EEPROM. `code` is the pointer to an array of 3 bytes. This function returns false if the code does not exist in the EEPROM.
```C++
void ASKRmt_DeleteAllKeys(void);
```
Deletes all of the key codes from the EEPROM.
```C++
bool ASKRmt_GetKeyCodeByIndex(uint8_t index, uint8_t *code);
```
This function reads a key code from the EEPROM by index and copies 3 bytes of code to the `code` array. This function returns false if the index is out of range.

## Test Project
I made a simple circuit to test this program.
![ASK Remote Controls Decoder](Test%20Circuit/ASKRmtCntrlDcdr_bb.png)
![ASK Remote Controls Decoder](Test%20Circuit/ASKRmtCntrlDcdr_schem.png)
The microcontroller is configured to run by the 1MHz internal RC oscillator. Fuse bits are H=0xD9 L=0xE1.

The *Test Project* works in 4 modes:

**1. Normal mode (PB0:H, PB1:H, PB2:H) [LED on PB3 is off]:** When pressing any key on the remote control, the data will be sent to the UART and if the remote control/key code has already saved, the key code will be displayed by LEDs on pins PC0 to PC3.

**2. Add mode (PB0:L, PB1:H, PB2:H) [LED on PB3 is on]:** The remote control will be saved by pressing key 1 or A in "save remote controls" mode. The key code will be saved by pressing any key in "save keys" mode. After a successful operation LED on PB3 will blink fast 10 times. The data will be sent to the UART and the key code will be displayed by LEDs on pins PC0 to PC3.

**3. Remove mode (PB0:H, PB1:L, PB2:H) [LED on PB3 is blinking]:** The remote control/key code will be removed by pressing any key. After a successful operation LED on PB3 will blink fast 10 times. The data will be sent to the UART.

**4. Delete All mode (PB0: H, PB1: H, PB2: L):** All saved remote controls/key codes will be removed by making PB2 low for a short time. After a successful operation LED on PB3 will blink fast 10 times.

Modes 1-3 can be selected by 2 switches (SW1). Mode 4 is just a momentary mode activate by pressing the S1 button.
