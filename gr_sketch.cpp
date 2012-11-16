/*
* http://arms22.blog91.fc2.com/blog-entry-445.html
*
* http://rx.tokudenkairo.co.jp/manual/classTEthernet.html#a423bb5e61f40effba72fdee873ef084e
*/

#include <rxduino.h>
#include <ethernet.h>
TEthernet ethernet;

#include "ntp_time.h"

byte mac[] = {0x08, 0x09, 0x3c, 0x12, 0x34, 0x56};
byte ip[] = {172,20,67,52};
byte dns[]= {202,18,74,9};
byte gateway[] = {172,20,67,1};
byte subnet[] = {255,255,255,0};

#define INTERVAL 5*1000

///////////////////////////////////////
// NTPタイムサーバIPアドレス(ntp.nict.jp NTP server)
byte timeServer[] = {133, 243, 238, 164};
// NTPパケットバッファサイズ
const int NTP_PACKET_SIZE= 48;
// NTP送受信用パケットバッファ
byte packetBuffer[NTP_PACKET_SIZE];
// 最後にパケットを送信した時間（ミリ秒）
unsigned long lastSendPacketTime = 0;
// send an NTP request to the time server at the given address 
unsigned long sendNTPpacket(byte ipaddr[4])
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  // NTP requests are to port 123
  /*Udp.beginPacket(address, 123);
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();*/
  ethernet.sendUDP(ipaddr,123,packetBuffer,NTP_PACKET_SIZE);
}

void time_change_RTC(unsigned char *data)
{
    // バッファに受信データを読み込む
    //Udp.read(packetBuffer, NTP_PACKET_SIZE);

    // 時刻情報はパケットの40バイト目からはじまる4バイトのデータ
    
    //unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    //unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    //unsigned long highWord = word(byte((char)data[40]), byte((char)data[41]) );
    unsigned long highbyte0 = word((char)data[40] );
    unsigned long highbyte1 = word((char)data[41] );
    unsigned long highWord = highbyte0 << 8 | highbyte1 ;
    //unsigned long lowWord = word(byte((char)data[42]), byte((char)data[43]) );
    unsigned long lowbyte0 = word((char)data[42] );
    unsigned long lowbyte1 = word((char)data[43] );
    unsigned long lowWord = lowbyte0 << 8 | lowbyte1 ;
    // NTPタイムスタンプは64ビットの符号無し固定小数点数（整数部32ビット、小数部32ビット）
    // 1900年1月1日0時との相対的な差を秒単位で表している
    // 小数部は切り捨てて、秒を求めている
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);

    // NTPタイムスタンプをUNIXタイムに変換する
    // UNITタイムは1970年1月1日0時からはじまる
    // 1900年から1970年の70年を秒で表すと2208988800秒になる
    const unsigned long seventyYears = 2208988800UL;
    // NTPタイムスタンプから70年分の秒を引くとUNIXタイムが得られる
    //unsigned long epoch = secsSince1900 - seventyYears;  
    //Serial.print("Unix time = ");
    //Serial.println(epoch);
    
    //////////////////////////////////////////
    time tm ; 
    char tbuff[40];
    /* NTP時刻変換 */
    conv_ntpdate( &tm, secsSince1900 - 1*24*60*60  ) ;//1日分の謎の誤差修正＠TBT
    printf("since@1900->:%s\n",strtime(tbuff, &tm ));
    /////////////////////////////////////////////
    
/*
    // Timeライブラリに時間を設定(UNIXタイム)
    // 日本標準時にあわせるために＋9時間しておく
    
    setTime(epoch + (9 * 60 * 60));

    Serial.print("JST is ");
    Serial.print(year());
    Serial.print('/');
    Serial.print(month());
    Serial.print('/');
    Serial.print(day());
    Serial.print(' ');
    Serial.print(hour());
    Serial.print(':'); 
    Serial.print(minute());
    Serial.print(':'); 
    Serial.println(second());
    Serial.println();
*/
}
//////////////////////////////////////////////////////

BOOL onReceiveUDP( // UDPを受信したときに呼び出される関数
    unsigned char dest_ip[4],
    unsigned char src_ip[4],
    /*unsigned*/ short dest_port,
    /*unsigned*/ short src_port,
    unsigned char *data,
    short datalen)
{
    printf("UDP Received %d.%d.%d.%d (%d) =>",src_ip[0],src_ip[1],src_ip[2],src_ip[3],src_port);
    printf(" %d.%d.%d.%d (%d) ",dest_ip[0],dest_ip[1],dest_ip[2],dest_ip[3],dest_port);
    printf(" length = %d \n",datalen);
    for(int i=0;i<datalen;i++)
    {
        if((i & 15) == 0) printf("%04X ",i);
        printf("%02X ",data[i]);
        if((i & 15) == 15) printf("\n");
    }
    printf("\n");
    //
    time_change_RTC(data);
    //
    return TRUE;
}
void setup()
{
    pinMode(PIN_LED0, OUTPUT);
    Serial.begin(38400,SCI_AUTO);
    Serial.setDefault();
    setvbuf(stdout, (char *)NULL, _IONBF, 0);
    digitalWrite(PIN_LED3, 1);
    
    printf("RXduino ethernet UDP receive test program\n");
    ethernet.setHostname("test01");
    ethernet.begin(mac,ip,dns,gateway,subnet );
    printf("%s\n",ethernet.localIP());
    //
    sendNTPpacket(timeServer);
    lastSendPacketTime = millis();
    //ethernet.registUdpHandler(onReceiveUDP);
    tketh_regist_udp_handler(onReceiveUDP);          //今はこちらを使用してます
}
void loop()
{
    if ( millis() - lastSendPacketTime > INTERVAL ){
        // NTPサーバへ時刻リクエストを送信
        sendNTPpacket(timeServer);
        // 時間を更新
        lastSendPacketTime = millis();
    }
}