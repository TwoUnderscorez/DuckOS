rm disk.img
printf "\n###Preparing Disk image...###\n"
sudo dd if=/dev/zero of=disk.img bs=512 count=131072
sudo fdisk disk.img
printf "\n###Mounting raw disk image...###\n"
sudo losetup /dev/loop6 disk.img
sudo losetup /dev/loop7 disk.img -o 1048576
printf "\n###Partitioning and formatting disk image...###\n"
sudo mkdosfs -F32 -f 2 /dev/loop7
sudo mount /dev/loop7 /mnt
sudo chmod 777 ./disk.img
sudo chown ronyu: ./disk.img
printf "\n###Installing GRUB...###\n"
sudo grub-install --target=i386-pc --root-directory=/mnt --no-floppy --modules="normal part_msdos fat multiboot" /dev/loop6
printf "\n###Unmounting...###\n"
sudo umount /mnt
sudo losetup -d /dev/loop6
sudo losetup -d /dev/loop7