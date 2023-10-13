#ifndef CANOPEN_LOCAL_H
#define CANOPEN_LOCAL_H
#include <iostream>
#include <linux/can.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

namespace CODT
{
    typedef uint8_t canbyte;
    typedef uint8_t cannode;
    typedef uint16_t canindex;
    typedef uint8_t cansubind;
    typedef uint16_t canlink;
}//can open data types

enum node_number{
    min_number=1,
    max_number=127
};

enum func_codes:CODT::canbyte
{

    NMT=0b0000,
    SDO_tx=0b1100,
    SDO_rx=0b1011,
    PDO_tx=0b0100,
    PDO_rx=0b0011,
    Emergency=0b0001,
    SYNC=0b0001
};

enum boudrates
{
    _20=0x0,
    _50=0x1,
    _100=0x2,
    _125=0x3,
    _250=0x4,
    _500=0x5,
    _800=0x6,
    _1000=0x7
};

struct COB_ID
{
    func_codes FC       :4;
    CODT::cannode NN    :7;
};

struct OpenData
{

    CODT::canbyte command;
    CODT::canindex index;
    CODT::cansubind subindex;
    CODT::canbyte* SP_data_buff;
};

static canid_t construct_can_id(const COB_ID& id,int RTR/*mask*/,int EFF/*mask*/)
{//30,31 биты(32-битное хранение масок)
    canid_t can_id=0;
    can_id=(static_cast<unsigned int>(id.NN))|((static_cast<unsigned int>(id.FC))<<7)|RTR|EFF;
    return can_id;
}

static can_frame create_open_frame(func_codes code, CODT::cannode NN, OpenData* data,unsigned int rtr_mask, unsigned int eff_mask,std::uint8_t data_length=8)
{
    COB_ID ID;
    ID.FC=code;
    ID.NN=NN;
    canid_t can_id=construct_can_id(ID,rtr_mask,eff_mask);
    struct can_frame can_frame;

    std::uint16_t low_mask=0x00FF;
    std::uint16_t high_mask=0xFF00;

    can_frame.can_id=can_id;
    can_frame.len=8;
    can_frame.data[0]=data->command;
    can_frame.data[1]=static_cast<CODT::canbyte>((data->index)&low_mask);
    can_frame.data[2]=static_cast<CODT::canbyte>(((data->index)&high_mask)>>8);
    can_frame.data[3]=data->subindex;
    try
    {

        CODT::canbyte* addr=data->SP_data_buff;
        for(int i=4;i<8;i++)
        {
            if(i<data_length)
            {
                can_frame.data[i]=(*addr);
                addr++;
            }
            else
                can_frame.data[i]=0;
        }

    }
    catch(...)
    {
    //ошибка доступа к памяти
    }

    return can_frame;
}

static void send_SDO_msg(int handle,func_codes code,CODT::cannode NN, OpenData* data, std::uint8_t data_length=8,bool new_node_flag=false)
{//желательно в пре-операционном режиме

    if(NN>=node_number::min_number && NN<=node_number::max_number){
        if(new_node_flag)
            NN--;
    can_frame sdo_frame=create_open_frame(code,NN,data,0x0,0x0,data_length);
        int nbytes=send(handle,&sdo_frame,sizeof(struct can_frame),0);
    if(nbytes<0)
        throw("Parameter isn't set. Make sure you to turn on a pre-operational mode and check node number!");
    }
    else
        throw("Incorrect node number");
}

static void recv_SDO_msg(int handle){
    struct can_frame sdo_frame;
}

static void send_PDO_msg(int handle,func_codes code,CODT::cannode NN, OpenData* data,int rtr_mask=0x40000000)
{
    if(NN>=node_number::min_number && NN<=node_number::max_number){
        can_frame sdo_frame=create_open_frame(code,NN,data,rtr_mask,0x0);
        int nbytes=send(handle,&sdo_frame,sizeof(struct can_frame),0);
        if(nbytes<0)
        throw("Обмен данными не выполнен");
    }
    else
        throw("Incorrect node number");
}

static void recv_PDO_msg()
{

}

static void send_rule_msg(int handle, func_codes code, CODT::cannode NN, uint8_t command_code){
    if(NN>=node_number::min_number && NN<=node_number::max_number){
        OpenData data;
        data.command=command_code;
        data.index=NN;
        data.subindex=0x0;
        data.SP_data_buff=nullptr;
        can_frame rul_frame=create_open_frame(code,NN,&data,0x0,0x0,4);
        int nbytes=send(handle,&rul_frame,sizeof(struct can_frame),0);
        if(nbytes<0)
            throw("Обмен данными не выполнен");
    }
    else
        throw("Incorrect node number");
}


#endif // CANOPEN_LOCAL_H
