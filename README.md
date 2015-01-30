# ColdFireSynth
A synth for the Jm128 coldfire microcontroller.

The idea: 
* create a synthesizer utilizing an interrupt at 22khz to generate a sound sample
* samples from 8 oscillators (by just reading memory for the value of the osc) and mixing these together
* sending the mixed signal to an MCP 4921 Digital to Analog converter via SPI
** RC filter to filter the output nicely
* communicate status via character LCD
* take input by using USB host capability to connect to a USB midi mini keyboard


![The hardware](http://www.empiricalnerd.com/wp-content/uploads/2012/09/temp_2.jpg)
![Better hardwware](http://www.empiricalnerd.com/wp-content/uploads/2012/09/IMG_20120923_223652.jpg)
