#include <stdint.h>

// NOTE: RCC -> [R]eset and [C]lock [C]ontrol register(s)
// control which ports have clocks enabled; they should not all
// be enabled to save power on the STM32 (duh)
// --
// APBx registers are connected to peripherals; in manual
// APB2 register is reponsible for GPIO port management

struct rcc {
  volatile uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR, AHBENR, APB2ENR, APB1ENR,
      BDCR, CSR;
};

#define RCC ((struct rcc *)0x40021000)

#define GPIO_BASE 0x40010800

struct gpio {
  volatile uint32_t CRL, CRH, IDR, ODR, BSRR, BRR, LCKR;
};

// TODO: implement MODE_INPUT (needs to use more registers)
enum { MODE_INPUT, MODE_OUTPUT_10MHz, MODE_OUTPUT_2MHz, MODE_OUTPUT_50MHz };

#define GPIO(bank) ((struct gpio *)(GPIO_BASE + 0x400 * (bank - 'A')))

static inline void gpio_set_mode(struct gpio *gpio, uint8_t pin, uint8_t mode) {
  if (pin < 8) {
    gpio->CRL &= ~(3U << (pin * 4));
    gpio->CRL |= (mode & 3) << (pin * 4);
  } else {
    gpio->CRH &= ~(3U << ((pin - 8) * 4));      // clear mode
    gpio->CRH |= (mode & 3) << ((pin - 8) * 4); // Set new mode
  }
}

static inline void gpio_out_write(struct gpio *gpio, uint8_t pin, uint8_t set) {
  if (set == 0) {
    gpio->ODR &= ~(1U << pin);
  } else {
    gpio->ODR |= (1U << pin);
  }
}

int main(void) {
  // enable port C clock gate
  RCC->APB2ENR |= (1 << 4);

  // configure GPIO C pin 13 as output
  gpio_set_mode(GPIO('C'), 13, MODE_OUTPUT_10MHz);

  while (1) {
    // set output bit
    gpio_out_write(GPIO('C'), 13, 1);
    for (uint32_t i = 0; i < 400000; ++i) {
      __asm__ volatile("nop");
    }
    // Reset it again.
    gpio_out_write(GPIO('C'), 13, 0);
    for (uint32_t i = 0; i < 400000; ++i) {
      __asm__ volatile("nop");
    }
  }
}
