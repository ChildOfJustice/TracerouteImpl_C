#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/ip_icmp.h>


struct sockaddr_in source,dest;
char *address;

void print_ip_header(unsigned char* Buffer, int Size, char* getIpAddr)
{
    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)Buffer;
    iphdrlen =iph->ihl*4;

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;

    char *str = inet_ntoa(source.sin_addr);


    strcpy(getIpAddr, str);
    if(strcmp(address, str) == 0)
        return;

    strcpy(address, str);
//    strcpy(getIpAddr, address);
//    printf("%s\n", getIpAddr);
}

void print_icmp_packet(unsigned char* Buffer , int Size, char* getIpAddr)
{
    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)Buffer;
    iphdrlen = iph->ihl*4;

    struct icmphdr *icmph = (struct icmphdr *)(Buffer + iphdrlen);

    print_ip_header(Buffer, sizeof(iph), getIpAddr);
    memset(&Buffer, 0, sizeof(Buffer));
}


//struct icmp
//{
//    UCHAR   type;                //Types of
//    UCHAR   code;                //Code
//    USHORT  checksum;            //Check and verify
//    USHORT  id;                    //serial number
//    USHORT  sequence;            //Serial number
//    struct timeval timestamp;    //Timestamp
//};
//
//
////IP headed data structure
//struct ip
//{
//    //Host word sequence judgment
//    //#if __BYTE_ORDER == __LITTLE_ENDINA
//#if __BYTE_ORDER == __LITTLE_ENDIAN
//    UCHAR   hlen:4;            //Head length
//    UCHAR   version:4;        //version
//#endif
//
//#if __BYTE_ORDER == __BIG_ENDIAN
//    UCHAR   version:4;
//    UCHAR   hlen:4;
//#endif
//
//    UCHAR   tos;        //Service type
//    USHORT  len;        //Total length
//    USHORT  id;            //Identifier
//    USHORT  offset;        //Sign and film offset
//    UCHAR   ttl;        //Survival time
//    UCHAR   protocol;    //protocol
//    USHORT  checksum;    //Check and verify
//    struct in_addr  ipsrc;    //32-bit source IP address
//    struct in_addr  ipdst;    //32-bit IP address
//};