#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define LED1_PORT GPIOA
#define LED1_PIN  GPIO4

#define LED0_PORT GPIOA
#define LED0_PIN  GPIO3

int main(void)
{
        // Enable the GPIO port A
	rcc_periph_clock_enable(RCC_GPIOA);

        // set LED pins as floating output with pull up resistor
	gpio_mode_setup(LED0_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LED0_PIN);
	gpio_mode_setup(LED1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LED1_PIN);

        // turn LED0 on
	gpio_set(LED0_PORT, LED0_PIN);

        // turn LED0 off
	gpio_clear(LED1_PORT, LED1_PIN);

	while(1) {
                // busy waite some time
		for (int i = 0; i < 500000; ++i) {
			__asm__("nop");
		}

                // chenge the state of both LEDs
		gpio_toggle(LED1_PORT, LED0_PIN | LED1_PIN);
	}

	return 0;
}
