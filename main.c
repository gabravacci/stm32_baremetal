#include <stdint.h>

// NOTE: RCC -> [R]eset and [C]lock [C]ontrol register(s)
// control which ports have clocks enabled; they should not all
// be enabled to save power on the STM32 (duh)
#define RCC_BASE 0x40021000
// NOTE: APBx registers are connected to peripherals; in manual 
// APB2 register is reponsible for GPIO port management
// TODO: why do we define it as a pointer here
#define RCC_APB2ENR_REG *((volatile uint32_t*) (RCC_BASE + 0x18))
// TODO: this is completely unnecessary lol
#define RCC_APB2ENR_IOPCEN (1 << 4)

#define GPIO_BASE 0x40010800
#define GPIO_PORTC_BASE 0x40011000

#define GPIO_CRH_REGISTER(x) (*((volatile uint32_t*) (x + 0x4)))
#define GPIO_CRH_MODE_MASK(x) (0x3 << ((x - 8) * 4))
#define GPIO_CRH_MODE_OUTPUT(x) (0x1 << ((x - 8) * 4))

#define GPIO_BLINK_PORT GPIO_PORTC_BASE
#define GPIO_BLINK_NUM 13

#define GPIO_ODR_REGISTER(x) (*(volatile uint32_t *)(x + 0xC))
#define GPIO_ODR_PIN(x) (1 << x)

struct gpio {
    volatile uint32_t CRL, CRH, IDR, ODR, BSRR, BRR, LCKR;
};

enum {
    MODE_INPUT, MODE_OUTPUT_10MHz, MODE_OUTPUT_2MHz, MODE_OUTPUT_50MHz
};

#define GPIOC ((struct gpio*) 0x40011000)
#define GPIO(bank) ((struct gpio*) (GPIO_BASE + 0x400 * (bank - 'A')))

static inline void gpio_set_mode(struct gpio* gpio, uint8_t pin, uint8_t mode) {
  if (pin < 8) {
    gpio->CRL &= ~(3U << (pin * 2));
    gpio->CRL |= (mode & 3) << (pin * 2);
  } else {
    gpio->CRH &= ~(3U << ((pin - 8) * 4));      // clear mode
    gpio->CRH |= (mode & 3) << ((pin - 8) * 4); // Set new mode
  }
}

int main(void) {
    // enable port C clock gate
    RCC_APB2ENR_REG |= RCC_APB2ENR_IOPCEN; // this seems oddly verbose

    // configure GPIO C pin 13 as output
    gpio_set_mode(GPIO('C'), 13, MODE_OUTPUT_10MHz);

    while (1) {
	// set output bit
        // GPIO_ODR_REGISTER(GPIO_BLINK_PORT) |= GPIO_ODR_PIN(GPIO_BLINK_NUM);
        // for (uint32_t i = 0; i < 400000; ++i) {
        //     __asm__ volatile("nop");
        // }
        // // Reset it again.
        // GPIO_ODR_REGISTER(GPIO_BLINK_PORT) &= ~GPIO_ODR_PIN(GPIO_BLINK_NUM);
        // for (uint32_t i = 0; i < 10000; ++i) {
        //     __asm__ volatile("nop");
        // }
    }
}
