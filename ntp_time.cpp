//http://wiki.livedoor.jp/yamamaya_com/d/UDP%A5%AF%A5%E9%A5%A4%A5%A2%A5%F3%A5%C8
//やまねこのマイコン実験室
//
#include "ntp_time.h"

/* 32ビット固定小数点小数部取得 */
static float fixp32( unsigned char *p ) {
    float a ;
    a = (float)p[0] * 0x1000000 + (float)p[1] * 0x10000 + (float)p[2] * 0x100 + (float)p[3] ;
    return( a / 4294967296.0 ) ;
}

/* NTP時刻⇒時刻構造体変換 */
struct time *conv_ntpdate( struct time *tm, unsigned int t ) {
    unsigned int days ;
    int rem, i, j ;
    unsigned int *dp ;

    t += 9 * 3600 ;
    days = t / 86400 ;
    rem = t % 86400 ;
    tm->hour = rem / 3600 ;
    rem %= 3600 ;
    tm->minute = rem / 60 ;
    tm->second = rem % 60 ;
    tm->wday = ( 1 + days ) % 7 ;
    i = 1900 ;
    while ( days >= 365 + IS_LEAP( i ) ) {
        j = i + days / 365 ;
        days -= ( j - i ) * 365 + LEAPS_THRU_END_OF( j - 1 ) - LEAPS_THRU_END_OF( i - 1 ) ;
        i = j ;
    }
    tm->year = i ;
    dp = mon_days[IS_LEAP( i )] ;
    i = 0 ;
    while ( days >= dp[i] ) {
        days -= dp[i] ;
        i ++ ;
    }
    tm->month = i + 1 ;
    tm->day = days + 1 ;
    return( tm ) ;
}

/* 時刻構造体⇒YYYY/MM/DD hh/mm/ss文字列変換 */
char *strtime( char *buff, struct time *tm ) {
    sprintf( buff, "%04d-%02d-%02d %02d:%02d:%02d", tm->year, tm->month, tm->day, tm->hour, tm->minute, tm->second ) ;
    return( buff ) ;
}
