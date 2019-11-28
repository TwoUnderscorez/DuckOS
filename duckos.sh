copy_filesystem() {
    printf "\n###Mounting disk image...###\n"
    ld=$(sudo losetup --show -f ./disk.img -o 1048576)
    printf "Allocated loop device %s\n" $ld
    (set -x; sudo mount $ld /mnt)
    printf "\n###Copying filesystem...###\n"
    (set -x; sudo rm -rf /mnt/*)
    (set -x; sudo cp -R ./filesystem/. /mnt)
    (set -x; sudo cp ./thirdparty /mnt/thirdparty)
    printf "\n###Unmounting...###\n"
    sudo umount /mnt
    sudo losetup -d $ld
    printf "Deallocated loop device %s\n" $ld
}

compile() {
    printf "\n###Compiling and linking...###\n"
    (set -x; /usr/bin/make all)
    return $?
}

copy_kernel() {
    printf "\n###Copying kernel and cleaning...###\n"
    (set -x; cp ./src/kernel.sys ./filesystem/kernel/kernel.sys)
    (set -x; /usr/bin/make -C ./src clean)
}

run() {
    printf "\n###Running QEMU...###\n"
    # -d int -S
    (set -x; qemu-system-i386 -serial pipe:guest -s -m 4G -drive file=disk.img,format=raw)
}

clean() {
    (set -x; /usr/bin/make clean > /dev/null 2>&1)
}

start_klog() {
    if [ ! -p "guest.out" ]
    then
        printf "\n###Creating named pipe for klog###\n"
        (set -x; mkfifo guest.in guest.out)
    fi
    printf "\n###Printing kernel log to stdout###\n"
    cat guest.out &
}

make() {
    compile
    local sts=$?
    if [ "$sts" -eq 2 ]; then
        echo "Make failed. Stop."
    else
        copy_kernel
        copy_filesystem
        start_klog
        run
    fi
    clean
}

make