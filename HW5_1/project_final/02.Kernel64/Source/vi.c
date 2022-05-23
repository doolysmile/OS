#include "vi.h"

/*COMMANDENTRY gs_viCommandTable[] = 
{
	{ "w", "save file", kSaveFile },

};
*/

void kViEditor(int x, int y)
{
	kClearScreen();
	kSetCursor(0, 1);

	kPrintf("hellp\n");
	int i = 0;
	while(i < 100000000)
	{
		i++;
	}
	kSetCursor(x, y);
}

void kTest(int XX, int YY, char *vcFileName){
	FILE* fp;
    int iEnterCount;
    BYTE bKey;
	int iDirectoryEntryOffset;
	FILE* pstFile;
	DIRECTORYENTRY stEntry;
	iDirectoryEntryOffset = kFindDirectoryEntry( vcFileName, &stEntry );
	
	BYTE mode = VI_READ_MODE;
	int wx=0, wy=1, cx=0, cy=24, tmpx, tmpy;
	BYTE viBuffer[ VI_MAXBUFFERCOUNT ];
	int iviBufferIndex = 0;
	int break_temp = 0;
	CHARACTER* pstScreen = (CHARACTER*) CONSOLE_VIDEOMEMORYADDRESS;

	if(iDirectoryEntryOffset == -1){


		pstFile = fopen( vcFileName, "w" );
	
		if( pstFile == NULL )
		{
			kPrintf( "File Create Fail\n" );
			return;
		}
		fclose( pstFile );
    	kPrintf( "File Create Success\n" );		

	}
	
	kClearScreen();
	kSetCursor(0, 1);
    fp = fopen( vcFileName, "r" );
    if( fp == NULL )
    {
        kPrintf( "%s File Open Fail\n", vcFileName );
        return ;
    }
    iEnterCount = 0;
    while( 1 )
    {
        if( fread( &bKey, 1, 1, fp ) != 1 )
        {
            break;
        }
        kPrintf( "%c", bKey );
        
        if( bKey == KEY_ENTER )
        {
            iEnterCount++;
            
            if( ( iEnterCount != 0 ) && ( ( iEnterCount % 20 ) == 0 ) )
            {
                kPrintf( "Press any key to continue... ('q' is exit) : " );
                if( kGetCh() == 'q' )
                {
                    kPrintf( "\n" );
                    break;
                }
                kPrintf( "\n" );
                iEnterCount = 0;
            }
        }
    }
    fclose( fp );
	
	while(1)
	{
		bKey = kGetCh();

		if(bKey==KEY_LSHIFT || bKey==KEY_RSHIFT || bKey==KEY_CAPSLOCK || bKey==KEY_NUMLOCK || bKey==KEY_SCROLLLOCK)
			continue;

		if(mode == VI_READ_MODE)
		{
			for(int i=0; i<CONSOLE_WIDTH; i++)
				pstScreen[24*CONSOLE_WIDTH+i].bCharactor = ' ';

			switch(bKey)
			{
				case 'a':
					kSetCursor(++wx, wy);
					mode = VI_WRITE_MODE;
					break;
				case 'o':
					kSetCursor(0, ++wy);
					mode = VI_WRITE_MODE;
					break;
				case 'i':
					kSetCursor(wx, wy);
					mode = VI_WRITE_MODE;
					break;
				case ':':
					mode = VI_COMMAND_MODE;
					kGetCursor(&tmpx, &tmpy);
					if(tmpy != 24)
					{
						wx = tmpx;
						wy = tmpy;
					}
				//	for(int i=0; i<CONSOLE_WIDTH; i++)
				//		pstScreen[24*CONSOLE_WIDTH+i].bCharactor = ' ';
					kSetCursor(0, 24);
					kPrintf("%c", bKey);
					break;
				case 8:
				//	for(int i=0; i<CONSOLE_WIDTH; i++)
				//		pstScreen[24*CONSOLE_WIDTH+i].bCharactor = ' ';
					if(wx > 0)
						kSetCursor(--wx, wy);
					break;

			}
		}
		else if(mode == VI_COMMAND_MODE)
		{
			switch(bKey)
			{
				case 10:	//enter
					if(iviBufferIndex >0)
					{
						viBuffer [ iviBufferIndex ] = '\0';
						if(kMemCmp(viBuffer, "w", kStrLen(viBuffer)) == 0)
						{
							kSaveFile(vcFileName);
						}
						else if(kMemCmp(viBuffer, "q", kStrLen(viBuffer)) == 0)
		{
							break_temp = 1;
						}
						else
						{
							kSetCursor(0, 24);
							kPrintf("%s : not found", viBuffer);
							kSetCursor(0, 24);
						}
						kGetCursor(&cx,&cy);
					}
					kMemSet(viBuffer, '\0', VI_MAXBUFFERCOUNT);
					iviBufferIndex = 0;
					mode = VI_READ_MODE;
					break;
				case 27:	//ESC
					mode = VI_READ_MODE;
					break;	
				case 8:		//backspace
					kGetCursor(&cx, &cy);
					if(cx > 0)
					{
						viBuffer[--iviBufferIndex] = '\0';
						kSetCursor(--cx, cy);
						kPrintStringXY(cx, cy, " ");
					}
					else
					{
						kSetCursor(wx, wy);
						mode = VI_READ_MODE;
					}
					break;
				default:
						viBuffer[ iviBufferIndex++] = bKey;
						kPrintf("%c", bKey);
						break;
			}
		}
		else
		{
			switch(bKey)
			{
				case 27:	//ESC
					kGetCursor(&wx, &wy);
					mode = VI_READ_MODE;
					break;
				case 8:		//backspace
					kGetCursor(&wx, &wy);
					if(wx > 0)
					{
						kSetCursor(--wx, wy);
						kPrintStringXY(wx, wy, " ");
					}
					else
					{
						wx = 0;
						kSetCursor(wx, wy);
					}
					break;
				case 148:
					kGetCursor(&wx, &wy);
					kSetCursor(wx, --wy);
					break;
				case 154:
					kGetCursor(&wx, &wy);
					kSetCursor(wx, ++wy);
					break;
				case 150:
					kGetCursor(&wx, &wy);
					kSetCursor(--wx, wy);
					break;
				case 152:
					kGetCursor(&wx, &wy);
					kSetCursor(++wx, wy);
					break;
				default:
					kPrintf("%c", bKey);
					break;
			}
		}
		if(break_temp == 1)
		{
			break;
		}
	}
	kSetCursor(XX, YY);
}

void kSaveFile(char *vcFileName)
{
	int x, y;
	BYTE temp;
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
	kGetCursor(&x, &y);
	FILE* fp;
	fp = fopen( vcFileName, "w" );
	for(int i = 80; i < 90; i++)
	{
		temp = pstScreen[i].bCharactor;
		if( fwrite( &temp, 1, 1, fp ) != 1 )
        {
            kPrintf( "File Save Fail" );
            break;
        }
	}
	kSetCursor(0, 24);
	kPrintf("File Save Success");
	kSetCursor(0, 24);
}

