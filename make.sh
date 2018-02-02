export DISPLAY=:0
printf "\n###Unmounting...###\n"
sudo losetup -d /dev/loop6
sudo losetup -d /dev/loop7
sudo umount /mnt
rm disk.img
cd src
printf "\n###Compiling and linking...###\n"
make
printf "\n###Copying kernel and cleaning...###\n"
cp kernel.sys ../filesystem/kernel/kernel.sys
make clean
cd ..
printf "\n###Preparing Disk image...###\n"
sudo dd if=/dev/zero of=disk.img bs=512 count=131072
sudo fdisk disk.img
printf "\n###Mounting raw disk image...###\n"
sudo losetup /dev/loop6 disk.img
sudo losetup /dev/loop7 disk.img -o 1048576
printf "\n###Partitioning and formatting disk image...###\n"
sudo mkfs.fat -F 32 /dev/loop7 
sudo mount /dev/loop7 /mnt
printf "\n###Installing GRUB...###\n"
sudo dd if=./filesystem/boot/stage1 of=/dev/loop6 bs=512 count=1
sudo dd if=./filesystem/boot/stage2 of=/dev/loop6 bs=512 seek=1
printf "\n###Copying filesystem...###\n"
cp -R ./filesystem/. /mnt
sudo chmod 777 ./disk.img
sudo chown :ronyu ./disk.img
printf "\n###Unmounting...###\n"
sudo losetup -d /dev/loop6
sudo losetup -d /dev/loop7
sudo umount /mnt
printf "\n###Running QEMU...###\n"
qemu-system-i386 -hda disk.img `#-d cpu_reset`