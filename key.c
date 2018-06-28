#include "key.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"	
#include "usart3.h"	
#include "stm32f10x.h"
#include "string.h" 

void KEY_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12 ;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_Init(GPIOA,&GPIO_InitStructure);


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4|
									GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8|
									GPIO_Pin_9;               //
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         //
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;               //14AT 15CLEAR
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         //
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;               //13LINK STATUS
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;         //
    GPIO_Init(GPIOC,&GPIO_InitStructure);
}

//发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* check_cmd(u8 *str)
{
	char *strx=0;
	if(USART_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART_RX_BUF[USART_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//向sim900a发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while((USART1->SR&0X40)==0);//等待上一次数据发送完成  
		USART1->DR=(u32)cmd;
	}else printf("%s\r\n",cmd);//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(check_cmd(ack))break;//得到有效数据 
				USART_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 

void setrow0_0(void){ROW0_0;ROW1_0;ROW2_0;ROW3_0;ROW4_0;ROW5_0;ROW6_0;ROW7_0;ROW8_0;}
void setrow0_1(void){ROW0_1;ROW1_0;ROW2_0;ROW3_0;ROW4_0;ROW5_0;ROW6_0;ROW7_0;ROW8_0;}
void setrow1_1(void){ROW0_0;ROW1_1;}
void setrow2_1(void){ROW1_0;ROW2_1;}
void setrow3_1(void){ROW2_0;ROW3_1;}
void setrow4_1(void){ROW3_0;ROW4_1;}
void setrow5_1(void){ROW4_0;ROW5_1;}
void setrow6_1(void){ROW5_0;ROW6_1;}
void setrow7_1(void){ROW6_0;ROW7_1;}
void setrow8_1(void){ROW7_0;ROW8_1;}
void setrow9_1(void){ROW0_1;ROW1_1;ROW2_1;ROW3_1;ROW4_1;ROW5_1;ROW6_1;ROW7_1;ROW8_1;}

u8 read_inputdata(void){//if any col has valid data////for example//////////////////////////////
	static u8 byte[2] = {0,0};		
	byte[0] = 0;									// col0 col1 col2 col3 col4 col5 col6 col7
	if(COL0){										//	0	 1	  0	   1	0	 0	  0	   0
		byte[0] |= 0x80;//bit 1000 0000				///////////////////////////////////////////
		byte[1]++;									//
	}
	if(COL1){
		byte[0] |= 0x40;//bit 0100 0000
		byte[1]++;
	}	
	if(COL2){
		byte[0] |= 0x20;//bit 0010 0000
		byte[1]++;
	}	
	if(COL3){
		byte[0] |= 0x10;//bit 0001 0000
		byte[1]++; 
	}	
	if(COL4){
		byte[0] |= 0x08;//bit 0000 1000
		byte[1]++;
	}	
	if(COL5){
		byte[0] |= 0x04;//bit 0000 0100
		byte[1]++;
	}	
	if(COL6){
		byte[0] |= 0x02;//bit 0000 0010
		byte[1]++;
	}	
	if(COL7){
		byte[0] |= 0x01;//bit 0000 0001
		byte[1]++;
	}	
	//u3_printf("read_inputdata:%x\r\n",byte[0]);
	return byte[0];

}
u8 *Read_KeyValue(void){
	static u8 byte[16];
	u8 temp_byte[16];
	u8 i = 0;
	for(i = 0; i < 16; i++){
		byte[i] = 0x00;
	}
	setrow0_1();delay_us(50);temp_byte[0] |= read_inputdata();//get input col value	防抖动
	setrow1_1();delay_us(50);temp_byte[1] |= read_inputdata();//get input col value	
	setrow2_1();delay_us(50);temp_byte[2] |= read_inputdata();//get input col value	
	setrow3_1();delay_us(50);temp_byte[3] |= read_inputdata();//get input col value	
	setrow4_1();delay_us(50);temp_byte[4] |= read_inputdata();//get input col value	
	setrow5_1();delay_us(50);temp_byte[5] |= read_inputdata();//get input col value	
	setrow6_1();delay_us(50);temp_byte[6] |= read_inputdata();//get input col value	
	setrow7_1();delay_us(50);temp_byte[7] |= read_inputdata();//get input col value	
	setrow8_1();delay_us(50);temp_byte[8] |= read_inputdata();//get input col value	
	setrow9_1();
	setrow0_1();delay_us(50);byte[0] |= read_inputdata();//get input col value	
	setrow1_1();delay_us(50);byte[1] |= read_inputdata();//get input col value	
	setrow2_1();delay_us(50);byte[2] |= read_inputdata();//get input col value	
	setrow3_1();delay_us(50);byte[3] |= read_inputdata();//get input col value	
	setrow4_1();delay_us(50);byte[4] |= read_inputdata();//get input col value	
	setrow5_1();delay_us(50);byte[5] |= read_inputdata();//get input col value	
	setrow6_1();delay_us(50);byte[6] |= read_inputdata();//get input col value	
	setrow7_1();delay_us(50);byte[7] |= read_inputdata();//get input col value	
	setrow8_1();delay_us(50);byte[8] |= read_inputdata();//get input col value		
	setrow9_1();
	while(temp_byte[0] != byte[0] | temp_byte[1] != byte[1] | temp_byte[2] != byte[2] | temp_byte[3] != byte[3] | 
		temp_byte[4] != byte[4] | temp_byte[5] != byte[5] | temp_byte[6] != byte[6] | temp_byte[7] != byte[7] | 
		temp_byte[8] != byte[8])
	{
		for(i = 0; i < 9; i++){
			temp_byte[i] = byte[i];
		}
		setrow0_1();delay_us(50);byte[0] |= read_inputdata();//get input col value	
		setrow1_1();delay_us(50);byte[1] |= read_inputdata();//get input col value	
		setrow2_1();delay_us(50);byte[2] |= read_inputdata();//get input col value	
		setrow3_1();delay_us(50);byte[3] |= read_inputdata();//get input col value	
		setrow4_1();delay_us(50);byte[4] |= read_inputdata();//get input col value	
		setrow5_1();delay_us(50);byte[5] |= read_inputdata();//get input col value	
		setrow6_1();delay_us(50);byte[6] |= read_inputdata();//get input col value	
		setrow7_1();delay_us(50);byte[7] |= read_inputdata();//get input col value	
		setrow8_1();delay_us(50);byte[8] |= read_inputdata();//get input col value		
		setrow9_1();
	}
	
	return byte;
}

u8 *map_2_key(u8 *read_byte){		//static u8 read_byte[16];
	
	static u8 byte[7] = {0,0,0,0,0,0,0};
	u8 i = 0;
	u8 fn = 0;
	u8 app = 0;
	for(i = 0; i < 7; i++){
		byte[i] = 0x00;
	}
	// for(i = 0; i < 8; i++){
	// 	u3_printf("%d\t%x\t\t%d\t%x\r\n",2*i,read_byte[2*i],2*i+1,read_byte[2*i+1]);
	// }
	i = 1;
	if(read_byte[0]){
		if((read_byte[0] & 0x80) == 0x80){byte[i] = key_esc;i++;if(i == 7){return byte;}}
		if((read_byte[0] & 0x40) == 0x40){byte[i] = key_1;i++;	if(i == 7){return byte;}}
		if((read_byte[0] & 0x20) == 0x20){byte[i] = key_2;i++;	if(i == 7){return byte;}}
		if((read_byte[0] & 0x10) == 0x10){byte[i] = key_3;i++;	if(i == 7){return byte;}}
		if((read_byte[0] & 0x08) == 0x08){byte[i] = key_4;i++;	if(i == 7){return byte;}}
		if((read_byte[0] & 0x04) == 0x04){byte[i] = key_5;i++;	if(i == 7){return byte;}}
		if((read_byte[0] & 0x02) == 0x02){byte[i] = key_6;i++;	if(i == 7){return byte;}}
		if((read_byte[0] & 0x01) == 0x01){byte[i] = key_7;i++;	if(i == 7){return byte;}}
	}
	if(read_byte[1]){
		if((read_byte[1] & 0x80) == 0x80){byte[i] = key_tab;i++;if(i == 7){return byte;}}
		if((read_byte[1] & 0x40) == 0x40){byte[i] = key_q;i++;	if(i == 7){return byte;}}
		if((read_byte[1] & 0x20) == 0x20){byte[i] = key_w;i++;	if(i == 7){return byte;}}
		if((read_byte[1] & 0x10) == 0x10){byte[i] = key_e;i++;	if(i == 7){return byte;}}
		if((read_byte[1] & 0x08) == 0x08){byte[i] = key_r;i++;	if(i == 7){return byte;}}
		if((read_byte[1] & 0x04) == 0x04){byte[i] = key_T;i++;	if(i == 7){return byte;}}
		if((read_byte[1] & 0x02) == 0x02){byte[i] = key_y;i++;	if(i == 7){return byte;}}
		if((read_byte[1] & 0x01) == 0x01){byte[i] = key_u;i++;	if(i == 7){return byte;}}
	}
	if(read_byte[2]){
		if((read_byte[2] & 0x80) == 0x80){byte[i] = key_caps_lock;i++;if(i == 7){return byte;}}
		if((read_byte[2] & 0x40) == 0x40){byte[i] = key_a;i++;	if(i == 7){return byte;}}
		if((read_byte[2] & 0x20) == 0x20){byte[i] = key_s;i++;	if(i == 7){return byte;}}
		if((read_byte[2] & 0x10) == 0x10){byte[i] = key_d;i++;	if(i == 7){return byte;}}
		if((read_byte[2] & 0x08) == 0x08){byte[i] = key_f;i++;	if(i == 7){return byte;}}
		if((read_byte[2] & 0x04) == 0x04){byte[i] = key_g;i++;	if(i == 7){return byte;}}
		if((read_byte[2] & 0x02) == 0x02){byte[i] = key_h;i++;	if(i == 7){return byte;}}
		if((read_byte[2] & 0x01) == 0x01){byte[i] = key_j;i++;	if(i == 7){return byte;}}
	}
	if(read_byte[3]){
		if((read_byte[3] & 0x80) == 0x80){byte[0] |= L_Shi;}
		if((read_byte[3] & 0x40) == 0x40){byte[i] = key_insert;i++;	if(i == 7){return byte;}}
		if((read_byte[3] & 0x20) == 0x20){byte[i] = key_z;i++;	if(i == 7){return byte;}}
		if((read_byte[3] & 0x10) == 0x10){byte[i] = key_x;i++;	if(i == 7){return byte;}}
		if((read_byte[3] & 0x08) == 0x08){byte[i] = key_c;i++;	if(i == 7){return byte;}}
		if((read_byte[3] & 0x04) == 0x04){byte[i] = key_v;i++;	if(i == 7){return byte;}}
		if((read_byte[3] & 0x02) == 0x02){byte[i] = key_b;i++;	if(i == 7){return byte;}}
		if((read_byte[3] & 0x01) == 0x01){byte[i] = key_n;i++;	if(i == 7){return byte;}}
	}
	if(read_byte[4]){
		if((read_byte[4] & 0x80) == 0x80){byte[0] |= R_Ctl;}
		if((read_byte[4] & 0x40) == 0x40){byte[0] |= L_Gui;}
		if((read_byte[4] & 0x20) == 0x20){byte[0] |= L_Alt;}
		if((read_byte[4] & 0x10) == 0x10){byte[i] = L_APP;i++;	if(i == 7){return byte;}}
		if((read_byte[4] & 0x08) == 0x08){byte[i] = L_FN;i++;	if(i == 7){return byte;}}
		if((read_byte[4] & 0x04) == 0x04){byte[i] = key_spacebar;i++;	if(i == 7){return byte;}}
		if((read_byte[4] & 0x02) == 0x02){byte[i] = key_home;i++;	if(i == 7){return byte;}}
		if((read_byte[4] & 0x01) == 0x01){byte[i] = key_end;i++;	if(i == 7){return byte;}}
	}
	if(read_byte[5]){
		if((read_byte[5] & 0x80) == 0x80){byte[i] = key_8;i++;if(i == 7){return byte;}}
		if((read_byte[5] & 0x40) == 0x40){byte[i] = key_9;i++;	if(i == 7){return byte;}}
		if((read_byte[5] & 0x20) == 0x20){byte[i] = key_0;i++;	if(i == 7){return byte;}}
		if((read_byte[5] & 0x10) == 0x10){byte[i] = key_minus;i++;	if(i == 7){return byte;}}
		if((read_byte[5] & 0x08) == 0x08){byte[i] = key_equal;i++;	if(i == 7){return byte;}}
		if((read_byte[5] & 0x04) == 0x04){byte[i] = key_grave;i++;	if(i == 7){return byte;}}
		if((read_byte[5] & 0x02) == 0x02){byte[i] = key_backslash;i++;	if(i == 7){return byte;}}
		if((read_byte[5] & 0x01) == 0x01){byte[i] = key_backspace;i++;	if(i == 7){return byte;}}
	}
	if(read_byte[6]){
		if((read_byte[6] & 0x80) == 0x80){byte[i] = key_i;i++;if(i == 7){return byte;}}
		if((read_byte[6] & 0x40) == 0x40){byte[i] = key_k;i++;	if(i == 7){return byte;}}
		if((read_byte[6] & 0x20) == 0x20){byte[i] = key_l;i++;	if(i == 7){return byte;}}
		if((read_byte[6] & 0x10) == 0x10){byte[i] = key_semicolon;i++;	if(i == 7){return byte;}}
		if((read_byte[6] & 0x08) == 0x08){byte[i] = key_o;i++;	if(i == 7){return byte;}}
		if((read_byte[6] & 0x04) == 0x04){byte[i] = key_p;i++;	if(i == 7){return byte;}}
		if((read_byte[6] & 0x02) == 0x02){byte[i] = key_brace1;i++;	if(i == 7){return byte;}}
		if((read_byte[6] & 0x01) == 0x01){byte[i] = key_brace2;i++;	if(i == 7){return byte;}}
	}
	if(read_byte[7]){
		if((read_byte[7] & 0x80) == 0x80){byte[i] = key_m;i++;if(i == 7){return byte;}}
		if((read_byte[7] & 0x40) == 0x40){byte[i] = key_comma;i++;	if(i == 7){return byte;}}
		if((read_byte[7] & 0x20) == 0x20){byte[i] = key_full_stop;i++;	if(i == 7){return byte;}}
		if((read_byte[7] & 0x10) == 0x10){byte[i] = key_slash;i++;	if(i == 7){return byte;}}
		if((read_byte[7] & 0x08) == 0x08){byte[0] |= R_Shi;}
		if((read_byte[7] & 0x04) == 0x04){byte[i] = key_quotation_mark;i++;	if(i == 7){return byte;}}
		if((read_byte[7] & 0x02) == 0x02){byte[i] = key_enter;i++;	if(i == 7){return byte;}}
		if((read_byte[7] & 0x01) == 0x01){byte[i] = key_delete;i++;	if(i == 7){return byte;}}
	}
	if(read_byte[8]){
		
		if((read_byte[8] & 0x10) == 0x10){byte[i] = key_app;i++;	if(i == 7){return byte;}}
		if((read_byte[8] & 0x08) == 0x08){byte[i] = key_left;i++;	if(i == 7){return byte;}}
		if((read_byte[8] & 0x04) == 0x04){byte[i] = key_down;i++;	if(i == 7){return byte;}}
		if((read_byte[8] & 0x02) == 0x02){byte[i] = key_up;i++;	if(i == 7){return byte;}}
		if((read_byte[8] & 0x01) == 0x01){byte[i] = key_right;i++;	if(i == 7){return byte;}}
	}
	
	
	for(i = 1; i < 9; i++){
		if(byte[i] == R_FN | byte[i] == L_FN){
			byte[i] = 0;
			fn = 1;
		}
		if(byte[i] == R_APP | byte[i] == L_APP){
			byte[i] = 0;
			app = 1;
		}
	}

	if(byte[0] == R_Alt || byte[0] == L_Alt){
		if(byte[1] == key_4 && byte[2] == 0x00 && byte[3] == 0x00 && byte[4] == 0x00 && byte[5] == 0x00 && byte[6] == 0x00){
			byte[1] = key_F4;
		}
	}

	if(fn | app){//fn and app process
		if(fn){
			for(i = 1; i < 7; i++){//找到第一个其他按键键值，only response one fn function.返回给主发送程序。
				if(byte[i]){
					byte[1] = fn_fnc(byte[i]);
					byte[2]=0;byte[3]=0;byte[4]=0;byte[5]=0;byte[6]=0;
					break;
				}
			}
		}
		if(app){
			for(i = 1; i < 7; i++){//找到第一个其他按键键值，不让主发送程序响应，因为主程序只能发送一次按键，app需要支持复杂字串发送
				if(byte[i]){
					byte[1] = app_fnc(byte[i]);
					byte[2]=0;byte[3]=0;byte[4]=0;byte[5]=0;byte[6]=0;
					break;
				}
			}
		}
	}
	return byte;
}
u8 if_key_same(u8* byte_before,u8*byte){
	u8 downup = 0, updown = 0;
	u8 i = 0,j = 0;
	
	if(byte[0] == byte_before[0]){
		for(i = 1; i < 7; i++){
			for(j = 1; j < 7; j++){
				if(byte[i] == byte_before[j]){
					downup++;
					break;
				}
			}
		}
		for(i = 1; i < 7; i++){
			for(j = 1; j < 7; j++){
				if(byte_before[i] == byte[j]){
					updown++;
					break;
				}
			}
		}
		if(downup == 6 && updown == 6){
			return 1;
		}
	}

	return 0;
}

u8 send_key(void){		//static u8 byte[7]
	static u8 byte_before[7] = {0,0,0,0,0,0,0};//old values
	u8* byte;//read new key map
	u8 i = 0,j = 0,k = 0,less = 0,more = 0;
	u8 lesss = 0, moree = 0;
	byte = map_2_key(Read_KeyValue());//read new key map
	//u3_printf("before_byte:%x %x %x %x %x %x %x \r\n",byte_before[0],byte_before[1],byte_before[2],byte_before[3],byte_before[4],byte_before[5],byte_before[6]);
	//u3_printf("byte:%x %x %x %x %x %x %x \r\n",byte[0],byte[1],byte[2],byte[3],byte[4],byte[5],byte[6]);

	if(if_key_same(byte_before,byte))
	{
		//u3_printf("same\r\n");
		return 0;
	}else{//if key changed
		// u3_printf("before_byte:%x %x %x %x %x %x %x \r\n",byte_before[0],byte_before[1],byte_before[2],byte_before[3],byte_before[4],byte_before[5],byte_before[6]);
		// u3_printf("byte:	   %x %x %x %x %x %x %x \r\n",byte[0],byte[1],byte[2],byte[3],byte[4],byte[5],byte[6]);
		if(byte[1] >= APP1 && byte[1] <= APP10){
			if(byte[1] == APP1){mailbox();delay_ms(300);return 0;}
			if(byte[1] == APP2){password();delay_ms(300);return 0;}	
			if(byte[1] == APP8){changeDmode();delay_ms(300);return 0;}
			if(byte[1] == APP9){clear();delay_ms(300);return 0;}
			if(byte[1] == APP10){pair();delay_ms(300);return 0;}
			return 0;
		}
			
		for(i = 1; i < 7; i++){//find if less ,if less, set byte_before to 0x00
			less = 1;
			for(j = 1; j < 7; j++){
				if(byte_before[i] == byte[j]){
					less = 0;
					break;
				}
			}
			if(less){
				byte_before[i] = 0x00;
				lesss = 1;
			}
		}
		if(lesss == 1){
			u3_printf("%x%x%x%x%x%x%x\r\n",byte_before[0],byte_before[1],byte_before[2],byte_before[3],byte_before[4],byte_before[5],byte_before[6]);
			printf("%c%c%c%c%c%c%c%c%c%c%c%c",//
					0x0c,0x00,0xa1,0x01,byte_before[0],0x00,
					byte_before[1],byte_before[2],byte_before[3],byte_before[4],byte_before[5],byte_before[6]);
		}
		
		for(i = 1; i < 7; i++){//find if more , set byte to before_byte 0x00 position
			more = 1;
			for(j = 1; j < 7; j++){
				if(byte[i] == byte_before[j]){
					more = 0;
					break;
				}
			}
			if(more){//byte[i] is the more key down
				moree = 1;
				for(k = 1; k < 7; k++){
					if(byte_before[k] == 0x00){
						byte_before[k] = byte[i];
						break;
					}
				}
			}
		}
		if(moree == 1){
			u3_printf("%x%x%x%x%x%x%x \r\n",byte_before[0],byte_before[1],byte_before[2],byte_before[3],byte_before[4],byte_before[5],byte_before[6]);
			printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,byte_before[0],0x00,
					byte_before[1],byte_before[2],byte_before[3],byte_before[4],byte_before[5],byte_before[6]);
		}
		if(moree == 0 && lesss ==0){//only ctl key changed
			byte_before[0] = byte[0];
			u3_printf("%x%x%x%x%x%x%x \r\n",byte_before[0],byte_before[1],byte_before[2],byte_before[3],byte_before[4],byte_before[5],byte_before[6]);
			printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,byte_before[0],0x00,
					byte_before[1],byte_before[2],byte_before[3],byte_before[4],byte_before[5],byte_before[6]);
		}


	}
	return 0;
}


void password(void){
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",//shift+q
					0x0c,0x00,0xa1,0x01,0x02,0x00,key_q,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(90);

	printf("%c%c%c%c%c%c%c%c%c%c%c%c",//syok
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_s,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(100);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",//syok
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_y,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(90);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",//syok
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_o,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(100);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",//syok
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_k,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(90);

	printf("%c%c%c%c%c%c%c%c%c%c%c%c",//shift+2
					0x0c,0x00,0xa1,0x01,0x02,0x00,key_2,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(100);

	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_3,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
					delay_ms(90);
	
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_5,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
					delay_ms(100);
	
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_1,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
					delay_ms(90);
	
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_9,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
					delay_ms(100);
	
	
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_9,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
					delay_ms(90);
					
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_0,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
					
}

void mailbox(void){
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_q,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_s,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_y,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_2,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_0,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_1,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_1,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_8,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_1,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_5,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x02,0x00,key_2,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_1,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_6,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_3,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_full_stop,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_c,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_o,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
	delay_ms(10);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_m,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);

}
void changeDmode(void){
	AT_1;delay_ms(300);AT_0;
//	send_cmd("AT+DMODE=1","OK",200);
//	send_cmd("AT+EXIT","OK",200);
	send_cmd("AT+NAME=PandaBLT08","OK",200);
	send_cmd("AT+EXIT","OK",200);
}
void clear(void){
	AT_1;delay_ms(300);AT_0;
	send_cmd("AT+UNPLUG","OK",200);
	send_cmd("AT+EXIT","OK",200);
}
void pair(void){
	u8 times = 0;
	AT_1;delay_ms(300);AT_0;
	send_cmd("AT+PAIR","OK",200);
	//send_cmd("AT+EXIT","OK",200);
	while(!send_cmd("AT+STATE?","4",200)){
		delay_ms(1000);
		times++;
		if(times == 20){
			send_cmd("AT+EXIT","OK",200);   
			break;
		}
	}
	send_cmd("AT+EXIT","OK",200);
}

void low_power(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
/////////////////////////////////////////////////

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
//////////////////////////////////////////////

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

void a(void){
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,key_a,0x00,0x00,0x00,0x00,0x00);
	printf("%c%c%c%c%c%c%c%c%c%c%c%c",
					0x0c,0x00,0xa1,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
}
u8 fn_fnc(u8 key){
	u8 key_out = 0;
	if(key == key_1){return key_F1;}
	if(key == key_2){return key_F2;}
	if(key == key_3){return key_F3;}
	if(key == key_4){return key_F4;}
	if(key == key_5){return key_F5;}
	if(key == key_6){return key_F6;}
	if(key == key_7){return key_F7;}
	if(key == key_8){return key_F8;}
	if(key == key_9){return key_F9;}
	if(key == key_0){return key_F10;}
	if(key == key_minus){return key_F11;}
	if(key == key_equal){return key_F12;}

	if(key == key_w){return vol_up;}
	if(key == key_s){return mute;}
	if(key == key_x){return vol_down;}

	if(key == key_home){return key_page_up;}
	if(key == key_end){return key_page_down;}
	return key_out;
}
u8 app_fnc(u8 key){
	u8 key_out = 0;
	if(key == key_1){return APP1;}
	if(key == key_2){return APP2;}
	if(key == key_3){return APP3;}
	if(key == key_4){return APP4;}
	if(key == key_5){return APP5;}
	if(key == key_6){return APP6;}
	if(key == key_7){return APP7;}
	if(key == key_backspace){return APP8;}//changeDmode
	if(key == key_delete){return APP9;}//clear
	if(key == key_p){return APP10;}//pair
	return key_out;
}

