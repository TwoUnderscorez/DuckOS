printf "\n###Mounting raw disk image...###\n"
ld=$(sudo losetup --show -f ./disk.img -o 1048576)
printf "Allocated loop device %s\n" $ld
(set -x; sudo dumpe2fs $ld)
(set -x; sudo mount $ld /mnt)
(set -x; sudo ls -R -i /mnt/)
sudo ls -a -i -R /mnt
sudo stat /mnt
sudo stat /mnt/kernel/kernel.sys
printf "\n###Unmounting...###\n"
sudo umount /mnt
sudo losetup -d $ld
printf "Deallocated loop device %s\n" $ld