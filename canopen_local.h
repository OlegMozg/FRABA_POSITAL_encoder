#ifndef CANOPEN_LOCAL_H
#define CANOPEN_LOCAL_H
#include <iostream>
#include <linux/can.h>

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
    CODT::canbyte FC;
    CODT::cannode NN;
};

struct OpenData{
    std::uint8_t command;
    CODT::canindex index;
    std::uint8_t subindex;
    CODT::canbyte SP_data[4];
};

static canid_t construct_can_id(const COB_ID& id,int RTR,int EFF){
    canid_t can_id=((static_cast<uint16_t>(id.FC))<<7)|(static_cast<uint16_t>(id.NN));
    return can_id;
}

static can_frame create_open_frame(func_codes code, CODT::cannode NN, OpenData* data){
    COB_ID ID;
    ID.FC=code;
    ID.NN=NN;
    canid_t can_id=construct_can_id(ID,0,0);
    struct can_frame can_frame;

    std::uint16_t low_mask=0x00FF;
    std::uint16_t high_mask=0xFF00;

    can_frame.can_id=can_id;
    can_frame.len=8;
    can_frame.data[0]=data->command;
    can_frame.data[1]=static_cast<CODT::canbyte>(data->index&low_mask);
    can_frame.data[2]=static_cast<CODT::canbyte>(data->index&high_mask);
    can_frame.data[3]=data->subindex;
    int i=4;
    for( CODT::canbyte param:data->SP_data) {
        can_frame.data[i]=param;
        i++;
    }
    return can_frame;
}

static void send_SDO_msg(){

}

static void recv_SDO_msg(){

}

static void send_PDO_msg(){

}

static void recv_PDO_msg(){

}
#endif // CANOPEN_LOCAL_H
