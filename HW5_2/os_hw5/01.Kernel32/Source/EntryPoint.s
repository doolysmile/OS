[ORG 0x00]
[BITS 16]

SECTION .text

START:
    mov ax, 0x1000 
    mov ds, ax     
	mov es, ax     

	xor ebx, ebx
	mov di, 0
	mov ecx, 18h
	xor ebp, ebp

CALRAM:
	mov eax, 0xe820
	mov edx, 534d4150h
	int 15h

	test ecx, ecx
	jz NEXTDES

	mov edx, [es:di+16]	;type
	cmp dx, 1
	jne NEXTDES

	mov eax, [es:di+8]	;length
	add ebp, eax

NEXTDES:
	test ebx, ebx
	jnz CALRAM

	shr ebp, 20
	mov eax, ebp
	mov dl, 10
	div dl

	mov bl, al
	and bx, 0xff
	mov cl, BYTE[HEX+bx]
	mov [SIZESTR+11], cl

	shr ax, 8
	mov bx, ax
	mov cl, BYTE[HEX+bx]
	mov [SIZESTR+12], cl

    mov ax, 0x2401 
    int 0x15       

    jc .A20GATEERROR
    jmp .A20GATESUCCESS

.A20GATEERROR:
    in al, 0x92     
    or al, 0x02     
    and al, 0xFE    
    out 0x92, al    
	    
.A20GATESUCCESS:
    cli     
    lgdt [ GDTR ]   

    mov eax, 0x4000003B ; PG=0, CD=1, NW=0, AM=0, WP=0, NE=1, ET=1, TS=1, EM=0, MP=1, PE=1
    mov cr0, eax        
    jmp dword 0x18: ( PROTECTEDMODE - $$ + 0x10000 )

[BITS 32]     
PROTECTEDMODE:
    mov ax, 0x20   
    mov ds, ax   
    mov es, ax          ; ES 세그먼트 셀렉터에 설정
    mov fs, ax          ; FS 세그먼트 셀렉터에 설정
    mov gs, ax          ; GS 세그먼트 셀렉터에 설정
    
    mov ss, ax          ; SS 세그먼트 셀렉터에 설정
    mov esp, 0xFFFE     ; ESP 레지스터의 어드레스를 0xFFFE로 설정
    mov ebp, 0xFFFE     ; EBP 레지스터의 어드레스를 0xFFFE로 설정
    
	push ( SIZESTR - $$ + 0x10000 )
	push 3
	push 0
	call PRINTMESSAGE
	add esp, 12

	push ( SWITCHSUCCESSMESSAGE - $$ + 0x10000 )   
	push 4                                          ; 화면 Y 좌표(3)를 스택에 삽입
    push 0                                          ; 화면 X 좌표(0)를 스택에 삽입
    call PRINTMESSAGE                               ; PRINTMESSAGE 함수 호출
    add esp, 12                                     ; 삽입한 파라미터 제거

    jmp dword 0x18: 0x10200 

PRINTMESSAGE:
    push ebp        ; 베이스 포인터 레지스터(BP)를 스택에 삽입
    mov ebp, esp    ; 베이스 포인터 레지스터(BP)에 스택 포인터 레지스터(SP)의 값을 설정
    
	push esi        ; 함수에서 임시로 사용하는 레지스터로 함수의 마지막 부분에서
    push edi        ; 스택에 삽입된 값을 꺼내 원래 값으로 복원
    push eax
    push ecx
    push edx
    
    mov eax, dword [ ebp + 12 ] ; 파라미터 2(화면 좌표 Y)를 EAX 레지스터에 설정
    mov esi, 160                ; 한 라인의 바이트 수(2 * 80 컬럼)를 ESI 레지스터에 설정
    mul esi                     ; EAX 레지스터와 ESI 레지스터를 곱하여 화면 Y 어드레스 계산
    mov edi, eax                ; 계산된 화면 Y 어드레스를 EDI 레지스터에 설정
    
    mov eax, dword [ ebp + 8 ]  ; 파라미터 1(화면 좌표 X)를 EAX 레지스터에 설정
    mov esi, 2                  ; 한 문자를 나타내는 바이트 수(2)를 ESI 레지스터에 설정
    mul esi                     ; EAX 레지스터와 ESI 레지스터를 곱하여 화면 X 어드레스를 계산
    add edi, eax                ; 화면 Y 어드레스와 계산된 X 어드레스를 더해서
                                
    mov esi, dword [ ebp + 16 ] ; 파라미터 3(출력할 문자열의 어드레스)

.MESSAGELOOP:               ; 메시지를 출력하는 루프
    mov cl, byte [ esi ]    ; ESI 레지스터가 가리키는 문자열 위치에서 한 문자를 
                            ; CL 레지스터에 복사
    cmp cl, 0               ; 복사된 문자와 0을 비교
    je .MESSAGEEND          ; 복사한 문자의 값이 0이면 문자열이 종료되었음을

    mov byte [ edi + 0xB8000 ], cl  ; 0이 아니라면 비디오 메모리 어드레스 
    
    add esi, 1              ; ESI 레지스터에 1을 더하여 다음 문자열로 이동
    add edi, 2       

    jmp .MESSAGELOOP        ; 메시지 출력 루프로 이동하여 다음 문자를 출력

.MESSAGEEND:
    pop edx     ; 함수에서 사용이 끝난 EDX 레지스터부터 EBP 레지스터까지를 스택에
    pop ecx     ; 삽입된 값을 이용해서 복원
    pop eax     ; 스택은 가장 마지막에 들어간 데이터가 가장 먼저 나오는
    pop edi     ; 자료구조(Last-In, First-Out)이므로 삽입(push)의 역순으로
    pop esi     ; 제거(pop) 해야 함
    pop ebp     ; 베이스 포인터 레지스터(BP) 복원
    ret         ; 함수를 호출한 다음 코드의 위치로 복귀

align 8, db 0

dw 0x0000

GDTR:
    dw GDTEND - GDT - 1         ; 아래에 위치하는 GDT 테이블의 전체 크기
    dd ( GDT - $$ + 0x10000 )   ; 아래에 위치하는 GDT 테이블의 시작 어드레스

GDT:
    NULLDescriptor:
        dw 0x0000
        dw 0x0000
        db 0x00
        db 0x00
        db 0x00
        db 0x00

    IA_32eCODEDESCRIPTOR:     
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
        db 0x9A         ; P=1, DPL=0, Code Segment, Execute/Read
        db 0xAF         ; G=1, D=0, L=1, Limit[19:16]
        db 0x00         ; Base [31:24]  
        
    IA_32eDATADESCRIPTOR:
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
        db 0x92         ; P=1, DPL=0, Data Segment, Read/Write
        db 0xAF         ; G=1, D=0, L=1, Limit[19:16]
        db 0x00         ; Base [31:24]
        
    CODEDESCRIPTOR:     
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
        db 0x9A         ; P=1, DPL=0, Code Segment, Execute/Read
        db 0xCF         ; G=1, D=1, L=0, Limit[19:16]
        db 0x00         ; Base [31:24]  
        
    DATADESCRIPTOR:
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
        db 0x92         ; P=1, DPL=0, Data Segment, Read/Write
        db 0xCF         ; G=1, D=1, L=0, Limit[19:16]
        db 0x00         ; Base [31:24]
GDTEND:

SWITCHSUCCESSMESSAGE:	db 'Switch To Protected Mode Success~!!', 0
SIZESTR:				db 'RAM SIZE : ** MB', 0
HEX:					db '0123456789abcdef'

times 512 - ( $ - $$ )  db  0x00    ; 512바이트를 맞추기 위해 남은 부분을 0으로 채움
