#ifndef CANOPEN_H
#define CANOPEN_H
#include <iostream>
struct COB_ID
{
    std::uint16_t ID;
};

struct OpenData{
    std::uint8_t command;
    std::uint16_t index;
    std::uint8_t subindex;
    std::uint32_t Serv_Proc_data;
};

struct OpenFrame{
    std::uint8_t FI;
    COB_ID ID;
    OpenData data;
};





#endif // CANOPEN_H
