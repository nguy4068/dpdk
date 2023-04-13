
#command for rx
sudo ./minimal_rx/build/minimal_rx --single-file -m 512 --file-prefix minimal_rx  --no-pci --vdev=net_virtio_user3,mac=00:00:00:00:00:03,path=/usr/local/var/run/openvswitch/vhost-user3 --proc-type=primary --file-prefix=rte_10000

#command for tx
sudo ./minimal_tx/build/minimal_tx --single-file -m 512 --file-prefix minimal_tx --no-pci --vdev=net_virtio_user2,mac=00:00:00:00:00:02,path=/usr/local/var/run/openvswitch/vhost-user2 -- -m 00:00:00:00:00:03 -s 192.0.0.1 -d 192.0.0.2


# sudo ./Builddir/app/pktgen -l 0-4 -n 3 --single-file -m 512 --file-prefix pktgen --no-pci --vdev=net_virtio_user2,mac=00:00:00:00:00:02,path=/usr/local/var/run/openvswitch/vhost-user2 -- -T -P
