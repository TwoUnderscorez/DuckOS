printf "\n###Mounting disk image...###\n"
ld=$(sudo losetup --show -f ./disk.img -o 1048576)
printf "Allocated loop device %s\n" $ld
(set -x; sudo mount $ld /mnt)
printf "\n###Copying filesystem...###\n"
(set -x; sudo rm -rf /mnt/*)
(set -x; cp -R ./filesystem/. /mnt)
printf "\n###Unmounting...###\n"
sudo umount /mnt
sudo losetup -d $ld
printf "Deallocated loop device %s\n" $ld
