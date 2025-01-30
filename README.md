<p align="center"><img src="https://socialify.git.ci/411568/Atmega8_FM_radio/image?custom_description=Simple+FM+radio+based+on+Atmega8+and+TEA5767+module.&description=1&name=1&pattern=Circuit+Board&theme=Dark" alt="Atmega8_FM_radio" width="640" height="320" /></p>


<h2>🧐 Features</h2>

This project is a revision of one of my old designs, created a long long time ago when atmel atmega chips where still dirty cheap.

It contains two seperate boards: 
 - main_radio_board - this one contains an Atmega8 microcontroller, TEA5767 FM radio module, PAM8302 amplifier module and power connector. 
 - display_and_controls_board - this one is specificly designed to fit in an old Unitra radio case, it has LEDs used as radio frequency indicators, rotary encoder for changing the frequency, potentiometer for volume and some additional buttons.


<h2>📱 Electronics design</h2>

My FM radio is based on the TEA7567 module, that is connected to an Atmega8A-PU microconotroller via I2C. Then it's output goes to the connector, then on the main panel passes through a logarithmic potentiometer for volume control 
and comes back to the PAM8302 D-class amplifier module. I chose it because it works well with my speaker, which is the one used in the original Unitra radio (it is probably over 40 years old and still works very well).

The schematic and PCB were both designed using KiCAD 6.0

![schematic kicad](/images/schematic_1.png)

![PCB kicad](/images/pcb_image_1.png)

![3d view](/images/pcb_image_2.png)


<h2>💈 Connector pinouts</h2>

![pcb view with pinout](/images/pcb_pinout.png)

In the image above you can see the 9V DC input jack location, the speaker output connector and two pin headers (IDC connectors).

J4 connector pinout (all of these are generic GPIO pins of the Atmega8):
* 1  - switch 3
* 2  - switch 1
* 3  - NC
* 4  - LE(ED1)
* 5  - SDI
* 6  - switch 4
* 7  - switch 2
* 8  - switch 0
* 9  - OE(ED2)
* 10 - CLK

J3 connector pinout:
* 1  - TEA5767 output (to potentiometer)
* 2  - transistor 0
* 3  - transistor 2
* 4  - encoder B
* 5  - 5V DC
* 6  - 9V DC
* 7  - PAM8302 input (from potentiometer)
* 8  - GND
* 9  - transistor 1
* 10 - encoder A


<h2>📧 Atmega code</h2>

The code is split into several files, you can find them all on my github. 
* encoder.c - containing functions for interfacing with the rotary encoder
* i2c_lib.c - bit banging i2c library
* led_screen.c - functions for interfacing with led driver
* tea5767.c - functions for the TEA5767 FM radio module
* main.c - main program

Main function for the tea5767 is the send_freq function that tells the module to set a certain frequency:

```c
void send_freq()
{
	unsigned char frequencyH;
	unsigned char frequencyL;
	unsigned int frequencyB;

	frequencyB = 4 * (frequency * 1000000 + 225000) / 32768;
	frequencyH = frequencyB >> 8;
	frequencyL = frequencyB & 0xFF;

    //data transmition 
    I2C_Start();
	I2C_Write(TEA5767_ADDRESS_W);
	I2C_Write(frequencyH);
	I2C_Write(frequencyL);
	I2C_Write(0xB0);
	I2C_Write(0x10);
	I2C_Write(0x00);
    I2C_Stop();

    _delay_ms(10);
}
```

and the search function:

```c
void search(uint8_t direction)
{
	unsigned char frequencyH;
	unsigned char frequencyL;
	unsigned int frequencyB;
    double temp = 88.0;

    if(direction == 1)
    {
        temp = frequency + 0.1; 
    }
    else
    {
        temp = frequency - 0.1; 
    }

	frequencyB = 4 * (temp * 1000000 + 225000) / 32768;
	frequencyH = frequencyB >> 8;
	frequencyL = frequencyB & 0xFF;

    I2C_Start();
	I2C_Write(TEA5767_ADDRESS_W); //send address for writing
	I2C_Write(frequencyH |= 0b01000000); //search mode
	I2C_Write(frequencyL);
    if(direction == 1)
    {
        I2C_Write(0b11010000); //search up with mid adc 
    }
    else
    {
        I2C_Write(0b01010000); //search down with mid adc
    }
	I2C_Write(0x10);
	I2C_Write(0x00);

    I2C_Stop();

    _delay_ms(10);
}
```

You can find what the specific bytes being sent to the module mean in its datasheet.


In the main program loop we check the encoder status:

```c
if(val != val_tmp)
{
    if( (val == 3 && val_tmp == 1) || (val == 0 && val_tmp == 2) )
    {
        if(frequency < 108.0)
        {
            frequency += 1.0;
            send_freq(); 
            display(frequency); //display the current frequency
            search(1); //search up from the set freq
        }
    }
    else if( (val == 2 && val_tmp == 0) || (val == 1 && val_tmp == 3) )
    {
        if(frequency > 88.0)
        {
            frequency -= 1.0;
            send_freq();
            display(frequency); //display the current frequency
            search(0); //search down from the set freq
        }
    }
}
```
and change the frequency by 1Hz, then search up from that frequency to fine tune the radio automatically.

In addition to that there are also button inputs:

```c
if(bit_is_clear(button_1_port, button_1_pin)) 
{
    _delay_ms(15); //debounce
    if(bit_is_clear(button_1_port, button_1_pin))
    {
        search(1);
        _delay_ms(500);
        read_freq();
        display(frequency); //display the current frequency
    }
}
```

after pressing the button, the module starts an up search from the current frequency. 
Same aplies to the search down.
You can also change the frequency to one from the predefined tabel by pressing button number 4 or 5.

<h2>💻 Parts list</h2>

As it was just a revision of an older schematic, I did not bother to try and redesign it with newer uC etc. I just used the parts from the previous version (soldered onto a prototype board) or those I had lying around. 
Because of that the design may look a bit peculiar and I decided not to make a BOM file for it. Let me just list them in here for you:
* Atmega8A-PU microcontroller
* TEA5767 module
* PAM8302 amplifier module
* 2x IDC connector for the front panel
* LM317 for power step-down
* 9V DC jack input
* 3x ceramic 100nF capacitor
* 10uF and 4.7uF electrolytic capacitors
* 3x 10k resistor


<h2>📺 Front panel board</h2>

This board is used as my front panel for controls and displaying the current frequency etc.

![schematic kicad](/images/schematic_2.PNG)

![3d view](/images/pcb_image_3.PNG)

![3d view](s/images/3d_view_2.PNG)


<h2>💻 Parts list</h2>

* LTV847 optocoupler
* TLC5916IN LED driver
* 3x 1kOhm resistor
* 1.5kOhm resistor
* 5x tact switch
* logarithmic potentiometer for audio control
* rotary encoder
* 2x IDC connector
* a lot of LEDs (red)

<h2>🛡️ License:</h2>

This project is licensed under the MIT License.


<h2> 🙋‍♂️Authors </h2>

- Krzysztof Sikora

<h2>💖Like my work?</h2>

If you have any questions about the project feel free to contact us via email: krzysieksikora717@gmail.com




