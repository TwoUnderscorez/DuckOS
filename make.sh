cd src
printf "\n###Compiling and linking...###\n"
make
printf "\n###Copying kernel and cleaning...###\n"
(set -x; cp kernel.sys ../filesystem/kernel/kernel.sys)
make clean
cd ..
sudo bash ./run.sh