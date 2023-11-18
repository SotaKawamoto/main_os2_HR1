//mbed-os-2
#include "mbed.h"

Serial pc(USBTX,USBRX,9600);//main - pc USB
Serial im920(PA_9,PA_10,19200);//main im - PC im//mainのUart tx,rxピンにつなぐ
//https://forums.mbed.com/t/no-member-named-printf-in-mbed-bufferserial/13574 

Ticker status;
Ticker launch;
Ticker t;

DigitalOut M2S(PA_2);
DigitalOut S2M(PA_3);

DigitalOut cameraPow(PA_8);
DigitalOut cameraRec(PA_11);
DigitalOut solenoid(PB_6);
DigitalOut buzzer(PB_1);
DigitalIn flightPin(PA_12);


int temp1;
char temp2;
char str1[100];
char str2[100];
int a;
int mode = 0;

int getmode();
void sendstatus();
void FlightPinDriver();
void solenoid_ON();
void buzzerON();

int main()
{
    im920.printf("TXDA  開始\r\n");
    wait_us(5000000);//5秒
    im920.printf("TXDA  5秒経過\r\n");
    im920.printf("TXDA  10秒毎のロケットの状態送信開始\r\n");
    wait_us(1000000);
    launch.attach(FlightPinDriver,0.5);
    status.attach(sendstatus,3);
    

    //ループさせて、pcと送受信し続ける?受信？送受信？
    while(1){
        im920.printf("TXDA  modeを入手して送信し返すループ開始\r\n");
        wait_us(1000000);
        int temp1 = getmode();
        if(temp1 != 0){//mode変更のコマンドが送られるとmodeを変更し、現在のmodeをPC側に送信する ex)送信側で01のモード１を打ち込むとgetmode関数からreturn 1が返ってきてmodeを送り返す
            mode = temp1;
        }
    }
}

int getmode(){
        if(im920.readable()){
            int i=0;
            temp2 = im920.getc();
            while(temp2 != '\r'){
                str1[i] = temp2;
                i++;
                temp2=im920.getc();
                if(str1[0] != str2[0] || str1[1] != str2[1]){ //二桁コマンド用なので変更の余地あり   

                    if((str1[i-2]==0)&&(str1[i-1]==1)){
                        im920.printf("TXDA モードが01に変更されました\r\n");
                        return 1;//1モードへ変更の指示
                    }

                    str2[0] = str1[0];
                    str2[1] = str1[1];//2度目のモード変更通知を防ぐ

                }            
            }
        }
    return 0;//0モードへの変更指示
}

void sendstatus(){
    im920.printf("TXDA　現在のmodeは%d\r\n",mode);
}

void FlightPinDriver()
{
    if(flightPin==1) {
        //M2S = 1; マイコン同士の通信
        //S2M = 0;     
        im920.printf("TXDA flightworked\r\n");
        launch.detach();
        t.attach(solenoid_ON,3.6);//ソレノイドを動かすまでの時間、十分頂点時間？センサ？
    }
}

void solenoid_ON()
{
    //M2S = 1;
    //S2M = 1;
    solenoid=1;//開放！！
    wait(1.0);//ソレノイドをプルする時間//限界は知らない
    solenoid=0;
    t.detach();
    t.attach(buzzerON,60);//ブザー作動までの時間/
}

void buzzerON()
{
    im920.printf("TXDA buzzer ON\n\r");
    //M2S = 0;
    //S2M = 1;
    buzzer=1;//buzzer発振
}