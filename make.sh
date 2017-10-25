export DISPLAY=:0
cd src
printf "\n###Compiling and linking...###\n"
make
printf "\n###Copying kernel and cleaning...###\n"
cp kernel.bin ../iso/kernel/kernel.bin
make clean
cd ..
printf "\n###Preparing ISO image...###\n"
mkisofs -R -input-charset utf8 -b boot/grub/stage2_eltorito -boot-info-table -no-emul-boot -boot-load-size 4 -o os.iso iso
printf "\n###Running QEMU...###\n"
qemu-system-i386 -cdrom os.iso


