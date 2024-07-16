#include <stdint.h>

/*
* NOTE: RCC -> [R]eset and [C]lock [C]ontrol register(s)
* control which ports have clocks enabled; they should not all
*  be enabled to save power on the STM32 (duh)
*  --
*  APBx registers are connected to peripherals; in manual
*  APB2 register is reponsible for GPIO port management
*/

// TODO: make these structs more readable?
struct rcc {
  volatile uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR, AHBENR, APB2ENR, APB1ENR,
      BDCR, CSR;
};

#define RCC ((struct rcc *)0x40021000)

/*
 * NOTE: these are ARM _standard_  registers and so 
 * should follow default locations on all ARM MCUs
 */
struct systick {
  volatile uint32_t CTRL, LOAD, VAL, CALIB;
};

#define SYSTICK ((struct systick *) 0xE000E010)
//                                  ^^^^^^^^^^
//                                  CMSIS standard (xd)

#define GPIO_BASE 0x40010800 // refer to rm0008-stm32f103xx doc (page 51)

struct gpio {
  volatile uint32_t CRL, CRH, IDR, ODR, BSRR, BRR, LCKR;
};

/*
* TODO: implement MODE_INPUT (needs to use more registers)
*/
enum { MODE_INPUT, MODE_OUTPUT_10MHz, MODE_OUTPUT_2MHz, MODE_OUTPUT_50MHz };

#define GPIO(bank) ((struct gpio *)(GPIO_BASE + 0x400 * (bank - 'A')))

/*
* Initializes the ARM SysTick (down) counter to reload at the value
* (ticks - 1).
*
* By default the counter is clocked at X MHz "(1UL << 2)"
*/
static inline void systick_init(uint32_t ticks) {
  // from spec, systick is a 24 bit counter and so greater values are invalid
  if ((ticks - 1) > 0xFFFFFF) return;
  SYSTICK->LOAD = ticks - 1;
  SYSTICK->VAL = 0;
  SYSTICK->CTRL = (1UL << 0) | (1UL << 1) | (1UL << 2);
}

// static restricts access here
static volatile uint32_t s_ticks;

void SysTick_Handler(void) {
  s_ticks++;
}

void delay(unsigned ms) {
  uint32_t wait_until = s_ticks + ms;
  while (s_ticks < wait_until) {
    (void) 0;
  }
}

static inline void gpio_set_mode(struct gpio *gpio, uint8_t pin, uint8_t mode) {
  if (pin < 8) {
    // maybe these should be macros?
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

  gpio_set_mode(GPIO('C'), 13, MODE_OUTPUT_10MHz);

  systick_init(8000000 / 1000);

  for (;;) {
    // set output bit
    gpio_out_write(GPIO('C'), 13, 1);
    delay(150);
    // Reset it again.
    gpio_out_write(GPIO('C'), 13, 0);
    delay(1000);
  }
}

__attribute__((naked, noreturn)) void _reset(void) {
  // Initialise memory
  extern long _sbss, _ebss, _sdata, _edata, _sidata;
  // first we load ".bss" with 0's
  for (long *dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
  // then we copy something to ".data" (in SRAM)
  for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;

  main();
  for (;;) (void) 0;  // infinite loop if we happen to return from main
}

// why is this down here?
extern void _estack(void); // this is defined in the linker file

__attribute__((section(".vectors"))) void (*const tab[16 + 67])(void) = {
    _estack, _reset, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SysTick_Handler};
