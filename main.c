#include "sys.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "wakeup.h"
#include "adc.h"
#include "wkup.h"
#include "usart.h"
#include "usart3.h"	
#define OPENBLT 1


u8 KeyDown(void)
{	
	if(COL0 | COL1 | COL2 | COL3 | COL4 | COL5 | COL6 | COL7 ){ 
		return 1;//keydown
	}
	else {
		return 0;
	}
}

int main(void){
	u32 waittime=0;
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(9600);
	usart3_init(115200);
	WKUP_Init();
	WAKEUP_BLT_Init();
	KEY_Init();
	Adc_Init();
	//low_power();
	LED0=1;
	if(OPENBLT){
		WAKEUP=1;//蓝牙开启
	}else{ 
		WAKEUP=0;//不开启蓝牙
	}
	delay_ms(3000);
	WAKEUP=0;
	delay_ms(200);
	WAKEUP=1;
	setrow0_0();
	setrow9_1();
	while(1){
		if(KeyDown() && WAKEUP == 1){
			waittime = 0;//将等待时间清零
		}else if(KeyDown() && WAKEUP == 0){//如果有按键按下,而且蓝牙待机
			waittime = 0;//将等待时间清零
			if(OPENBLT){
				WAKEUP=1;//蓝牙开启
			}else{
				WAKEUP=0;//不开启蓝牙
			}	
			delay_ms(20);//等待蓝牙启动成功
		}else if(!KeyDown()){//如果没有按键被按下，等待时间加1个单位，1个单位按10ms计算
			if(WAKEUP==1){//如果蓝牙开机，才开始算等待时间
				waittime++;
			}
		}
		if(waittime%100==1){
			u3_printf("runing..\r\n");
			u3_printf("t=%d\r\n",waittime);
		}
		if(waittime>=60000){//X10ms进入休眠，如果进入休眠比蓝牙待机时间早，则蓝牙同时也会被待机
			u3_printf("Enter standby..\r\n");
			Sys_Enter_Standby();
		}
		send_key();
		delay_ms(10);
		//u3_printf("times:%d\r\n", times);

	}
}

