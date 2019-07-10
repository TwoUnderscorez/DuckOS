copy_filesystem() {
    printf "\n###Mounting disk image...###\n"
    ld=$(sudo losetup --show -f ./disk.img -o 1048576)
    printf "Allocated loop device %s\n" $ld
    (set -x; sudo mount $ld /mnt)
    printf "\n###Copying filesystem...###\n"
    (set -x; sudo rm -rf /mnt/*)
    (set -x; sudo cp -R ./filesystem/. /mnt)
    printf "\n###Unmounting...###\n"
    sudo umount /mnt
    sudo losetup -d $ld
    printf "Deallocated loop device %s\n" $ld
}

compile() {
    printf "\n###Compiling and linking...###\n"
    (set -x; /usr/bin/make -C ./src all)
}

copy_kernel() {
    printf "\n###Copying kernel and cleaning...###\n"
    (set -x; cp ./src/kernel.sys ./filesystem/kernel/kernel.sys)
    (set -x; /usr/bin/make -C ./src clean)
}

run() {
    printf "\n###Running QEMU...###\n"
    export DISPLAY=:0
    (set -x; qemu-system-i386 -d int -s -S -m 4G -drive file=disk.img,format=raw)
}

make() {
    compile
    copy_kernel
    copy_filesystem
    run
}

make