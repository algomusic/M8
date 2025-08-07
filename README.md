# M8
An 8-bit PWM Arduino audio synthesis library for the ESP32

A simple audio library for output via a GPIO pin using PWM encoding.

Small, DIY and low-fi.

Usage overview:
- Include M8 and required M8 classes
- Instantiate audio objects
- In setup(), set parameters such as PWM pin, osc pitch, filter cutoff, and decay amount
- In loop(), call a ledcWrite() function every 9 microseconds passing the calculated values to the M8_pwm_pin
- Don't block loop() with delays, print statements or complex functions

M8 uses ESP32-specific LEDC functions, but may work on other microcontrollers using analogWrite() instead.
