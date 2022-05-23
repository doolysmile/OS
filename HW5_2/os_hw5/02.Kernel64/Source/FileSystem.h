/**
 *  file    FileSystem.h
 *  date    2009/05/01
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief  ���� �ý��ۿ� ���õ� ��� ����
 */

#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "Types.h"
#include "Synchronization.h"
#include "HardDisk.h"
#include "CacheManager.h"

////////////////////////////////////////////////////////////////////////////////
//
// ��ũ�ο� �Լ� ������
//
////////////////////////////////////////////////////////////////////////////////
// MINT ���� �ý��� �ñ׳�ó(Signature)
#define FILESYSTEM_SIGNATURE                0x7E38CF10
// Ŭ�������� ũ��(���� ��), 4Kbyte
#define FILESYSTEM_SECTORSPERCLUSTER        8
// ���� Ŭ�������� ������ ǥ��
#define FILESYSTEM_LASTCLUSTER              0xFFFFFFFF
// �� Ŭ������ ǥ��
#define FILESYSTEM_FREECLUSTER              0x00
// ��Ʈ ���͸��� �ִ� �ִ� ���͸� ��Ʈ���� ��
#define FILESYSTEM_MAXDIRECTORYENTRYCOUNT   ( ( FILESYSTEM_SECTORSPERCLUSTER * 512 ) / \
        sizeof( DIRECTORYENTRY ) )
// Ŭ�������� ũ��(����Ʈ ��)
#define FILESYSTEM_CLUSTERSIZE              ( FILESYSTEM_SECTORSPERCLUSTER * 512 )

// �ڵ��� �ִ� ����, �ִ� �½�ũ ���� 3��� ����
#define FILESYSTEM_HANDLE_MAXCOUNT          ( TASK_MAXCOUNT * 3 )

// ���� �̸��� �ִ� ����
#define FILESYSTEM_MAXFILENAMELENGTH        24

// �ڵ��� Ÿ���� ����
#define FILESYSTEM_TYPE_FREE                0
#define FILESYSTEM_TYPE_FILE                1
#define FILESYSTEM_TYPE_DIRECTORY           2

// SEEK �ɼ� ����
#define FILESYSTEM_SEEK_SET                 0
#define FILESYSTEM_SEEK_CUR                 1
#define FILESYSTEM_SEEK_END                 2

// �ϵ� ��ũ ��� ���õ� �Լ� ������ Ÿ�� ����
typedef BOOL (* fReadHDDInformation ) ( BOOL bPrimary, BOOL bMaster, 
        HDDINFORMATION* pstHDDInformation );
typedef int (* fReadHDDSector ) ( BOOL bPrimary, BOOL bMaster, DWORD dwLBA, 
        int iSectorCount, char* pcBuffer );
typedef int (* fWriteHDDSector ) ( BOOL bPrimary, BOOL bMaster, DWORD dwLBA, 
        int iSectorCount, char* pcBuffer );

// MINT ���� �ý��� �Լ��� ǥ�� ����� �Լ� �̸����� ������
#define fopen       kOpenFile
#define fread       kReadFile
#define fwrite      kWriteFile
#define fseek       kSeekFile
#define fclose      kCloseFile
#define remove      kRemoveFile
#define opendir     kOpenDirectory
#define readdir     kReadDirectory
#define rewinddir   kRewindDirectory
#define closedir    kCloseDirectory

// MINT ���� �ý��� ��ũ�θ� ǥ�� ������� ��ũ�θ� ������
#define SEEK_SET    FILESYSTEM_SEEK_SET
#define SEEK_CUR    FILESYSTEM_SEEK_CUR
#define SEEK_END    FILESYSTEM_SEEK_END

// MINT ���� �ý��� Ÿ�԰� �ʵ带 ǥ�� ������� Ÿ������ ������
#define size_t      DWORD       
#define dirent      kDirectoryEntryStruct
#define d_name      vcFileName

////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
// 1����Ʈ�� ����
#pragma pack( push, 1 )

// ��Ƽ�� �ڷᱸ��
typedef struct kPartitionStruct
{
    // ���� ���� �÷���. 0x80�̸� ���� ������ ��Ÿ���� 0x00�� ���� �Ұ�
    BYTE bBootableFlag;
    // ��Ƽ���� ���� ��巹��. ����� ���� ������� ������ �Ʒ��� LBA ��巹���� ��� ���
    BYTE vbStartingCHSAddress[ 3 ];
    // ��Ƽ�� Ÿ��
    BYTE bPartitionType;
    // ��Ƽ���� ������ ��巹��. ����� ���� ��� �� ��
    BYTE vbEndingCHSAddress[ 3 ];
    // ��Ƽ���� ���� ��巹��. LBA ��巹���� ��Ÿ�� ��
    DWORD dwStartingLBAAddress;
    // ��Ƽ�ǿ� ���Ե� ���� ��
    DWORD dwSizeInSector;
} PARTITION;


// MBR �ڷᱸ��
typedef struct kMBRStruct
{
    // ��Ʈ �δ� �ڵ尡 ��ġ�ϴ� ����
    BYTE vbBootCode[ 430 ];

    // ���� �ý��� �ñ׳�ó, 0x7E38CF10
    DWORD dwSignature;
    // ����� ������ ���� ��
    DWORD dwReservedSectorCount;
    // Ŭ������ ��ũ ���̺� ������ ���� ��
    DWORD dwClusterLinkSectorCount;
    // Ŭ�������� ��ü ����
    DWORD dwTotalClusterCount;
    
    // ��Ƽ�� ���̺�
    PARTITION vstPartition[ 4 ];
    
    // ��Ʈ �δ� �ñ׳�ó, 0x55, 0xAA
    BYTE vbBootLoaderSignature[ 2 ];
} MBR;

// ���͸� ��Ʈ�� �ڷᱸ��
typedef struct kDirectoryEntryStruct
{
    //�������� ���丮���� ����
    int flag;
    // ���� �̸�
    char vcFileName[ FILESYSTEM_MAXFILENAMELENGTH ];
    // ������ ���� ũ��
    DWORD dwFileSize;
    // ������ �����ϴ� Ŭ������ �ε���
    DWORD dwStartClusterIndex;
    int user;
    int group;
    int other;
    BYTE bSecond, bMinute, bHour, bDayOfWeek, bDayOfMonth, bMonth;
	WORD wYear;
    
    DWORD ParentDirectoryCluserIndex;
	int ParentDirectoryEntryIndex;
	char ParentPath[100];
} DIRECTORYENTRY;

#pragma pack( pop )

typedef struct kFileHandleStruct
{
    int iDirectoryEntryOffset;
    DWORD dwFileSize;
    DWORD dwStartClusterIndex;
    DWORD dwCurrentClusterIndex;
    DWORD dwPreviousClusterIndex;
    DWORD dwCurrentOffset;
} FILEHANDLE;

typedef struct kDirectoryHandleStruct
{
    DIRECTORYENTRY* pstDirectoryBuffer;
    int iCurrentOffset;
} DIRECTORYHANDLE;

typedef struct kFileDirectoryHandleStruct
{
    BYTE bType;
    union
    {
        FILEHANDLE stFileHandle;
        DIRECTORYHANDLE stDirectoryHandle;
    };    
} FILE, DIR;

typedef struct kFileUserStruct
{
   char Id[40];
   char Passwd[50];
   int usernum;
   int Idlen, Pwdlen;
   FILEHANDLE stFileHandle;
} FUSER;

typedef struct kFileSystemManagerStruct
{
    BOOL bMounted;
    DWORD dwReservedSectorCount;
    DWORD dwClusterLinkAreaStartAddress;
    DWORD dwClusterLinkAreaSize;
    DWORD dwDataAreaStartAddress;   
    DWORD dwTotalClusterCount;
    
    // ���������� Ŭ�����͸� �Ҵ��� Ŭ������ ��ũ ���̺��� ���� �������� ����
    DWORD dwLastAllocatedClusterLinkSectorOffset;
    
    // ���� �ý��� ����ȭ ��ü
    MUTEX stMutex;    
    
    // �ڵ� Ǯ(Handle Pool)�� ��巹��
    FILE* pstHandlePool;
    
    // ĳ�ø� ����ϴ��� ����
    BOOL bCacheEnable;
} FILESYSTEMMANAGER;


////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
BOOL kInitializeFileSystem( void );
BOOL kFormat( void );
BOOL kMount( void );
BOOL kGetHDDInformation( HDDINFORMATION* pstInformation);

FUSER* kOpenUserFile( const char* pcFileName, const char* pcMode ,int count,char id[], char pwd[], int idlen, int pwdlen);
int kCloseUserFile( FUSER* pstFile );
static void kFreeUserFileHandle( FUSER* pstFile );

//  ������ �Լ�(Low Level Function)
static BOOL kReadClusterLinkTable( DWORD dwOffset, BYTE* pbBuffer );
static BOOL kWriteClusterLinkTable( DWORD dwOffset, BYTE* pbBuffer );
static BOOL kReadCluster( DWORD dwOffset, BYTE* pbBuffer );
static BOOL kWriteCluster( DWORD dwOffset, BYTE* pbBuffer );
static DWORD kFindFreeCluster( void );
static BOOL kSetClusterLinkData( DWORD dwClusterIndex, DWORD dwData );
static BOOL kGetClusterLinkData( DWORD dwClusterIndex, DWORD* pdwData );
static int kFindFreeDirectoryEntry( void );
static BOOL kSetDirectoryEntryData( int iIndex, DIRECTORYENTRY* pstEntry );
BOOL kGetDirectoryEntryData( int iIndex, DIRECTORYENTRY* pstEntry );
int kFindDirectoryEntry( const char* pcFileName, DIRECTORYENTRY* pstEntry );
void kGetFileSystemInformation( FILESYSTEMMANAGER* pstManager );

// ĳ�� ���� �Լ�
static BOOL kInternalReadClusterLinkTableWithoutCache( DWORD dwOffset, 
        BYTE* pbBuffer );
static BOOL kInternalReadClusterLinkTableWithCache( DWORD dwOffset, 
        BYTE* pbBuffer );
static BOOL kInternalWriteClusterLinkTableWithoutCache( DWORD dwOffset, 
        BYTE* pbBuffer );
static BOOL kInternalWriteClusterLinkTableWithCache( DWORD dwOffset, 
        BYTE* pbBuffer );
static BOOL kInternalReadClusterWithoutCache( DWORD dwOffset, BYTE* pbBuffer );
static BOOL kInternalReadClusterWithCache( DWORD dwOffset, BYTE* pbBuffer );
static BOOL kInternalWriteClusterWithoutCache( DWORD dwOffset, BYTE* pbBuffer );
static BOOL kInternalWriteClusterWithCache( DWORD dwOffset, BYTE* pbBuffer );

static CACHEBUFFER* kAllocateCacheBufferWithFlush( int iCacheTableIndex );
BOOL kFlushFileSystemCache( void );

//  ������ �Լ�(High Level Function)
FILE* kOpenFile( const char* pcFileName, const char* pcMode, int ParentEntryIndex );
DWORD kReadFile( void* pvBuffer, DWORD dwSize, DWORD dwCount, FILE* pstFile );
DWORD kWriteFile( const void* pvBuffer, DWORD dwSize, DWORD dwCount, FILE* pstFile );
int kSeekFile( FILE* pstFile, int iOffset, int iOrigin );
int kCloseFile( FILE* pstFile );
int kRemoveFile( const char* pcFileName );
DIR* kOpenDirectory( const char* pcDirectoryName, int currentDirectoryEntryIndex );
struct kDirectoryEntryStruct* kReadDirectory( DIR* pstDirectory );
void kRewindDirectory( DIR* pstDirectory );
int kCloseDirectory( DIR* pstDirectory );
BOOL kWriteZero( FILE* pstFile, DWORD dwCount );
BOOL kIsFileOpened( const DIRECTORYENTRY* pstEntry );

static void* kAllocateFileDirectoryHandle( void );
static void kFreeFileDirectoryHandle( FILE* pstFile );
static BOOL kCreateFile( const char* pcFileName, DIRECTORYENTRY* pstEntry, 
        int* piDirectoryEntryIndex, int ParentEntryIndex );
static BOOL kFreeClusterUntilEnd( DWORD dwClusterIndex );
static BOOL kUpdateDirectoryEntry( FILEHANDLE* pstFileHandle );

static BOOL kCreateDirectory( const char* pcFileName, DIRECTORYENTRY* pstEntry, 
        int* piDirectoryEntryIndex, int ParentEntryIndex );
void kSetDotInDirectory();
DIRECTORYENTRY* kFindDirectory( int iIndex );
BOOL kUpdateDirectory( int piDirectoryEntryIndex,const char* fileName,const char* parentPath, int parentIndex );
void kUpdateFileMod(DIRECTORYENTRY pstEntry,int u_mod, int g_mod, int o_mod,int index);
void kSetEntryIndex(DWORD currentDirectoryEntryIndex);
#endif /*__FILESYSTEM_H__*/