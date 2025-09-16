# M8
An 8-bit PWM Arduino audio synthesis library for the ESP32

A simple audio library for output via a GPIO pin using PWM encoding.

Small, DIY, and low-fi.

Usage overview:
- Include M8 and desired M8 audio classes
- Instantiate audio objects for each class
- In setup(), set parameters such as PWM pin, osc pitch, filter cutoff, and decay amount
- In loop(), call an ledcWrite() function every 9 microseconds passing the calculated values to the M8_pwm_pin
- Include an Audio callback function which is called automatically by the M8 timer: uint8_t audioCallback()

M8 uses ESP32-specific LEDC and Timer functions, but may work on other microcontrollers using analogWrite() and native timer instead.
