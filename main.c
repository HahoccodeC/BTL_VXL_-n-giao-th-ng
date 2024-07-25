#include <stdint.h>
#include "SysTick.h"
#include "PLL.h"
#include "stm32f10x.h"
#include "Timer.h"

struct State {
	uint32_t out_traffic;
	uint32_t out_walk;
	uint32_t time;
	uint32_t next[16];
};
typedef const struct State State_t;

#define goS 0
#define waitS 1
#define allStop1 2
#define goW 3
#define waitW 4 
#define allStop2 5
#define walk 6
#define onred1 7
#define offred1 8
#define onred2 9
#define offred2 10
#define onred3 11
#define offred3 12
#define allStop3 13
#define prior 14
State_t FSM[15] = {
    {0x101, 0x2000, 500, {prior, prior, prior, prior,prior,prior,prior,prior,waitS,waitS,waitS,waitS,waitS,goS,waitS,goS}},   										 					// goS
    {0x102, 0x2000, 300, {prior, prior, prior, prior,prior,prior,prior,prior,allStop1,allStop1,allStop1,allStop1,allStop1,allStop1,allStop1,allStop1}},			// waitS		
    {0x120, 0x2000, 200, {prior, prior, prior, prior,prior,prior,prior,prior,goW,walk,goW,walk,goW,goS,goW,goW}},																						// allStop1
    {0x060, 0x2000, 500, {prior, prior, prior, prior,prior,prior,prior,prior,waitW,waitW,waitW,waitW,waitW,waitW,goW,goW}},												 				// goW	
    {0x0A0, 0x2000, 300, {prior, prior, prior, prior,prior,prior,prior,prior,allStop2,allStop2,allStop2,allStop2,allStop2,allStop2,allStop2,allStop2}},			// waitW
    {0x120, 0x2000, 200, {prior, prior, prior, prior,prior,prior,prior,prior,walk,walk,walk,walk,goS,goS,goW,goS}},
    {0x120, 0x4000, 300, {prior, prior, prior, prior,prior,prior,prior,prior,onred1,onred1,onred1,walk,waitS,onred1,onred1,walk}},
    {0x120, 0x2000, 100, {prior, prior, prior, prior,prior,prior,prior,prior,offred1,offred1,offred1,offred1,allStop1,offred1,offred1,offred1}},
    {0x120, 0x0000, 100, {prior, prior, prior, prior,prior,prior,prior,prior,onred2,onred2,onred2,onred2,goW,onred2,onred2,onred2}},
    {0x120, 0x2000, 100, {prior, prior, prior, prior,prior,prior,prior,prior,offred2,offred2,offred2,offred2,waitW,offred2,offred2,offred2}},
    {0x120, 0x0000, 100, {prior, prior, prior, prior,prior,prior,prior,prior,onred3,onred3,onred3,onred3,allStop2,onred3,onred3,onred3}},
    {0x120, 0x2000, 100, {prior, prior, prior, prior,prior,prior,prior,prior,offred3,offred3,offred3,offred3,goS,offred3,offred3,offred3}},
    {0x120, 0x0000, 100, {prior, prior, prior, prior,prior,prior,prior,prior,allStop3,allStop3,allStop3,allStop3,waitS,allStop3,allStop3,allStop3}},
    {0x120, 0x2000, 200, {prior, prior, prior, prior,prior,prior,prior,prior,goS,goS,goW,walk,allStop1,goS,goW,goS}}		,																	 					// allStop2
    {0x130, 0x2000, 300, {prior, prior, prior, prior,prior,prior,prior,prior,goS,goS,goW,walk,allStop1,goS,goW,goS}}
};
/*
State_t FSM[15] = {
    {0x101, 0x2000, 500, {waitS,waitS,waitS,waitS,waitS,goS,waitS,goS,prior, prior, prior, prior,prior,prior,prior,prior}},   										 					// goS
    {0x102, 0x2000, 300, {allStop1,allStop1,allStop1,allStop1,allStop1,allStop1,allStop1,allStop1,prior, prior, prior, prior,prior,prior,prior,prior}},			// waitS		
    {0x120, 0x2000, 200, {goW,walk,goW,walk,goW,goS,goW,goW,prior, prior, prior, prior,prior,prior,prior,prior}},																						// allStop1
    {0x060, 0x2000, 500, {waitW,waitW,waitW,waitW,waitW,waitW,goW,goW,prior, prior, prior, prior,prior,prior,prior,prior}},												 				// goW	
    {0x0A0, 0x2000, 300, {allStop2,allStop2,allStop2,allStop2,allStop2,allStop2,allStop2,allStop2,prior, prior, prior, prior,prior,prior,prior,prior}},			// waitW
    {0x120, 0x2000, 200, {walk,walk,walk,walk,goS,goS,goW,goS,prior, prior, prior, prior,prior,prior,prior,prior}},
    {0x120, 0x4000, 300, {onred1,onred1,onred1,walk,waitS,onred1,onred1,walk,prior, prior, prior, prior,prior,prior,prior,prior}},
    {0x120, 0x2000, 100, {offred1,offred1,offred1,offred1,allStop1,offred1,offred1,offred1,prior, prior, prior, prior,prior,prior,prior,prior}},
    {0x120, 0x0000, 100, {onred2,onred2,onred2,onred2,goW,onred2,onred2,onred2,prior, prior, prior, prior,prior,prior,prior,prior}},
    {0x120, 0x2000, 100, {offred2,offred2,offred2,offred2,waitW,offred2,offred2,offred2,prior, prior, prior, prior,prior,prior,prior,prior}},
    {0x120, 0x0000, 100, {onred3,onred3,onred3,onred3,allStop2,onred3,onred3,onred3,prior, prior, prior, prior,prior,prior,prior,prior}},
    {0x120, 0x2000, 100, {offred3,offred3,offred3,offred3,goS,offred3,offred3,offred3,prior, prior, prior, prior,prior,prior,prior,prior}},
    {0x120, 0x0000, 100, {allStop3,allStop3,allStop3,allStop3,waitS,allStop3,allStop3,allStop3,prior, prior, prior, prior,prior,prior,prior,prior}},
    {0x120, 0x2000, 200, {goS,goS,goW,walk,allStop1,goS,goW,goS,prior, prior, prior, prior,prior,prior,prior,prior}},																			 					// allStop2
    {0x130, 0x2000, 300, {goS,goS,goW,walk,allStop1,goS,goW,goS,prior, prior, prior, prior,prior,prior,prior,prior}}
};*/

/*State_t FSM[] = {
    {0x101, 0x2000, 500, {walk,walk,walk,walk,waitS,goS,waitS,goS}},   										 					// goS
    {0x102, 0x2000, 300, {walk,walk,walk,walk,allStop1,allStop1,allStop1,allStop1}},			// waitS		
    {0x120, 0x2000, 200, {walk,walk,walk,walk,goW,goS,goW,goW}},																						// allStop1
    {0x060, 0x2000, 500, {walk,walk,walk,walk,waitW,waitW,goW,goW}},												 				// goW	
    {0x0A0, 0x2000, 300, {walk,walk,walk,walk,allStop2,allStop2,allStop2,allStop2}},			// waitW
    {0x120, 0x2000, 200, {walk,walk,walk,walk,goS,goS,goW,goS}},
    {0x120, 0x4000, 300, {walk,walk,walk,walk,waitS,onred1,onred1,walk}},
    {0x120, 0x2000, 100, {walk,walk,walk,walk,allStop1,offred1,offred1,offred1}},
    {0x120, 0x0000, 100, {walk,walk,walk,walk,goW,onred2,onred2,onred2}},
    {0x120, 0x2000, 100, {walk,walk,walk,walk,waitW,offred2,offred2,offred2}},
    {0x120, 0x0000, 100, {walk,walk,walk,walk,allStop2,onred3,onred3,onred3}},
    {0x120, 0x2000, 100, {walk,walk,walk,walk,goS,offred3,offred3,offred3}},
    {0x120, 0x0000, 100, {walk,walk,walk,walk,waitS,allStop3,allStop3,allStop3}},
    {0x120, 0x2000, 200, {walk,walk,walk,walk,allStop1,goS,goW,goS}}																			 					// allStop2
};*/

// sensor A0(west),A1(south),A2(walk)
// walklight C13(red),C14(green)
// trafficlight B0(green_south),B1(yellow_south),B5(red_south),B6(green_west),B7(yellow_west),B8(red_west),

void GPIO_Config(void);

void GPIO_Config(void){
	// Initialize GPIO ports 
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN; // PortA,PortB,PortC
	
	// set A0,A1,A2 as sensor
	GPIOA->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3 | GPIO_CRL_MODE0 | GPIO_CRL_CNF0 | GPIO_CRL_MODE1 | GPIO_CRL_CNF1 | GPIO_CRL_MODE2 |GPIO_CRL_CNF2 |GPIO_CRL_MODE7 |GPIO_CRL_CNF7);
	GPIOA->CRL |= GPIO_CRL_CNF0_1 | GPIO_CRL_CNF1_1 | GPIO_CRL_CNF2_1 | GPIO_CRL_CNF7_1 | GPIO_CRL_CNF3_1; // pull up mode
	
	//0x4001 0C00
	// set B0,B1,B5,B6,B7,B8,B4 as trafic light output
	GPIOB->CRL &= ~(0xFFF000FF);
	GPIOB->CRH &= ~(0x0000000F);
	GPIOB->CRL |= GPIO_CRL_MODE0_0 | GPIO_CRL_MODE1_0 | GPIO_CRL_MODE5_0 | GPIO_CRL_MODE6_0 | GPIO_CRL_MODE7_0 |GPIO_CRL_MODE4_0;
	GPIOB->CRH |= GPIO_CRH_MODE8_0;
	
	
	// set C13,C14 as walk light output
	GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13 | GPIO_CRH_MODE14 | GPIO_CRH_CNF14);
	GPIOC->CRH |= GPIO_CRH_MODE13_0 | GPIO_CRH_MODE14_0;
	
	//set A3,A4,A5,A6 as output IC CD4511 South
//	GPIOA->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3 | GPIO_CRL_MODE4 | GPIO_CRL_CNF4 | GPIO_CRL_MODE5 | GPIO_CRL_CNF5 | GPIO_CRL_MODE6 | GPIO_CRL_CNF6);
//  GPIOA->CRL |= (GPIO_CRL_MODE3_0 | GPIO_CRL_MODE4_0 | GPIO_CRL_MODE5_0 | GPIO_CRL_MODE6_0);
	
	GPIOA->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4 | GPIO_CRL_MODE5 | GPIO_CRL_CNF5 | GPIO_CRL_MODE6 | GPIO_CRL_CNF6);
  GPIOA->CRL |= (GPIO_CRL_MODE4_0 | GPIO_CRL_MODE5_0 | GPIO_CRL_MODE6_0);
	
	//set A8,A9,A10,A11 as output IC CD4511 West
	GPIOA->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8 | GPIO_CRH_MODE9 | GPIO_CRH_CNF9 | GPIO_CRH_MODE10 | GPIO_CRH_CNF10 | GPIO_CRH_MODE11 | GPIO_CRH_CNF11);
  GPIOA->CRH |= (GPIO_CRH_MODE8_0 | GPIO_CRH_MODE9_0 | GPIO_CRH_MODE10_0 | GPIO_CRH_MODE11_0);
	
	//set B12,B13,B14,B15 as output IC CD4511 Walk
	GPIOB->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12 | GPIO_CRH_MODE13 | GPIO_CRH_CNF13 | GPIO_CRH_MODE14 | GPIO_CRH_CNF14 | GPIO_CRH_MODE15 | GPIO_CRH_CNF15);
  GPIOB->CRH |= (GPIO_CRH_MODE12_0 | GPIO_CRH_MODE13_0 | GPIO_CRH_MODE14_0 | GPIO_CRH_MODE15_0);
	
}

void decodeNumber_S(uint8_t n){
	GPIOA->ODR &= ~(0x78); // Clear bits 3, 4, 5, 6
	switch(n){
		case 0: {
			GPIOA->ODR |= 0x00;
			break;
		}
		case 1: {
			GPIOA->ODR |= 0x08;
			break;
		}
		case 2: {
			GPIOA->ODR |= 0x10;
			break;
		}
		case 3: {
			GPIOA->ODR |= 0x18;
			break;
		}
		case 4: {
			GPIOA->ODR |= 0x20;
			break;
		}		
		case 5: {
			GPIOA->ODR |= 0x28;
			break;
		}	
		case 6: {
			GPIOA->ODR |= 0x30;
			break;
		}	
		case 7: {
			GPIOA->ODR |= 0x38;
			break;
		}	
		case 8: {
			GPIOA->ODR |= 0x40;
			break;
		}	
		case 9: {
			GPIOA->ODR |= 0x48;
			break;
		}			
   }
}
void decodeNumber_W(uint8_t n){
	GPIOA->ODR &= ~(0xF00); // Clear bits 8, 9, 10, 11
	switch(n){
		case 0: {
			GPIOA->ODR |= 0x0000;
			break;
		}
		case 1: {
			GPIOA->ODR |= 0x0100;
			break;
		}
		case 2: {
			GPIOA->ODR |= 0x0200;
			break;
		}
		case 3: {
			GPIOA->ODR |= 0x0300;
			break;
		}
		case 4: {
			GPIOA->ODR |= 0x0400;
			break;
		}		
		case 5: {
			GPIOA->ODR |= 0x0500;
			break;
		}	
		case 6: {
			GPIOA->ODR |= 0x0600;
			break;
		}	
		case 7: {
			GPIOA->ODR |= 0x0700;
			break;
		}	
		case 8: {
			GPIOA->ODR |= 0x0800;
			break;
		}	
		case 9: {
			GPIOA->ODR |= 0x0900;
			break;
		}			
   }
}
void decodeNumber_Walk(uint8_t n){
	GPIOB->ODR &= ~(0xF000); // Clear bits 12, 13, 14, 15
	switch(n){
		case 0: {
			GPIOB->ODR |= 0x0000;
			break;
		}
		case 1: {
			GPIOB->ODR |= 0x1000;
			break;
		}
		case 2: {
			GPIOB->ODR |= 0x2000;
			break;
		}
		case 3: {
			GPIOB->ODR |= 0x3000;
			break;
		}
		case 4: {
			GPIOB->ODR |= 0x4000;
			break;
		}		
		case 5: {
			GPIOB->ODR |= 0x5000;
			break;
		}	
		case 6: {
			GPIOB->ODR |= 0x6000;
			break;
		}	
		case 7: {
			GPIOB->ODR |= 0x7000;
			break;
		}	
		case 8: {
			GPIOB->ODR |= 0x8000;
			break;
		}	
		case 9: {
			GPIOB->ODR |= 0x9000;
			break;
		}			
   }
}
void counterS(uint32_t initial_count) {
    uint32_t count = initial_count;
    while(count >= 0) {
        // Decode the number to set the appropriate pins on the CD4511
        decodeNumber_S(count);
        // Wait for 1 second
        SysTick_Wait10ms(100);
        count--;
			if (count == 0) break;
    }
}

void counterW(uint32_t initial_count) {
    uint32_t count = initial_count;
    while(count >= 0) {
        decodeNumber_W(count);
        // Wait for 1 second
        SysTick_Wait10ms(100);
        count--;
			if (count == 0) break;
    }
}

void counterWalk(uint32_t initial_count) {
    uint32_t count = initial_count;
    while(count >= 0) {
        decodeNumber_Walk(count);
        // Wait for 1 second
        SysTick_Wait10ms(100);
        count--;
			if (count == 0) break;
    }
}
void Update7SegmentDisplay(uint32_t current_state, uint32_t time);

void Update7SegmentDisplay(uint32_t current_state, uint32_t time) {
    // This function should be implemented to update the 7-segment display
    // You need to implement the logic to convert the time to individual digits
    // and send those digits to the 7-segment display decoder IC.
    // Example: time = 123 -> display 1 on first digit, 2 on second digit, 3 on third digit
		uint32_t time_temp = time / 100;
		switch(current_state){
			case goS:	
			{
				counterS(time_temp);
				break;
			}
			case waitS:
			{
				counterS(time_temp);
				break;
			}
			case allStop1:
			{
				counterS(time_temp);
				break;
			}
			
			case goW:
				{
				counterW(time_temp);
				break;
			}
			case waitW:
				{
				counterW(time_temp);
				break;
			}
			case allStop2:
				{
				counterW(time_temp);
				break;
					}
				case walk:
				{
				counterWalk(time_temp);
				break;
			}
			case allStop3:
				{
				counterWalk(time_temp);
				break;
					}
			default : break;
					
		}
}


 
int main(void) {
	PLL_Init();
	SysTick_Init();
	
	GPIO_Config();
	
	// Initialize state
	uint32_t S;
	uint32_t Input;
	S = waitS;
	
	while(1){
		// set traffic light
        GPIOB->ODR = (GPIOB->ODR & ~(0x1E3)) | FSM[S].out_traffic;
         
        // set walk light
        GPIOC->ODR = (GPIOC->ODR & ~(0x6000)) | FSM[S].out_walk;
        
        // update 7-segment display with the current state's time
       Update7SegmentDisplay(S, FSM[S].time);
        
        // wait 
 //    SysTick_Wait10ms(FSM[S].time);
    
//        // read input
      Input = GPIOA->IDR & 0x87;
			uint8_t x1 = Input & 0x80;
			x1 >>= 4;	
		uint8_t x2 = Input & 0x07;
		Input = x1 | x2;
        // next state depends on input and current state
        S = FSM[S].next[Input];

	
}
}
