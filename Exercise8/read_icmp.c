//Name - Ankur Vineet
//Id - 2018H1030144P

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<netinet/ip_icmp.h>
#include<netinet/icmp6.h>
#include<netinet/ip6.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define BUFSIZE 1500

struct in6_pktinfo {
       struct in6_addr ipi6_addr;    /* src/dst IPv6 address */
       unsigned int ipi6_ifindex; /* send/recv interface index */
};


int main(int argc, char **argv)
{
	int sock,len,size;
	int icmp_version = 4;
	if(argc == 2)
	{
		int ver = atoi(argv[1]);
		if(ver == 4 || ver == 6)
			icmp_version = ver;
		else
		{
			printf("Invalid ICMP Version\n");
			return -1;
		}
	}
	char recvbuf[BUFSIZE];
	char controlbuf[64000];
	struct iovec iov;
	struct msghdr msg;
	iov.iov_base = recvbuf;
	iov.iov_len = sizeof(recvbuf);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = controlbuf;
	msg.msg_controllen = 64000;//sizeof(controlbuf);
	if(icmp_version == 4)
	{
		if((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
			perror("socket: ");
		printf("ICMPV4 Output\n");
		
	}
	if(icmp_version == 6)
	{
		if((sock = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) < 0)
			perror("socket: ");
		printf("ICMPV6 Output\n");
	}
	int on = 1;
	//setsockopt(sock, IPPROTO_IPV6, IPV6_HOPLIMIT, &on, sizeof(on));
	setsockopt (sock, IPPROTO_IPV6, IPV6_RECVPKTINFO, &on,sizeof(on));
	while(1)
	{
		msg.msg_controllen = sizeof(controlbuf);
		struct cmsghdr *cmsg;
		if((len = recvmsg(sock, &msg, 0)) < 0)
		{
			printf("Try running with Super User Priviledge\n");
			return -1;
		}
		int hlenl, icmplen;
		struct ip *ip;
 		struct icmp *icmp;
		struct icmp6_hdr *icmp6;
		struct in_addr src,dst;
		if(icmp_version == 4)
		{
			ip = (struct ip *) recvbuf;
			hlenl = ip->ip_hl << 2;
			src = ip->ip_src;
			dst = ip->ip_dst;
			printf("Src IP: %s\n",inet_ntoa(src));
			printf("Dst IP: %s\n",inet_ntoa(dst));
			if (ip->ip_p != IPPROTO_ICMP)
			{
				printf("Not An ICMP Message\n");
				continue;
			}
			icmp = (struct icmp *) (recvbuf + hlenl);
			if ( (icmplen = len - hlenl) < 8)
			{
				printf("Invalid ICMP Packet \n");
				continue;
			}
			printf("ICMP Type: %d\n",icmp->icmp_type);
			printf("ICMP Code: %d\n",icmp->icmp_code);
			if (icmp->icmp_type == ICMP_ECHOREPLY) 
			{
				if (icmplen < 16)
				{
					printf("ICMP Length Less Than Expected\n");
					continue;
				}
				printf("ECHO Reply: ");
				printf("Id -> %d ",icmp->icmp_id);
				printf("Seq No. -> %d\n",icmp->icmp_seq);
			}
			printf("----------------------------------------------------\n");
		}
		if(icmp_version == 6)
		{
			icmp6 = (struct icmp6_hdr *)recvbuf;
			/*
			for (cmsg = CMSG_FIRSTHDR (&msg); cmsg != NULL; cmsg = CMSG_NXTHDR (&msg, cmsg)) 
			{
				if (cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_PKTINFO) 
				{
					struct in6_pktinfo *ip6src;
				        ip6src = (struct in6_pktinfo *) CMSG_DATA (cmsg);
					struct in6_addr ip6addr;
					ip6addr = ip6src->ipi6_addr;
					printf("Src IP: %s\n",ip6addr.s6_addr);
					//char *ip6src;
					//ip6src = (char *) CMSG_DATA(cmsg);
					//printf("Src IP: %s\n",ip6src);
					break;
				}
			}*/
			printf("ICMP Type: %d\n",icmp6->icmp6_type);
			printf("ICMP Code: %d\n",icmp6->icmp6_code);
			if( icmp6->icmp6_type == ICMP6_ECHO_REPLY)
			{
				printf("ECHO Reply: ");
				printf("Id -> %d ",icmp6->icmp6_id);
				printf("Seq No. -> %d\n",icmp6->icmp6_seq);
			}	
			printf("----------------------------------------------------\n");
		}	
	}
	return 0;
}
