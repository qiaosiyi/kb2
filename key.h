#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
#include "stm32f10x.h"

#define COL0	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)//读取
#define COL1	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)//读取
#define COL2	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//读取
#define COL3	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)//读取
#define COL4	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)//读取
#define COL5	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)//读取
#define COL6	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)//读取
#define COL7	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12)//读取

#define ROW0_1	GPIO_SetBits(GPIOB,GPIO_Pin_0)
#define ROW0_0	GPIO_ResetBits(GPIOB,GPIO_Pin_0)

#define ROW1_1	GPIO_SetBits(GPIOB,GPIO_Pin_1)
#define ROW1_0	GPIO_ResetBits(GPIOB,GPIO_Pin_1)

#define ROW2_1	GPIO_SetBits(GPIOB,GPIO_Pin_3)
#define ROW2_0	GPIO_ResetBits(GPIOB,GPIO_Pin_3)

#define ROW3_1	GPIO_SetBits(GPIOB,GPIO_Pin_4)
#define ROW3_0	GPIO_ResetBits(GPIOB,GPIO_Pin_4)

#define ROW4_1	GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define ROW4_0	GPIO_ResetBits(GPIOB,GPIO_Pin_5)

#define ROW5_1	GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define ROW5_0	GPIO_ResetBits(GPIOB,GPIO_Pin_6)

#define ROW6_1	GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define ROW6_0	GPIO_ResetBits(GPIOB,GPIO_Pin_7)

#define ROW7_1	GPIO_SetBits(GPIOB,GPIO_Pin_8)
#define ROW7_0	GPIO_ResetBits(GPIOB,GPIO_Pin_8)

#define ROW8_1	GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define ROW8_0	GPIO_ResetBits(GPIOB,GPIO_Pin_9)

#define AT_1	GPIO_SetBits(GPIOC,GPIO_Pin_14)
#define AT_0	GPIO_ResetBits(GPIOC,GPIO_Pin_14)

#define CLEAR_1	GPIO_SetBits(GPIOC,GPIO_Pin_15)
#define CLEAR_0	GPIO_ResetBits(GPIOC,GPIO_Pin_15)

 
void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数		
void ESC_KEY_DOWN(void);//
void ESC_KEY_UP(void);//
void password(void);//
void mailbox(void);
void low_power(void);
void changeDmode(void);
void clear(void);
void pair(void);

void setrow0_0(void);
void setrow0_1(void);
void setrow1_1(void);
void setrow2_1(void);
void setrow3_1(void);
void setrow4_1(void);
void setrow5_1(void);
void setrow6_1(void);
void setrow7_1(void);
void setrow8_1(void);
void setrow9_1(void);

u8 read_inputdata(void);//读入一行值
u8 *Read_KeyValue(void);//扫描键盘，过滤抖动，检测按键变动，并返回前六个按下的按键的位置
u8 *map_2_key(u8*);		//根据键盘布局，将位置翻译为按键意义，并做 fn 和 app  组合键的处理
u8 send_key(void);		//组合数据报文给蓝牙模块发送数据
u8 app_fnc(u8);			//自定义app组合键
u8 fn_fnc(u8);			//自定义fn组合键
u8* check_cmd(u8 *str);
u8 send_cmd(u8 *cmd,u8 *ack,u16 waittime);

//////////////////////////////
void a(void);
void b(void);
void c(void);
void d(void);
void e(void);
void f(void);
void g(void);
void h(void);
void i(void);
void j(void);
void k(void);
void l(void);
void m(void);
void n(void);
void o(void);
void p(void);
void q(void);
void r(void);
void s(void);
void t(void);
void u(void);
void v(void);
void w(void);
void x(void);
void y(void);
void z(void);
void ENTER(void);
void SPACE(void);
void a2z(void);
void tmp(void);


////////////////////////////
#define key_a 0x4
#define key_b 0x5
#define key_c 0x6
#define key_d 0x7
#define key_e 0x8
#define key_f 0x9
#define key_g 0xa
#define key_h 0xb
#define key_i 0xc
#define key_j 0xd
#define key_k 0xe
#define key_l 0xf
#define key_m 0x10
#define key_n 0x11
#define key_o 0x12
#define key_p 0x13
#define key_q 0x14
#define key_r 0x15
#define key_s 0x16
#define key_T 0x17
#define key_u 0x18
#define key_v 0x19
#define key_w 0x1a
#define key_x 0x1b
#define key_y 0x1c
#define key_z 0x1d
#define key_1 0x1e
#define key_2 0x1f
#define key_3 0x20
#define key_4 0x21
#define key_5 0x22
#define key_6 0x23
#define key_7 0x24
#define key_8 0x25
#define key_9 0x26
#define key_0 0x27
#define key_enter 0x28
#define key_esc 0x29
#define key_backspace 0x2a
#define key_tab 0x2b
#define key_spacebar 0x2c
#define key_minus 0x2d
#define key_equal 0x2e
#define key_brace1 0x2f
#define key_brace2 0x30
#define key_backslash 0x31
#define key_wave 0x32
#define key_semicolon 0x33
#define key_quotation_mark 0x34
#define key_grave 0x35
#define key_comma 0x36
#define key_full_stop 0x37
#define key_slash 0x38
#define key_caps_lock 0x39
#define key_F1 0x3a
#define key_F2 0x3b
#define key_F3 0x3c
#define key_F4 0x3d
#define key_F5 0x3e
#define key_F6 0x3f
#define key_F7 0x40
#define key_F8 0x41
#define key_F9 0x42
#define key_F10 0x43
#define key_F11 0x44
#define key_F12 0x45
#define key_print_screen 0x46
#define key_scroll_lock 0x47
#define key_pause 0x48
#define key_insert 0x49
#define key_home 0x4a
#define key_page_up 0x4b
#define key_delete 0x4c
#define key_end 0x4d
#define key_page_down 0x4e
#define key_right 0x4f
#define key_left 0x50
#define key_down 0x51
#define key_up 0x52
#define key_num_lock 0x53
#define key_divide 0x54
#define key_multiply 0x55
#define key_minus2 0x56
#define key_plus 0x57
#define key_enter2 0x58
#define key_1n 0x59
#define key_2n 0x5a
#define key_3n 0x5b
#define key_4n 0x5c
#define key_5n 0x5d
#define key_6n 0x5e
#define key_7n 0x5f
#define key_8n 0x60
#define key_9n 0x61
#define key_0n 0x62
#define key_dot 0x63
#define key_app 0x65
#define key_menu 0x76
#define mute 0x7f
#define vol_up 0x80
#define vol_down 0x81

#define R_FN 0xa5
#define L_FN 0xa6
#define R_APP 0xa7
#define L_APP 0xa8
#define APP1 0xa9
#define APP2 0xaa
#define APP3 0xab
#define APP4 0xac
#define APP5 0xad
#define APP6 0xae
#define APP7 0xaf
#define APP8 0xb0
#define APP9 0xb1
#define APP10 0xb2

#define key_at 0xce



#define R_Gui 0x80
#define R_Alt 0x40
#define R_Shi 0x20
#define R_Ctl 0x10
#define L_Gui 0x08
#define L_Alt 0x04
#define L_Shi 0x02
#define L_Ctl 0x01





#endif
