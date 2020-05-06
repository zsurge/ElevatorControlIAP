#include "bsp_beep.h" 

//��ʼ��PF8Ϊ�����		    
//BEEP IO��ʼ��

int melody[] = {50, 50, 50, 50, 200, 200, 200, 400, 400, 500, 500, 500};


void bsp_beep_init(void)
{   
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_ALL_BEEP, ENABLE);//ʹ��GPIOFʱ��
    
    //��ʼ����������Ӧ����GPIOF8
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_BEEP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
    GPIO_Init(GPIO_PORT_BEEP, &GPIO_InitStructure);//��ʼ��GPIO
      
//    GPIO_ResetBits(GPIO_PORT_BEEP,GPIO_PIN_BEEP);  //��������Ӧ����GPIOF6����

    BEEP=1;
}


void Sound(u16 frq)
{
	u32 time;
	if(frq != 1000)
	{
		time = 500000/((u32)frq);
		BEEP = 1;
		delay_us(time);
		BEEP = 0;
		delay_us(time);
	}else
		delay_us(1000);
}
void Sound2(u16 time)
{
    BEEP = 1;
    delay_ms(time);
    BEEP = 0;
    delay_ms(time);
}
void play_successful(void)
{
    int id=0;
    for(id = 0 ;id < 12 ;id++)
    {
        Sound2(melody[id]);
    }
}
void play_failed(void)
{
    int id=0;
    for(id = 11 ;id >=0 ;id--)
    {
        Sound2(melody[id]);
    }
}
void play_music(void)
{
	//              ��7  1   2   3   4   5   6   7  ��1 ��2 ��3 ��4 ��5 ������
	uc16 tone[] = {247,262,294,330,349,392,440,294,523,587,659,698,784,1000};//��Ƶ���ݱ�
	//�쳾���
	u8 music[]={5,5,6,8,7,6,5,6,13,13,//����
                5,5,6,8,7,6,5,3,13,13,
                2,2,3,5,3,5,6,3,2,1,
                6,6,5,6,5,3,6,5,13,13,

                5,5,6,8,7,6,5,6,13,13,
                5,5,6,8,7,6,5,3,13,13,
                2,2,3,5,3,5,6,3,2,1,
                6,6,5,6,5,3,6,1,	

                13,8,9,10,10,9,8,10,9,8,6,
                13,6,8,9,9,8,6,9,8,6,5,
                13,2,3,5,5,3,5,5,6,8,7,6,
                6,10,9,9,8,6,5,6,8};	
	u8 time[] = {2,4,2,2,2,2,2,8,4, 4, //ʱ��
                2,4,2,2,2,2,2,8,4, 4, 
                2,4,2,4,2,2,4,2,2,8,
                2,4,2,2,2,2,2,8,4 ,4, 

                2,4,2,2,2,2,2,8,4, 4, 
                2,4,2,2,2,2,2,8,4, 4, 
                2,4,2,4,2,2,4,2,2,8,
                2,4,2,2,2,2,2,8,

                4, 2,2,2, 4, 2,2,2, 2,2,8,
                4, 2,2,2,4,2,2,2,2,2,8,
                4, 2,2,2,4,2,2,5,2,6,2,4,
                2,2 ,2,4,2,4,2,2,12};	
	u32 yanshi;
	u16 i,e;
	yanshi = 10;
	for(i=0;i<sizeof(music)/sizeof(music[0]);i++){
		for(e=0;e<((u16)time[i])*tone[music[i]]/yanshi;e++){
			Sound((u32)tone[music[i]]);
		}	
	}
}





