//mbed-os-2
#include "mbed.h"
#include "getGPS.h"

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
DigitalOut debbag(PB_5);
//AnalogOut imRST(PA_1);これをつけると動かなくなった

int temp1;
char str1[100];
char str2[100];
int a;
int mode = 0;

int getmode();
void sendstatus();
void FlightPinDriver();
void solenoid_ON();
void buzzerON();
void TXgps();

int main()
{
    //im920.printf("TXDA  開始\r\n");
    wait_us(10000000);//5秒
    //im920.printf("TXDA  5s\r\n");
    //im920.printf("TXDA  SENDSTATUS\r\n");
    //wait_us(1000000);
    launch.attach(FlightPinDriver,0.5);//flightpinコマンド後にでもいいかも
    status.attach(sendstatus,5);
    

    //ループさせて、pcと送受信し続ける?受信？送受信？
    while(1){
        //im920.printf("TXDA  GETMODEループ\r\n");
        //wait_us(1000000);whileの中にwaitはだめっぽいで,スタックする
        temp1 = getmode();//getmodeでスタックしてるっぽい whileの中にwaitが渋い同様、非同期動作も渋いかも
        //wait
        //im920.printf("TXDA getmode突破");
        if(temp1 != 0){//mode変更のコマンドが送られるとmodeを変更し、現在のmodeをPC側に送信する ex)送信側で01のモード１を打ち込むとgetmode関数からreturn 1が返ってきてmodeを送り返す
            mode = temp1;
            sendstatus();
        }
    }
}

int getmode(){
        char temp2;
        //im920.printf("TXDA %c",temp2);

        if(im920.readable()){
            int i=0;

            temp2 = im920.getc();

            while(temp2 != '\n'){//whileの中に非同期動作のim920.printf等を入れるのはよくないかも、これを無くしたらスタック直った
                str1[i] = temp2;
                i++;
                temp2=im920.getc();
                //im920.printf("TXDA %c",temp2);
            }
                
                if(str1[0] != str2[0] || str1[1] != str2[1]){ //00もしくはok,ngの判定   
                    
                    if((str1[0] != 'O' && str2[1] != 'K')&&(str1[0] != 'N' && str2[1] != 'G')){
                        im920.printf("TXDA %c%c %c%c\n\r",str1[0],str1[1],str1[12],str1[13]);
                        if((str1[12]=='0')&&(str1[13]=='1')){
                            im920.printf("TXDA モード01に変更\r\n");
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
    im920.printf("TXDA solenoidworked\r\n");
    wait(4.0);//ソレノイドをプルする時間//限界は知らない
    solenoid=0;
    t.detach();
    t.attach(buzzerON,10);//ブザー作動までの時間/
}

void buzzerON()
{
    im920.printf("TXDA buzzer ON\n\r");
    //M2S = 0;
    //S2M = 1;
    buzzer=1;//buzzer発振
    t.detach();
    //t.attach(TXgps,4);
    TXgps();
}


void TXgps()
{
    GPS gps(A7, A2);
    //GPS gps(PA_2, PA_3);
    im920.printf("TXDA GPS Start\r\n");
    /* 1秒ごとに現在地を取得してターミナル出力 */
    if(gps.getgps()){ //現在地取得
        printf("TXDA (%lf, %lf)\r\n", gps.latitude, gps.longitude);//緯度と経度を出力
    }
    else{
        printf("TXDA No data\r\n");//データ取得に失敗した場合
    }
}

