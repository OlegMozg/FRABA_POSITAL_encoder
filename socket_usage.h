#ifndef SOCKET_USAGE_H
#define SOCKET_USAGE_H
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <QString>
#include <string>
#include <exception.h>

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
        exception ex(unknown,"Unknown error while creating socket",true);
        throw ex;
        break;
    }
    int handle=socket(sockt.domain,sockt.type,sockt.protocol);
    if(handle==-1)
    {
        exception ex(socket_not_created,"Unknown error while creating socket",true);
        throw ex;
    }
    else
        return handle;

}

static void bind_can_sock_with_ifs(const char* if_name, can_socket_type sock_type,bool bind_to_all=false){
    struct sockaddr_can addr;
    struct ifreq ifr;
    int handle=0;
    try
    {
        handle=create_can_socket(sock_type);
    }
    catch(const exception& ex){
        throw ex;
    }
    strcpy(ifr.ifr_name,if_name);
    int state=ioctl(handle,SIOCGIFINDEX,&ifr);
    //if(state<0)
    //    bind_to_all=true;
    addr.can_family=AF_CAN;

    if(!bind_to_all)
        addr.can_ifindex=ifr.ifr_ifindex;
    else
        addr.can_ifindex=0;

    if(bind(handle, (sockaddr*) &addr,sizeof(addr))<0){
        exception ex(socket_not_binded,"Socket can't be bind with can address family",true);
        throw(ex);
    }
}

#endif // SOCKET_USAGE_H
