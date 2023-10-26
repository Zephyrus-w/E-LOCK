#include<reg51.h>
#include<stddef.h>
#define LCD1602_FLAG
#define LCD1602_PORT P0
#define uchar unsigned char
#define uint unsigned int 
#define MAX_LENGTH 6
sbit lcd1602_busy=P0^7;
sbit lcd1602_RS=P2^0;
sbit lcd1602_RW=P2^1;
sbit lcd1602_EN=P2^2;
//sbit BUZZER=P3^0;
sbit LED=P3^4;

uchar str[]="INPUT PASSWORD:";
uchar str1[]="INPUT NEW PW:   ";
uchar str2[]="INPUT AGAIN:    ";
uchar str3[]="PASSWORD UPDATED";
uchar keyval,keystr[MAX_LENGTH],keytemp[MAX_LENGTH];

void delay(uchar a)//Time Delay
{
uchar i;
while(a--)
	for(i=0;i<120;i++);
}

void Beep()//Buzzer Beep
{
uint i;
for(i=0;i<100;i++)
{
delay(1);
P3^=0x01;
}
}

void lcd1602_CheckBusy()
{
do
{
lcd1602_busy=1;
lcd1602_RS=0;
lcd1602_RW=1;
lcd1602_EN=0;
lcd1602_EN=1;
}
while(lcd1602_busy);
}

void lcd1602_WriteCmd(const uchar cmd)
{
lcd1602_CheckBusy();
lcd1602_RS=0;
lcd1602_RW=0;
lcd1602_EN=1;
LCD1602_PORT=cmd;
lcd1602_EN=0;
}

void lcd1602_WriteData(const uchar c)
{
lcd1602_CheckBusy();
lcd1602_RS=1;
lcd1602_RW=0;
lcd1602_EN=1;
LCD1602_PORT=c;
lcd1602_EN=0;
}

void lcd1602_Init()
{
lcd1602_WriteCmd(0x38); //
lcd1602_WriteCmd(0x0c); //display enable,flag enable,flash enable,
lcd1602_WriteCmd(0x06); //flag move to right,screen don't move
lcd1602_WriteCmd(0x01); //clear screen
}

void lcd1602_Display(const uchar *ptr,uchar line)
{
uchar data i=0;
uchar *data q;
q=ptr;
switch(line)
{
case 0:
lcd1602_WriteCmd(0x80);
while(q!=NULL /*&& (*q!='\0')*/ && i<16)
{
lcd1602_WriteData(*q);
q++;
i++;
}
break;
case 1:
lcd1602_WriteCmd(0xc0);
while(q!=NULL && (*q!='\0') && i<MAX_LENGTH)
{
lcd1602_WriteData(*q);
q++;
i++;
}
break;
}//end of switch
}//end of lcd1602_Display

void lcd1602_Clear(uchar line)
{
	uint i=0;
	switch(line)
{
case 0:
lcd1602_WriteCmd(0x80);
for(i=0;i<16;i++)
{lcd1602_WriteData(0x20);}
break;
case 1:
lcd1602_WriteCmd(0xc0);
for(i=0;i<16;i++)
{lcd1602_WriteData(0x20);}
break;
}//end of switch
}//end of lcd1602_Clear

uchar kbscan(void)
{
unsigned char sccode,recode;
P1=0x0f;
if ((P1 & 0x0f) != 0x0f)
{
delay(20);
if ((P1&0x0f)!= 0x0f)
{
sccode = 0xef;
while((sccode&0x01)!=0)
{
P1=sccode;
if((P1&0x0f)!=0x0f)
{
recode=(P1&0x0f)|0xf0;
while((P1&0x0f)!=0x0f);
return((~sccode)+(~recode));
}
else
sccode=(sccode<<1)|0x01;
}
}
}
return 0;
}

uchar getkey(void)
{
uchar key;
key=kbscan();
if(key==0){keyval=0xff;return(0);}
switch(key)
{
case 0x11:keyval=0;break;
case 0x12:keyval=1;break;
case 0x14:keyval=2;break;
case 0x18:keyval=3;break;
case 0x21:keyval=4;break;
case 0x22:keyval=5;break;
case 0x24:keyval=6;break;
case 0x28:keyval=7;break;
case 0x41:keyval=8;break;
case 0x42:keyval=9;break;
case 0x44:keyval=17;break;
case 0x48:keyval=18;break;
case 0x81:keyval=19;break;
case 0x82:keyval=20;break;
case 0x84:keyval=21;break;
case 0x88:keyval=22;break;
default:keyval=0xff;break;
}
if(keyval!=0xff)return(1);
else return(0);
}

main()
{
uchar num=0,i=0,j=0,time=0;
uchar pw=000000;//default password
uchar npw;//score new password
uchar in=0;//score pw input	
uchar temp=0;//scorenpw input
uchar flag=0;//record user is setting new pw(1) or not(0)
uchar lock=0;//record locked=0 or unlocked=1
lcd1602_Init();
lcd1602_Display(str,0);
while(1)
{
if(getkey())
{
	if(keyval>=0&&keyval<=9&&num<6)//input password
	{
		if(lock==0)//input pw
		{
			keystr[num]=keyval+0x30;
		  lcd1602_Display(keystr,1);
		  Beep();
		  in=in*10+keyval;
		  num++;
    }
		if(lock==1&&flag==1)//change pw
		{
			if(time==0)//first input, set
			{
				keytemp[num]=keyval+0x30;
			  lcd1602_Display(keytemp,1);
			  Beep();
			  npw=npw*10+keyval;
			  num++;
      }
			if(time==1)//second input, confirm
			{
				keytemp[num]=keyval+0x30;
				lcd1602_Display(keytemp,1);
				Beep();
				temp=temp*10+keyval;
				num++;
			}
		}
		
  }
	if(keyval==17&&lock==1&&flag==0)//set new password(only when unlocked)
	{
		flag=1;
		num=0;
		npw=0;
		temp=0;
		time=0;
		lcd1602_Clear(0);
		lcd1602_Display(str1,0);
		lcd1602_Clear(1);
  }
	if(keyval==18&&flag==1)//cancel set
	{
		npw=0;
		temp=0;
		flag=0;
		time=0;
		lcd1602_Clear(0);
		lcd1602_Display(str,0);
		lcd1602_Clear(1);
  }
	if(keyval==19&&lock==1&&flag==0)//lock
	{
		lock=0;
		P3^=0xff;
		in=0;
		num=0;
		for(i=0;i<6;i++) {keystr[i]=0x20;}
  }
	if(keyval==20)//clear all password input
	{
		lcd1602_Clear(1);
		if(flag==0)
		  {for(num;num>0;num--) keystr[num]=0x20;}
		if(flag==1)
			{for(num;num>0;num--) keytemp[num]=0x20;}
  }
	if(keyval==21)//backspace
	{
		if(num>0)
		{
			num--;
			if(lock==0)
			{
				keystr[num]=0x20;
				lcd1602_Display(keystr,1);
				in=in/10;
			}
			if(flag==1)
			{
				keytemp[num]=0x20;
				lcd1602_Display(keytemp,1);
				temp=temp/10;
			}
		}
  }
	if(keyval==22&&num==6)//check password or set new pw
	{
		if(lock==0)//check pw
		{
			if(in!=pw)//wrong password
		  {
				lcd1602_Clear(1);
			  for(i=0;i<6;i++) {keystr[i]=0x20;}
			  num=0;
			  in=0;
			  while(!getkey()||keyval!=20)//Buzzer beeps before user "clear" input
			  {Beep();}
		  }
		else if(in==pw)//right password
		  {
			  P3^=0x10;//light LED
			  lock=1;//successfully unlock
			  num=0;
			  in=0;
			  lcd1602_Clear(1);
				for(i=0;i<6;i++) {keystr[i]=0x20;}
		  }
    }//end of lock=0
		if(lock==1&&flag==1)//set npw
		{
			if(time==0)//finish first input, prepare for second input
			{
				num=0;
			  for(i=0;i<6;i++) {keytemp[i]=0x20;}
			  lcd1602_Clear(0);
			  lcd1602_Display(str2,0);
			  lcd1602_Clear(1);
				time=1;
			}
			else if(time==1)//finish second input, next compare two input
			{
			  if(temp==npw)//second=first, succeed
				  {
					  pw=npw;//update new password
						for(i=0;i<20;i++)//Buzzer beeps to remind
						{
							Beep();
            }
					  lcd1602_Clear(0);
					  lcd1602_Display(str,0);
					  lcd1602_Clear(1);
				  }
			    if(temp!=npw)//second!=first, unsucceed
			    {
				    lcd1602_Clear(0);
				    lcd1602_Display(str,0);
				    lcd1602_Clear(1);
			    }
					npw=0;
				  temp=0;
				  flag=0;
					time=0;
					for(i=0;i<6;i++) {keytemp[i]=0x20;}
			}//end of if(time==1)
		}
  }
}//end of if(getkey())
}//end of while(1)
}//end of main