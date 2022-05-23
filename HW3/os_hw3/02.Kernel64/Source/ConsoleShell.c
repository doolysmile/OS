
#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"

// Ŀ�ǵ� ���̺� ����
SHELLCOMMANDENTRY gs_vstCommandTable[] =
{
        { "help", "Show Help", kHelp },
        { "cls", "Clear Screen", kCls },
        { "totalram", "Show Total RAM Size", kShowTotalRAMSize },
        { "strtod", "String To Decial/Hex Convert", kStringToDecimalHexTest },
        { "shutdown", "Shutdown And Reboot OS", kShutdown },
        { "history", "Show history", kHistory}, 
        { "raisefault", "Raisefault", kRaiseFault},
        { "dummyabb", "dummyabb", kDummy},
        { "dummyacc", "dummyacc", kDummy}, 
        { "dummyb", "dummyb", kDummy},
        { "dummyc", "dummyc", kDummy},

};       

char log[10][100];
char tablog[5][100];
char* commandp;
int history = 0;
//==============================================================================
//  ���� ���� �����ϴ� �ڵ�
//==============================================================================
/**
 *  ���� ���� ����
 */
void kStartConsoleShell( void )
{
    BYTE		vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
	BYTE 		bKey;
	int			iCommandBufferIndex = 0;
    int			iCursorX, iCursorY;
	int	 		j=0, CommandTableCnt=0, TabCnt=0, CmdCnt = 0, TabCheck = 0;
	int			logcur=-1, logcnt=0;
	int			klogIndex=0, kUpCheck=0, kDownCheck=0, kSB = 0;
	CHARACTER*	pstScreen = (CHARACTER*)CONSOLE_VIDEOMEMORYADDRESS;

    kPrintf( CONSOLESHELL_PROMPTMESSAGE );//print prompt(MINT64>)
	while( 1 )
    {
        bKey = kGetCh();

        if ((TabCnt == 1) && (bKey != KEY_TAB))
			InitTabValue(tablog, &CmdCnt, &TabCnt);

        if( bKey == KEY_BACKSPACE )//backspace
        {
            if( iCommandBufferIndex > 0 )
            {
				iCommandBufferIndex--;
				vcCommandBuffer[iCommandBufferIndex] = '\0';
                kGetCursor( &iCursorX, &iCursorY );
				kSetCursor(iCursorX-1, iCursorY);
				kPrintStringXY(iCursorX-1, iCursorY, " ");
            }
        }
        else if( bKey == KEY_ENTER )//enter
        {
            kPrintf( "\n" );
            
			if(iCommandBufferIndex > 0)
			{
            	vcCommandBuffer[ iCommandBufferIndex ] = '\0';

				logcnt++;
				if(logcnt > 10 && logcnt % 10 == 1) logcur = -1;
				    logcur++;

                kMemSet(log[logcur], '\0', kStrLen(log[logcur]));

				for(int i=0; i<iCommandBufferIndex; i++)
					log[logcur][i] = vcCommandBuffer[i];
			
				klogIndex = logcur+1;
				kUpCheck = 0;
				kDownCheck = 0;
                kSB = 0;
                history++;
                kExecuteCommand( vcCommandBuffer );//execute commmand
			}
			kPrintf( "%s", CONSOLESHELL_PROMPTMESSAGE );            
            kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
			iCommandBufferIndex = 0;
        }
		//ignore Shift, NumLock, Caps Lock, Scroll Lock
        else if( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
                 ( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
                 ( bKey == KEY_SCROLLLOCK ) ) {;}
        else if(bKey == KEY_UP || bKey == KEY_DOWN)
		{
			if(bKey == KEY_UP)
			{
				klogIndex--;
                if(kSB == 1){
                    kSB = 0;
                    klogIndex++;
                }
				else if(logcnt < 10 && klogIndex < 0) klogIndex = 0;
				else if(logcnt % 10 == 0 && klogIndex < 0) klogIndex = 0;
				else if(logcnt % 10 != 0)
				{
					if(klogIndex < 0) klogIndex = 9;
					else if(klogIndex == logcur && kUpCheck == 1) klogIndex++;
				}
				kUpCheck = 1;
				commandp = log[klogIndex];
			}
			else
			{
				klogIndex++;
				if (klogIndex-logcur == 2 && kDownCheck == 0){
                    klogIndex = logcur;
                    commandp = "";
                }
				else if(klogIndex-logcur == 1){ 
                    klogIndex--;
                    kSB = 1;
                    commandp = "";
                }
				else if(klogIndex == 10 && logcnt > 10){
                    klogIndex = 0;
                    commandp = log[klogIndex];
                }
                else
				    commandp = log[klogIndex];
                
			}
			kDownCheck = 1;
			kGetCursor(&iCursorX, &iCursorY);
			for(int i=7; i < CONSOLE_WIDTH; i++)
			{
				pstScreen[(iCursorY)*CONSOLE_WIDTH+i].bCharactor = ' ';
				pstScreen[(iCursorY)*CONSOLE_WIDTH+i].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
			}
			kSetCursor(7, iCursorY);
			kPrintf("%s", commandp);
			j = 0;
			while(commandp[j] != '\0')
			{
				vcCommandBuffer[j] = commandp[j];
				j++;
			}
			iCommandBufferIndex = j;
		}
        else if(bKey==KEY_RIGHT)
        {
			kGetCursor( &iCursorX, &iCursorY);
			if (iCommandBufferIndex+7 != iCursorX)
				kSetCursor(iCursorX+1, iCursorY);
	
		}
		else if (bKey == KEY_LEFT)
		{
			kGetCursor(&iCursorX, &iCursorY);
			if(7<iCursorX)
				kSetCursor(iCursorX-1, iCursorY);
		}
		else if( bKey == KEY_TAB )//tab
		{	
			TabCnt++;

			if(TabCnt == 1)
			{
				if(iCommandBufferIndex > 0)
				{
				 	CommandTableCnt = sizeof(gs_vstCommandTable) / sizeof(SHELLCOMMANDENTRY);
		       		for(int i=0; i<CommandTableCnt; i++)
					{
						if(kMemCmp(gs_vstCommandTable[i].pcCommand, vcCommandBuffer, iCommandBufferIndex)==0)
						{		
							kMemCpy(tablog[CmdCnt],gs_vstCommandTable[i].pcCommand,kStrLen(gs_vstCommandTable[i].pcCommand));
							CmdCnt++;
						}
					}
                    TabCheck = 0;
                    j = 0;
                    while(j < kStrLen(tablog[0]))
                    {
                        for(int i=0; i<CmdCnt; i++)
                            if(tablog[i][j] != tablog[0][j])
                            {
                                TabCheck = 1;
                                break;
                            }
                        if(TabCheck == 1)
                            break;
                        j++;
                    }
                    if(CmdCnt == 1)
                    {
                        TabCheck = 1;
                        j = kStrLen(tablog[0]);
                    }
					//if(CmdCnt == 1)
					if(TabCheck == 1)
                    {
						//j = 0;
						//while (tablog[0][j]!='\0')
						//{
						//	vcCommandBuffer[j] = tablog[0][j];
						//	j++;
						//}
                        for(int i=0; i<j; i++)
                            vcCommandBuffer[i] = tablog[0][i];
						iCommandBufferIndex = j;				
						kGetCursor(&iCursorX, &iCursorY);
						kSetCursor(7, iCursorY);
						kPrintf("%s",vcCommandBuffer);
					}
				}
			} 	//tabcnt = 1
			else
			{
				j = 0;
				while (j<CmdCnt)
				{
					if(kMemCmp(vcCommandBuffer, tablog[j],kStrLen(tablog[j]))==0)
						break;
					j++;
				}
				if(j == CmdCnt-1)//completed command
				{
					InitTabValue(tablog, &CmdCnt, &TabCnt);
					/*for(int i=0; i<CmdCnt; i++)
						kMemSet(tablog[i], '\0', kStrLen(tablog[i]));
					TabCnt = 0;
					CmdCnt = 0;*/
					continue;
				}
				if(iCommandBufferIndex > 0)
				{
					for (int i = 0; i<CmdCnt;i++)
						kPrintf("\n%s",tablog[i]);
					if(CmdCnt > 1)
					    kPrintf("\n%s%s", CONSOLESHELL_PROMPTMESSAGE, vcCommandBuffer);
					/*for(int i=0; i<CmdCnt; i++)
						kMemSet(tablog[i], '\0', kStrLen(tablog[i]));
					CmdCnt = 0;
					TabCnt = 0;*/
					InitTabValue(tablog, &CmdCnt, &TabCnt);
				}
			}
		}
		else
		{
            if( iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT )
            {
                vcCommandBuffer[ iCommandBufferIndex++ ] = bKey;
                kPrintf( "%c", bKey );
            }
        }
    }
}

void InitTabValue(char (*tablog)[100], int* CmdCnt, int* TabCnt)
{
	for(int i=0; i<*CmdCnt; i++)
		kMemSet(tablog[i], '\0', kStrLen(tablog[i]));
	*CmdCnt = 0;
	*TabCnt = 0;
}
/*
 *  Ŀ�ǵ� ���ۿ� �ִ� Ŀ�ǵ带 ���Ͽ� �ش� Ŀ�ǵ带 ó���ϴ� �Լ��� ����
 */
void kExecuteCommand( const char* pcCommandBuffer )
{
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;
    
    // �������� ���е� Ŀ�ǵ带 ����
    iCommandBufferLength = kStrLen( pcCommandBuffer );
    for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
    {
        if( pcCommandBuffer[ iSpaceIndex ] == ' ' )
        {
            break;
        }
    }
    
    // Ŀ�ǵ� ���̺��� �˻��ؼ� ������ �̸��� Ŀ�ǵ尡 �ִ��� Ȯ��
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iCommandLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        // Ŀ�ǵ��� ���̿� ������ ������ ��ġ�ϴ��� �˻�
        if( ( iCommandLength == iSpaceIndex ) &&
            ( kMemCmp( gs_vstCommandTable[ i ].pcCommand, pcCommandBuffer,
                       iSpaceIndex ) == 0 ) )
        {
            gs_vstCommandTable[ i ].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
            break;
        }
    }

    // ����Ʈ���� ã�� �� ���ٸ� ���� ���
    if( i >= iCount )
    {
        kPrintf( "'%s' is not found.\n", pcCommandBuffer );
    }
}

/**
 *  �Ķ���� �ڷᱸ���� �ʱ�ȭ
 */
void kInitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

/**
 *  �������� ���е� �Ķ������ ����� ���̸� ��ȯ
 */
int kGetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    // �� �̻� �Ķ���Ͱ� ������ ����
    if( pstList->iLength <= pstList->iCurrentPosition )
    {
        return 0;
    }
    
    // ������ ���̸�ŭ �̵��ϸ鼭 ������ �˻�
    for( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if( pstList->pcBuffer[ i ] == ' ' )
        {
            break;
        }
    }
    
    // �Ķ���͸� �����ϰ� ���̸� ��ȯ
    kMemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength = i - pstList->iCurrentPosition;
    pcParameter[ iLength ] = '\0';

    // �Ķ������ ��ġ ������Ʈ
    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}
    
//==============================================================================
//  Ŀ�ǵ带 ó���ϴ� �ڵ�
//==============================================================================
/**
 *  �� ������ ���
 */
void kHelp( const char* pcCommandBuffer )
{
    int i;
    int iCount;
    int iCursorX, iCursorY;
    int iLength, iMaxCommandLength = 0;
    
    
    kPrintf( "=========================================================\n" );
    kPrintf( "                    MINT64 Shell Help                    \n" );
    kPrintf( "=========================================================\n" );
    
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );

    // ���� �� Ŀ�ǵ��� ���̸� ���
    for( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( iLength > iMaxCommandLength )
        {
            iMaxCommandLength = iLength;
        }
    }
    
    // ���� ���
    for( i = 0 ; i < iCount ; i++ )
    {
        kPrintf( "%s", gs_vstCommandTable[ i ].pcCommand );
        kGetCursor( &iCursorX, &iCursorY );
        kSetCursor( iMaxCommandLength, iCursorY );
        kPrintf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );
    }
}

/**
 *  ȭ���� ���� 
 */
void kCls( const char* pcParameterBuffer )
{
    // �� ������ ����� ������ ����ϹǷ� ȭ���� ���� ��, ���� 1�� Ŀ�� �̵�
    kClearScreen();
    kSetCursor( 0, 1 );
}

/**
 *  �� �޸� ũ�⸦ ���
 */
void kShowTotalRAMSize( const char* pcParameterBuffer )
{
    kPrintf( "Total RAM Size = %d MB\n", kGetTotalRAMSize() );
}

/**
 *  ���ڿ��� �� ���ڸ� ���ڷ� ��ȯ�Ͽ� ȭ�鿡 ���
 */
void kStringToDecimalHexTest( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    int iLength;
    PARAMETERLIST stList;
    int iCount = 0;
    long lValue;
    
    // �Ķ���� �ʱ�ȭ
    kInitializeParameter( &stList, pcParameterBuffer );
    
    while( 1 )
    {
        // ���� �Ķ���͸� ����, �Ķ������ ���̰� 0�̸� �Ķ���Ͱ� ���� ���̹Ƿ�
        // ����
        iLength = kGetNextParameter( &stList, vcParameter );
        if( iLength == 0 )
        {
            break;
        }

        // �Ķ���Ϳ� ���� ������ ����ϰ� 16�������� 10�������� �Ǵ��Ͽ� ��ȯ�� ��
        // ����� printf�� ���
        kPrintf( "Param %d = '%s', Length = %d, ", iCount + 1, 
                 vcParameter, iLength );

        // 0x�� �����ϸ� 16����, �׿ܴ� 10������ �Ǵ�
        if( kMemCmp( vcParameter, "0x", 2 ) == 0 )
        {
            lValue = kAToI( vcParameter + 2, 16 );
            kPrintf( "HEX Value = %q\n", lValue );
        }
        else
        {
            lValue = kAToI( vcParameter, 10 );
            kPrintf( "Decimal Value = %d\n", lValue );
        }
        
        iCount++;
    }
}

/**
 *  PC�� �����(Reboot)
 */
void kShutdown( const char* pcParamegerBuffer )
{
    kPrintf( "System Shutdown Start...\n" );
    
    // Ű���� ��Ʈ�ѷ��� ���� PC�� �����
    kPrintf( "Press Any Key To Reboot PC..." );
    kGetCh();
    kReboot();
}
// history
void kHistory( const char* pcParamegerBuffer )
{   
    if(history < 11){
        for(int i = 0; i < history; i++)
            kPrintf("%d %s\n",  i+1,  log[i]);
    }
    else
    {
        for(int i = 0; i < 10; i++)
            kPrintf("%d %s\n",  i+1,  log[(history+i)%10]);
    }
    
}
// fault �߻� �κ�
void kRaiseFault( const char* pcParamegerBuffer )
{
    long *ptr = (long*)0x1ff000;
	*ptr = 0;
}

void kDummy( const char* pcParamegerBuffer )
{
    kPrintf( "This is Dummy\n");
}

