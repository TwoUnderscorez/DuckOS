export DISPLAY=:0
printf "\n###Unmounting...###\n"
sudo umount /mnt
sudo losetup -d /dev/loop6
sudo losetup -d /dev/loop7
cd src
printf "\n###Compiling and linking...###\n"
make
printf "\n###Copying kernel and cleaning...###\n"
cp kernel.sys ../filesystem/kernel/kernel.sys
make clean
cd ..
printf "\n###Mounting disk image...###\n"
sudo losetup /dev/loop7 disk.img -o 1048576
sudo mount /dev/loop7 /mnt
printf "\n###Copying filesystem...###\n"
cp -R ./filesystem/. /mnt
printf "\n###Unmounting...###\n"
sudo umount /mnt
sudo losetup -d /dev/loop7
printf "\n###Running QEMU...###\n"
qemu-system-i386 -drive file=disk.img,format=raw `#-d cpu_reset`