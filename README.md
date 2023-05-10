# ESP32-WS2812B-Clock
An abstract clock utilising an ESP32 and WS2812B addressable LEDS

Using an ESP32 dev board and some WS2812B led strips, the project allows us to pull the current time from an NTP pool and represent the 4 digits in 24 hour time as a lit led over 4 columns.

On power up, a wifi connection is attempted and if successful, it pulls the current time from an NTP pool and syncs the internal RTC clock to it, the wifi is then disconnected as it has served it's purpose.

I have arranged my LEDS vertically over 4 strips to represent the time and one horitonally across the bottom to display the current day value (1-7)

     x   x
     x   x
     x   x
     x   x
     x x x
     x x x
     x x x
   x x x x
   x x x x
x x x x x x x 

All leds are filled with a white value and the current digit is represented by a single colored led.

In order they would be lit to match 24 hour time, a value of 19:15 would light the first led of the first column, the ninth led of the second, the first led of the third and the fifth led of fourth.

The current day is represented from left to right as a value of 0-6.

Using a simple switch case I have added an option to decreases brightness by a specified hour, this simply works by assigning an int a value to be used to divide the preset color values by evenly.

![clock](https://github.com/aaronalam1987/ESP32-WS2812B-Clock/assets/46248931/8fb5cfb5-d5cc-4bc1-9cbe-7594cbe5d601 | width=250)
