#!/bin/bash

sudo -i
echo 1024 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
sudo modprobe uio_pci_generic
cd ~/dpdk-stable-21.11.1/usertools
# first turn off the VMware Ethernet NIC
sudo ./dpdk-devbind.py -b uio_pci_generic 3b:00.0

