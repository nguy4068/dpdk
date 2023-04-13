// Adapted from: https://github.com/DPDK/dpdk/blob/master/examples/skeleton/basicfwd.c
// by Thomas Edwards, Walt Disney Television

/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2015 Intel Corporation
 */
#include <pthread.h>

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <signal.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <unistd.h>
#include <time.h>

#define RX_RING_SIZE 1024
#define TX_RING_SIZE 1024

#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 32



void DumpHex(const void*, size_t);
void rx_packets(void);
void exit_stats(int);

uint64_t packet_count = 0;
time_t start_time;

uint16_t eth_len = sizeof(struct rte_ether_hdr);
uint16_t ip_len = sizeof(struct rte_ipv4_hdr);
uint16_t udp_len = sizeof(struct rte_udp_hdr);                     
uint16_t gtp_len = sizeof(struct rte_gtp_hdr);
    
    



static const struct rte_eth_conf port_conf_default = {
	.rxmode = {
//		.max_rx_pkt_len = RTE_ETHER_MAX_LEN,
	},
};


void delay(int number_of_milliseconds){int milli_seconds = number_of_milliseconds;
clock_t start_time =clock()+milli_seconds;
while( clock() < start_time ){ int x;}
}

char* toIP( uint32_t ip){
struct in_addr ip_addr;
    ip_addr.s_addr = ip;
    //printf("%s", inet_ntoa(ip_addr));
    return inet_ntoa(ip_addr);
}



pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t packet_counter_lock = PTHREAD_MUTEX_INITIALIZER;
struct cond_t{
	pthread_cond_t cond;//= PTHREAD_COND_INITIALIZER;
};

//struct

struct cond_t *conds;
int numActivePDUSessions = 0;
void addUEPDUSessions(){
	if (numActivePDUSessions == 0){
		conds = malloc(sizeof(struct cond_t)*10);	
	}
}

// A linked list (LL) node to store a queue entry
struct QNode {
    struct rte_mbuf *buf;
    struct QNode* next;
};
 
// The queue, front stores the front node of LL and rear
// stores the last node of LL
struct Queue {
    struct QNode *front, *rear;
};
 
// A utility function to create a new linked list node.
struct QNode* newNode(struct rte_mbuf *buf)
{
    struct QNode* temp
        = (struct QNode*)malloc(sizeof(struct QNode));
    temp->buf = buf;
    temp->next = NULL;
    return temp;
}
 
// A utility function to create an empty queue
struct Queue* createQueue()
{
    struct Queue* q
        = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}
 
// The function to add a key k to q
void enQueue(struct Queue* q, struct rte_mbuf *buf)
{
    // Create a new LL node
    struct QNode* temp = newNode(buf);
 
    // If queue is empty, then new node is front and rear
    // both
    if (q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }
 
    // Add the new node at the end of queue and change rear
    q->rear->next = temp;
    q->rear = temp;
}
 
// Function to remove a key from given queue q
void deQueue(struct Queue* q)
{
    // If queue is empty, return NULL.
    if (q->front == NULL)
        return;
 
    // Store previous front and move front one node ahead
    struct QNode* temp = q->front;
 
    q->front = q->front->next;
 
    // If front becomes NULL, then change rear also as NULL
    if (q->front == NULL)
        q->rear = NULL;
 
    free(temp);
}


struct rte_mbuf* peek(struct Queue* q)
{
    // If queue is empty, return NULL.
    if (q->front == NULL)
        return NULL;
 
    // Store previous front and move front one node ahead
    struct QNode* temp = q->front;
 
    //q->front = q->front->next;
 
    // If front becomes NULL, then change rear also as NULL
    //if (q->front == NULL)
    //    q->rear = NULL;
    //free(temp);
    return temp->buf;
}


struct UE_properties_t {
 int teid;
 char *payload;
 pthread_cond_t cond;
 pthread_mutex_t lock;
 int isEmpty;
 struct Queue* q;
};

struct UE_properties_t UE_properties[3];

void *PDUSessionThread(void *arg){
	//int teid = *((int *)arg)
	int OFFSET = eth_len + ip_len + udp_len + gtp_len;
	struct UE_properties_t property = *((struct UE_properties_t *)arg);
	int teid=property.teid;
	UE_properties[teid].q = createQueue();
	printf("Running teid %d \n",teid);
	//This PDU session thread will run infinitely until being shut down
	while (true){
	        pthread_mutex_lock(&UE_properties[teid].lock);
	        //get your data here
	        int isEmpty=UE_properties[teid].isEmpty;
	        
	        //printf("%d",UE_properties[teid].isEmpty);
	        //if(isEmpty==0){
	        //printf("Waiting %d\n",teid);
		//pthread_cond_wait(&property.cond, &property.lock);
		//printf("Hi! I am teid %d \n",teid);
		
		struct rte_mbuf* buf = peek(UE_properties[teid].q);
		pthread_mutex_unlock(&UE_properties[teid].lock);
		
		if(buf==NULL){ continue;}
		
		//printf("processng packet %d\n",teid);
                char *gtp_payload = (char *) (rte_pktmbuf_mtod(buf, char*)+ OFFSET );
		
		//int next_index = property.next;
		//char *payload = property.incoming_packets[next_index];
		//UE_properties[teid].isEmpty=1;
		//}
		rte_pktmbuf_free(buf);
		deQueue(UE_properties[teid].q);
		
		//printf("Do work\n");
		//delay(500);
		usleep(50000);
		//printf("Done work\n");
		pthread_mutex_lock(&packet_counter_lock);
		packet_count++;
		pthread_mutex_unlock(&packet_counter_lock);
		
		//printf("Packet processes at UE %d\n", teid);
		
		//check if the tunnel has SDAP property
		//SDAP call back, or PDCP call back
	}
	
}

void createThreadForUE(){
        pthread_t tid[2];
	UE_properties[1].teid = 1;
	UE_properties[1].isEmpty = 1;
	UE_properties[1].cond=(pthread_cond_t)PTHREAD_COND_INITIALIZER;
	pthread_create(&tid[0],NULL,PDUSessionThread, (void *)&UE_properties[1]);
	UE_properties[2].teid = 2;
	UE_properties[2].isEmpty = 1;
	pthread_create(&tid[1],NULL,PDUSessionThread, (void *)&UE_properties[2]);
}




void process_pkt(struct rte_mbuf *bufs, size_t size) {
    
    int OFFSET = eth_len + ip_len + udp_len;

    /*
    struct rte_ether_hdr *eth_hdr;			
    eth_hdr = rte_pktmbuf_mtod(bufs,struct rte_ether_hdr *);
    printEthHeader(eth_hdr);
                                
    struct rte_ipv4_hdr *ip_hdr;
    uint16_t eth_len = sizeof(struct rte_ether_hdr);
    ip_hdr = (struct rte_ipv4_hdr *)(rte_pktmbuf_mtod(bufs, char *) + eth_len);
    //https://stackoverflow.com/questions/63392719/how-do-we-can-access-to-payload-of-received-packets-in-dpdk-18-11-9
    printIpHeader(ip_hdr);
                                
    uint16_t ip_len = sizeof(struct rte_ipv4_hdr);
                               
    struct rte_udp_hdr *udp_hdr;
    uint16_t udp_len = sizeof(struct rte_udp_hdr);
                                
    //udp_hdr = (struct rte_udp_hdr *)(rte_pktmbuf_mtod(bufs[i], char *) + eth_len+ip_len);
    udp_hdr = (struct rte_udp_hdr *)((unsigned char *)ip_hdr + sizeof(struct rte_ipv4_hdr));
    printf("src_port: --> %d\n",ntohs(udp_hdr->src_port));
    printf("Dgram Len: --> %d\n",ntohs(udp_hdr->dgram_len));
                                
    //https://doc.dpdk.org/api/structrte__gtp__hdr.html
    */
    struct rte_gtp_hdr *gtp_hdr;
                                
    gtp_hdr = (struct rte_gtp_hdr *)(rte_pktmbuf_mtod(bufs, char *) + OFFSET);                                
    //printf("teid--> %d\n",ntohl(gtp_hdr->teid));
    int teid = ntohl(gtp_hdr->teid);
    
    if(teid>2) {return;}  //skip invalid packet
				
                                pthread_mutex_lock(&UE_properties[teid].lock);
				//UE_properties[teid].incoming_packets[UE_properties[teid].next] = gtp_payload;
				//UE_properties[teid].next++;
				enQueue(UE_properties[teid].q, bufs);
				UE_properties[teid].isEmpty=0;
				pthread_mutex_unlock(&UE_properties[teid].lock);
				//pthread_cond_signal(&UE_properties[teid].cond);
				
				

}

void printEthHeader(struct rte_ether_hdr * eth){

printf("Ethernet:\n");
printf("  src: %d\n",(eth->src_addr));
printf("  dst: %d\n",(eth->dst_addr));
}

void printIpHeader(struct rte_ipv4_hdr * ip){

printf("IP:\n");

//toIP(ip->src_addr);toIP(ip->dst_addr);
printf("  src: %s\n",toIP(ip->src_addr));
printf("  dst: %s\n",toIP(ip->dst_addr));
printf("Total len %d \n",ntohs(ip->total_length));
}
void DumpHex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%02X ", ((const unsigned char*)data)[i]);
		if (((const unsigned char*)data)[i] >= ' ' && ((const unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((const unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}

/*
 * Initializes a given port using global settings and with the RX buffers
 * coming from the mbuf_pool passed as a parameter.
 */
static inline int
port_init(uint16_t port, struct rte_mempool *mbuf_pool)
{
	struct rte_eth_conf port_conf = port_conf_default;
	const uint16_t rx_rings = 1, tx_rings = 1;
	uint16_t nb_rxd = RX_RING_SIZE;
	uint16_t nb_txd = TX_RING_SIZE;

	int retval;
	uint16_t q;

    struct rte_eth_dev_info dev_info;
    struct rte_eth_txconf txconf;

    rte_eth_dev_info_get(port, &dev_info);
    if (dev_info.tx_offload_capa & DEV_TX_OFFLOAD_MBUF_FAST_FREE)
        port_conf.txmode.offloads |=
                DEV_TX_OFFLOAD_MBUF_FAST_FREE;


	if (!rte_eth_dev_is_valid_port(port))
		return -1;

	/* Configure the Ethernet device. */
	retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
	if (retval != 0)
		return retval;

	retval = rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rxd, &nb_txd);
	if (retval != 0)
		return retval;

	/* Allocate and set up 1 RX queue per Ethernet port. */
	for (q = 0; q < rx_rings; q++) {
		retval = rte_eth_rx_queue_setup(port, q, nb_rxd,
				rte_eth_dev_socket_id(port), NULL, mbuf_pool);
		if (retval < 0)
			return retval;
	}


    txconf = dev_info.default_txconf;
    txconf.offloads = port_conf.txmode.offloads;


    //Allocate and set up 1 TX queue
    for (q = 0; q < tx_rings; q++) {
        retval = rte_eth_tx_queue_setup(port, q, nb_txd,
                                        rte_eth_dev_socket_id(port), &txconf);
        if (retval < 0)
            return retval;
    }


    /* Start the Ethernet port. */
	retval = rte_eth_dev_start(port);
	if (retval < 0)
		return retval;

	/* Display the port MAC address. */
	struct rte_ether_addr addr;
	rte_eth_macaddr_get(port, &addr);
	
	printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
			   " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
			port,
			addr.addr_bytes[0], addr.addr_bytes[1],
			addr.addr_bytes[2], addr.addr_bytes[3],
			addr.addr_bytes[4], addr.addr_bytes[5]);

	/* Enable RX in promiscuous mode for the Ethernet device. */
	rte_eth_promiscuous_enable(port);

	return 0;
}

// recieve packets
void rx_packets(void)
{
	uint16_t port;
	int i;
	int start = 100;
	int end_track = 150;
	int track = start;
	FILE *fd = fopen("output2.csv","w+");

	printf("\nCore %u receiving packets. [Ctrl+C to quit]\n",
			rte_lcore_id());

	/* Run until the application is quit or killed. */
	for (;;) {
		pthread_mutex_lock(&packet_counter_lock);
		if(packet_count==track){
            double end = clock();
            double time = (end-(double)start_time)/CLOCKS_PER_SEC;
			printf("Done receiving %d packets now restart\n", packet_count);
			fprintf(fd, "%d, %f\n", track, time);
            track = track + 5;
			if (track > end_track){
				break;
			}
			packet_count = 0;
                        
        }
		pthread_mutex_unlock(&packet_counter_lock);
		start_time = clock();
		RTE_ETH_FOREACH_DEV(port) {

			struct rte_mbuf *bufs[BURST_SIZE];
			const uint16_t nb_rx = rte_eth_rx_burst(port, 0,
					bufs, BURST_SIZE);

			if (unlikely(nb_rx == 0))
				continue;
                        
			//packet_count += nb_rx;
                        
            
			//printf("received %d packets:\n",nb_rx);

			for(i=0;i<nb_rx;++i){
                process_pkt(bufs[i],bufs[i]->pkt_len);
			}

		}
	}
	fclose(fd);
	exit(0);
}

void exit_stats(int sig)
{
	printf("Caught signal %d\n", sig);
	printf("Total received packets: %lu\n", packet_count);
	exit(0);
}

int main(int argc, char *argv[])
{
	struct rte_mempool *mbuf_pool;
	unsigned nb_ports;
	uint16_t portid;

	/* Initialize the Environment Abstraction Layer (EAL). */
	int ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");

	argc -= ret;
	argv += ret;

	nb_ports = rte_eth_dev_count_avail();
	printf("rte_eth_dev_count_avail()=%d\n",nb_ports);

	/* Creates a new mempool in memory to hold the mbufs. */
	mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NUM_MBUFS * nb_ports,
		MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());

	if (mbuf_pool == NULL)
		rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");

	/* Initialize all ports. */
	RTE_ETH_FOREACH_DEV(portid)
		if (port_init(portid, mbuf_pool) != 0)
			rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu16 "\n",
					portid);

	signal(SIGINT, exit_stats);
	
	createThreadForUE(); // UE thread
	
	start_time=clock();
	rx_packets();

	return 0;
}
