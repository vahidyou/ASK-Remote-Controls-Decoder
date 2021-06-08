/*
 * ASKRmtCtrlDcdr.cpp
 *  ASK RF remote controls signal decoder test project. FixCode and LearningCode remote controls are supported.
 *  Fuse Bits: H=0xD9 L=0xE1
 *  Operation modes:
 *   Normal mode (PB0:H, PB1:H, PB2:H) [LED on PB3 is off]: When pressing any key on the remote control, the data 
 *    will be sent to the UART and if the remote control/key code has already saved, the key code will be 
 *    displayed by LEDs on pins PC0 to PC3.
 *   Add mode (PB0:L, PB1:H, PB2:H) [LED on PB3 is on]: The remote control will be saved by pressing key 1 or A 
 *    in "save remote controls" mode. The key code will be saved by pressing any key in "save keys" mode. After a 
 *    successful operation LED on PB3 will blink fast 10 times. The data will be sent to the UART and the key code 
 *    will be displayed by LEDs on pins PC0 to PC3.
 *   Remove mode (PB0:H, PB1:L, PB2:H) [LED on PB3 is blinking]: The remote control/key code will be removed by 
 *    pressing any key. After a successful operation LED on PB3 will blink fast 10 times. The data will be sent to 
 *    the UART.
 *   Delete All mode (PB0:H, PB1:H, PB2:L): All saved remote controls/key codes will be removed by making PB2 low 
 *    for a short time. After a successful operation LED on PB3 will blink fast 10 times.
 *
 * This program is published under the terms of the MIT License.
 * This program is free software and can be distributed by everyone.
 * You can modify this program and distribute it with your name and contact information as the author.
 * No warranty of any kind is expressed or implied. You use this program at your own risk.
 *
 *   Created: 05 May 2019 1:02 AM
 *    Author: Mohammad Yousefi (www.dihav.com - mohammad-yousefi.id.ir - vahidyou@gmail.com)
 * Last Edit: 08 Jun 2021
 */

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../ASK Remote Control Decoder/ASKRemoteControlDecoder.h"

ISR(INT0_vect)
{
	ASKRmt_RFSignalPinChanged(PIND & (1 << PIND2));
}

ISR(TIMER1_OVF_vect)
{
	ASKRmt_TwoByte1MHzTimerOverflowInterrupt();
}

void UART_TX(uint8_t d)
{
	while (!(UCSRA & (1 << UDRE))) ;
	UDR = d;
}

void LEDWorkDoneSignal(void) {
	// blink LED 10 times fast
	for (uint8_t i = 0; i < 20; i++)
	{
		PORTB ^= (1 << PORTB3);
		_delay_ms(50);
	}
}

int main(void)
{
	// ports configurations
	ACSR |= (1 << ACD); // turn off analog comparator
	DDRB  = 0b11111000; // add switch, remove switch and delete all button pins input, others output
	PORTB = 0b00000111; // enable inputs pull-ups
	DDRC  = 0b111111;   // all output
	DDRD  = 0b11111011; // INT0 input for ASK, others output
	// interrupts configurations
	MCUCR = (1 << ISC00); // select both edges for INT0
	GICR  = (1 << INT0);  // enable INT0 interrupt
	TIMSK = (1 << TOIE1); // enable timer1 overflow interrupt
	// UART	configurations
	UBRRH = 0; UBRRL = 25;                              // 2400bps
	UCSRB = (1 << TXEN);                                // enable TX
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0); // select 8-bit data
	
	uint8_t dataASK[3];
	#ifdef ASKRmt_SAVEREMOTECONTROLSTOEEPROM
	ASKRmt_AutoDiscardUnsavedRemotes = false;
	#endif
	#ifdef ASKRmt_SAVEKEYCODESTOEEPROM
	ASKRmt_AutoDiscardUnsavedKeys = false;
	#endif
	
	sei();
	while (1)
	{
		
		
		if (!(PINB & (1 << PINB0))) // add mode switch
		{
			PORTB |= (1 << PORTB3); // turn on LED
			if (ASKRmt_IsDataReceived())
				#ifdef ASKRmt_SAVEREMOTECONTROLSTOEEPROM
				if (ASKRmt_SaveRemoteAutoDetectType())
				#endif
				#ifdef ASKRmt_SAVEKEYCODESTOEEPROM
				if (ASKRmt_SaveKey())
				#endif
					LEDWorkDoneSignal();
		}
		else if (!(PINB & (1 << PINB1))) // remove mode switch
		{
			PORTB ^= (1 << PORTB3); // blink LED
			if (ASKRmt_IsDataReceived())
				#ifdef ASKRmt_SAVEREMOTECONTROLSTOEEPROM
				if (ASKRmt_PickDataAndDeleteRemote())
				#endif
				#ifdef ASKRmt_SAVEKEYCODESTOEEPROM
				if (ASKRmt_PickDataAndDeleteKey())
				#endif
					LEDWorkDoneSignal();
		}
		else if (!(PINB & (1 << PINB2))) // delete all button
		{
			#ifdef ASKRmt_SAVEREMOTECONTROLSTOEEPROM
			ASKRmt_DeleteAllRemotes();
			#endif
			#ifdef ASKRmt_SAVEKEYCODESTOEEPROM
			ASKRmt_DeleteAllKeys();
			#endif
			LEDWorkDoneSignal();
		}
		else
			PORTB &= ~(1 << PORTB3); // turn off LED
		
		
		// read received data
		if (ASKRmt_GetData(dataASK))
		{
			// print data to the serial port for saved and unsaved remote controls
			UART_TX(dataASK[0]);
			UART_TX(dataASK[1]);
			UART_TX(dataASK[2]);
			// show key on LEDs only for saved remote controls
			#ifdef ASKRmt_SAVEREMOTECONTROLSTOEEPROM
			int8_t key = ASKRmt_PickKeyIfRemoteSaved();
			if (key >= 0)
			#endif
			#ifdef ASKRmt_SAVEKEYCODESTOEEPROM
			uint8_t key;
			if (ASKRmt_PickKeyIfKeySaved(&key))
			#endif
			{
				PORTC = key;
				_delay_ms(200);
				PORTC = 0;
			}
		}
		
		
		_delay_ms(200);
	}
}