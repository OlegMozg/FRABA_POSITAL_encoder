#ifndef FRABA_ENCODER_H
#define FRABA_ENCODER_H
#include <map>
#include <iostream>

typedef std::map<std::uint16_t,std::uint8_t> can_rates;

class encoder{
protected:
    can_rates boudrate;
public:
    enum encoder_status:std::uint8_t{
        Pre_Operational=0x1,
        Operational=0x2,
        Reset=0x3
    };

};

class fraba_posital_encoder:public encoder{

public:
    fraba_posital_encoder(){
        boudrate[20]=0x0;
        boudrate[50]=0x1;
        boudrate[100]=0x2;
        boudrate[125]=0x3;
        boudrate[500]=0x5;
        boudrate[250]=0x4;
        boudrate[800]=0x6;
        boudrate[1000]=0x7;
    }
    can_rates get_rates(){
        return boudrate;
    }
};

#endif // FRABA_ENCODER_H
