[ORG 0x00] ;μ½λ? ?? ?΄?? ?€λ₯? 0x00?Όλ‘? ?€? 
[BITS 16]   ; ?΄? μ½λ? 16λΉνΈλ‘? ?€? 

SECTION .text
  
jmp 0x07C0:START
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   MINT64 OS? κ΄?? ¨? ?κ²½μ€?  κ°?
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   μ½λ ??­ 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
    mov ax, 0x07C0
    mov ds, ax      ;ds?Έκ·Έλ¨Ό?Έ ? μ§??€?°? λΆ??Έλ‘λ ???΄?? ?€ ?€? 
    mov ax, 0xB800
    mov es, ax      ;es?Έκ·Έλ¨Ό?Έ ? μ§??€?°? λΉλ?€λ©λͺ¨λ¦? ?? ?΄?? ?€ ?€? 

    ;?€? μ΄κΈ°? μ½λ
    mov ax, 0x0000
	mov ss, ax
	mov sp, 0xFFFF
	mov bp, 0xFFFF
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; ?λ©΄μ???°κ³? ??±κ°μ ?Ή??Όλ‘? ?€? 
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov si,    0

.SCREENCLEARLOOP:
	mov byte [ es: si ], 0
    mov byte [ es: si + 1 ], 0x0A	

    add si, 2
    cmp si, 80 * 25 * 2		

    jl .SCREENCLEARLOOP

;?Ό/?/?? λ°? ??Ό? κ°?? Έ?€? ?¨?
GETDAY:
    mov ah, 0x04
    int 0x1a
    ;TODO : error λ©μΈμ§? μΆκ??(kch)
    ;?Ό? ?΄?Ή κ°? ?£κΈ?
    xor bx, bx
    mov al, dl
    call .CALCDAY
    ;?Ό? ?΄?Ή κ°? ?£κΈ?
    mov al, dh
    call .CALCDAY
    xor ax, ax
    ;?ΈκΈ°μ ?΄?Ή κ°? ?£κΈ?
    mov al, ch
    call .CALCDAY
    ;??? ?΄?Ή κ°? ?£κΈ?
    mov al, cl
    sub bx, 0x1
    call .CALCDAY
    xor ax, ax
    ;?ΈκΈ°μ?? ??? ?΄?Ή?? κ°μ BCDκ°μ? ? ?λ‘? λ³???? λΆ?λΆ?
    ;?? 
    mov bl, cl
    mov bh, cl
    and bx, 0xf00f
    shr bh, 4
    mov ax, 0xa
    mul bh
    and bh, 0x0
    add bx, ax
    push bx
    ;?ΈκΈ?
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
    ;?€?? ?λ₯? κ°?? Έ?€? ?¨? ?ΈμΆ? ??¬ ????? ?΄?Ή ?? -1λ₯? ?Έ?λ‘? μ€??€(?΄?Ή ??κ°? ?€??Έμ§? λΉκ΅?κΈ? ??΄?)
    call .CHECKLEAPYEAR     
    mov word[LEAPYEAR1], cx
	sub bx, 1		
    call .CHECKLEAPYEAR
    mov word[LEAPYEAR2], cx
    ;1900?? λΆ??° ?΄?Ή ?? κΉμ??? ?€?? ?λ₯? κ³μ°??€.
    sub cx, 0x1c8   ;456
    sub bx, 0x76b   ;1900-2018?? ?(365 mod 7 == 1?΄κΈ°λλ¬Έμ)
    add bx, cx   ;1900-2018(????? ?€?? ??κ°?)
	;?? ?΄?Ή?? κ°μ κ°?? Έ?€? λΆ?λΆ?
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
    ;λ―Έλ¦¬ ? ?Έ? λ°°μ΄?? ?΄?Ή ?? ?΄?Ή?? κ°μ κ°?? Έ?¨?€(?-1)κΉμ??
    mov ax, [MONTH+ecx*2]
    add bx, ax  ;year + month
    ;?Ό? ?΄?Ή?? κ°μ κ°?? Έ?€? λΆ?λΆ?
    mov al, byte[DAY + 0]
	sub al, 0x30
	mov cl, 0xa
	mul cl
	mov cl, byte[DAY + 1]
	sub cl, 0x30
	add cl, al
    add bx, cx ;year + month + day
    ;?€??Έμ§? ??¨?? λΆ?λΆ?
    mov cx, word[LEAPYEAR1]
    mov ax, word[LEAPYEAR2]
    ;?€??Έκ°? ??Έ
    sub ax, cx        
    test ax, ax
    jz .PRINTWEEK
    ;2??¬? ???κ°? ??Έ
    cmp di, 2
    js .PRINTWEEK
    add bx, 0x1
    jmp .PRINTWEEK
;BCDλ₯? ? ?λ‘? λ°κΎΈ? ?¨?
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
;?€?? ?λ₯? ?Έλ¦¬λ ?¨?
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
;λ―Έλ¦¬ ? ?Έ? λ°°μ΄?? ??Ό κ°μ μ°Ύμ? μΆλ ₯
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
	; mov si, 0x1000  ;λ¬Όλ¦¬?  μ£Όμ(0x10000)? ?Έκ·Έλ¨Ό?Έ? μ§??€?° κ°μΌλ‘? λ³??
    mov si, 0x07E0
	mov es, si      ;es?Έκ·Έλ¨Ό?Έ ? μ§??€?°? κ°? ?€? 
	mov bx, 0x0000  ;bx? μ§??€?°? 0x0000? ?€? ??¬ λ³΅μ¬?  ?΄?? ?€λ₯? 0x10000?Όλ‘? ?€? 
READDATA:
    mov ah, 0x02                    ; BIOS ?λΉμ€ λ²νΈ 2(?Ή?° ?½κΈ?)
	mov al, 0x1                     ; ?½? ?Ή?° ?? 1
	mov ch, byte [ TRACKNUMBER ]	;?½? ?Έ? λ²νΈ ?€? 
	mov cl, byte [ SECTORNUMBER ]	;?½? ?Ή?° λ²νΈ ?€? 
	mov dh, byte [ HEADNUMBER ]     ;?½? ?€? λ²νΈ ?€? 
	mov dl, 0x00                    ;?½? ??Ό?΄λΈ? λ²νΈ(?λ‘νΌ??€?¬? 0)
	int 0x13                        ;??€?¬ io?λΉμ€λ₯? ?¬?©?κΈ? ??΄ ?Έ?°?½?Έλ°μ
                                    ; (0x13 : ?Ή?° κΈ°λ° ??/?λ‘νΌ ??€?¬ ?½κΈ°Β·μ°κΈ? ?λΉμ€ ? κ³?)
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

;?°?΄?° ??­--------------------------
DAY:		db '00/00/0000', 0	
DISKERRORMESSAGE:	db 'DISK Err', 0

DAYS:	db 'SUNMONTUEWEDTHUFRISAT', 0
LEAPYEAR1:    dw  0x00
LEAPYEAR2:    dw  0x00
MONTH: dw 0,3,3,6,1,4,6,2,5,0,3,5   ;?? ? ?? 7λ‘? ?? κ°?

SECTORNUMBER:           db  0x02    
HEADNUMBER:             db  0x00   
TRACKNUMBER:            db  0x00 
times 510 - ( $ - $$ ) db 0x00  ;??¬λΆ??° 510 address κΉμ?? 1λ°μ΄?Έλ‘? ? ?Έ
                                ; ??¬ ?μΉμ? 510 address κΉμ?? λ°λ³΅????¬ 0x00?Όλ‘? μ±μ??

db 0x55
db 0xAA ; λ§μ??λ§? λ°μ΄?Έ κ°μ΄ 0xAA?Έμ§? κ²??¬ 
        ;λΆ??Έ ?Ή?° 512 λ°μ΄?Έ?? λ§μ??λ§? 2λ°μ΄?Έκ°? 0x55? 0xAAκ°? ???Όλ©? λΆ??Έλ‘λλ‘? ?Έ? ?μ§? ?? 
