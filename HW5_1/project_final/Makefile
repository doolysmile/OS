all: BootLoader Kernel32 Kernel64 Utility Disk.img

BootLoader:
	@echo 
	@echo ============== Build Boot Loader ===============
	@echo 
	
	make -C 00.BootLoader

	@echo 
	@echo =============== Build Complete ===============
	@echo 

Kernel32:
	@echo 
	@echo ============== Build 32bit Kernel ===============
	@echo 
	
	make -C 01.Kernel32

	@echo 
	@echo =============== Build Complete ===============
	@echo 
# IA-32e 모드 커널 이미지를 빌드하기 위해 IA-32e 모드 디렉터리에서 make 실행
Kernel64:
	@echo 
	@echo ============== Build 64bit Kernel ===============
	@echo 
	
	make -C 02.Kernel64

	@echo 
	@echo =============== Build Complete ===============
	@echo 
	
Utility:
	@echo 
	@echo =========== Utility Build Start ===========
	@echo 

	make -C 04.Utility

	@echo 
	@echo =========== Utility Build Complete ===========
	@echo
HDD.img:
	qemu-img create HDD.img 20M
# OS 이미지 생성
Disk.img: 00.BootLoader/BootLoader1.bin 00.BootLoader/BootLoader2.bin 01.Kernel32/Kernel32.bin 02.Kernel64/Kernel64.bin
	@echo 
	@echo =========== Disk Image Build Start ===========
	@echo 

	./ImageMaker.exe $^

	@echo 
	@echo ============= All Build Complete =============
	@echo


run:
	# qemu-system-x86_64 -L . -fda Disk.img -m 64 -rtc base=localtime -M pc
	qemu-system-x86_64 -L . -m 64 -fda Disk.img -hda HDD.img -localtime -M pc -rtc base=localtime

clean:
	make -C 00.BootLoader clean
	make -C 01.Kernel32 clean
	make -C 02.Kernel64 clean
	make -C 04.Utility clean
	rm -f Disk.img
	rm -f ImageMaker.exe
