STM32F103 & ILI9341 with SPI/DMA & overclocking
----

This project has blog post here:
http://www.stupid-projects.com/driving-an-ili9341-lcd-with-an-overclocked-stm32f103/

This project is about an stm32f103 (blue-pill) driving an ILI9341 LCD with
SPI and DMA. On top of that the stm32 is also overclocked from 72 to 128MHz.
The lcd driver is in `source/libs/ILI9341`, but it's using another SPI
drivers I've implemented which is located in `source/dev_spi.c`.

The LCD module used is: `TJCTM24028-SPI`

The SPI driver is using DMA for both TX/RX and you can use it for all
available SPI options:

- `SPI1`: on `GPIOA` (see en_spi_mode::DEV_SPI1_GPIOA)
- `SPI1`: on `GPIOB` (see en_spi_mode::DEV_SPI1_GPIOB)
- `SPI2`: on `GPIOB` (see en_spi_mode::DEV_SPI2)

The pinout is used for the STM32 is:
- common for all modes:
    - `PA0`: LED
    - `PA2`: RESET
    - `PA3`: D/C

- `en_spi_mode::DEV_SPI1_GPIOA` (default):
    - `PA4`: CS
    - `PA5`: SCK
    - `PA6`: SDO(MISO)
    - `PA7`: SDI(MOSI)

- `en_spi_mode::DEV_SPI1_GPIOB`:
    - `PB2`: CS
    - `PB3`: SCK
    - `PB4`: SDO(MISO)
    - `PB5`: SDI(MOSI)

- `en_spi_mode::DEV_SPI2`:
    - `PB12`: CS
    - `PB13`: SCK
    - `PB14`: SDO(MISO)
    - `PB15`: SDI(MOSI)

- serial port (115200-8-N-1):
    - `PA9`: Tx
    - `PA10`: Rx

> Note: The ILI9341 is both 3V3 and 5V. On the back of the module there are
two SMD pads (J1). This supposed to be soldered when you power the module,
but I haven't shorted those.

You can either connect the `VCC` on an external 3V3 PSU, but in my case I've
just connected the LCD's `VCC` to the `3.3` pin of the stm32f013 and it worked
fine.

### What the code does
The code in main is very simple and you won't see any graphics. If it works,
you'll only see a yellow screen. This is because I wanted to benchmark the
framerate of filling the screen without any additional calculations that
spend cpu cycles. The code that does the drawing is inside `main()`:
```cpp
LCD_setCursor(0, 0);
LCD_fillScreen(YELLOW);
```

The above code runs on every main loop and there's also a counter `glb.fps` that
increments on every screen draw. Finally, in the `SysTick_Handler()` interrupt
in `stm3210x_it.c` every second the `glb.fps` is stored and reset and then the
code prints the fps value.

Therefore, if you need more advanced graphics, the library is there and you can
add any code you like.

Also, there's a `dbg_uart_parser()` function in `main.c` that is ready to be used
to parse and execute commands from the uart.

### Overclocking
I've done two benchmarks. The one with the default maximum frequency (72MHz) and the
other with the stm32f103 overclocked to 128MHz. These are the FPS I got for each
frequency (using `-O3` for GCC optimization):

- `72MHz`: 29-30 fps
- `128MHz`: 52-53 fps

I haven't seen any difference in the fps when using other optimization flags.

> Note: When stm32f103 is overclocked then the USB is not working as the available
divider options don't achieve the 48MHz it needs.

### How to compile and flash
You need cmake to build this project either on Windows or Linux.
To setup the cmake properly
follow the instructions from [here](https://bitbucket.org/dimtass/cmake_toolchains/src/master/README.md).
Then edit the `cmake/TOOLCHAIN_arm_none_eabi_cortex_m3.cmake` file
and point `TOOLCHAIN_DIR` to the correct GCC path.

e.g. on Windows
```sh
set(TOOLCHAIN_DIR C:/opt/gcc-arm-none-eabi-4_9-2015q3-20150921-win32)
```

or on Linux
```sh
set(TOOLCHAIN_DIR /opt/gcc-arm-none-eabi-4_9-2015q3)
```

Then on Windows run ```build.cmd``` or on Linux run ```./build.bash```
and the .bin and .hex files should be created in the ```build-stm32/src```
folder. Also, a .cproject and .project files are created if you want to
edit the source code.

To flash the HEX file in windows use st-link utility like this:
```"C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe" -c SWD -p build-stm32\src\stm32f103_wifi_usb_psu.hex -Rst```

To flash the bin in Linux:
```st-flash --reset write build-stm32/src/stm32f103_wifi_usb_psu.bin 0x8000000```

## FW details
* `CMSIS version`: 5.3.0
* `StdPeriph Library version`: 3.6.1

