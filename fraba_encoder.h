#ifndef FRABA_ENCODER_H
#define FRABA_ENCODER_H
#include <map>
#include <iostream>
#include <QString>
#include <canopen_local.h>
#include <exception.h>


typedef std::map<std::uint16_t,std::uint8_t> can_rates;


struct SDO_telegram
{
    uint8_t status                :1;
    uint32_t abort_code;
};

struct PDO_telegram
{
    //???
};

struct fraba_posital_encoder
{
    uint8_t node_num              :7;
    uint8_t min_node_num;
    uint8_t max_node_num;
    uint8_t work_mode;
    uint8_t trans_mode;
    uint8_t min_cycle_time;
    uint16_t max_cycle_time;
    uint16_t cycle_time_value;
    uint16_t resolution;
    uint8_t boudrate              :4;
    QString default_rate;
    uint8_t status                :2;
    uint8_t transmit_mode         :2;


    SDO_telegram sdo;
    PDO_telegram pdo;

    fraba_posital_encoder(){
        default_rate="20000";
        min_node_num=node_number::min_number;
        max_node_num=node_number::max_number;
        min_cycle_time=period_settings::MIN_period;
        max_cycle_time=period_settings::MAX_period;
        //node_num
        //boudrate
        status=Work_Mode::Stopped;
        transmit_mode=Transmission_Mode::Cyclic;
    }

    enum Work_Mode:uint8_t//or status
    {
        Pre_Operational=0x1,
        Operational=0x2,
        Stopped=0x3//at the beginning
    };

    enum Transmission_Mode{
        Cyclic=0x1,
        Syncronius=0x2,
        Polled=0x3
    };

    enum rule_commands:uint8_t{
        set_pre_operational=0x80,
        set_operational=0x01,
        reset=0x81
    };

    enum sdo_commands:uint8_t{
        set_param=0x22,
        set_param_confirm=0x60,
        get_param=0x40,
        recv_param_u8=0x4F,
        recv_param_u16=0x4B,
        recv_param_u32=0x43,
        warning=0x80
    };

    enum Resolution:uint16_t
    {
        _0_001=0x1,
        _0_01=0xA,
        _0_1=0x64,
        _1=0x3E8
    };

    enum period_settings{
        MIN_period=1,
        MAX_period=65535,
        default_value=256,
    };

};

#endif // FRABA_ENCODER_H
