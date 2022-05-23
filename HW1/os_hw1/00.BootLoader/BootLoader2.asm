[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x1000:START    ;부트로더 2개 쓰기 위해서 바꿔줌

TOTALSECTORCOUNT: dw 1024	;부트로더 제외한 MINT64 OS 이미지 크기

START:
;TODO : Check(kch)
    mov ax, cs
    mov ds, ax
    mov ax, 0xB800
    mov es, ax

    mov si, 0
    mov di, 0

	;TODO: 함수로 빼야할듯 함(kch)
	push MESSAGE1	; 출력할 메세지의 어드레스를 스택에 삽입
	push 0	;화면 Y좌표(0)를 스택에 삽입
	push 0	;화면 X좌표(0)를 스택에 삽입
	call PRINTMESSAGE	;PRINTMESSAGE 함수 호출
	add sp, 6	;삽입한 파라미터 제거

	push MESSAGEDAY	;출력할 메세지 - 날짜/요일 의 어드레스를 스택에 삽입
	push 1	;화면 Y좌표(1)를 스택에 삽입
	push 0	;화면 X좌표(0)를 스택에 삽입
	call PRINTMESSAGE	;PRINTMESSAGE 함수 호출
	add sp, 6	;삽입한 파라미터 제거

	push IMAGELOADINGMESSAGE	; 출력할 메세지의 어드레스를 스택에 삽입
	push 2	;화면 Y좌표(2)를 스택에 삽입
	push 0	;화면 X좌표(0)를 스택에 삽입
	call PRINTMESSAGE	;PRINTMESSAGE 함수 호출
	add sp, 6	;삽입한 파라미터 제거

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 	디스크에서 OS 이미지를 로딩
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; 	디스크를 읽기 전에 먼저 리셋
RESETDISK:			; 디스크를 리셋하는 코드의 시작

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 	BIOS Reset Function 호출
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; 서비스 번호 0, 드라이브 번호 (0=Floppy)
	mov ax, 0
	mov dl, 0
	int 0x13
	; 에러가 발생하면 에러 처리로 이동 
	jc HANDLEDISKERROR
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 	디스크에서 섹터를 읽음
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov si, 0x1020	; OS 이미지를 복사할 어드레스(어드레스를 바꿨음)
	mov es, si		;ES 세그먼트 레지스터에 값 설정
	mov bx, 0x0000	;BX 레지스터에 0x0000을 설정하여 복사함

	mov di, word[ TOTALSECTORCOUNT ]	;복사할 OS 이미지의 섹터 수를 DI 레지스터에 설정

READDATA:			; 모든 섹터를 다 읽었는지 확인
	cmp di, 0		; 복사할 OS 이미지의 섹터 수를 0과 비교
	je READEND		; 복사할 섹터 수가 0이라면 다 복사 한 것 이므로 READEND로 이동
	sub di, 0x1		; 복사할 섹터 수를 1 감소시킴
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 	BIOS Read Function 호출
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov ah, 0x02         ; BIOS 서비스 번호 2 - Read Sector        
    mov al, 0x1          ; 읽을 섹터 수는 1
	mov ch, byte[ TRACKNUMBER ]			; 읽을 트랙 번호 설정
	mov cl, byte[ SECTORNUMBER ]		; 읽을 섹터 번호 설정
	mov dh, byte[ HEADNUMBER ]			; 읽을 헤더 번호 설정
	mov dl, 0x00                  		; 읽을 드라이브 번호 (0=Floppy) 설정
    int 0x13                        	; 인터럽트 서비스 수행 
										; (0x13 : 섹터 기반 하드/플로피 디스크 읽기·쓰기 서비스 제공)
	jc HANDLEDISKERROR					; 에러가 발생했다면 에러를 출력하는 곳으로 이동 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 	복사할 어드레스와 트랙, 헤드, 섹터 어드레스 계산 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    add si, 0x0020      ; 512(0x200)바이트 만큼 읽었으므로 이를 세그먼트 레지스터 값으로 변환
    mov es, si          ; ES 세그먼트 레지스터에 더해서 어드레스를 한 섹터만큼 증가

    ; 마지막 섹터가 아니면 섹터 읽기로 이동해서 다시 섹터 읽기 수행


	mov al, byte [ SECTORNUMBER ]   ; 섹터 번호를 AL레지스터에 설정
    add al, 0x01                    ; 섹터 번호를 1 증가
    mov byte [ SECTORNUMBER ], al   ; 증가시킨 섹터 번호를 SECTORNUMBER에 다시 설정
    cmp al, 19                      ; 증가시킨 섹터 번호를 19와 비교
    jl READDATA                     ; 섹터 번호가 19미만이라면 READDATA로 이동

	; 마지막 섹터까지 읽었으면 헤드를 토글하고, 섹터 번호를 1로 설정
    xor byte [ HEADNUMBER ], 0x01   ; 헤드 번호를 0x01과 XOR하여 토글
    mov byte [ SECTORNUMBER ], 0x01 ; 섹터 번호를 1 로 설정

    ; 만약 헤드가 1->0으로 바뀌었으면 양쪽 헤드를 모두 읽은것이므로 아래로 이동하여 트랙 번호를 1 증가
    cmp byte [ HEADNUMBER ], 0x00   ; 헤드 번호를 0x00과 비교
    jne READDATA                    ; 헤드 번호가 0이 아니면 READDATA로 이동

    ; 트랙을 1 증가시킨 후, 다시 섹터 읽기로 이동
    add byte [ TRACKNUMBER ] , 0x01 ; 트랙 번호를 1 증가
    jmp READDATA                    ; READDATA로 이동

READEND:
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; OS 이미지가 완료되었다는 메시지를 출력
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	push LOADINGCOMPLETEMESSAGE	; 출력할 메시지의 어드레스를 스택에 삽입
	push 2						; 화면 Y 좌표 1를 스택에 삽입
	push 20						; 화면 X 좌표 20을 스택에 삽입
	call PRINTMESSAGE			; PRINTMESSAGE 함수 호출
	add sp, 6					; 삽입한 파라미터 제거

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; 가상 OS 이미지 실행 (VirtualOS))
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	jmp 0x1020:0x0000
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 함수 코드 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 디스크 에러를 처리하는 함수
HANDLEDISKERROR:
    push DISKERRORMESSAGE   ; 에러 문자열의 어드레스를 스택에 삽입
    push 2                  ; 화면 Y 좌표 1를 스택에 삽입
    push 20                 ; 화면 X 좌표 20을 스택에 삽입
    call PRINTMESSAGE       ; PRINTMESSAGE 함수 호출
    add sp, 6               ; 삽입한 파라미터 제거

	jmp $					; 현재 위치에서 무한 루프 수행
; 메시지를 출력하는 함수 - PARAM: x좌표, y좌표, 문자열
PRINTMESSAGE:
    push bp     ; 베이스 포인터 레지스터(BP)를 스택에 삽입
    mov bp, sp  ; 베이스 포인터 레지스터(BP)에 스택 포인터 레지스터(SP)의 값을 설정
    push es     ; ES 세그먼트 레지스터부터 DX 레지스터까지 스택에 삽입
	push si
	push di
	push ax
	push cx
	
    mov ax, 0xB800          ; 비디오 메모리 시작 어드레스(0xB8000)를 세그먼트 레지스터 값으로 변환
    mov es, ax              ; ES 세그먼트 레지스터에 설정
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; X, Y의 좌표로 비디오 메모리의 어드레스를 계산함
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; Y 좌표를 이용해서 먼저 라인 어드레스를 구함
	mov ax, word [ bp + 6 ] ; 파라미터 2(Y좌표)를 AX 레지스터에 설정
    mov si, 160             ; 한 라인의 바이트 수(2 * 80 컬럼)를 SI 레지스터에 설정
    mul si                  ; AX 레지스터와 SI 레지스터를 곱하여 화면 Y어드레스 계산
    mov di, ax              ; 계산된 화면 Y 어드레스를 DI 레지스터에 설정

    ; X 좌표를 이용해서 2를 곱한 후 최종 어드레스를 구함
    mov ax, word [ bp + 4 ] ; 파라미터 1(X좌표)를 AX 레지스터에 설정
    mov si, 2               ; 한 문자를 나타내는 바이트수(2)를 SI 레지스터에 설정
    mul si                  ; AX 레지스터와 SI 레지스터를 곱하여 화면 X 어드레스를 계산
    add di, ax              ; 화면 Y 어드레스와 계산된 X 어드레스를 더해서 실제 비디오 메모리 어드레스를 계산

    mov si, word [ bp + 8 ] ; 파라미터 3(출력할 문자열의 어드레스)

.MESSAGELOOP:               ; 메시지를 출력하는 루프
    mov cl, byte [ si ]     ; SI 레지스터가 가리키는 문자열 위치에서 한 문자를 CL 레지스터에 복사
                            ; 문자열은 1바이트면 충분하므로 CX 레지스터의 하위 1바이트만 사용

    cmp cl, 0               ; 복사된 문자와 0을 비교
    je  .MESSAGEEND         ; 복사한 문자의 값이 0이면 .MESSAGEEND로 이동하여 문자 출력 종료


    mov byte [ es: di ], cl ; 0이 아니라면 메모리 어드레스 비디오 메모리 어드레스에 문자를 출력

    add si, 1               ; SI 레지스터에 1을 더하여 다음 문자열로 이동
    add di, 2               ; DI 레지스터에 2를 더하여 비디오 메모리의 다음 문자 위치로 이동 문자만 출력하려면 2를 더해야함

    jmp .MESSAGELOOP        ; 메시지 출력 루프로 이동하여 다음 문자를 출력

.MESSAGEEND:
	pop cx
	pop ax
	pop di
	pop si
	pop es
	pop bp
	ret

;데이터 영역 ------------------------------------------
MESSAGE1:    db 'MINT64 OS Boot Loader Start~!!', 0 	;출력할 메시지 정의 -> 마지막은 0으로 설정하여 .MESSAGELOOP 에서 처리할 수 있게 함 


DISKERRORMESSAGE:       db  'DISK Error~!!', 0
IMAGELOADINGMESSAGE:    db  'OS Image Loading...', 0
LOADINGCOMPLETEMESSAGE: db  'Complete~!!', 0

MESSAGEDAY:					db 'Current Date:' , 0

SECTORNUMBER:				db 0x03	;부트로더 2개로 인한 섹터수 변경
HEADNUMBER: 				db 0x00
TRACKNUMBER: 				db 0x00

times 510 - ( $ - $$ )    db    0x00	;현재부터 510 address 까지 1바이트로 선언
                                		; 현재 위치에서 510 address 까지 반복수행하여 0x00으로 채움

db 0x55 
db 0xAA ; 마지막 바이트 값이 0xAA인지 검사 
    ;부트 섹터 512 바이트에서 마지막 2바이트가 0x55나 0xAA가 아니라면 부트로더로 인식 하지 않음 
