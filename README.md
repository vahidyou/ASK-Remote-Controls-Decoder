# ASK RF Remote Controls Signal Decoder
This project is a C++ program written in Atmel Studio environment and compiled by GCC for ATmega8A to decode common FixCode and LearningCode ASK RF remote controls. These remote controls usually encode data using PT2262, EV1527, HS1527, or RT1527 ICs. This program contain functions to read received data, extract key code, save the remoted control to the EEPROM and etc and is very useful for making a reciever circuit.
## License
This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
## Author
* **Mohammad Yousefi** - *Initial work* - [vahidyou](https://github.com/vahidyou)
## Test Project
I made a simple circuit to test this program.
![ASK Remote Controls Decoder](Test%20Circuit/ASKRmtCntrlDcdr_bb.png)
![ASK Remote Controls Decoder](Test%20Circuit/ASKRmtCntrlDcdr_schem.png)
The *Test Project* works in 4 modes:
1. Normal mode (PB0:H, PB1: H, PB2:H) [LED of PB3 is off]: When pressing any key on the remote control, the data will be sent to the UART and if the remote control has already saved, the key code will be displayed by LEDs on pins PC0 to PC3.
2. Add mode (PB0:L, PB1: H, PB2:H) [LED of PB3 is on]: The remote control will be saved by pressing key 1 or A. After a successful operation LED on PB3 will blink fast 10 times. The data will be sent to the UART and the key code will be displayed by LEDs on pins PC0 to PC3.
3. Remove mode (PB0:H, PB1: L, PB2:H) [LED of PB3 is blinking]: The remote control will be removed by pressing any key. After a successful operation LED on PB3 will blink fast 10 times. The data will be sent to the UART.  
4. Delete All mode (PB0:H, PB1:H , PB2:L): All saved remote controls will be removed by making PB2 low for a short time. After a successful operation LED on PB3 will blink fast 10 times.
