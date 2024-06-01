i386-elf-gcc -I/home/maro/Documents/maroOS/maroOS/kernel/include -ffreestanding -m32 -c "../kernel/include/mstd.c" -o "../obj/mstd.o"

i386-elf-gcc -I/home/maro/Documents/maroOS/maroOS/kernel/include -ffreestanding -m32 -g -c "../kernel/kernel.c" -o "../obj/kernel.o"

nasm "../asm/kernel_entry.asm" -f elf -o "../obj/kernel_entry.o"

i386-elf-ld -o "../bin/kernel.bin" -Ttext 0x1000 "../obj/kernel_entry.o" "../obj/kernel.o" "../obj/mstd.o" --oformat binary

nasm "../asm/boot.asm" -f bin -o "../bin/boot.bin"

cat "../bin/boot.bin" "../bin/kernel.bin" > "../bin/everything.bin"

cat "../bin/everything.bin" "../bin/zero.bin" > "../maroOS.bin"
