rm -f disk.img
printf "\n###Preparing Disk image...###\n"
sudo dd if=/dev/zero of=disk.img bs=512 count=131072
sudo fdisk disk.img
printf "\n###Mounting raw disk image...###\n"
ld=$(sudo losetup --show -f ./disk.img)
ld2=$(sudo losetup --show -f ./disk.img -o 1048576)
printf "Allocated loop devices %s and %s\n" $ld $ld2
printf "\n###Partitioning and formatting disk image...###\n"
sudo mke2fs -t ext2 -b 1024 -O none -L DuckOS $ld2
sudo mount $ld2 /mnt
sudo chmod 777 ./disk.img
sudo chown $USER: ./disk.img
printf "\n###Installing GRUB...###\n"
sudo grub-install --directory=./grub/usr/lib/grub/i386-pc --root-directory=/mnt --no-floppy --modules="normal part_msdos ext2 multiboot" $ld
printf "\n###Unmounting...###\n"
sudo umount /mnt
sudo losetup -d $ld
sudo losetup -d $ld2
printf "Deallocated loop devices %s and %s\n" $ld $ld2
