import scapy

import scapy_layer_gtp
from scapy.all import *
from scapy.utils import rdpcap

import threading
import time
import argparse

parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('--delay', type=float,default=0.1,
                    help='an integer for the accumulator')

args = parser.parse_args()
print(args.delay)

pkts=rdpcap("enb_pcap.pcap")  # could be used like this rdpcap("filename",500) fetches first 500 pkts

#https://stackoverflow.com/questions/33740583/writing-to-a-pcap-with-scapy

def write(pkt):
    wrpcap('filtered.pcap', pkt, append=True)  #appends packet to output file

i=0

for pkt in pkts:
     #pkt[Ether].src= new_src_mac  # i.e new_src_mac="00:11:22:33:44:55"
     #pkt[Ether].dst= new_dst_mac
     #pkt[IP].src= new_src_ip # i.e new_src_ip="255.255.255.255"
     #pkt[IP].dst= new_dst_ip
     #pkt.show()
     
     
        if pkt[IP].src == "10.80.95.10" and pkt.haslayer(UDP): 
            if i%2==0: 
                pkt["GTPHeader"].TEID=1  #change TEID
            else: 
                pkt["GTPHeader"].TEID=2  #change TEID
          #sendp(pkt, count=1, iface="vm1") #sending packet at layer 2
            i+=1     



def thread_function(pkt):
    sendp(pkt, iface="vm1") #sending packet at layer 2
        
    

while True:
    for pkt in pkts:
        #sendp(pkt, iface="vm1", loop=1, inter=0.5) #sending packet at layer 2
        x = threading.Thread(target=thread_function, args=(pkt,))
        x.start()
        time.sleep(args.delay)
    

    # x.join()
        
