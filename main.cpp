//mbed-os-2
#include "mbed.h"

Serial pc(USBTX,USBRX,9600);//main - pc USB
Serial im920(PA_9,PA_10,19200);//main im - PC im//mainのUart tx,rxピンにつなぐ
//https://forums.mbed.com/t/no-member-named-printf-in-mbed-bufferserial/13574 

Ticker status;
Ticker flightpin;
char str[1000];

int getmode();
void sendstatus();

 //設定//

    int mode = 0;

///////

int main()
{
    im920.printf("TXDA  開始\r\n");
    wait_us(5000000);//5秒
    im920.printf("TXDA  5秒経過\r\n");
    status.attach(sendstatus,5);//これちゃんと5秒毎になってる？
    im920.printf("TXDA  ロケットの状態送信開始\r\n");

    //センサの割込み..一定時間ごとにの設定

    //ループさせて、pcと送受信し続ける?受信？送受信？
    while(1){
        int temp = getmode();
        im920.printf("TXDA %d\r\n",temp);
        if(temp != 0){//mode変更のコマンドが送られるとmodeを変更し、現在のmodeをPC側に送信する ex)送信側で01のモード１を打ち込むとgetmode関数からreturn 1が返ってきてmodeを送り返す
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
        if(im920.readable()){
            temp=im920.getc();
            str[i]=temp;
            i++;

            if((str[i-2]==0)&&(str[i-1]==1)){
                im920.printf("TXDA モードが01に変更されました\r\n");
                return 1;//1モードへ変更の指示
            }
        }
    }

    return 0;//0モードへの変更指示
}

void sendstatus(){
    im920.printf("TXDA　モードが変更されました mode = %d",mode);
    im920.printf("\r\n");
}
