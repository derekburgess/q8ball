Inspired by: https://github.com/Spooky-Manufacturing/QRNG

That QRNG repo originally had an ardunio sketch and a skematic for a hardware. It made me think of magic 8-balls and so I made this. Changing a few aspects of the original hardware, logic, and interface (OLED). Waits in standby until you shake it... first it takes a sample reading for the environment, then uses that to offset the actual reading- display all values in real time. The higher of the two values returns a random 8-ball phrase in the true or false list.

Using the Arduino IDE: https://www.arduino.cc/en/software -- Push the sketch (`q8ball.ino`) to the board... good luck.

TODO Wiring diagram...

![photo](/assets/photo.jpg)