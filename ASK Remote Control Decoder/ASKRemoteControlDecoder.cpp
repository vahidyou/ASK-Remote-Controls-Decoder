/*
 * ASKRemoteControlDecoder.cpp
 *  ASK RF remote controls signal decoder. FixCode and LearningCode remote controls are supported.
 *
 * This program is published under the terms of the MIT License.
 * This program is free software and can be distributed by everyone.
 * You can modify this program and distribute it with your name and contact information as the author.
 * No warranty of any kind is expressed or implied. You use this program at your own risk.
 *
 *   Created: 07 May 2019 01:27 AM
 *    Author: Mohammad Yousefi (www.dihav.com - mohammad-yousefi.id.ir - vahidyou@gmail.com)
 * Last Edit: 07 Jun 2021
 */

#include <avr/io.h>
#include <avr/cpufunc.h>
#include <avr/eeprom.h>
#include "ASKRemoteControlDecoder.h"

uint8_t          BitIndex = 254;
uint16_t         HighTime, LowTime;
volatile uint8_t ReceivedData[3];
volatile bool    DataReceived = false;

#ifdef ASKRmt_SAVEREMOTECONTROLSTOEEPROM
volatile bool    ASKRmt_AutoDiscardUnsavedRemotes = true;
uint16_t         RemoteCodeAddr;
bool             IsRemoteFixCode, IsRemoteSaved;

bool CheckIsRemoteSaved(void);
#endif

#ifdef ASKRmt_SAVEKEYCODESTOEEPROM
volatile bool    ASKRmt_AutoDiscardUnsavedKeys = true;
uint16_t         KeyCodeAddr;
bool             IsKeySaved;

bool CheckIsKeySaved(void);
#endif

void ASKRmt_RFSignalPinChanged(uint8_t pinValue)
{
	// discard signal if there is unread data
	if (DataReceived) return;
	// read timer counter value and reset it
	uint16_t tim;
	tim = ASKRmt_2BYTE1MHZTIMER_COUNTERVALUE; // atomic read/write is not needed inside ISR
	ASKRmt_2BYTE1MHZTIMER_RESETCOUNTER; // atomic read/write is not needed inside ISR
	// check signal pin
	if (pinValue) // raise
	{
		LowTime = tim;
		if (24 > BitIndex) // analyze received bit
		{
			if ((HighTime > (LowTime * 2)) && (HighTime < (LowTime * 4))) // check 1 signal (HighTime/LowTime~3)
				ReceivedData[BitIndex / 8] |= (1 << (7 - (BitIndex % 8)));
			else if ((LowTime > (HighTime * 2)) && (LowTime < (HighTime * 4))) // check 0 signal (LowTime/HighTime~3)
				_NOP();
			else // ignore the entire packet if data is invalid
				BitIndex = 253;
		}
		if (255 == BitIndex) // check preamble signal (LowTime/HighTime~30)
		{
			if ((LowTime > (HighTime * 27)) && (LowTime < (HighTime * 33)))
			{
				ReceivedData[0] = 0;
				ReceivedData[1] = 0;
				ReceivedData[2] = 0;
			}
			else
				BitIndex = 253;
		}
		if (254 == BitIndex) ASKRmt_2BYTE1MHZTIMER_START; // start timer
		BitIndex++;
		if (24 == BitIndex) // if 24 bits received
		{
			DataReceived = true; // raise the received flag
			BitIndex = 254;      // reset BitIndex counter
			#ifdef ASKRmt_SAVEREMOTECONTROLSTOEEPROM
			IsRemoteSaved = false;
			if (ASKRmt_AutoDiscardUnsavedRemotes) {
				IsRemoteSaved = CheckIsRemoteSaved();
				DataReceived = IsRemoteSaved;
			}
			#endif
			#ifdef ASKRmt_SAVEKEYCODESTOEEPROM
			IsKeySaved = false;
			if (ASKRmt_AutoDiscardUnsavedKeys) {
				IsKeySaved = CheckIsKeySaved();
				DataReceived = IsKeySaved;
			}
			#endif
		}
	}
	else // fall
		HighTime = tim;
}

void ASKRmt_TwoByte1MHzTimerOverflowInterrupt(void)
{
	// stop timer and reset bit counter after about 65 milliseconds of no signal
	// This part will never executes when the ASK RF receiver module is on. Because there is a lot of RF noise.
	ASKRmt_2BYTE1MHZTIMER_STOP;
	BitIndex = 254;
}

bool ASKRmt_IsDataReceived(void)
{
	return DataReceived;
}

void ASKRmt_DiscardData(void)
{
	DataReceived = false;
}

bool ASKRmt_GetData(uint8_t *data)
{
	if (DataReceived)
	{
		data[0] = ReceivedData[0];
		data[1] = ReceivedData[1];
		data[2] = ReceivedData[2];
		return true;
	}
	return false;
}

bool ASKRmt_PickData(uint8_t *data)
{
	if (DataReceived)
	{
		data[0] = ReceivedData[0];
		data[1] = ReceivedData[1];
		data[2] = ReceivedData[2];
		DataReceived = false;
		return true;
	}
	return false;
}

uint8_t GetFixCodeKey(void)
{
	return (((ReceivedData[2] >> 3) & 0b1100) | ((ReceivedData[2] >> 1) & 0b0011));
}

int8_t ASKRmt_GetKey(bool isFixCode)
{
	if (DataReceived)
	{
		if (isFixCode)
			return GetFixCodeKey();
		else
			return (ReceivedData[2] & 0xF);
	}
	return -1;
}

int8_t ASKRmt_PickKey(bool isFixCode)
{
	if (DataReceived)
	{
		int8_t r;
		if (isFixCode)
			r = GetFixCodeKey();
		else
			r = ReceivedData[2] & 0xF;
		DataReceived = false;
		return r;
	}
	return -1;
}

#ifdef ASKRmt_SAVEREMOTECONTROLSTOEEPROM

bool CheckIsRemoteSaved(void)
{
	uint8_t val01, val2;
	for (RemoteCodeAddr = ASKRmt_EEPROM_START; RemoteCodeAddr < ASKRmt_EEPROM_END; RemoteCodeAddr += 3)
	{
		val2 = eeprom_read_byte((const uint8_t *)(RemoteCodeAddr + 2));
		if (0xFF == val2) continue;
		val01 = eeprom_read_byte((const uint8_t *)RemoteCodeAddr);
		if (val01 != ReceivedData[0]) continue;
		val01 = eeprom_read_byte((const uint8_t *)(RemoteCodeAddr + 1));
		if (val01 != ReceivedData[1]) continue;
		// least significant nibble of 3rd byte is the remote control type (0:LearningCode, 1:FixCode)
		IsRemoteFixCode = (val2 & 1);
		if (IsRemoteFixCode) return true; // if remote control is code fix don't compare third byte
		if (val2 == (ReceivedData[2] & 0xF0)) return true;
	}
	return false;
}

int8_t ASKRmt_GetKeyIfRemoteSaved(void)
{
	if (DataReceived)
	{
		if (!IsRemoteSaved) IsRemoteSaved = CheckIsRemoteSaved();
		if (IsRemoteSaved)
		{
			if (IsRemoteFixCode)
				return GetFixCodeKey();
			else
				return (ReceivedData[2] & 0xF);
		}
	}
	return -1;
}

int8_t ASKRmt_PickKeyIfRemoteSaved(void)
{
	if (DataReceived)
	{
		if (!IsRemoteSaved) IsRemoteSaved = CheckIsRemoteSaved();
		if (IsRemoteSaved)
		{
			int8_t r;
			if (IsRemoteFixCode)
				r = GetFixCodeKey();
			else
				r = ReceivedData[2] & 0xF;
			DataReceived = false;
			return r;
		}
		DataReceived = false;
	}
	return -1;
}

bool SaveRemote(bool isFixCode)
{
	uint8_t val;
	for (RemoteCodeAddr = ASKRmt_EEPROM_START; RemoteCodeAddr < ASKRmt_EEPROM_END; RemoteCodeAddr += 3)
	{
		val = eeprom_read_byte((const uint8_t *)(RemoteCodeAddr + 2));
		if (0xFF == val) {
			eeprom_write_byte((uint8_t *)RemoteCodeAddr, ReceivedData[0]);
			eeprom_write_byte((uint8_t *)(RemoteCodeAddr + 1), ReceivedData[1]);
			if (isFixCode) 
				eeprom_write_byte((uint8_t *)(RemoteCodeAddr + 2), 1);
			else
				eeprom_write_byte((uint8_t *)(RemoteCodeAddr + 2), ReceivedData[2] & 0xF0);
			return true;
		}
	}
	return false;
}

bool ASKRmt_SaveRemote(bool isFixCode)
{
	if (DataReceived)
	{
		if (!IsRemoteSaved) IsRemoteSaved = CheckIsRemoteSaved();
		if (IsRemoteSaved) return false;
		return SaveRemote(isFixCode);
	}
	return false;
}

bool ASKRmt_PickDataAndSaveRemote(bool isFixCode)
{
	if (DataReceived)
	{
		if (!IsRemoteSaved) IsRemoteSaved = CheckIsRemoteSaved();
		if (IsRemoteSaved)
		{
			DataReceived = false;
			return false;
		}
		bool r = SaveRemote(isFixCode);
		DataReceived = false;
		return r;
	}
	return false;
}

bool ASKRmt_SaveRemoteAutoDetectType(void)
{
	if (DataReceived)
	{
		if (!IsRemoteSaved) IsRemoteSaved = CheckIsRemoteSaved();
		if (IsRemoteSaved) return false;
		if (0b0001 == (ReceivedData[2] & 0xF)) return SaveRemote(false);
		if (0b0011 == (ReceivedData[2] & 0xF)) return SaveRemote(true);
	}
	return false;
}

bool ASKRmt_PickDataAndSaveRemoteAutoDetectType(void)
{
	if (DataReceived)
	{
		if (!IsRemoteSaved) IsRemoteSaved = CheckIsRemoteSaved();
		if (IsRemoteSaved)
		{
			DataReceived = false;
			return false;
		}
		bool r = false;
		if (0b0001 == (ReceivedData[2] & 0xF)) r = SaveRemote(false);
		if (0b0011 == (ReceivedData[2] & 0xF)) r = SaveRemote(true);
		DataReceived = false;
		return r;
	}
	return false;
}

bool ASKRmt_DeleteRemote(void)
{
	if (DataReceived)
	{
		if (!IsRemoteSaved) IsRemoteSaved = CheckIsRemoteSaved();
		if (IsRemoteSaved)
		{
			eeprom_write_byte((uint8_t *)(RemoteCodeAddr + 2), 0xFF);
			return true;
		}
	}
	return false;
}

bool ASKRmt_PickDataAndDeleteRemote(void)
{
	if (DataReceived)
	{
		if (!IsRemoteSaved) IsRemoteSaved = CheckIsRemoteSaved();
		if (IsRemoteSaved)
		{
			eeprom_write_byte((uint8_t *)(RemoteCodeAddr + 2), 0xFF);
			DataReceived = false;
			return true;
		}
		DataReceived = false;
	}
	return false;
}

bool ASKRmt_DeleteRemoteByCode(uint8_t *code)
{
	uint8_t val01, val2;
	for (uint16_t addr = ASKRmt_EEPROM_START; addr < ASKRmt_EEPROM_END; addr += 3)
	{
		val2 = eeprom_read_byte((const uint8_t *)(addr + 2));
		if (0xFF == val2) continue;
		val01 = eeprom_read_byte((const uint8_t *)addr);
		if (val01 != code[0]) continue;
		val01 = eeprom_read_byte((const uint8_t *)(addr + 1));
		if (val01 != code[1]) continue;
		if ((val2 & 1) || (val2 == (code[2] & 0xF0))) 
			eeprom_write_byte((uint8_t *)(addr + 2), 0xFF);
		return true;
	}
	return false;
}

void ASKRmt_DeleteAllRemotes(void)
{
	for (uint16_t addr = ASKRmt_EEPROM_START; addr < ASKRmt_EEPROM_END; addr += 3)
		if (0xFF != eeprom_read_byte((const uint8_t *)(addr + 2)))
			eeprom_write_byte((uint8_t *)(addr + 2), 0xFF);
}

bool ASKRmt_GetRemoteCodeByIndex(uint8_t index, uint8_t *code)
{
	uint16_t addr = ASKRmt_EEPROM_START;
	addr += index * 3;
	if (addr + 2 > ASKRmt_EEPROM_END) return false;
	code[0] = eeprom_read_byte((const uint8_t *)addr);
	code[1] = eeprom_read_byte((const uint8_t *)(addr + 1));
	code[2] = eeprom_read_byte((const uint8_t *)(addr + 2));
	return true;
}

#endif

#ifdef ASKRmt_SAVEKEYCODESTOEEPROM

bool CheckIsKeySaved(void)
{
	uint8_t val;
	for (KeyCodeAddr = ASKRmt_EEPROM_START; KeyCodeAddr < ASKRmt_EEPROM_END; KeyCodeAddr += 3)
	{
		val = eeprom_read_byte((const uint8_t *)(KeyCodeAddr + 2));
		if (0xFF == val) continue;
		if (val != ReceivedData[2]) continue;
		val = eeprom_read_byte((const uint8_t *)KeyCodeAddr);
		if (val != ReceivedData[0]) continue;
		val = eeprom_read_byte((const uint8_t *)(KeyCodeAddr + 1));
		if (val == ReceivedData[1]) return true;
	}
	return false;
}

bool ASKRmt_GetKeyIfKeySaved(uint8_t *key)
{
	if (DataReceived)
	{
		if (!IsKeySaved) IsKeySaved = CheckIsKeySaved();
		if (IsKeySaved)
		{
			*key = ReceivedData[2];
			return true;
		}
	}
	return false;
}

bool ASKRmt_PickKeyIfKeySaved(uint8_t *key)
{
	if (DataReceived)
	{
		if (!IsKeySaved) IsKeySaved = CheckIsKeySaved();
		if (IsKeySaved)
		{
			*key = ReceivedData[2];
			DataReceived = false;
			return true;
		}
		DataReceived = false;
	}
	return false;
}

bool SaveKey(void)
{
	uint8_t val;
	for (KeyCodeAddr = ASKRmt_EEPROM_START; KeyCodeAddr < ASKRmt_EEPROM_END; KeyCodeAddr += 3)
	{
		val = eeprom_read_byte((const uint8_t *)(KeyCodeAddr + 2));
		if (0xFF == val) {
			eeprom_write_byte((uint8_t *)KeyCodeAddr, ReceivedData[0]);
			eeprom_write_byte((uint8_t *)(KeyCodeAddr + 1), ReceivedData[1]);
			eeprom_write_byte((uint8_t *)(KeyCodeAddr + 2), ReceivedData[2]);
			return true;
		}
	}
	return false;
}

bool ASKRmt_SaveKey(void)
{
	if (DataReceived)
	{
		if (!IsKeySaved) IsKeySaved = CheckIsKeySaved();
		if (IsKeySaved) return false;
		return SaveKey();
	}
	return false;
}

bool ASKRmt_PickDataAndSaveKey(void)
{
	if (DataReceived)
	{
		if (!IsKeySaved) IsKeySaved = CheckIsKeySaved();
		if (IsKeySaved)
		{
			DataReceived = false;
			return false;
		}
		bool r = SaveKey();
		DataReceived = false;
		return r;
	}
	return false;
}

bool ASKRmt_DeleteKey(void)
{
	if (DataReceived)
	{
		if (!IsKeySaved) IsKeySaved = CheckIsKeySaved();
		if (IsKeySaved)
		{
			eeprom_write_byte((uint8_t *)(KeyCodeAddr + 2), 0xFF);
			return true;
		}
	}
	return false;
}

bool ASKRmt_PickDataAndDeleteKey(void)
{
	if (DataReceived)
	{
		if (!IsKeySaved) IsKeySaved = CheckIsKeySaved();
		if (IsKeySaved)
		{
			eeprom_write_byte((uint8_t *)(KeyCodeAddr + 2), 0xFF);
			DataReceived = false;
			return true;
		}
		DataReceived = false;
	}
	return false;
}

bool ASKRmt_DeleteKeyByCode(uint8_t *code)
{
	uint8_t val;
	for (uint16_t addr = ASKRmt_EEPROM_START; addr < ASKRmt_EEPROM_END; addr += 3)
	{
		val = eeprom_read_byte((const uint8_t *)(addr + 2));
		if (0xFF == val) continue;
		val = eeprom_read_byte((const uint8_t *)addr);
		if (val != code[0]) continue;
		val = eeprom_read_byte((const uint8_t *)(addr + 1));
		if (val != code[1]) continue;
		eeprom_write_byte((uint8_t *)(addr + 2), 0xFF);
		return true;
	}
	return false;
}

void ASKRmt_DeleteAllKeys(void)
{
	for (uint16_t addr = ASKRmt_EEPROM_START; addr < ASKRmt_EEPROM_END; addr += 3)
		if (0xFF != eeprom_read_byte((const uint8_t *)(addr + 2)))
			eeprom_write_byte((uint8_t *)(addr + 2), 0xFF);
}

bool ASKRmt_GetKeyCodeByIndex(uint8_t index, uint8_t *code)
{
	uint16_t addr = ASKRmt_EEPROM_START;
	addr += index * 3;
	if (addr + 2 > ASKRmt_EEPROM_END) return false;
	code[0] = eeprom_read_byte((const uint8_t *)addr);
	code[1] = eeprom_read_byte((const uint8_t *)(addr + 1));
	code[2] = eeprom_read_byte((const uint8_t *)(addr + 2));
	return true;
}

#endif