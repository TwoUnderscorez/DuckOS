printf "\n###Mounting disk image...###\n"
(set -x; sudo losetup /dev/loop7 disk.img -o 1048576)
(set -x; sudo mount /dev/loop7 /mnt)
printf "\n###Copying filesystem...###\n"
(set -x; sudo rm -rf /mnt/*)
(set -x; cp -R ./filesystem/. /mnt)
sudo bash ./unmount.sh
