i386-elf-gcc -ffreestanding -m32 -g -c "kernel/kernel.c" -o "obj/kernel.o"
nasm "asm/kernel_entry.asm" -f elf -o "obj/kernel_entry.o"
i386-elf-ld -o "bin/kernel.bin" -Ttext 0x1000 "obj/kernel_entry.o" "obj/kernel.o" --oformat binary
nasm "asm/nigger.asm" -f bin -o "bin/boot.bin"
cat "bin/boot.bin" "bin/kernel.bin" > "bin/everything.bin"
cat "bin/everything.bin" "bin/zero.bin" > "niggerOS.bin"