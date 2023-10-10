#ifndef CANOPEN_LOCAL_H
#define CANOPEN_LOCAL_H
#include <iostream>
#include <linux/can.h>
#include <sys/socket.h>

namespace CODT{
typedef uint8_t canbyte;
typedef uint8_t cannode;
typedef uint16_t canindex;
typedef uint8_t cansubind;
typedef uint16_t canlink;
}//can open data types


enum func_codes:CODT::canbyte{

    NMT=0x0,
    SDO_tx=0xC,
    SDO_rx=0xB,
    PDO=0x3,
    SYNC=0x1
};

struct COB_ID
{
    func_codes FC;
    CODT::cannode NN;
};

struct OpenData{

    CODT::canbyte command;
    CODT::canindex index;
    CODT::cansubind subindex;
    CODT::canbyte SP_data[4];
};

static canid_t construct_can_id(const COB_ID& id,int RTR/*mask*/,int EFF/*mask*/){//30,31 биты(32-битное хранение масок)
    canid_t can_id=0;
    can_id=(static_cast<unsigned int>(id.NN))|((static_cast<unsigned int>(id.FC))<<7)|RTR|EFF;
    return can_id;
}

static can_frame create_open_frame(func_codes code, CODT::cannode NN, OpenData* data,unsigned int rtr_mask, unsigned int eff_mask,std::uint8_t frame_length=8){
    COB_ID ID;
    ID.FC=code;
    ID.NN=NN;
    canid_t can_id=construct_can_id(ID,rtr_mask,eff_mask);
    struct can_frame can_frame;

    std::uint16_t low_mask=0x00FF;
    std::uint16_t high_mask=0xFF00;

    can_frame.can_id=can_id;
    can_frame.len=frame_length;
    can_frame.data[0]=data->command;
    can_frame.data[1]=static_cast<CODT::canbyte>((data->index)&low_mask);
    can_frame.data[2]=static_cast<CODT::canbyte>(((data->index)&high_mask)>>8);
    can_frame.data[3]=data->subindex;
    int i=4;
    for( CODT::canbyte param:data->SP_data) {
        can_frame.data[i]=param;
        i++;
    }
    return can_frame;
}

static void send_SDO_msg(int handle, const sockaddr_can& addr,func_codes code,CODT::cannode NN, OpenData* data){//желательно в пре-операционном режиме

    if(NN>=0 && NN<=89){
    can_frame sdo_frame=create_open_frame(code,NN,data,0x0,0x0);
    int nbytes=sendto(handle,&sdo_frame,sizeof(struct can_frame),0,(struct sockaddr*)&addr,sizeof(addr));
    if(nbytes<0)
        throw("Parameter isn't set. Make sure you to turn on a pre-operational mode!");
    }
    else
        throw("Incorrect node number");
}

static void recv_SDO_msg(){

}

static void send_PDO_msg(int handle, const sockaddr_can& addr,func_codes code,CODT::cannode NN, OpenData* data,int rtr_mask=0x40000000){
    if(NN>=0 && NN<=89){
        can_frame sdo_frame=create_open_frame(code,NN,data,rtr_mask,0x0);
        int nbytes=sendto(handle,&sdo_frame,sizeof(struct can_frame),0,(struct sockaddr*)&addr,sizeof(addr));
        if(nbytes<0)
        throw("Обмен данными не выполнен");
    }
    else
        throw("Incorrect node number");
}

static void recv_PDO_msg(){

}



#endif // CANOPEN_LOCAL_H
