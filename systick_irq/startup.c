/*
 * 	startup.c
 */

__attribute__((naked)) 
__attribute__((section (".start_section")) )

void startup ( void )
{
__asm__ volatile(" LDR R0,=0x2001C000\n");		// set stack
__asm__ volatile(" MOV SP,R0\n");
__asm__ volatile(" BL main\n");					// call main
__asm__ volatile(".L1: B .L1\n");				// never return
}


                // -- Definitions -- //


// Port Definition Adresses
#define GPIO_D          ((volatile unsigned int   *) 0x40020C00)
#define GPIO_D_MODER    ((volatile unsigned int   *) GPIO_D + 0x0) 
#define GPIO_D_OTYPER   ((volatile unsigned int   *) GPIO_D + 0x4) 
#define GPIO_D_OSPEEDR  ((volatile unsigned int   *) GPIO_D + 0x8) 
#define GPIO_D_PUPDR    ((volatile unsigned int   *) GPIO_D + 0xC)
#define GPIO_D_IDR      ((volatile unsigned short *) GPIO_D + 0x10)
#define GPIO_D_ODR_LOW  ((volatile unsigned char  *) GPIO_D + 0x14)
#define GPIO_D_ODR_HIGH ((volatile unsigned char  *) GPIO_D + 0x15)

#define SysTick  ((volatile unsigned int *) 0xE000E010)
#define STK_CTRL ((volatile unsigned int *) SysTick + 0x0)
#define STK_LOAD ((volatile unsigned int *) SysTick + 0x4)
#define STK_VAL  ((volatile unsigned int *) SysTick + 0x8)

// Register Definition Adresses
#define SCB_VTOR ((volatile unsigned int *) 0xE000ED08)

// Value Definitions
// * Adjusted
#define SIMULATOR 1

#ifdef SIMULATOR
#define DELAY_COUNT 100
#else
#define DELAY_COUNT 1000000
#endif

// Global Variables
// * Status
static int systick_flag;
static int delay_count;


                // -- Functions -- //


// Function to configure ports and initialize exception vector table
void init_app(void)
{
    *GPIO_D_MODER = 0x55555555;
    *SCB_VTOR      = 0x2001C000;
    *((void (**)(void)) *SCB_VTOR + 0x0000003C) = systick_irq_handler();
}


void delay_1mikro(void)
{
    // Guarantee flag isn't set
    systick_flag = 0;
    
    // Delay Routine (0xA8 = 168)
    *STK_CTRL = 0;      // Reset SysTick
    *STK_LOAD = 0xA8-1; // Load value (N clockcycles is given by countvalue N-1)
    *STK_VAL  = 0;      // Reset counter
    *STK_CTRL = 7;      // Prepare SysTick 
                        // * (STK_LOAD to STK_VAL)
                        // * Exception generated when STK_VAL = 0
    
}

void delay(unsigned int count)
{
    delay_count = count;
    delay_1mikro();
}

// Function to handle systick exception request
void systick_irq_handler(void)
{
    // Reset Counter Circuit
     *STK_CTRL = 0;
    
    // Countdown
    // * When we reach 0 -> set flag to 1 to notify main program
    delay_count = delay_count - 1;
    
    if (delay_count > 0)
    {
        delay_1mikro();
    }
    else
    {
        systick_flag = 1;
    }
     
}


void main(void)
{
    init_app();
    *GPIO_D_ODR_LOW = 0x00;     // Set output to 0x0 
    delay(DELAY_COUNT);      // Initiate separate counter
    *GPIO_D_ODR_LOW = 0xFF;  // Set output to 0xFF
    
    // Code to be executed while separate counter runs
    // * Runs until exception is generated -> set systick_flag = 1 -> Specific Exception Routine
    while(1)
    {
        if (systick_flag)
        {
            break;
        }
        // Waiting Code
    }
    // Code that runs on timeout
    *GPIO_D_ODR_LOW = 0;
}

