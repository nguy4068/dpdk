#!/bin/bash
DPDK_VERSION="/home/gaurav/dpdk-stable-21.11.1"
OVS_VERSION="/home/gaurav/openvswitch-2.17.3"
export RTE_SDK="/home/gaurav/dpdk-stable-21.11.1"
export OVS_DIR="/home/gaurav/openvswitch-2.17.3"

export DPDK_DIR=$RTE_SDK
export RTE_TARGET="x86_64-native-linuxapp-gcc"
export DPDK_BUILD=$RTE_TARGET

cd "$OVS_DIR"

echo "Show br0 info:"
./utilities/ovs-vsctl show

echo "Showing the current flow configuration:"
./utilities/ovs-ofctl dump-flows br0
