[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x1020:START    ; BootLoader2에서 가상 이미지를 로딩 했을 때 시작 됨

SECTORCOUNT:    dw 0x0000
TOTALSECTORCOUNT    equ 1024

START:
    mov ax, cs  ;CS 세그먼트 레지스터의 값을 AX 레지스터에 설정
    mov ds, ax  ; AX 레지스터의 값을 DS 세그먼트 레지스터에 설정
    mov ax, 0xB800  ;비디오 메모리 어드레스인 0x0B80000 을 세그먼트 레지스터 값으로 변환
    mov es, ax  ;ES 세그먼트 레지스터에 설정

 
    %assign i    0      
    ;i라는 변수를 지정하고 0으로 초기화
    %rep    TOTALSECTORCOUNT    
    ;TOTALSECTORCOUNT에 저장된 값 만큼 아래 코드를 반복
        %assign i   i + 1   
        ;i를 1 증가시킴

        mov ax, 2   ; 한 문자를 나타내는 2바이트 수를 AX 레지스터에 설정
        mul word [SECTORCOUNT]  ;AX 레지스터와 섹터 수를 곱함
        mov si, ax  ;곱한 결과를 si 레지스터에 저장

        
        add word [SECTORCOUNT], 1       ;섹터 수를 1 증가

        %if i == TOTALSECTORCOUNT       
        ;i가 마지막 섹터를 가르키면
            jmp $           ;현재 위치에서 무한루프 수행       
        %else   
        ;마지막 섹터가 아니라면
            jmp (0x1020 + i * 0x20): 0x0000     ;다음 섹터 오프셋으로 이동 
        %endif  

        times (512 - ($ - $$) % 512)    db 0x00;현재부터 510 address 까지 1바이트로 선언
                                ; 현재 위치에서 510 address 까지 반복수행하여 0x00으로 채움

%endrep 
;반복문의 끝 