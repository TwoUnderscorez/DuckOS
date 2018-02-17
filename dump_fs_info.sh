printf "\n###Mounting raw disk image...###\n"
(set -x; sudo losetup /dev/loop7 disk.img -o 1048576)
(set -x; sudo dumpe2fs /dev/loop7)
(set -x; sudo mount /dev/loop7 /mnt)
(set -x; sudo ls -R -i /mnt/)
sudo ls -a -i -R /mnt
sudo stat /mnt
sudo stat /mnt/kernel/kernel.sys
sudo bash ./unmount.sh