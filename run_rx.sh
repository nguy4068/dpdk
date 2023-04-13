#command for rx

echo "Compiling"
bash -c "cd minimal_rx_edited && make"
bash -c "cd minimal_rx_edited_2ue && make"

echo " "
echo "1UE Running..."
echo " "
sudo ./minimal_rx_edited/build/minimal_rx --single-file -m 512 --file-prefix minimal_rx  --no-pci --vdev=net_virtio_user3,mac=00:00:00:00:00:03,path=/usr/local/var/run/openvswitch/vhost-user3 --proc-type=primary --file-prefix=rte_10000

echo " "
echo "2UE Running..."
echo " "

sudo ./minimal_rx_edited_2ue/build/minimal_rx --single-file -m 512 --file-prefix minimal_rx  --no-pci --vdev=net_virtio_user3,mac=00:00:00:00:00:03,path=/usr/local/var/run/openvswitch/vhost-user3 --proc-type=primary --file-prefix=rte_10000
