#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<netdb.h>
#include <sys/poll.h>

#include "icmp_listener.h"

#define ICMP_SIZE (sizeof(struct icmp))
#define ICMP_ECHO 8 ////THERE WAS THE ERROR!!!!
#define ICMP_ECHOREPLY 0
#define NUM 43    //Number of messages sent
#define TIME_OUT_SEC 1

#define UCHAR unsigned char
#define USHORT unsigned short
#define UINT unsigned int


USHORT checkSum(USHORT *, int);        //Calculate checksum
float timediff(struct timeval *, struct timeval *);    //Calculation time difference
void pack(struct icmp *, int);        //Package an ICMP message
int unpack(char *, int , char *);    //Uncheck the received IP heat preservation
int sendAngGetTheIp(char *, int);

int main(int argc, char *argv[])
{
//    argv[1] = "8.8.8.8";

    struct hostent *host;
    struct icmp sendicmp;
    struct sockaddr_in from;
    struct sockaddr_in to;
    int sockfd;
    int nsend = 0;
    int nreceived = 0;
    in_addr_t inaddr;

    memset(&from, 0, sizeof(struct sockaddr_in));
    memset(&to, 0, sizeof(struct sockaddr_in));

    if (argc < 2)
    {
        printf("use : %s hostname/IP address \n", argv[0]);
        exit(1);
    }

    //Generate original socket
    //NEED PERMISSIONS!!!
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
    {
        printf("socket() error\n");
        exit(1);
    }

    //Set the destination address information
    to.sin_family = AF_INET;
    from.sin_family = AF_INET;

    // Judging the domain name or IP address (it doesn't seem to be implemented)
    if (inaddr = inet_addr(argv[1]) == INADDR_NONE)
    {
        printf("it is a domain name!: %s\n",argv[1]);
        //domain name
        if ((host = gethostbyname(argv[1])) == NULL)
        {
            printf("gethostbyname() error \n");
            exit(1);
        }
        to.sin_addr = *(struct in_addr *)host->h_addr_list[0];
        from.sin_addr = *(struct in_addr *)host->h_addr_list[0];
    }
    else
    {
        //IP address
        printf("setting IP addr: %s\n",argv[1]);
        to.sin_addr.s_addr = inaddr;
        // to.sin_addr.s_addr = inaddr;
        from.sin_addr.s_addr = inaddr;

        inaddr = inet_addr(argv[1]);
        // printf("setting IP addr: %s\n",argv[1]);
        to.sin_addr.s_addr = inaddr;
        from.sin_addr.s_addr = inaddr;
    }


    //Output domain name IP address information           inet_ntoa(to.sin_addr)
    printf("ping %s (%s) : %d bytes of data.\n", argv[1], inet_ntoa(to.sin_addr),
           (int)ICMP_SIZE);

    //Cycling send reception packet
    for (int ttl = 1; ttl < NUM; ttl++)
    {
        nsend++;
        memset(&sendicmp, 0, ICMP_SIZE);
        pack(&sendicmp, nsend);

        //printf("sending to: %s\n",inet_ntoa(to.sin_addr));
        setsockopt(sockfd,IPPROTO_IP,IP_TTL,(char *)&ttl,sizeof(ttl));

        if (sendto(sockfd, &sendicmp, ICMP_SIZE, 0, (struct sockaddr*)&to, sizeof(to)) == -1)
        {
            printf("sendto() error\n");
            continue;
        }



        char *ipaddr = (char *)malloc(65536);
        if(sendAngGetTheIp(ipaddr, TIME_OUT_SEC) == -1){
            printf("timeout, will increase ttl: %d\n", ttl);
            continue;
        }
        printf("ip address is: %s\n", ipaddr);
        fflush(stdout);





        if(strcmp(inet_ntoa(to.sin_addr), ipaddr) == 0){
            printf("Got there!!!\n");
            break;
        }

        //printf("receiving from: %s\n",inet_ntoa(from.sin_addr));
//        if ((n = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *) &from, &fromlen)) < 0)
//        {
//            printf("recvfrom() error\n");
//            continue;
//        }
//
//        nreceived++;
//
//        if (unpack(buf, n, inet_ntoa(from.sin_addr)) == -1)
//        {
//            printf("unpack() error \n");
//        }

        sleep(1);
    }

    printf("--- %s ping statistics ---\n", argv[1]);
    printf("%d packets transmitted, %d received, %%%d packet loss\n", nsend, nreceived,
           (nsend - nreceived) / nsend * 100);

    return 0;
}

//returns -1 when timeout occurs
int sendAngGetTheIp(char* getIpAddr, int timeoutSec){
    struct sockaddr saddr;
    int saddr_size;
    int count=0;
    int fd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
    unsigned char *buf = (unsigned char *)malloc(65536);
    int data_size;

    address = (char *) malloc(50 * sizeof(char));
    address[0] = '\0';


    printf("Waiting on poll()...\n");
    fflush(stdout);
    struct pollfd pfd = {.fd = fd, .events = POLLIN};
    int poll_ret;
    poll_ret = poll(&pfd, 1, timeoutSec*1000);

    if (poll_ret == -1) {
        perror ("poll");
        return -2;
    }
    if (poll_ret == 0)
    {
        //printf("  poll() timed out.  End program.\n");
        printf ("%d seconds elapsed.\n", timeoutSec);
        fflush(stdout);
        return -1;
    }
//    if (pfd.revents & POLLIN){
//        printf("moving on!!!\n");
//    }

    saddr_size = sizeof(saddr);
    if(data_size=recvfrom(fd,buf,65536,0,&saddr, &saddr_size)>0)
    {print_icmp_packet(buf,sizeof(buf), getIpAddr);}
}

























USHORT checkSum(USHORT *addr, int len)
{
    UINT sum = 0;
    while(len > 1)
    {
        sum += *addr++;
        len -= 2;
    }

    //Processing the remaining bytes
    if (len == 1)
    {
        sum += *(UCHAR *)addr;
    }

    //32-bit high 16 bits and low 16 digits
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return (USHORT) ~sum;
}
float timediff(struct timeval *begin, struct timeval *end)
{
    int n;
    n = (end->tv_sec - begin->tv_sec)
        + (end->tv_usec - begin->tv_usec);

    //Go to milliseconds
    return (float) (n / 1000);
}
void pack(struct icmp *icmp, int sequence)
{
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_cksum = 0;
    icmp->icmp_id = getpid();
//    icmp->icmp_sequence = sequence;
//    gettimeofday(&icmp->timestamp, 0);
    icmp->icmp_cksum = checkSum((USHORT *)icmp, ICMP_SIZE);
}
int unpack(char *buf, int len, char *addr)
{
    int i, ipheadlen;
    struct ip * ip;
    struct icmp * icmp;
    float rtt;
    struct timeval end;

    ip = (struct ip *) buf;

    //Calculate the length of the IP, that is, the length identity of the IP header is multiplied by 4
    ipheadlen = ip->ip_hl << 2;

    //Crossing the first IP header, pointing to ICMP packets
    icmp = (struct icmp *)(buf + ipheadlen);

    //ICMP packet total length
    len -= ipheadlen;

    if (len < 8)
    {
        printf("ICMP packets\'s length is less than 8 \n");
        return -1;
    }

    if (icmp->icmp_type != ICMP_ECHOREPLY ||
        icmp->icmp_id != getpid())
    {
        printf("ICMP packets are not send by us \n");
        return -1;
    }

    gettimeofday(&end, 0);
//    rtt = timediff(&icmp->timestamp, &end);
    if(rtt < 0){rtt = -rtt;}
//    printf("%d bytes from %s : icmp_seq=%u ttl=%d rtt=%fms \n",
//           len, addr, icmp->sequence, ip->ttl, rtt);
//
//    printf("Destination is: %s\n",inet_ntoa(ip->ipsrc));

    return 0;
}
