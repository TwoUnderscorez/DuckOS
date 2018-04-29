# DuckOS
A simple operating system for ducks.

# Compilation instructions
### Install stuff

`# apt update && apt upgrade`

`# apt install qemu mkisofs make gcc nasm`

### Prepare the image:

`# ./prepare_image.sh`

make a new primary partition (n then p then enter for defaults)
also make the parition bootable by entering a then write the chenges
to the image with w.

### Actually running DuckOS 
#### compile, copy fs and run:

`# ./make.sh`

#### copy fs and run:

`# ./run.sh`
# Video
[Watch Demo](https://1drv.ms/v/s!AuOa6AuDFR2egqMCWBcWFKsGTR2mng)

# arbel03's OS
An operating system similar to mine, but written in rust, utilizes segmentation and implements FAT32, [click here to check it out!](https://github.com/arbel03/os)

# DuckOS Screenshots

### Kernel Heap
![Kernel Heap](https://user-images.githubusercontent.com/25303006/35394829-68be0952-01f1-11e8-9d00-018af7fac4c7.png)
### Page frame allocator
![Page frame allocator](https://user-images.githubusercontent.com/25303006/35394879-8da57ab6-01f1-11e8-8800-67ffb3e567d4.png)
### Multitasking in userland
![Multitasking in userland](https://planq.io/DuckOS/screenshots/FCFS.png)
### Printing filesystem recursively
![Printing filesystem recursively](https://planq.io/DuckOS/screenshots/print-R-fs.png)
