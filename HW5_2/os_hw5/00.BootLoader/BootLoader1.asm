[ORG 0x00] ;ì½”ë“œ?˜ ?‹œ?ž‘ ?–´?“œ? ˆ?Š¤ë¥? 0x00?œ¼ë¡? ?„¤? •
[BITS 16]   ; ?´?•˜ ì½”ë“œ?Š” 16ë¹„íŠ¸ë¡? ?„¤? •

SECTION .text
  
jmp 0x07C0:START
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   MINT64 OS?— ê´?? ¨?œ ?™˜ê²½ì„¤? • ê°?
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   ì½”ë“œ ?˜?—­ 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
    mov ax, 0x07C0
    mov ds, ax      ;ds?„¸ê·¸ë¨¼?Š¸ ? ˆì§??Š¤?„°?— ë¶??Š¸ë¡œë” ?‹œ?ž‘?–´?“œ? ˆ?Š¤ ?„¤? •
    mov ax, 0xB800
    mov es, ax      ;es?„¸ê·¸ë¨¼?Š¸ ? ˆì§??Š¤?„°?— ë¹„ë””?˜¤ë©”ëª¨ë¦? ?‹œ?ž‘ ?–´?“œ? ˆ?Š¤ ?„¤? •

    ;?Š¤?ƒ ì´ˆê¸°?™” ì½”ë“œ
    mov ax, 0x0000
	mov ss, ax
	mov sp, 0xFFFF
	mov bp, 0xFFFF
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; ?™”ë©´ì???š°ê³? ?†?„±ê°’ì„ ?…¹?ƒ‰?œ¼ë¡? ?„¤? •
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov si,    0

.SCREENCLEARLOOP:
	mov byte [ es: si ], 0
    mov byte [ es: si + 1 ], 0x0A	

    add si, 2
    cmp si, 80 * 25 * 2		

    jl .SCREENCLEARLOOP

;?¼/?›”/?…„?„ ë°? ?š”?¼?„ ê°?? ¸?˜¤?Š” ?•¨?ˆ˜
GETDAY:
    mov ah, 0x04
    int 0x1a
    ;TODO : error ë©”ì„¸ì§? ì¶”ê??(kch)
    ;?¼?— ?•´?‹¹ ê°? ?„£ê¸?
    xor bx, bx
    mov al, dl
    call .CALCDAY
    ;?¼?— ?•´?‹¹ ê°? ?„£ê¸?
    mov al, dh
    call .CALCDAY
    xor ax, ax
    ;?„¸ê¸°ì— ?•´?‹¹ ê°? ?„£ê¸?
    mov al, ch
    call .CALCDAY
    ;?…„?„?— ?•´?‹¹ ê°? ?„£ê¸?
    mov al, cl
    sub bx, 0x1
    call .CALCDAY
    xor ax, ax
    ;?„¸ê¸°ì?? ?…„?„?— ?•´?‹¹?•˜?Š” ê°’ì„ BCDê°’ì—?„œ ? •?ˆ˜ë¡? ë³??™˜?•˜?Š” ë¶?ë¶?
    ;?…„?„ 
    mov bl, cl
    mov bh, cl
    and bx, 0xf00f
    shr bh, 4
    mov ax, 0xa
    mul bh
    and bh, 0x0
    add bx, ax
    push bx
    ;?„¸ê¸?
    mov bl, ch
    mov bh, ch
    and bx, 0xf00f
    shr bh, 4
    mov ax, 0xa
    mul bh
    and bh, 0x0
    add bx, ax
    mov ax, 0x64
    mul bx
    pop bx
    add bx, ax                  
    ;?œ¤?…„?˜ ?ˆ˜ë¥? ê°?? ¸?˜¤?Š” ?•¨?ˆ˜ ?˜¸ì¶? ?˜„?ž¬ ?…„?„??? ?•´?‹¹ ?…„?„ -1ë¥? ?¸?žë¡? ì¤??‹¤(?•´?‹¹ ?…„?„ê°? ?œ¤?…„?¸ì§? ë¹„êµ?•˜ê¸? ?œ„?•´?„œ)
    call .CHECKLEAPYEAR     
    mov word[LEAPYEAR1], cx
	sub bx, 1		
    call .CHECKLEAPYEAR
    mov word[LEAPYEAR2], cx
    ;1900?…„?„ ë¶??„° ?•´?‹¹ ?…„?„ ê¹Œì???˜ ?œ¤?…„?˜ ?ˆ˜ë¥? ê³„ì‚°?•œ?‹¤.
    sub cx, 0x1c8   ;456
    sub bx, 0x76b   ;1900-2018?…„?„ ?ˆ˜(365 mod 7 == 1?´ê¸°ë•Œë¬¸ì—)
    add bx, cx   ;1900-2018(?…„?„?ˆ˜?—?„œ ?œ¤?…„?ˆ˜ ?”?•œê°?)
	;?›”?— ?•´?‹¹?•˜?Š” ê°’ì„ ê°?? ¸?˜¤?Š” ë¶?ë¶?
	mov al, byte[DAY + 3]
	sub al, 0x30
	mov cx, 10
	mul cx
	mov cl, byte[DAY + 4]
	sub cl, 0x30
	add cl, al 
    sub cl, 0x1
    mov di, cx  ;di = 8month
    and ecx, 0xf
    ;ë¯¸ë¦¬ ?„ ?–¸?œ ë°°ì—´?—?„œ ?•´?‹¹ ?›”?— ?•´?‹¹?•˜?Š” ê°’ì„ ê°?? ¸?˜¨?‹¤(?›”-1)ê¹Œì??
    mov ax, [MONTH+ecx*2]
    add bx, ax  ;year + month
    ;?¼?— ?•´?‹¹?•˜?Š” ê°’ì„ ê°?? ¸?˜¤?Š” ë¶?ë¶?
    mov al, byte[DAY + 0]
	sub al, 0x30
	mov cl, 0xa
	mul cl
	mov cl, byte[DAY + 1]
	sub cl, 0x30
	add cl, al
    add bx, cx ;year + month + day
    ;?œ¤?…„?¸ì§? ?Œ?‹¨?•˜?Š” ë¶?ë¶?
    mov cx, word[LEAPYEAR1]
    mov ax, word[LEAPYEAR2]
    ;?œ¤?…„?¸ê°? ?™•?¸
    sub ax, cx        
    test ax, ax
    jz .PRINTWEEK
    ;2?›”?‹¬?„ ?„˜?—ˆ?Š”ê°? ?™•?¸
    cmp di, 2
    js .PRINTWEEK
    add bx, 0x1
    jmp .PRINTWEEK
;BCDë¥? ? •?ˆ˜ë¡? ë°”ê¾¸?Š” ?•¨?ˆ˜
.CALCDAY:
    mov ah, al
    and ax, 0xf00f
    shr ah, 4
    add ax, 0x3030
    mov byte[DAY + bx], ah
    add bx, 1
    mov byte[DAY + bx], al
    add bx, 2
    ret
;?œ¤?…„?˜ ?ˆ˜ë¥? ?„¸ë¦¬ëŠ” ?•¨?ˆ˜
.CHECKLEAPYEAR:
    mov cx, bx
    shr cx, 2
    mov ax, bx
    mov si, 0x64
    xor dx, dx
    div si
    sub cx, ax
    mov ax, bx
    mov si, 0x190
    xor dx, dx
    div si
    add cx, ax
    ret
;ë¯¸ë¦¬ ?„ ?–¸?œ ë°°ì—´?—?„œ ?š”?¼ ê°’ì„ ì°¾ì•„?„œ ì¶œë ¥
.PRINTWEEK:
    add bx, 0x3
    xor dx, dx
    mov ax, bx 
    mov cx, 0x7
    div cx    
    mov ax, dx
    mov bx, 0x3
    mul bx
    mov di, ax
    mov cl, byte[DAYS + di]
    mov byte [ es: 210 ], cl
    mov cl, byte[DAYS + di + 1]
    mov byte [ es: 212 ], cl
    mov cl, byte[DAYS + di + 2]
    mov byte [ es: 214 ], cl
	xor si, si
	mov di, 188
PRINTDAY:
	mov bl, byte [DAY + si]
	test bl, bl
	je RESETDISK

	mov byte [ es: di ], bl
	add si, 1
	add di, 2

	jmp PRINTDAY

RESETDISK:
	mov ax, 0
	mov dl, 0
	int 0x13
	jc HANDLEDISKERROR
;kch stack test
	; mov si, 0x1000  ;ë¬¼ë¦¬?  ì£¼ì†Œ(0x10000)?„ ?„¸ê·¸ë¨¼?Š¸? ˆì§??Š¤?„° ê°’ìœ¼ë¡? ë³??™˜
    mov si, 0x07E0
	mov es, si      ;es?„¸ê·¸ë¨¼?Š¸ ? ˆì§??Š¤?„°?— ê°? ?„¤? •
	mov bx, 0x0000  ;bx? ˆì§??Š¤?„°?— 0x0000?„ ?„¤? •?•˜?—¬ ë³µì‚¬?•  ?–´?“œ? ˆ?Š¤ë¥? 0x10000?œ¼ë¡? ?„¤? •
READDATA:
    mov ah, 0x02                    ; BIOS ?„œë¹„ìŠ¤ ë²ˆí˜¸ 2(?„¹?„° ?½ê¸?)
	mov al, 0x1                     ; ?½?„ ?„¹?„° ?ˆ˜?Š” 1
	mov ch, byte [ TRACKNUMBER ]	;?½?„ ?Š¸?ž™ ë²ˆí˜¸ ?„¤? •
	mov cl, byte [ SECTORNUMBER ]	;?½?„ ?„¹?„° ë²ˆí˜¸ ?„¤? •
	mov dh, byte [ HEADNUMBER ]     ;?½?„ ?—¤?“œ ë²ˆí˜¸ ?„¤? •
	mov dl, 0x00                    ;?½?„ ?“œ?¼?´ë¸? ë²ˆí˜¸(?”Œë¡œí”¼?””?Š¤?¬?Š” 0)
	int 0x13                        ;?””?Š¤?¬ io?„œë¹„ìŠ¤ë¥? ?‚¬?š©?•˜ê¸? ?œ„?•´ ?¸?„°?Ÿ½?Š¸ë°œìƒ
                                    ; (0x13 : ?„¹?„° ê¸°ë°˜ ?•˜?“œ/?”Œë¡œí”¼ ?””?Š¤?¬ ?½ê¸°Â·ì“°ê¸? ?„œë¹„ìŠ¤ ? œê³?)
	jc HANDLEDISKERROR
READEND:
;kch stack test
    ; jmp 0x1000:0x0000
    jmp 0x07E0:0x0000
HANDLEDISKERROR:
	mov si, 0
	mov di, 360
	
.MESSAGELOOP:
	mov cl, byte [ DISKERRORMESSAGE + si ]
	cmp cl, 0
	jmp .MESSAGEEND

	mov byte [ es: di ], cl
	add si, 1
	add di, 2

	jmp .MESSAGELOOP
.MESSAGEEND:
	jmp $

;?°?´?„° ?˜?—­--------------------------
DAY:		db '00/00/0000', 0	
DISKERRORMESSAGE:	db 'DISK Err', 0

DAYS:	db 'SUNMONTUEWEDTHUFRISAT', 0
LEAPYEAR1:    dw  0x00
LEAPYEAR2:    dw  0x00
MONTH: dw 0,3,3,6,1,4,6,2,5,0,3,5   ;?ˆ„? ?œ ?›”?„ 7ë¡? ?‚˜?ˆˆ ê°?

SECTORNUMBER:           db  0x02    
HEADNUMBER:             db  0x00   
TRACKNUMBER:            db  0x00 
times 510 - ( $ - $$ ) db 0x00  ;?˜„?ž¬ë¶??„° 510 address ê¹Œì?? 1ë°”ì´?Š¸ë¡? ?„ ?–¸
                                ; ?˜„?ž¬ ?œ„ì¹˜ì—?„œ 510 address ê¹Œì?? ë°˜ë³µ?ˆ˜?–‰?•˜?—¬ 0x00?œ¼ë¡? ì±„ì??

db 0x55
db 0xAA ; ë§ˆì??ë§? ë°”ì´?Š¸ ê°’ì´ 0xAA?¸ì§? ê²??‚¬ 
        ;ë¶??Š¸ ?„¹?„° 512 ë°”ì´?Š¸?—?„œ ë§ˆì??ë§? 2ë°”ì´?Š¸ê°? 0x55?‚˜ 0xAAê°? ?•„?‹ˆ?¼ë©? ë¶??Š¸ë¡œë”ë¡? ?¸?‹ ?•˜ì§? ?•Š?Œ 
