/**
 *  file    FileSystem.h
 *  date    2009/05/01
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief  파일 시스템에 관련된 헤더 파일
 */

#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "Types.h"
#include "Synchronization.h"
#include "HardDisk.h"
#include "CacheManager.h"

////////////////////////////////////////////////////////////////////////////////
//
// 매크로와 함수 포인터
//
////////////////////////////////////////////////////////////////////////////////
// MINT 파일 시스템 시그너처(Signature)
#define FILESYSTEM_SIGNATURE                0x7E38CF10
// 클러스터의 크기(섹터 수), 4Kbyte
#define FILESYSTEM_SECTORSPERCLUSTER        8
// 파일 클러스터의 마지막 표시
#define FILESYSTEM_LASTCLUSTER              0xFFFFFFFF
// 빈 클러스터 표시
#define FILESYSTEM_FREECLUSTER              0x00
// 루트 디렉터리에 있는 최대 디렉터리 엔트리의 수
#define FILESYSTEM_MAXDIRECTORYENTRYCOUNT   ( ( FILESYSTEM_SECTORSPERCLUSTER * 512 ) / \
        sizeof( DIRECTORYENTRY ) )
// 클러스터의 크기(바이트 수)
#define FILESYSTEM_CLUSTERSIZE              ( FILESYSTEM_SECTORSPERCLUSTER * 512 )

// 핸들의 최대 개수, 최대 태스크 수의 3배로 생성
#define FILESYSTEM_HANDLE_MAXCOUNT          ( TASK_MAXCOUNT * 3 )

// 파일 이름의 최대 길이
#define FILESYSTEM_MAXFILENAMELENGTH        24

// 핸들의 타입을 정의
#define FILESYSTEM_TYPE_FREE                0
#define FILESYSTEM_TYPE_FILE                1
#define FILESYSTEM_TYPE_DIRECTORY           2

// SEEK 옵션 정의
#define FILESYSTEM_SEEK_SET                 0
#define FILESYSTEM_SEEK_CUR                 1
#define FILESYSTEM_SEEK_END                 2

// 하드 디스크 제어에 관련된 함수 포인터 타입 정의
typedef BOOL (* fReadHDDInformation ) ( BOOL bPrimary, BOOL bMaster, 
        HDDINFORMATION* pstHDDInformation );
typedef int (* fReadHDDSector ) ( BOOL bPrimary, BOOL bMaster, DWORD dwLBA, 
        int iSectorCount, char* pcBuffer );
typedef int (* fWriteHDDSector ) ( BOOL bPrimary, BOOL bMaster, DWORD dwLBA, 
        int iSectorCount, char* pcBuffer );

// MINT 파일 시스템 함수를 표준 입출력 함수 이름으로 재정의
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

// MINT 파일 시스템 매크로를 표준 입출력의 매크로를 재정의
#define SEEK_SET    FILESYSTEM_SEEK_SET
#define SEEK_CUR    FILESYSTEM_SEEK_CUR
#define SEEK_END    FILESYSTEM_SEEK_END

// MINT 파일 시스템 타입과 필드를 표준 입출력의 타입으로 재정의
#define size_t      DWORD       
#define dirent      kDirectoryEntryStruct
#define d_name      vcFileName

////////////////////////////////////////////////////////////////////////////////
//
// 구조체
//
////////////////////////////////////////////////////////////////////////////////
// 1바이트로 정렬
#pragma pack( push, 1 )

// 파티션 자료구조
typedef struct kPartitionStruct
{
    // 부팅 가능 플래그. 0x80이면 부팅 가능을 나타내며 0x00은 부팅 불가
    BYTE bBootableFlag;
    // 파티션의 시작 어드레스. 현재는 거의 사용하지 않으며 아래의 LBA 어드레스를 대신 사용
    BYTE vbStartingCHSAddress[ 3 ];
    // 파티션 타입
    BYTE bPartitionType;
    // 파티션의 마지막 어드레스. 현재는 거의 사용 안 함
    BYTE vbEndingCHSAddress[ 3 ];
    // 파티션의 시작 어드레스. LBA 어드레스로 나타낸 값
    DWORD dwStartingLBAAddress;
    // 파티션에 포함된 섹터 수
    DWORD dwSizeInSector;
} PARTITION;


// MBR 자료구조
typedef struct kMBRStruct
{
    // 부트 로더 코드가 위치하는 영역
    BYTE vbBootCode[ 430 ];

    // 파일 시스템 시그너처, 0x7E38CF10
    DWORD dwSignature;
    // 예약된 영역의 섹터 수
    DWORD dwReservedSectorCount;
    // 클러스터 링크 테이블 영역의 섹터 수
    DWORD dwClusterLinkSectorCount;
    // 클러스터의 전체 개수
    DWORD dwTotalClusterCount;
    
    // 파티션 테이블
    PARTITION vstPartition[ 4 ];
    
    // 부트 로더 시그너처, 0x55, 0xAA
    BYTE vbBootLoaderSignature[ 2 ];
} MBR;

// 디렉터리 엔트리 자료구조
typedef struct kDirectoryEntryStruct
{
    //파일인지 디렉토리인지 구분
    int flag;
    // 파일 이름
    char vcFileName[ FILESYSTEM_MAXFILENAMELENGTH ];
    // 파일의 실제 크기
    DWORD dwFileSize;
    // 파일이 시작하는 클러스터 인덱스
    DWORD dwStartClusterIndex;
    int user;
    int group;
    int other;
    BYTE bSecond, bMinute, bHour, bDayOfWeek, bDayOfMonth, bMonth;
	WORD wYear;
    
    //부모 디렉터리 표시
    char ParentDirectoryPath[FILESYSTEM_MAXFILENAMELENGTH];
    DWORD ParentDirectoryCluserIndex;
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
    char Name[ FILESYSTEM_MAXFILENAMELENGTH ];
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
    
    // 마지막으로 클러스터를 할당한 클러스터 링크 테이블의 섹터 오프셋을 저장
    DWORD dwLastAllocatedClusterLinkSectorOffset;
    
    // 파일 시스템 동기화 객체
    MUTEX stMutex;    
    
    // 핸들 풀(Handle Pool)의 어드레스
    FILE* pstHandlePool;
    
    // 캐시를 사용하는지 여부
    BOOL bCacheEnable;
} FILESYSTEMMANAGER;


////////////////////////////////////////////////////////////////////////////////
//
// 함수
//
////////////////////////////////////////////////////////////////////////////////
BOOL kInitializeFileSystem( void );
BOOL kFormat( void );
BOOL kMount( void );
BOOL kGetHDDInformation( HDDINFORMATION* pstInformation);

FUSER* kOpenUserFile( const char* pcFileName, const char* pcMode ,int count,char id[], char pwd[], int idlen, int pwdlen);
int kCloseUserFile( FUSER* pstFile );
static void kFreeUserFileHandle( FUSER* pstFile );

//  저수준 함수(Low Level Function)
static BOOL kReadClusterLinkTable( DWORD dwOffset, BYTE* pbBuffer );
static BOOL kWriteClusterLinkTable( DWORD dwOffset, BYTE* pbBuffer );
static BOOL kReadCluster( DWORD dwOffset, BYTE* pbBuffer );
static BOOL kWriteCluster( DWORD dwOffset, BYTE* pbBuffer );
static DWORD kFindFreeCluster( void );
static BOOL kSetClusterLinkData( DWORD dwClusterIndex, DWORD dwData );
static BOOL kGetClusterLinkData( DWORD dwClusterIndex, DWORD* pdwData );
static int kFindFreeDirectoryEntry( void );
static BOOL kSetDirectoryEntryData( int iIndex, DIRECTORYENTRY* pstEntry );
static BOOL kGetDirectoryEntryData( int iIndex, DIRECTORYENTRY* pstEntry );
int kFindDirectoryEntry( const char* pcFileName, DIRECTORYENTRY* pstEntry );
void kGetFileSystemInformation( FILESYSTEMMANAGER* pstManager );

// 캐시 관련 함수
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

//  고수준 함수(High Level Function)
FILE* kOpenFile( const char* pcFileName, const char* pcMode );
DWORD kReadFile( void* pvBuffer, DWORD dwSize, DWORD dwCount, FILE* pstFile );
DWORD kWriteFile( const void* pvBuffer, DWORD dwSize, DWORD dwCount, FILE* pstFile );
int kSeekFile( FILE* pstFile, int iOffset, int iOrigin );
int kCloseFile( FILE* pstFile );
int kRemoveFile( const char* pcFileName );
DIR* kOpenDirectory( const char* pcDirectoryName );
struct kDirectoryEntryStruct* kReadDirectory( DIR* pstDirectory );
void kRewindDirectory( DIR* pstDirectory );
int kCloseDirectory( DIR* pstDirectory );
BOOL kWriteZero( FILE* pstFile, DWORD dwCount );
BOOL kIsFileOpened( const DIRECTORYENTRY* pstEntry );

static void* kAllocateFileDirectoryHandle( void );
static void kFreeFileDirectoryHandle( FILE* pstFile );
static BOOL kCreateFile( const char* pcFileName, DIRECTORYENTRY* pstEntry, 
        int* piDirectoryEntryIndex );
static BOOL kFreeClusterUntilEnd( DWORD dwClusterIndex );
static BOOL kUpdateDirectoryEntry( FILEHANDLE* pstFileHandle );

static BOOL kCreateDirectory( const char* pcFileName, DIRECTORYENTRY* pstEntry, 
        int* piDirectoryEntryIndex );
void kSetDotInDirectory();
DIRECTORYENTRY* kFindDirectory( DWORD currentCluster );
BOOL kUpdateDirectory( int piDirectoryEntryIndex,const char* fileName,const char* parentPath, int parentIndex );
void kUpdateFileMod(DIRECTORYENTRY pstEntry,int u_mod, int g_mod, int o_mod,int index);
void kSetClusterIndex(DWORD currentDirectoryClusterIndex);
#endif /*__FILESYSTEM_H__*/
