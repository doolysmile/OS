/**
 *  file    InterruptHandler.c
 *  date    2009/01/24
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   인터럽트 및 예외 핸들러에 관련된 소스 파일
 */

#include "InterruptHandler.h"
#include "PIC.h"
#include "Keyboard.h"
#include "Console.h"
#include "Page.h"

/**
 *  공통으로 사용하는 예외 핸들러
 */
void kCommonExceptionHandler( int iVectorNumber, QWORD qwErrorCode )
{
    char vcBuffer[ 3 ] = { 0, };
    int iCursorX, iCursorY;
    // 인터럽트 벡터를 화면 오른쪽 위에 2자리 정수로 출력
    vcBuffer[ 0 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 1 ] = '0' + iVectorNumber % 10;
    
    // kPrintStringXY( 0, 0, "====================================================" );
    // kPrintStringXY( 0, 1, "                 Exception Occur~!!!!               " );
    // kPrintStringXY( 0, 2, "                    Vector:                         " );
    // kPrintStringXY( 27, 2, vcBuffer );
    // kPrintStringXY( 0, 3, "====================================================" );
    kGetCursor( &iCursorX, &iCursorY );
    kPrintf("====================================================\n" );
    kPrintf("                 Exception Occur~!!!!               \n" );
    kPrintf("                    Vector: %c%c                    \n", vcBuffer[0], vcBuffer[1] );
    kPrintf("====================================================" );
    while( 1 ) ;
}
/**
 *  for paging exception 인터럽트 핸들러
 */

void kPagingInterruptHandler(int faultAdress, QWORD qwErrorCode)
{
	int page_mask = 0x00000001;
	int protection_mask = 0x00000002;
	if((qwErrorCode & page_mask) == 0 )
	{
		kPageFaultExceptionHandler(faultAdress);
	}
	else if((qwErrorCode & protection_mask) == 2)
	{
		kProtectionFaultExceptionHandler(faultAdress);
	}
    else
    {
        kRemainingInterruptHandler(faultAdress);
    }
}
void kPageFaultExceptionHandler( int faultAdress)
{
    char vcBuffer[ 7 ] = { 0, };
	int number = 0;
	int mask = 0x00F00000;
	PTENTRY* pstPTEntry;
	int iCursorX, iCursorY;

	for(int i=0;i<6;i++){
		if((number = ((faultAdress & mask)>>(20-i*4)))<=9)
			vcBuffer[ i ] = '0' + number;
		else
			vcBuffer[ i ] = 87 + number;
		mask >>= 4;
	}
    kPrintf("====================================================\n" );
    kPrintf("                 Page Fault Occur~!!!!               \n" );
    kGetCursor( &iCursorX, &iCursorY );
    kPrintf("                    Address: 0x                   ");
    kSetCursor( 31, iCursorY++);
    kPrintf(vcBuffer);
    kPrintf("\n====================================================\n" );

	pstPTEntry = (PTENTRY*)0x142000;
	kSetPageEntryData(&(pstPTEntry[511]),0,0x1FF000,0x00000003,0);
	invlpg(&faultAdress);
    kSendEOIToPIC( faultAdress - PIC_IRQSTARTVECTOR );
}
void kProtectionFaultExceptionHandler( int faultAdress)
{
    char vcBuffer[ 7 ] = { 0, };
	int number = 0;
	int mask = 0x00F00000;
	PTENTRY* pstPTEntry;
	int iCursorX, iCursorY;

	for(int i=0;i<6;i++){
		if((number = ((faultAdress & mask)>>(20-i*4)))<=9)
			vcBuffer[ i ] = '0' + number;
		else
			vcBuffer[ i ] = 87 + number;
		mask >>= 4;
	}
    
    kPrintf("====================================================\n" );
    kPrintf("                 Protection Fault Occur~!               \n" );
    kGetCursor( &iCursorX, &iCursorY );
    kPrintf("                    Address: 0x                   ");
    kSetCursor( 31, iCursorY++);
    kPrintf(vcBuffer);
    kPrintf("\n====================================================\n" );

	pstPTEntry = (PTENTRY*)0x142000;
	kSetPageEntryData(&(pstPTEntry[511]),0,0x1FF000,0x00000003,0);
	invlpg(&faultAdress);
    kSendEOIToPIC( faultAdress - PIC_IRQSTARTVECTOR );
}
void kRemainingInterruptHandler( int faultAdress)
{
    char vcBuffer[ 7 ] = { 0, };
	int number = 0;
	int mask = 0x00F00000;
	PTENTRY* pstPTEntry;
	int iCursorX, iCursorY;

	for(int i=0;i<6;i++){
		if((number = ((faultAdress & mask)>>(20-i*4)))<=9)
			vcBuffer[ i ] = '0' + number;
		else
			vcBuffer[ i ] = 87 + number;
		mask >>= 4;
	}
    kPrintf("====================================================\n" );
    kPrintf("                 Fault Occur~!!!!               \n" );
    kGetCursor( &iCursorX, &iCursorY );
    kPrintf("                    Address: 0x                   ");
    kSetCursor( 31, iCursorY++);
    kPrintf(vcBuffer);
    kPrintf("\n====================================================\n" );

    kSendEOIToPIC( faultAdress - PIC_IRQSTARTVECTOR );
}
void kSetPageEntryData( PTENTRY* pstEntry, DWORD dwUpperBaseAddress, 
        DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags )
{
    pstEntry->dwAttributeAndLowerBaseAddress = dwLowerBaseAddress | dwLowerFlags;
    pstEntry->dwUpperBaseAddressAndEXB = ( dwUpperBaseAddress & 0xFF ) |
        dwUpperFlags;
}
/**
 *  공통으로 사용하는 인터럽트 핸들러
 */
void kCommonInterruptHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iCommonInterruptCount = 0;

    //=========================================================================
    // 인터럽트가 발생했음을 알리려고 메시지를 출력하는 부분
    // 인터럽트 벡터를 화면 오른쪽 위에 2자리 정수로 출력
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // 발생한 횟수 출력
    vcBuffer[ 8 ] = '0' + g_iCommonInterruptCount;
    g_iCommonInterruptCount = ( g_iCommonInterruptCount + 1 ) % 10;
    kPrintStringXY( 70, 0, vcBuffer );
    //=========================================================================
    
    // EOI 전송
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

/**
 *  키보드 인터럽트의 핸들러
 */
void kKeyboardHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iKeyboardInterruptCount = 0;
    BYTE bTemp;

    //=========================================================================
    // 인터럽트가 발생했음을 알리려고 메시지를 출력하는 부분
    // 인터럽트 벡터를 화면 왼쪽 위에 2자리 정수로 출력
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // 발생한 횟수 출력
    vcBuffer[ 8 ] = '0' + g_iKeyboardInterruptCount;
    g_iKeyboardInterruptCount = ( g_iKeyboardInterruptCount + 1 ) % 10;
    kPrintStringXY( 0, 0, vcBuffer );
    //=========================================================================

    // 키보드 컨트롤러에서 데이터를 읽어서 ASCII로 변환하여 큐에 삽입
    if( kIsOutputBufferFull() == TRUE )
    {
        bTemp = kGetKeyboardScanCode();
        kConvertScanCodeAndPutQueue( bTemp );
    }

    // EOI 전송
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

static inline void invlpg(void *m)
{   
	asm volatile("invlpg (%0)" :  : "b"(m) : "memory");
}
