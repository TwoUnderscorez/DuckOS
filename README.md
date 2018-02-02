# DuckOS
DuckOS

# Compilation instructions
one time setup setup:

`# apt update && apt upgrade`

`# apt install qemu mkisofs make gcc nasm`

if for some reason the disk.img file is not there:

`# ./prepare_image.sh`

compile, copy fs and run:

`# ./make.sh`
