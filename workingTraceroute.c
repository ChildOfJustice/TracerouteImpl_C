///*****************************************************************************
//Программа отслеживания интернет-маршрутов Traceroute
//         Программа использует пакет UDP для отправки тестовой датаграммы и постоянно изменяет значение TTL заголовка данных. TTL каждый раз
// Время », ICMP-сообщение (ошибка тайм-аута передачи или ошибка недоступности порта назначения) будет возвращено нам, мы только
// Чтобы проверить адрес хоста, отправившего нам сообщение ICMP, мы можем узнать, через какие хосты (маршрутизаторы) прошла дейтаграмма.
//         В программе используются два сокета. Один - это обычный сокет дейтаграммы UDP. Используйте параметр IP_TTL, чтобы изменить отправку.
// Значение TTL; второй - исходный сокет, используемый для получения возвращенного сообщения ICMP.
//******************************************************************************/
//#include <sys/time.h>
//#include <sys/socket.h>
//#include <netinet/in_systm.h>
//#include <netinet/in.h>
//#include <netinet/ip.h>
//#include <netinet/ip_icmp.h>
//#include <netinet/udp.h>
//#include <arpa/inet.h>
//
//#include <netdb.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//
//#define MAXPACKET 65535 /* Максимальный размер IP-пакета */
//#define UDPPACKETSIZE 36 /* размер дейтаграммы UDP */
//#define SRCPORT 23156 /* Порт источника UDP-пакета */
//#define DSTPORT 58127 /* Порт назначения UDP-пакета */
///* Объявление функции */
//double deltaT (struct timeval * t1p, struct timeval * t2p); /* Вычислить разницу во времени */
//int check_packet (u_char * buf, int cc); /* Проверяем, является ли IP-пакет ожидаемой датаграммой ICMP */
//void send_probe (int sndsock, struct sockaddr_in * whereto, int ttl); /* Отправляем тестовый пакет */
///* Получение сообщения ICMP */
//int wait_for_reply(int rcvsock,struct sockaddr_in *from,char *databuf,int buflen);
///*Основная функция*/
//int main(int argc,char * argv[])
//{
//    const int max_ttl = 48; /* максимальное количество переходов по умолчанию */
//    const int nprobes = 3; /* Количество обнаружений по умолчанию на переход */
//    // Обрабатываем командную строку, допустимый формат командной строки: имя хоста tracert или IP-адрес хоста */
//    if(argc!=2) {
//        fprintf(stderr,"Usage: %s host\r\n",argv[0]);
//        exit(-1);
//    }
//
//    struct hostent * host; /* указатель на структуру имени хоста */
//    struct sockaddr_in haddr; /* Структура адреса удаленного хоста */
//    struct sockaddr_in loc_addr; /* Структура локального адреса, используемая для привязки службы UDP к указанному порту */
//    bzero(&haddr,sizeof(haddr));
//    /* Заполняем структуру адреса хоста назначения */
//    haddr.sin_family=AF_INET;
//    haddr.sin_addr.s_addr=inet_addr(argv[1]);
//    haddr.sin_port=htons(DSTPORT);
//    /* Если это имя хоста, запросить разрешение DNS */
//    if(haddr.sin_addr.s_addr==INADDR_NONE){
//        if(NULL==(host=gethostbyname(argv[1]))) {
//            fprintf(stderr,"unknown host %s\r\n",argv[1]);
//            exit(-1);
//        }
//        memcpy(&haddr.sin_addr,host->h_addr,host->h_length);
//    }
//    /* Заполняем структуру локального адреса */
//    loc_addr.sin_family=AF_INET;
//    loc_addr.sin_addr.s_addr=htonl(INADDR_ANY);
//    loc_addr.sin_port=htons(SRCPORT);
//    int sndsock,rcvsock;
//    /* Создание сокета UDP */
//    if ((sndsock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
//        fprintf(stderr,"traceroute: udp socket\r\n");
//        exit(-1);
//    }
//    /* Создаем сокет RAW, тип сокета IPPROTO_ICMP */
//    if ((rcvsock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
//        fprintf(stderr,"traceroute: raw socket\r\n");
//        exit(-1);
//    }
//    /* Привязать сокет UDP к указанному порту */
//    if(bind(sndsock,(struct sockaddr*)&loc_addr,sizeof(loc_addr))) {
//        fprintf(stderr,"bind error\r\n");
//        exit(-1);
//    }
//
//    fprintf(stdout, "traceroute to %s (%s)", argv[1],inet_ntoa(haddr.sin_addr));
//    fprintf(stdout, ", %d hops max, %ld byte packets\r\n", max_ttl,
//            UDPPACKETSIZE+sizeof(struct ip)+sizeof(struct udphdr));
//
//    char databuf [MAXPACKET]; /* Буфер для приема датаграммы ICMP */
//    struct sockaddr_in from; /* Структура адреса удаленного хоста */
//    int ttl;
//
//
//    // Циклически меняем значение TTL отправленной дейтаграммы UDP и отправляем тестовый пакет */
//    for (ttl = 1; ttl <= max_ttl; ++ttl) {
//        u_long lastaddr = 0; /* Записываем адрес источника последнего полученного пакета */
//        int got_there = 0; /* Достигла ли запись целевого хоста */
//
//        printf("%2d ", ttl);
//        fflush(stdout);
//
//        int  probe;
//        /* Для каждого перехода (значение TTL) пакеты nprobes отправляются циклически */
//        for (probe = 0; probe < nprobes; ++probe) {
//            int cc=0;
//            struct timeval t1, t2; /* Записываем время отправки и получения */
//            struct timezone tz;
//            struct ip *ip;
//
//            gettimeofday (& t1, & tz); /* Записываем время отправки */
//
//            /* Если это имя хоста, запросить разрешение DNS */
//            if(haddr.sin_addr.s_addr==INADDR_NONE){
//                if(NULL==(host=gethostbyname(argv[1]))) {
//                    fprintf(stderr,"unknown host %s\r\n",argv[1]);
//                    exit(-1);
//                }
//                memcpy(&haddr.sin_addr,host->h_addr,host->h_length);
//            }
//
//            send_probe (sndsock, & haddr, ttl); /* Отправляем UDP-пакет */
//            /* Ждем ответного пакета ICMP в течение указанного времени до истечения таймаута */
//            while (cc = wait_for_reply(rcvsock, &from,databuf,sizeof(databuf))) {
//                gettimeofday (& t2, & tz); /* Время приема записи */
//                if (check_packet (databuf, cc)) {/* Проверяем наличие ожидаемой датаграммы ICMP */
//
//                    /* Определяем, является ли это пакетом, возвращенным хостом последнего перехода, если нет, выводим его IP-адрес */
//                    if (from.sin_addr.s_addr != lastaddr) {
//                        printf("%s  ",inet_ntoa(from.sin_addr));
//                        lastaddr = from.sin_addr.s_addr;
//                    }
//                    /* Рассчитываем интервал между отправкой и получением пакетов данных и отображаем их */
//                    printf("  %g ms  ", deltaT(&t1, &t2));
//                    /* Оцените, достигли ли вы конечного пункта назначения, и отметьте, если да */
//                    if(from.sin_addr.s_addr==haddr.sin_addr.s_addr)
//                        got_there++;
//                    break;
//                }
//            }
//            if (cc == 0) /* cc равно нулю означает, что таймаут ожидания, хост не отвечает на скачок */
//                printf("   *   ");
//        }
//        printf("\r\n");
//        /* Если вы достигли места назначения, выходим из цикла */
//        if (got_there)
//            break;
//    }
//    return 0;
//}
///***********************************************************************************
// Функция: дождаться сообщения ICMP
// rcvsock: дескриптор сокета для получения сообщения
//from: адрес структуры адресов удаленного хоста
// databuf: первый адрес буфера для получения сообщения
// buflen: длина буфера сообщений
//***********************************************************************************/
//int wait_for_reply(int rcvsock,struct sockaddr_in *from,char *databuf,int buflen)
//{
//    const int waittime = 2; /* Время ожидания по умолчанию составляет 4 секунды */
//
//    int cc = 0;
//    int fromlen = sizeof (*from);
//    /* Параметры ввода-вывода сокета */
//    fd_set fds; /* Установка ввода / вывода сокета */
//    FD_ZERO(&fds);
//    FD_SET (rcvsock, & fds); /* Добавить rcvsock к установленным fds */
//    /* (Тайм-аут) временная структура */
//    struct timeval wait;
//    wait.tv_sec = waittime;
//    wait.tv_usec = 0;
//    /* Выбираем набор fds и проверяем, есть ли отложенные операции ввода-вывода на сокетах в наборе (здесь мы читаем операции) */
//    /* Время ожидания по умолчанию - 4 секунды */
//    if (select(rcvsock+1, &fds, (fd_set *)0, (fd_set *)0, &wait) > 0) {
//        /* Есть данные для чтения, чтения в указанный буфер */
//        cc=recvfrom(rcvsock, databuf, buflen, 0,
//                    (struct sockaddr *)from, &fromlen);
//    }
//
//    return(cc);
//}
///**********************************************************************************
// Функция: отправить дейтаграмму UDP на указанный адрес
// sndsock: отправить сокет датаграммы
// whereto: указатель на структуру адреса хоста назначения
// ttl: значение TTL отправленного пакета
//**********************************************************************************/
//void send_probe(int sndsock,struct sockaddr_in *whereto,int ttl)
//{
//    char databuf [UDPPACKETSIZE]; /* Отправить буфер пакета данных (часть данных) */
//    bzero(databuf,sizeof(databuf));
//    /* Устанавливаем параметры отправки сокета (уровень IPPROTO_IP, тип IP_TTL), меняем TTL в заголовке IP на указанное значение */
//    setsockopt(sndsock,IPPROTO_IP,IP_TTL,(char *)&ttl,sizeof(ttl));
//    /* Отправка пакета данных (содержащего нулевые байты UDPPACKETSIZE) */
//    int n = sendto(sndsock, databuf, sizeof(databuf), 0,(struct sockaddr *)whereto,
//                   sizeof(struct sockaddr));
//    if(n!=UDPPACKETSIZE) {
//        fprintf(stderr,"Error in sendto\r\n");
//    }
//}
///**********************************************************************************
// Функция: проверьте, является ли возвращенный IP-пакет ожидаемой датаграммой ICMP (тайм-аут TTL или недоступен порт назначения)
// buf: первый адрес буфера данных
// cc: размер данных в буфере
//**********************************************************************************/
//int check_packet(u_char *buf,int  cc)
//{
//    /* Заголовок IP процесса */
//    struct ip *ip= (struct ip *) buf;
//    /* Рассчитываем размер IP-заголовка */
//    int hlen = ip->ip_hl << 2;
//    /* Судя по размеру, ICMP-пакет */
//    if (cc < hlen + ICMP_MINLEN) {
//        return 0;
//    }
//    cc -= hlen;
//    /* Обработка заголовка ICMP */
//    struct icmp *icp= (struct icmp *)(buf + hlen);
//    u_char type = icp-> icmp_type; /* Тип сообщения ICMP */
//    u_char code = icp-> icmp_code; /* код сообщения ICMP */
//    /* Ожидается только два сообщения ICMP: тайм-аут передачи (TTL становится 0), порт назначения недоступен (целевой хост достигнут) */
//    if(type==ICMP_TIMXCEED || type==ICMP_UNREACH) {
//        struct ip *hip=&icp->icmp_ip;
//        hlen=hip->ip_hl<<2;
//        /* Дейтаграмма ICMP отправит обратно IP-заголовок датаграммы с ошибкой и первые 8 байтов IP-дейтаграммы
//        Мы проверяем порт назначения и порт источника дейтаграммы, чтобы убедиться, что это тестовая датаграмма, которую мы отправили */
//        struct udphdr *udp=(struct udphdr *)((u_char *)hip+hlen);
//        if(hip->ip_p==IPPROTO_UDP && udp->dest==htons(DSTPORT) &&
//           udp->source==htons(SRCPORT))
//            /*if(hip->ip_p==IPPROTO_UDP && udp->uh_dport==htons(DSTPORT) &&
//                udp->uh_sport==htons(SRCPORT))*/
//            return 1;
//    }
//    return 0;
//}
///**********************************************************************************
// Функция: вычислить разницу во времени, представленную двумя структурами временных значений.
//**********************************************************************************/
//double deltaT(struct timeval *t1p,struct timeval *t2p)
//{
//    double dt;
//
//    dt = (double)(t2p->tv_sec - t1p->tv_sec) * 1000.0 +
//         (double)(t2p->tv_usec - t1p->tv_usec) / 1000.0;
//    return (dt);
//}