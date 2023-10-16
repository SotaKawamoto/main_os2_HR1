//mbed-os-6
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

    wait_us(5000000);//5秒
    status.attach(sendstatus,5);//これちゃんと5秒毎になってる？

    //センサの割込み..一定時間ごとにの設定

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
