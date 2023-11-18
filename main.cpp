//mbed-os-2
#include "mbed.h"

Serial pc(USBTX,USBRX,9600);//main - pc USB
Serial im920(PA_9,PA_10,19200);//main im - PC im//mainのUart tx,rxピンにつなぐ
//https://forums.mbed.com/t/no-member-named-printf-in-mbed-bufferserial/13574 

Ticker status;
Ticker flightpin;
int temp1;
char temp2;
char str[100];
char str2[100];
int a;

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
    im920.printf("TXDA  10秒毎のロケットの状態送信開始\r\n");
    wait_us(1000000);
    status.attach(sendstatus,5);
    //im920.printfをこの下に置くと誤字が出る これ回し続けたらteratermでコマンドを送れなくないか？？teratermが分かってない
    //センサの割込み..一定時間ごとにの設定

    //ループさせて、pcと送受信し続ける?受信？送受信？
    while(1){
        im920.printf("TXDA  modeを入手して送信し返すループ開始\r\n");
        wait_us(1000000);
        int temp1 = getmode();
        im920.printf("TXDA %d\r\n",temp1);
        if(temp1 != 0){//mode変更のコマンドが送られるとmodeを変更し、現在のmodeをPC側に送信する ex)送信側で01のモード１を打ち込むとgetmode関数からreturn 1が返ってきてmodeを送り返す
            mode = temp1;
            sendstatus();
        }
    }
}

int getmode(){
        if(im920.readable()){
            i=0
            while(temp2 != '\n'){//1サイクル目でwhileに入って改行までループ
    //これimが読み込めるようになるまで、mainでループし続ける気がする.割り込むからいいのか？
        
            printf("readable");
            temp2=im920.getc();
            str[i]=temp2;
            i++;
            im920.printf("TXDA 入力された文字は%c\r\n",temp2);
            if((str[i-2]==0)&&(str[i-1]==1)){
                im920.printf("TXDA モードが01に変更されました\r\n");
                return 1;//1モードへ変更の指示
            }
            printf("readable");
        }
    }
    return 0;//0モードへの変更指示
}

void sendstatus(){
    im920.printf("TXDA　現在のmodeは%d\r\n",mode);
}
