savedcmd_/home/edward/Documents/vMem/kernel_modules/test.ko := ld -r -m elf_x86_64 -z noexecstack --build-id=sha1  -T scripts/module.lds -o /home/edward/Documents/vMem/kernel_modules/test.ko /home/edward/Documents/vMem/kernel_modules/test.o /home/edward/Documents/vMem/kernel_modules/test.mod.o;  make -f ./arch/x86/Makefile.postlink /home/edward/Documents/vMem/kernel_modules/test.ko