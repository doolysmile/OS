/**
* read only가 정상적으로 되는가 확인하는 코드가 포함되어 있다. 
* 주석 test1, test2, test3 하단의 내용을 주석 해제하면 test 해볼 수 있다. 
*/

#include "Types.h"
// 함수 선언
void kPrintString( int iX, int iY, const char* pcString );
BOOL ReadTest_1fe000();
BOOL WriteTest_1fe000();
BOOL ReadTest_1ff000();
//test1
// BOOL WriteTest_1ff000();
/**
 *  아래 함수는 C 언어 커널의 시작 부분임
 */
void Main( void )
{
    kPrintString( 0, 12, "Switch To IA-32e Mode Success~!!" );
    kPrintString( 0, 13, "IA-32e C Language Kernel Start..............[Pass]" );

    kPrintString( 0, 14, "This message is printed through the video memory relocated to 0xAB8000" );

    kPrintString( 0, 15, "Read from 0x1fe000 [No]" );
    if(ReadTest_1fe000())
        kPrintString( 20, 15, "Ok");
    
    kPrintString( 0, 16, "Write to 0x1fe000  [No]" );
    if(WriteTest_1fe000())
        kPrintString( 20, 16, "Ok");

    kPrintString( 0, 17, "Read from 0x1ff000 [No]" );
    if(ReadTest_1ff000())
        kPrintString( 20, 17, "Ok");
    //test2
    // kPrintString( 0, 18, "Write to 0x1ff000  [No]" );
    // if(WriteTest_1ff000())
    //     kPrintString( 20, 18, "Ok");
}

/**
 *  문자열을 X, Y 위치에 출력
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xAB8000;
    int i;
    
    // X, Y 좌표를 이용해서 문자열을 출력할 어드레스를 계산
    pstScreen += ( iY * 80 ) + iX;

    // NULL이 나올 때까지 문자열 출력
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}
//read 성공시 True 0x1fe000은 read and write 영역
BOOL ReadTest_1fe000( void )
{
    DWORD testValue = 0xFF;

    DWORD* pdwCurrentAddress = ( DWORD* ) 0x1fe000;
    
    testValue = *pdwCurrentAddress;

    if(testValue != 0xFF)
    {
        return TRUE;
    }
    return FALSE;
}
//write 성공시 True 0x1fe000은 read and write 영역
BOOL WriteTest_1fe000( void )
{
    DWORD* pdwCurrentAddress = ( DWORD* ) 0x1fe000;
    *pdwCurrentAddress =0x12345678;
    
    if(*pdwCurrentAddress !=0x12345678){
        return FALSE;
    }
    return TRUE;
}
//read 성공시 True 0x1ff000은 read only 영역
BOOL ReadTest_1ff000( void )
{
    DWORD testValue = 0xFF;

    DWORD* pdwCurrentAddress = ( DWORD* ) 0x1ff000;
    
    testValue = *pdwCurrentAddress;

    if(testValue != 0xFF)
    {
        return TRUE;
    }
    return FALSE;
}

//test3
//write 성공시 True 0x1ff000은 read only 영역
// BOOL WriteTest_1ff000( void )
// {
//     DWORD* pdwCurrentAddress = ( DWORD* ) 0x1ff000;
//     *pdwCurrentAddress =0x12345678;
    
//     if(*pdwCurrentAddress !=0x12345678){
//         return FALSE;
//     }
//     return TRUE;
// }