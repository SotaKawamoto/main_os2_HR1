//mbed-os-6
#include "mbed.h"

Serial pc(USBTX,USBRX,9600);//main - pc USB
Serial im920(PA_9,PA_10,19200);//main im - PC im//mainのUart tx,rxピンにつなぐ
//https://forums.mbed.com/t/no-member-named-printf-in-mbed-bufferserial/13574 

Ticker status;
Ticker flightpin;
Ticker solenoid;
Ticker buzzer;
char str[1000];

int getmode();
void sendstatus();
void flightpinDriver();
void solenoid_on();
void buzzer_on();

DigitalIn flightPin(PA_12);
DigitalOut Solenoid(PB_6);
DigitalOut Buzzer(PB_1);

 //設定//

    int mode = 0;

///////

int main()
{

    wait_us(5000000);//5秒
    status.attach(sendstatus,5);//これちゃんと5秒毎になってる？

    //センサの割込み..一定時間ごとにの設定
    flightpin.attach(flightpinDriver,0.5);//数字はなぜ0.5秒刻みにするのか

    //ループさせて、pcと送受信し続ける?受信？送受信？
    while(1){
        int temp = getmode();
        if(temp != 0){//modeが0から変わればmodeを変更し、modeをPC側に送信する
            mode = temp;
            sendstatus();
        }
    }

}

int getmode(){

    char temp;
    int i;
    
    while(temp != '\n'){//1サイクル目でwhileに入って改行までループ
    //これimが読み込めるようになるまで、mainでループし続ける気がする.割り込むからいいのか？
    //モードが増えたらその都度追加する
        if(im920.readable()){
            temp=im920.getc();
            str[i]=temp;
            i++;

            if((str[i-2]==0)&&(str[i-1]==1)){
                return 1;//1モードへ変更の指示
            }
        }
    }

    return 0;//0モードへの変更指示
}

void sendstatus(){
    im920.printf("TXDA %d",mode);
    im920.printf("\r\n");
}

void flightpinDriver(){

    if(flightPin == 1){
        im920.printf("TXDA flightPinDriverworked");
        flightpin.detach();
        solenoid.attach(solenoid_on,3.6);
        //フライトピンは発射検知か、、で3.6秒はロケットが頂点に達しているだろう、かつ、センサが反応しきれていない時のバックアップ頂点検知として働いている。
        //実際は加速度と気圧と高度でも頂点検知が出来るはずだから、それらのセンサの情報をsubから送ってもらう関数を入れる必要がある。
        /*
        while(sensordeta =! true){
            if(sesordeta <= threshold){
                solenoid.detach();//いらない？上とそろえる用
                soleonoi_on();
            }
        }
        みたいな感じのが必要となるんじゃないかな
        */
    }

}

void solenoid_on(){
    Solenoid = 1;
    wait_us(5000000);//5秒間電圧ソレノイドにかける。これ何秒間くらい電圧かけて良い？？
    Solenoid = 0;
    solenoid.detach();
    buzzer.attach(buzzer_on,3.0);//パラシュート展開してから、3秒後にブザーを鳴らし始めるとする
}

void buzzer_on(){
    while(1){
        Buzzer = 1;
    }
}
