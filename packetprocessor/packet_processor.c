#include <linux/netfilter.h>
#include <libipq/libipq.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/netfilter_ipv4.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/tcp.h>

#define BUFSIZE 2048
#define DEBUG 0

#define bitset_elem(ptr,bit) ((unsigned char *)(ptr))[(bit)/8]

#include "zccencryption.h"

ipq_packet_msg_t *m = NULL;
struct ipq_handle *h= NULL;

/**
 * These are the calls to encrypt and decrypt
 */
struct enc_results_t * zcc_encrypt (int total_len, void *ip_checksum_ptr, unsigned char dst_addr[4], int src_port, int dst_port, void *tcp_checksum_ptr, void *payload_ptr, int unsigned payload_length);
struct enc_results_t * zcc_decrypt (int total_len, void *ip_checksum_ptr, unsigned char dst_addr[4], int src_port, int dst_port, void *tcp_checksum_ptr, void *payload_ptr, int unsigned payload_length);

static void cleanup() 
{
	if (h) {
		if (m) {
			ipq_set_verdict(h, m->packet_id, NF_DROP, 0, NULL);
			m = NULL;
		}
		ipq_destroy_handle(h);
	}

}

static void die() 
{
	if (h) {
		ipq_perror("passer");
	}
	cleanup();
	exit(1);
}

void sigproc()
{
	cleanup();
	exit(0);
}

void process_packets() 
{
	int status;
	struct enc_results_t * encrypt_result = NULL;
	unsigned char buf[BUFSIZE];

	h = ipq_create_handle(0, PF_INET);
	if (!h) {
		die();
	}

	status = ipq_set_mode(h, IPQ_COPY_PACKET, BUFSIZE);
	if (status < 0) {
		die();
	}

	do{
		status = ipq_read(h, buf, BUFSIZE, 0);
		if (status < 0) {
			die();
		}
		switch (ipq_message_type(buf)) {
	
			case NLMSG_ERROR:
				fprintf(stderr, "Received error message %d\n",
						ipq_get_msgerr(buf));
				break;

			case IPQM_PACKET: {
				m = ipq_get_packet(buf);

				if ((m->hook == NF_IP_LOCAL_OUT) || (m->hook == NF_IP_LOCAL_IN)) {

					/* Cast the IP Header from the raw packet */
					unsigned char *pkt1 = m->payload;
					struct iphdr *iph = ((struct iphdr *)m->payload);
					struct tcphdr *tcph;

					/* calculate the IP header length */
					int iphdr_size = ((int)(pkt1[0] & 0x0f))*4;
					int ippacket_size = (pkt1[2]<<8) + pkt1[3];

					/* Cast the TCP Header from the raw packet */
					struct tcphdr *tcp = (struct tcphdr *)(m->payload + (iph->ihl << 2));

					/* calculate the TCP header */
					tcph = (struct tcphdr *)(m->payload + iphdr_size);
					int tcphdr_size = (((int)pkt1[iphdr_size+12])>>4)*4;	
					int tcpdata_size = ippacket_size - (iphdr_size + tcphdr_size);
					int tcpdata_offset = iphdr_size + tcphdr_size;

					unsigned char src_addr[4], dst_addr[4];

					memcpy(src_addr, &iph->saddr,4);
					memcpy(dst_addr, &iph->daddr,4);

					int src_port = ntohs(tcph->source);
					int dst_port = ntohs(tcph->dest);

					if (DEBUG) {
						printf("Packet receivd!\n");
						printf("Packet length = %d!\n",m->data_len);
						printf("Packet address in the memory :\%X!\n",(unsigned int)(m->payload));
						printf("payload address in the memory :\%X!\n",(unsigned int)((m->payload)+tcpdata_offset));
						printf("\n src/dst port=%d:%d\n", src_port,dst_port) ;
						printf("IP Header size: %d\n", iphdr_size);
						printf("\n protocol = %d ",iph->protocol);
						printf("TCP Header size: %d\n", tcphdr_size);
						printf("Payload Size : %d\n", tcpdata_size);
						printf("Payload Offset : %d\n", tcphdr_size+iphdr_size);
						printf("TOTAL IP Packet size: %d\n", ntohs(iph->tot_len));
						printf("DATA: %s\n", (m->payload)+tcpdata_offset);
						printf("\n----------------------------------------------\n");
					}

					if (tcpdata_size > 0) {
						if(m->hook == NF_IP_LOCAL_OUT) {
							encrypt_result = zcc_encrypt(iph->tot_len, &iph->check, dst_addr, src_port, dst_port, &tcp->check, (m->payload)+tcpdata_offset, tcpdata_size);
						} else { /*  (m->hook == NF_IP_LOCAL_IN) */ 
							encrypt_result = zcc_decrypt(iph->tot_len, &iph->check, dst_addr, src_port, dst_port, &tcp->check, (m->payload)+tcpdata_offset, tcpdata_size); 
						}

						if (encrypt_result) {
							if (DEBUG) {
								fprintf(stderr, "old packet:");
								int i = 0;
								while (i<iphdr_size+tcphdr_size+tcpdata_size) {
									if ((i%4) == 0) {
										fprintf(stderr, "\n    ");
									}
									if ((i==iphdr_size || i==iphdr_size+tcphdr_size)) {
										fprintf(stderr, "----------\n    ");
									}
									fprintf(stderr, "%02x ", (m->payload)[i]);
									i++;
								}
								fprintf(stderr, "\n");
								printf("Old packet len = %d\n", m->data_len);
							}

							int int_len = iphdr_size+tcphdr_size+encrypt_result->size;
							iph->tot_len=htons(int_len);
							ippacket_size=int_len;
							tcpdata_size = encrypt_result->size;

							unsigned char *pkt = malloc(10000);
							if (DEBUG) {
								printf("Encrypted data = %s\n", encrypt_result->results);
							}	 
							// zero our local copy
							bzero(pkt,10000);

							// copy into our own, safe, reusable memory:
							int i = 0;
							for (i=0; i<m->data_len; i++) {
								pkt[i] = (m->payload)[i];
							}
							m->data_len=int_len;

							memcpy((char *)(pkt+tcpdata_offset),encrypt_result->results, encrypt_result->size);

							// BEGIN: compute the TCP checksum
							int total = 0; 
							i = iphdr_size; // the offset of the start of the TCP packet header
							int imax = iphdr_size + 16; // the offset of the TCP checksum field

							while (i < imax) {
								total += ((pkt[i] << 8) | pkt[i+1]);
								i += 2;
							}
							// Skip existing checksum.
							i = iphdr_size + 18;
							imax = ippacket_size;

							while (i < imax) {
								total += ((pkt[i] << 8) | pkt[i+1]);
								i += 2;
							}

							// add source and dest addresses, grouped as 16-byte integers
							total += (pkt[12] << 8) | pkt[13];
							total += (pkt[14] << 8) | pkt[15];
							total += (pkt[16] << 8) | pkt[17];
							total += (pkt[18] << 8) | pkt[19];
							total += 6; // add protocol number
							total += tcphdr_size+tcpdata_size; // add TCP packet length

							// fold to 16 bits
							while ((total & 0xffff0000) != 0) {
								total = (total & 0xffff) + (total >> 16);
							}
							total = ~total; // take one's complement

							// replace the old checksum value with the new one
							pkt[iphdr_size+16] = (total >> 8);
							pkt[iphdr_size+17] = total & 0xff;
							// END: compute the TCP checksum

							// BEGIN: compute the IP checksum
							i = 0;
							total = 0;

							// 10 is the offset of the checksum field itself
							while (i < 10) {
								total += ((pkt[i] << 8) | pkt[i+1]);
								i += 2;
							}

							i = 12; // skip existing checksum field
							while (i < iphdr_size) {
								total += ((pkt[i] << 8) | pkt[i+1]);
								i += 2;
							}

							// fold to 16 bits
							while ((total & 0xffff0000) != 0) {
								total = (total & 0xffff) + (total >> 16);
							}
							total = ~total; // one's complement

							// put the new checksum in place over the old one
							pkt[10] = (unsigned char)(total >> 8);
							pkt[11] = (unsigned char)(total & 0xff);
							// END: compute the IP checksum


							// print the bytes of the new, modified packet
							if (DEBUG) {
								fprintf(stderr, "new packet:");
								int i = 0;
								while (i<iphdr_size+tcphdr_size+tcpdata_size) {
									if ((i%4) == 0) {
										fprintf(stderr, "\n    ");
									}
									if ((i==iphdr_size || i==iphdr_size+tcphdr_size)) {
										fprintf(stderr, "----------\n    ");
									}
									fprintf(stderr, "%02x ", pkt[i]);
									i++;
								}
								fprintf(stderr, "\n");
							}

							status = ipq_set_verdict(h, m->packet_id, NF_ACCEPT, m->data_len, pkt);
						} else {
							fprintf(stderr, "Encryption operation failed. Perhaps someone is doing something nasty? \n");
							status = ipq_set_verdict(h, m->packet_id, NF_DROP, 0, NULL);
						}

					} else {
						status = ipq_set_verdict(h, m->packet_id, NF_ACCEPT, 0, NULL);
					}
				} else {	
					fprintf(stderr, "Received a packet from a forward queue, we don't know what to \n do with these, perhaps you have a bad configuration rule?\n");
					status = ipq_set_verdict(h, m->packet_id, NF_DROP, 0, NULL);
				}

				m = NULL;

				if (status < 0) {
					die();
				}
				break;
					  }

			default:
				  fprintf(stderr, "Unknown message type!\n");
				  break;
		}
	} while (1);

	ipq_destroy_handle(h);
}

int main(int argc, char **argv)
{
	signal(SIGINT, sigproc);
	signal(SIGQUIT, sigproc);
	process_packets();
	return 0;
}
