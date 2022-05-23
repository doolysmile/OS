#ifndef __PAGE_H__
#define __PAGE_H__

#include "Types.h"

////////////////////////////////////////////////////////////////////////////////
//
// Îß§ÌÅ¨Î°?
//
////////////////////////////////////////////////////////////////////////////////
// ?ïò?úÑ 32ÎπÑÌä∏?ö© ?Üç?Ñ± ?ïÑ?ìú
#define PAGE_FLAGS_P        0x00000001  // Present
#define PAGE_FLAGS_RW       0x00000002  // Read/Write
#define PAGE_FLAGS_US       0x00000004  // User/Supervisor(?îå?ûòÍ∑? ?Ñ§?†ï ?ãú ?ú†??? ?†àÎ≤?)
#define PAGE_FLAGS_PWT      0x00000008  // Page Level Write-through
#define PAGE_FLAGS_PCD      0x00000010  // Page Level Cache Disable
#define PAGE_FLAGS_A        0x00000020  // Accessed
#define PAGE_FLAGS_D        0x00000040  // Dirty
#define PAGE_FLAGS_PS       0x00000080  // Page Size
#define PAGE_FLAGS_G        0x00000100  // Global
#define PAGE_FLAGS_PAT      0x00001000  // Page Attribute Table Index
// ?ÉÅ?úÑ 32ÎπÑÌä∏?ö© ?Üç?Ñ± ?ïÑ?ìú
#define PAGE_FLAGS_EXB      0x80000000  // Execute Disable ÎπÑÌä∏  
// ?ïÑ?ûò?äî ?é∏?ùòÎ•? ?úÑ?ï¥ ?†ï?ùò?ïú ?îå?ûòÍ∑?
#define PAGE_FLAGS_DEFAULT  ( PAGE_FLAGS_P | PAGE_FLAGS_RW )

// Í∏∞Ì?? ?éò?ù¥Ïß? Í¥??†®
#define PAGE_TABLESIZE      0x1000
#define PAGE_MAXENTRYCOUNT  512
#define PAGE_DEFAULTSIZE    0x200000

////////////////////////////////////////////////////////////////////////////////
//
// Íµ¨Ï°∞Ï≤?
//
////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )

// ?éò?ù¥Ïß? ?óî?ä∏Î¶¨Ïóê ????ïú ?ûêÎ£åÍµ¨Ï°?
typedef struct kPageTableEntryStruct
{
    // PML4T??? PDPTE?ùò Í≤ΩÏö∞
    // 1 ÎπÑÌä∏ P, RW, US, PWT, PCD, A, 3 ÎπÑÌä∏ Reserved, 3 ÎπÑÌä∏ Avail, 
    // 20 ÎπÑÌä∏ Base Address
    // PDE?ùò Í≤ΩÏö∞
    // 1 ÎπÑÌä∏ P, RW, US, PWT, PCD, A, D, 1, G, 3 ÎπÑÌä∏ Avail, 1 ÎπÑÌä∏ PAT, 8 ÎπÑÌä∏ Avail, 
    // 11 ÎπÑÌä∏ Base Address
    DWORD dwAttributeAndLowerBaseAddress;
    // 8 ÎπÑÌä∏ Upper BaseAddress, 12 ÎπÑÌä∏ Reserved, 11 ÎπÑÌä∏ Avail, 1 ÎπÑÌä∏ EXB 
    DWORD dwUpperBaseAddressAndEXB;
} PML4TENTRY, PDPTENTRY, PDENTRY, PTENTRY;

#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////
//
//  ?ï®?àò
//
////////////////////////////////////////////////////////////////////////////////
void kInitializePageTables( void );
void kSetPageEntryData( PTENTRY* pstEntry, DWORD dwUpperBaseAddress,
        DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags );

#endif /*__PAGE_H__*/
