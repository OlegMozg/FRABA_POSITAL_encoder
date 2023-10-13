#ifndef SOCKET_USAGE_H
#define SOCKET_USAGE_H
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <QString>
#include <string>

enum can_socket_type{
    RAW_SOCKET=0x1,
    BCM_SOCKET=0x2
};

struct SOCKET{
    int domain;
    int type;
    int protocol;
};

static int create_can_socket(can_socket_type sock_type){
    SOCKET sockt;
    sockt.domain=PF_CAN;
    switch(sock_type) {
    case RAW_SOCKET:
        sockt.type=SOCK_RAW;
        sockt.protocol=CAN_RAW;
        break;
    case BCM_SOCKET:
        sockt.type=SOCK_DGRAM;
        sockt.protocol=CAN_BCM;
        break;
    default:
        throw("Error type");
        break;
    }
    int handle=socket(sockt.domain,sockt.type,sockt.protocol);
    if(handle==-1)
        throw("Socket is not created");
    else
        return handle;

}

static void bind_can_sock_with_ifs(const char* if_name, can_socket_type sock_type,bool bind_to_all=false){
    struct sockaddr_can addr;
    struct ifreq ifr;
    int handle=0;
    try{
        handle=create_can_socket(sock_type);
    }
    catch(...){
        bind_to_all=true;
    }
    strcpy(ifr.ifr_name,if_name);
    ioctl(handle,SIOCGIFINDEX,&ifr);
    addr.can_family=AF_CAN;

    if(!bind_to_all)
        addr.can_ifindex=ifr.ifr_ifindex;
    else
        addr.can_ifindex=0;

    if(bind(handle, (sockaddr*) &addr,sizeof(addr))<0)
        throw("Socket can't be bind with can address family");

}

static void check_data(int nbytes){
    if(nbytes<1)
        throw "error";
    if(nbytes<sizeof(struct can_frame))
        throw "encomplete frame";
}

#endif // SOCKET_USAGE_H
