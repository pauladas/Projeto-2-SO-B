dmesg -c
clear
sudo eject /media/vinicius/disk
sudo umount /mnt/point1
cd /home/vinicius/Desktop/Projeto-2-SO-B/minixMod
sleep 0.5
sudo rmmod minix
sudo make -C /lib/modules/$(uname -r)/build M=$(pwd) modules
sleep 0.5
sudo insmod minix.ko key="abcd"
sleep 0.5
lsmod | grep minix
sleep 0.5
sudo mount -t minix /dev/loop1 /mnt/point1/
dmesg -c
