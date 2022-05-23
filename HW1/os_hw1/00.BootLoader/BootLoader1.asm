[ORG 0x00] ;코드의 시작 어드레스를 0x00으로 설정
[BITS 16]   ; 이하 코드는 16비트로 설정

SECTION .text
  
jmp 0x07C0:START
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   MINT64 OS에 관련된 환경설정 값
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   코드 영역 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
    mov ax, 0x07C0
    mov ds, ax      ;ds세그먼트 레지스터에 부트로더 시작어드레스 설정
    mov ax, 0xB800
    mov es, ax      ;es세그먼트 레지스터에 비디오메모리 시작 어드레스 설정

    ;스택 초기화 코드
    mov ax, 0x0000
	mov ss, ax
	mov sp, 0xFFFF
	mov bp, 0xFFFF
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; 화면지우고 속성값을 녹색으로 설정
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov si,    0

.SCREENCLEARLOOP:
	mov byte [ es: si ], 0
    mov byte [ es: si + 1 ], 0x0A	

    add si, 2
    cmp si, 80 * 25 * 2		

    jl .SCREENCLEARLOOP

;일/월/년도 및 요일을 가져오는 함수
GETDAY:
    mov ah, 0x04
    int 0x1a
    ;TODO : error 메세지 추가(kch)
    ;일에 해당 값 넣기
    xor bx, bx
    mov al, dl
    call .CALCDAY
    ;일에 해당 값 넣기
    mov al, dh
    call .CALCDAY
    xor ax, ax
    ;세기에 해당 값 넣기
    mov al, ch
    call .CALCDAY
    ;년도에 해당 값 넣기
    mov al, cl
    sub bx, 0x1
    call .CALCDAY
    xor ax, ax
    ;세기와 년도에 해당하는 값을 BCD값에서 정수로 변환하는 부분
    ;년도 
    mov bl, cl
    mov bh, cl
    and bx, 0xf00f
    shr bh, 4
    mov ax, 0xa
    mul bh
    and bh, 0x0
    add bx, ax
    push bx
    ;세기
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
    ;윤년의 수를 가져오는 함수 호출 현재 년도와 해당 년도 -1를 인자로 준다(해당 년도가 윤년인지 비교하기 위해서)
    call .CHECKLEAPYEAR     
    mov word[LEAPYEAR1], cx
	sub bx, 1		
    call .CHECKLEAPYEAR
    mov word[LEAPYEAR2], cx
    ;1900년도 부터 해당 년도 까지의 윤년의 수를 계산한다.
    sub cx, 0x1c8   ;456
    sub bx, 0x76b   ;1900-2018년도 수(365 mod 7 == 1이기때문에)
    add bx, cx   ;1900-2018(년도수에서 윤년수 더한값)
	;월에 해당하는 값을 가져오는 부분
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
    ;미리 선언된 배열에서 해당 월에 해당하는 값을 가져온다(월-1)까지
    mov ax, [MONTH+ecx*2]
    add bx, ax  ;year + month
    ;일에 해당하는 값을 가져오는 부분
    mov al, byte[DAY + 0]
	sub al, 0x30
	mov cl, 0xa
	mul cl
	mov cl, byte[DAY + 1]
	sub cl, 0x30
	add cl, al
    add bx, cx ;year + month + day
    ;윤년인지 판단하는 부분
    mov cx, word[LEAPYEAR1]
    mov ax, word[LEAPYEAR2]
    ;윤년인가 확인
    sub ax, cx        
    test ax, ax
    jz .PRINTWEEK
    ;2월달을 넘었는가 확인
    cmp di, 2
    js .PRINTWEEK
    add bx, 0x1
    jmp .PRINTWEEK
;BCD를 정수로 바꾸는 함수
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
;윤년의 수를 세리는 함수
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
;미리 선언된 배열에서 요일 값을 찾아서 출력
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

	mov si, 0x1000  ;물리적 주소(0x10000)을 세그먼트레지스터 값으로 변환
	mov es, si      ;es세그먼트 레지스터에 값 설정
	mov bx, 0x0000  ;bx레지스터에 0x0000을 설정하여 복사할 어드레스를 0x10000으로 설정
READDATA:
    mov ah, 0x02                    ; BIOS 서비스 번호 2(섹터 읽기)
	mov al, 0x1                     ; 읽을 섹터 수는 1
	mov ch, byte [ TRACKNUMBER ]	;읽을 트랙 번호 설정
	mov cl, byte [ SECTORNUMBER ]	;읽을 섹터 번호 설정
	mov dh, byte [ HEADNUMBER ]     ;읽을 헤드 번호 설정
	mov dl, 0x00                    ;읽을 드라이브 번호(플로피디스크는 0)
	int 0x13                        ;디스크 io서비스를 사용하기 위해 인터럽트발생
                                    ; (0x13 : 섹터 기반 하드/플로피 디스크 읽기·쓰기 서비스 제공)
	jc HANDLEDISKERROR
READEND:
    jmp 0x1000:0x0000

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

;데이터 영역--------------------------
DAY:		db '00/00/0000', 0	
DISKERRORMESSAGE:	db 'DISK Err', 0

DAYS:	db 'SUNMONTUEWEDTHUFRISAT', 0
LEAPYEAR1:    dw  0x00
LEAPYEAR2:    dw  0x00
MONTH: dw 0,3,3,6,1,4,6,2,5,0,3,5   ;누적된 월을 7로 나눈 값

SECTORNUMBER:           db  0x02    
HEADNUMBER:             db  0x00   
TRACKNUMBER:            db  0x00 
times 510 - ( $ - $$ ) db 0x00  ;현재부터 510 address 까지 1바이트로 선언
                                ; 현재 위치에서 510 address 까지 반복수행하여 0x00으로 채움

db 0x55
db 0xAA ; 마지막 바이트 값이 0xAA인지 검사 
        ;부트 섹터 512 바이트에서 마지막 2바이트가 0x55나 0xAA가 아니라면 부트로더로 인식 하지 않음 
