export DISPLAY=:0
cd src
printf "\n###Compiling and linking...###\n"
make
printf "\n###Copying kernel and cleaning...###\n"
cp kernel.sys ../iso/kernel/kernel.sys
make clean
cd ..
printf "\n###Preparing ISO image...###\n"
mkisofs -R -J -c boot/BOOT.CAT -input-charset utf8 -b boot/grub/stage2_eltorito -boot-info-table -no-emul-boot -boot-load-size 4 -o os.iso iso
printf "\n###Running QEMU...###\n"
qemu-system-i386 -m 4G -cdrom os.iso