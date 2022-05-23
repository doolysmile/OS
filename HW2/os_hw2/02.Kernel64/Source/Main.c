/**
* read only�� ���������� �Ǵ°� Ȯ���ϴ� �ڵ尡 ���ԵǾ� �ִ�. 
* �ּ� test1, test2, test3 �ϴ��� ������ �ּ� �����ϸ� test �غ� �� �ִ�. 
*/

#include "Types.h"
// �Լ� ����
void kPrintString( int iX, int iY, const char* pcString );
BOOL ReadTest_1fe000();
BOOL WriteTest_1fe000();
BOOL ReadTest_1ff000();
//test1
// BOOL WriteTest_1ff000();
/**
 *  �Ʒ� �Լ��� C ��� Ŀ���� ���� �κ���
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
 *  ���ڿ��� X, Y ��ġ�� ���
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xAB8000;
    int i;
    
    // X, Y ��ǥ�� �̿��ؼ� ���ڿ��� ����� ��巹���� ���
    pstScreen += ( iY * 80 ) + iX;

    // NULL�� ���� ������ ���ڿ� ���
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}
//read ������ True 0x1fe000�� read and write ����
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
//write ������ True 0x1fe000�� read and write ����
BOOL WriteTest_1fe000( void )
{
    DWORD* pdwCurrentAddress = ( DWORD* ) 0x1fe000;
    *pdwCurrentAddress =0x12345678;
    
    if(*pdwCurrentAddress !=0x12345678){
        return FALSE;
    }
    return TRUE;
}
//read ������ True 0x1ff000�� read only ����
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
//write ������ True 0x1ff000�� read only ����
// BOOL WriteTest_1ff000( void )
// {
//     DWORD* pdwCurrentAddress = ( DWORD* ) 0x1ff000;
//     *pdwCurrentAddress =0x12345678;
    
//     if(*pdwCurrentAddress !=0x12345678){
//         return FALSE;
//     }
//     return TRUE;
// }