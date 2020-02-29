# Capslock

With this program, the board will register as USB keyboard and listens to the status of caps lock and the numpad status. Depending on the status it will turn on the LEDs.

This is based on [tomu-capslock](https://github.com/der-b/tomu-capslock)

## Compile

`` bash
TARGETS=stm32/f0 make -C libopencm3
make
``

## download to board

`` bash
make download
``
