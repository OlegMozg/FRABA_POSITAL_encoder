#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <QString>

struct exception{
    int code;
    QString description;
    bool is_fatal;//требует ли закрытия программы
};

enum exception_code{
    unknown=-10,
    socket_not_created=-1,
    socket_not_binded=-2,
    node_error=-3,
    send_sdo_error=-4,
    send_pdo_error=-5,
    incomplete_frame=-6,
    if_not_found=-7
};

#endif // EXCEPTION_H
