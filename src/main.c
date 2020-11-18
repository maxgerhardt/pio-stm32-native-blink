#include <stdint.h>
#define GPIO_CRL_OFF 0x00  /* configuration register low */
#define GPIO_CRH_OFF 0x04  /* configuration register high */
#define GPIO_IDR_OFF 0x08  /* Input data register */
#define GPIO_ODR_OFF 0x0C  /* output data register */
#define GPIO_BSRR_OFF 0x10 /* bit set reset register */
#define GPIO_BRR_OFF 0x14  /* bit reset register */
#define GPIO_LCKR_OFF 0x18 /* lock register */

#define GPIOC_BASE_ADDR 0x40011000

struct GPIOx_CRL_RegisterDef
{
	uint8_t MODE0 : 2;
	uint8_t CNF0 : 2;
	uint8_t MODE1 : 2;
	uint8_t CNF1 : 2;
	uint8_t MODE2 : 2;
	uint8_t CNF2 : 2;
	uint8_t MODE3 : 2;
	uint8_t CNF3 : 2;
	uint8_t MODE4 : 2;
	uint8_t CNF4 : 2;
	uint8_t MODE5 : 2;
	uint8_t CNF5 : 2;
	uint8_t MODE6 : 2;
	uint8_t CNF6 : 2;
	uint8_t MODE7 : 2;
	uint8_t CNF7 : 2;
};

struct GPIOx_CRH_RegisterDef
{
	uint8_t MODE8 : 2;
	uint8_t CNF8 : 2;
	uint8_t MODE9 : 2;
	uint8_t CNF9 : 2;
	uint8_t MODE10 : 2;
	uint8_t CNF10 : 2;
	uint8_t MODE11 : 2;
	uint8_t CNF11 : 2;
	uint8_t MODE12 : 2;
	uint8_t CNF12 : 2;
	uint8_t MODE13 : 2;
	uint8_t CNF13 : 2;
	uint8_t MODE14 : 2;
	uint8_t CNF14 : 2;
	uint8_t MODE15 : 2;
	uint8_t CNF15 : 2;
};

struct GPIOx_BSRR_RegisterDef
{
	uint8_t BS0 : 1;
	uint8_t BS1 : 1;
	uint8_t BS2 : 1;
	uint8_t BS3 : 1;
	uint8_t BS4 : 1;
	uint8_t BS5 : 1;
	uint8_t BS6 : 1;
	uint8_t BS7 : 1;
	uint8_t BS8 : 1;
	uint8_t BS9 : 1;
	uint8_t BS10 : 1;
	uint8_t BS11 : 1;
	uint8_t BS12 : 1;
	uint8_t BS13 : 1;
	uint8_t BS14 : 1;
	uint8_t BS15 : 1;
	uint8_t BR0 : 1;
	uint8_t BR1 : 1;
	uint8_t BR2 : 1;
	uint8_t BR3 : 1;
	uint8_t BR4 : 1;
	uint8_t BR5 : 1;
	uint8_t BR6 : 1;
	uint8_t BR7 : 1;
	uint8_t BR8 : 1;
	uint8_t BR9 : 1;
	uint8_t BR10 : 1;
	uint8_t BR11 : 1;
	uint8_t BR12 : 1;
	uint8_t BR13 : 1;
	uint8_t BR14 : 1;
	uint8_t BR15 : 1;
};

#define GPIOC_CRL ((volatile struct GPIOx_CRL_RegisterDef *)(GPIOC_BASE_ADDR + GPIO_CRL_OFF))
#define GPIOC_CRH ((volatile struct GPIOx_CRH_RegisterDef *)(GPIOC_BASE_ADDR + GPIO_CRH_OFF))
#define GPIOC_BSSR ((volatile struct GPIOx_BSRR_RegisterDef *)(GPIOC_BASE_ADDR + GPIO_BSRR_OFF))
#define GPIOC_ODR *(uint32_t *)(GPIOC_BASE_ADDR + GPIO_ODR_OFF)

#define RCC_BASE_ADDR 0x40021000
#define RCC_APB2ENR_OFF 0x18 /* Bit 4 IOPCEN: IO port C clock enable */

typedef void (*ISR_func)(void);
#define SECTION(x) __attribute__((section(#x)))
#define ISR_VECTOR_SECTION SECTION(.isr_vector)

void Reset_Handler();
void Systick_Handler();

const uint32_t isr_vector[83] ISR_VECTOR_SECTION = {
		0x200000EC + (20 * 1024 - 0xEC),
		(uint32_t)&Reset_Handler,
		0,						   /* NMI */
		0,						   /* HardFault */
		0,						   /* MemManage */
		0,						   /* BusFault */
		0,						   /* Usage Fault */
		0, 0, 0, 0,				   /* reserved */
		0,						   /* SV Call */
		0,						   /* Debug Monitor */
		0,						   /* reserved */
		0,						   /* PendSV */
		(uint32_t)&Systick_Handler /* SysTick */
};

void ms_delay(int ms)
{
	while (ms-- > 0)
	{
		volatile int x = 500;
		while (x-- > 0)
			__asm("nop");
	}
}

static inline void flip()
{
	GPIOC_ODR ^= (1 << 13);
}

#define SCB_ICSR *(uint32_t *)(0xE000ED00 + 0x04)

void Systick_Handler()
{
	flip();
	/* SCB_ICSR offset 0x04 */
	/* Bit 25 PENDSTCLR: SysTick exception clear-pending bit. */
	//SCB_ICSR = (1 << 25);
	//also works without it lol!
}

#define SYSTICK_BASE 0xE000E010
#define STK_CTRL_OFF 0x00
#define STK_LOAD_OFF 0x04
#define STK_VAL_OFF 0x08
#define SYSTICK_CTRL *(volatile uint32_t *)(SYSTICK_BASE + STK_CTRL_OFF)
#define SYSTICK_LOAD *(volatile uint32_t *)(SYSTICK_BASE + STK_LOAD_OFF)
#define SYSTICK_VAL *(volatile uint32_t *)(SYSTICK_BASE + STK_VAL_OFF)

void Reset_Handler()
{
	/* enable clock for GPIOC */
	*(uint32_t *)(RCC_BASE_ADDR + RCC_APB2ENR_OFF) = (1 << 4);
	GPIOC_CRH->CNF13 = 0x00;
	GPIOC_CRH->MODE13 = 0x3;

	SYSTICK_CTRL = 0; 
	SYSTICK_LOAD = 8 * 1000000 / 2; // fire after 8 mio clock cycles aka once per sec
	SYSTICK_VAL = 0;
	SYSTICK_CTRL = (1 << 2)   /* clock source = AHB */
				   | (1 << 1) /* enable exception when counter reaches zero */
				   | (1 << 0) /* start counting */;

	while (1)
	{
		;
	}
}