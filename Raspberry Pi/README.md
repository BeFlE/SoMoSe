# SoMoSe with Raspberry Pi

To use the SoMoSe with the Raspberry Pi you have to activate the I2C interface on the Pi first. A good tutorial can be found here:
https://developer-blog.net/raspberry-pi-i2c-aktivieren/

The SoMoSe is then connected to the Pi as shown in the picture. 
A test of the function can be done with
`sudo i2cdetect -y 1`
can be done.

![wiring](SoMoSe_Raspi_Wiring.png?raw=true "wiring")

