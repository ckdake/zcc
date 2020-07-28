#include <stdio.h>   
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <netinet/in.h>
//#include <common.h>
#include <netpacket/packet.h>
#include <sys/socket.h>
#include <endian.h>
#define MTU 1500

/* a helper method to calculate the checksum in headers after 
* fragmentation
*/

unsigned short csum (unsigned short *buf, int nwords)
{
  unsigned long sum;
  for (sum = 0; nwords > 0; nwords--)
    sum += *buf++;
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return ~sum;
}

/* this method divides the packet into two, places the first packet back
* to the queue, and send the second packet manually. 
*/
int main(void * packet)  
{


 struct in_addr saddr;
 unsigned char *first_packet, *second_packet;


 if(sizeof(packet)<=MTU)
    return 0;

 struct iphdr *ip_header = (struct iphdr *)(strncpy((char *)malloc(14),packet,14));
 ip_header->tot_len = htons(MTU);
 ip_header->frag_off = ip_header->frag_off & htons(0x0000);
 ip_header->frag_off = ip_header->frag_off | htons(0x2000);
 ip_header->check = htons(0);
 char *ip_header_dup = strdup((char *)ip_header);
 saddr.s_addr = (unsigned long)ip_header-> saddr;
// daddr.s_addr = (unsigned long)ip_header-> daddr;  

 struct tcphdr *tcp_header = (struct tcphdr *)(strncpy((char *)malloc(20), packet+14, 20));
 tcp_header->check = htons(0);
 char *tcp_header_dup = strdup((char *)tcp_header);
 
 first_packet = strncpy((char *)malloc(MTU),packet+34,MTU-34);

// the dup values are used to calculate the checksum
 ip_header_dup = strncat(ip_header_dup,tcp_header_dup,20);
 ip_header_dup = strncat(ip_header_dup, first_packet, sizeof(first_packet)); 

 ip_header->check = htons(csum((short *)ip_header_dup,sizeof(ip_header_dup)/4));
 tcp_header->check = htons(csum((short *)ip_header_dup,sizeof(ip_header_dup)/4)); 
 ip_header_dup = strdup((char *)ip_header);
 tcp_header_dup = strdup((char *)tcp_header);
 //char *buf = (char *)malloc(MTU);
 ip_header_dup=strncat(ip_header_dup, tcp_header_dup, 20);
 ip_header_dup=strncat(ip_header_dup, first_packet, sizeof(first_packet));
 memcpy(first_packet, ip_header_dup, MTU); 
  
//2nd packet
 ip_header->tot_len = (htons)((unsigned short)(sizeof(packet))-MTU);
 ip_header->frag_off = ip_header->frag_off & htons(0x0000);
 ip_header->frag_off = ip_header->frag_off | htons((unsigned short)MTU);
 ip_header->check = htons(0);
  
 tcp_header->check = htons(0); 
 
 ip_header_dup = strdup((char *)ip_header);
 tcp_header_dup = strdup((char *)tcp_header);
 
 second_packet = strncpy((char *)malloc(sizeof(packet)-MTU),packet+MTU,sizeof(packet)-MTU);
 ip_header_dup = strncat(ip_header_dup,tcp_header_dup,20);
 ip_header_dup = strncat(ip_header_dup, second_packet, sizeof(second_packet));

 ip_header->check = htons(csum((short *)ip_header_dup,sizeof(ip_header_dup)/4));
 tcp_header->check = htons(csum((short *)ip_header_dup,sizeof(ip_header_dup)/4)); 
 
 ip_header_dup = strdup((char *)ip_header);
 tcp_header_dup = strdup((char *)tcp_header);
 ip_header_dup=strncat(ip_header_dup, tcp_header_dup, 20);
 ip_header_dup=strncat(ip_header_dup, second_packet, sizeof(second_packet));

 //replace the original packet by the first packet, and send the second one
 packet = first_packet; 
 int s = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
 //struct sockaddr_in sin;
			
 //sin.sin_family = AF_INET;
 //sin.sin_port = htons (25);
 //sin.sin_addr.s_addr = (struct in_addr *)ip_header->daddr;
// sin.sin_addr.s_addr = (struct in_addr)ip_header->daddr;
 int i = sendto(s, second_packet, sizeof(second_packet), 0, (struct sockaddr *) &saddr, sizeof (saddr));
 if(i<sizeof(second_packet))
    return 0;
 else 
   return 1; 
 }
