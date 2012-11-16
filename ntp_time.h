#ifndef _NTP_TIME_H_
#define _NTP_TIME_H_
/*
* http://wiki.livedoor.jp/yamamaya_com/d/UDP%A5%AF%A5%E9%A5%A4%A5%A2%A5%F3%A5%C8
* やまねこのマイコン実験室
* UDPクライアント
*/
#include <rxduino.h>

#define NTP_SERVER    IPADDR( 210, 173, 160, 57 ) /* NTPサーバ */
#define NTP_PORT      123 /* NTPポート番号 */
#define BUFFSIZE      1500

#define LEAPS_THRU_END_OF(y)  ( (y)/4 - (y)/100 + (y)/400 )
#define IS_LEAP(y)            ( (y)/4 == 0 && ( (y)/400 == 0 || (y)/100 != 0 ) ? 1 : 0 )

/* 時刻構造体 */
struct time {
    int year ;
    int month ;
    int day ;
    int wday ;
    int hour ;
    int minute ;
    int second ;
} ;

static unsigned int mon_days[2][13] = {
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
} ;

/* リクエストパケット */
static unsigned char req_packet[] = {
    0xd9, 0x00, 0x0a, 0xf5, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x83, 0xaa, 0x7e, 0x80, 0x00, 0x00, 0x00, 0x00
} ;
/* 32ビット固定小数点小数部取得 */
static float fixp32( unsigned char *p );
/* NTP時刻⇒時刻構造体変換 */
struct time *conv_ntpdate( struct time *tm, unsigned int t );
/* 時刻構造体⇒YYYY/MM/DD hh/mm/ss文字列変換 */
char *strtime( char *buff, struct time *tm ) ;

#endif _NTP_TIME_H_
