/*
 * 	startup.c
 */
__attribute__((naked)) 
__attribute__((section (".start_section")) )

void startup ( void )
{
__asm__ volatile(" LDR R0,=0x2001C000\n");		/* set stack */
__asm__ volatile(" MOV SP,R0\n");
__asm__ volatile(" BL main\n");					/* call main */
__asm__ volatile(".L1: B .L1\n");				/* never return */
}


                // -- Declarations -- //

                
// Port Definition Adresses 
// * Port D
#define GPIO_D          ((volatile unsigned int   *) 0x40020C00)
#define GPIO_D_MODER    ((volatile unsigned int   *) 0x40020C00) 
#define GPIO_D_OTYPER   ((volatile unsigned int   *) 0x40020C04) 
#define GPIO_D_OSPEEDR  ((volatile unsigned int   *) 0x40020C08) 
#define GPIO_D_PUPDR    ((volatile unsigned int   *) 0x40020C0C)
#define GPIO_D_IDR      ((volatile unsigned short *) 0x40020C10)
#define GPIO_D_ODR_LOW  ((volatile unsigned char  *) 0x40020C14)
#define GPIO_D_ODR_HIGH ((volatile unsigned char  *) 0x40020C15)

// * Port E
#define GPIO_E          ((volatile unsigned int  *) 0x40021000)
#define GPIO_E_MODER    ((volatile unsigned int  *) 0x40021000)
#define GPIO_E_OTYPER   ((volatile unsigned int  *) 0x40021004)
#define GPIO_E_OSPEEDR  ((volatile unsigned int  *) 0x40021008)
#define GPIO_E_PUPDR    ((volatile unsigned int  *) 0x4002100C)
#define GPIO_E_IDR_LOW  ((volatile unsigned char *) 0x40021010)
#define GPIO_E_IDR_HIGH ((volatile unsigned char *) 0x40021011)
#define GPIO_E_ODR_LOW  ((volatile unsigned char *) 0x40021014)
#define GPIO_E_ODR_HIGH ((volatile unsigned char *) 0x40021015)

// System Configuration Register Adresses (Exceptions)
#define SYSCFG          ((unsigned int  *) 0x40013800)
#define SYSCFG_EXTICR1  ((unsigned int  *) 0x40013808)

// External Interrupt & Event Controller Register Adresses
#define EXTI            ((unsigned int  *) 0x40013C00)
#define EXTI_IMR        ((unsigned int  *) 0x40013C00)
#define EXTI_EMR        ((unsigned int  *) 0x40013C04)
#define EXTI_RTSR       ((unsigned int  *) 0x40013C08)
#define EXTI_FTSR       ((unsigned int  *) 0x40013C0C)
#define EXTI_SWIER      ((unsigned int  *) 0x40013C10)
#define EXTI_PR         ((unsigned int  *) 0x40013C14)

// NVIC Module Adresses
#define NVIC            ((unsigned int  *) 0xE000E100)
#define NVIC_ISER_0     ((unsigned int  *) 0xE000E100)

// NVIC Module Bit Position
#define NVIC_EXTI_0_IRQ_BPOS   (1<<6)
#define NVIC_EXTI_1_IRQ_BPOS   (1<<7)
#define NVIC_EXTI_2_IRQ_BPOS   (1<<8)
#define NVIC_EXTI_3_IRQ_BPOS   (1<<9)

// Exception Vector Table
#define SCB_VTOR ((volatile unsigned int *) 0xE000ED08)

// Exception Vector 
#define EXTI_0_IRQVEC ((void (**)(void)) (0x2001C058))
#define EXTI_1_IRQVEC ((void (**)(void)) (0x2001C05C))
#define EXTI_2_IRQVEC ((void (**)(void)) (0x2001C060))
#define EXTI_3_IRQVEC ((void (**)(void)) (0x2001C064))

// Exception Vector Bit Position
#define EXTI_0_IRQ_BPOS (1<<0)
#define EXTI_1_IRQ_BPOS (1<<1)
#define EXTI_2_IRQ_BPOS (1<<2)
#define EXTI_3_IRQ_BPOS (1<<3)

// Global Variables
static int volatile count = 0;


                // -- Functions -- //
                

void interrupt_irq_0_handler(void)
{
    // If Statement: Exception Occurred
    if (*EXTI_PR & EXTI_0_IRQ_BPOS)
    {
        // Reset Pending 
        *EXTI_PR |= EXTI_0_IRQ_BPOS; 
         
        // Reset Flipflop
        *GPIO_E_ODR_LOW |=  0x10;
        *GPIO_E_ODR_LOW &= ~0x10;
        
        // Increment Counter
        count ++;
        
    }
}     

void interrupt_irq_1_handler(void)
{
    if (*EXTI_PR & EXTI_1_IRQ_BPOS)
    {
        // Reset Pending
        *EXTI_PR |= EXTI_1_IRQ_BPOS;
        
        // Reset Flipflop
        *GPIO_E_ODR_LOW |=  0x20;
        *GPIO_E_ODR_LOW &= ~0x20;
        
        // Reset Counter
        count = 0;
    }
    
}

void interrupt_irq_2_handler(void)
{
    if (*EXTI_PR & EXTI_2_IRQ_BPOS)
    {
        // Reset Pending
        *EXTI_PR |= EXTI_2_IRQ_BPOS;
        
        // Reset Flipflop
        *GPIO_E_ODR_LOW |=  0x40;
        *GPIO_E_ODR_LOW &= ~0x40;
        
        // Turn Bargraph On or Off
        // * If Statement: On -> Off
        if (*GPIO_D_ODR_HIGH)
        {
            *GPIO_D_ODR_HIGH = 0x00;
        }
        // * Else Statement: Off -> On
        else
        {
            *GPIO_D_ODR_HIGH = 0xFF;
        }
    }
}

/*
void interrupt_handler(void)
{
        // If statement: Exception Occurred
        if (*EXTI_PR & 8)
        {
            *EXTI_PR |= 8; 
            
            // If Statement: IRQ0
            if (*GPIO_E_IDR_LOW & (1<<0))
            {  
                *GPIO_E_ODR_LOW |=  0x10;
                *GPIO_E_ODR_LOW &= ~0x10;
                count ++;
            }
            
            // If Statement: IRQ1
            if (*GPIO_E_IDR_LOW & (1<<1))
            {
                *GPIO_E_ODR_LOW |=  0x20;
                *GPIO_E_ODR_LOW &= ~0x20;
                count = 0;
            }
            
            // If Statement: IRQ2
            if (*GPIO_E_IDR_LOW & (1<<2))
            {
                *GPIO_E_ODR_LOW |=  0x40;
                *GPIO_E_ODR_LOW &= ~0x40;
                
                // If Statement: On -> Off
                if (*GPIO_D_ODR_HIGH)
                {
                    *GPIO_D_ODR_HIGH = 0x00;
                }
                // Else Statement: Off -> On
                else
                {
                    *GPIO_D_ODR_HIGH = 0xFF;
                }
            
            }
            
        }
         
}
*/


void init_app(void)
{
    // Set Port D:0-7 to Outport (Hexadecimal Display)
    *GPIO_D_MODER &= 0xFFFF0000;
    *GPIO_D_MODER |= 0x00005555;
    
    // Set Port D:8-15 to Outport (Bargraph)
    *GPIO_D_MODER &= 0x0000FFFF;
    *GPIO_D_MODER |= 0x55550000;
    
    // Set Port E:0-7 to Inport/Outport
    *GPIO_E_MODER &= 0xFFFF0000;
    *GPIO_E_MODER |= 0x00005500;
    
    // Set Port E
    // * Reset Control Register 1
    *SYSCFG_EXTICR1 &= 0x0000;
    
    // * Pin 0 to Exception Line 0
    *SYSCFG_EXTICR1 |= 0x0004;
    
    // * Pin 1 to Exception Line 0
    *SYSCFG_EXTICR1 |= 0x0040;
    
    // * Pin 2 to Exception Line 0
    *SYSCFG_EXTICR1 |= 0x0400;
    
    // Configure Exception Handling
    // * For Exception Line 0
    *EXTI_IMR  |=  EXTI_0_IRQ_BPOS; 
    *EXTI_RTSR |=  EXTI_0_IRQ_BPOS;
    *EXTI_FTSR &= ~EXTI_0_IRQ_BPOS;
    
    // * For Exception Line 1
    *EXTI_IMR  |=  EXTI_1_IRQ_BPOS; 
    *EXTI_RTSR |=  EXTI_1_IRQ_BPOS;
    *EXTI_FTSR &= ~EXTI_1_IRQ_BPOS;
    
    // * For Exception Line 2
    *EXTI_IMR  |=  EXTI_2_IRQ_BPOS; 
    *EXTI_RTSR |=  EXTI_2_IRQ_BPOS;
    *EXTI_FTSR &= ~EXTI_2_IRQ_BPOS;
    
    // * For Exception Line 3
    //*EXTI_IMR  |=  8; 
    //*EXTI_RTSR |=  8;
    //*EXTI_FTSR &= ~8;
    
    // Set Exception Vector Table in Configurable Memory Space
    *SCB_VTOR = 0x2001C000;
    
    // Initiate Exception Vector
    // * For Exception Line 0
    *EXTI_0_IRQVEC = interrupt_irq_0_handler;
    // * For Exception Line 1
    *EXTI_1_IRQVEC = interrupt_irq_1_handler;
    // * For Exception Line 2
    *EXTI_2_IRQVEC = interrupt_irq_2_handler;
    
    //*EXTI_3_IRQVEC = interrupt_handler;
    
    // Configure NVIC Module for specific exception
    // * For Exception Line 0
    *NVIC_ISER_0 |= NVIC_EXTI_0_IRQ_BPOS;
    
    // * For Exception Line 1
    *NVIC_ISER_0 |= NVIC_EXTI_1_IRQ_BPOS;
    
    // * For Exception Line 2
    *NVIC_ISER_0 |= NVIC_EXTI_2_IRQ_BPOS;
    
    //*NVIC_ISER_0 |= NVIC_IRQ_BPOS; 
    
}

void main(void)
{
    init_app();
    while(1)
    {
        *GPIO_D_ODR_LOW = count;
    }
}


