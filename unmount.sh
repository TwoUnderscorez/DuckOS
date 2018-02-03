printf "\n###Unmounting...###\n"
(set -x; sudo umount /mnt)
(set -x; sudo losetup -d /dev/loop6)
(set -x; sudo losetup -d /dev/loop7)