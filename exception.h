#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <QString>
#include <QMessageBox>


enum exception_code{
    unknown=-10,
    socket_not_created=-1,
    socket_not_binded=-2,
    node_error=-3,
    send_sdo_error=-4,
    send_pdo_error=-5,
    incomplete_frame=-6,
    if_not_found=-7,
    boot_up_error=-8,
    unknown_frame=-9
};

struct abstract_exception{
    int code;
    QString description;
    bool is_fatal;
    virtual void standart_exception_info() const =0;
};

struct exception:public abstract_exception{

    exception();
    exception(int error_code,const QString& info, bool is_fatal=false){
        code=error_code;
        description=info;
        this->is_fatal=is_fatal;
    }
    void standart_exception_info() const override{
        qDebug()<<"Исключение с кодом "<<this->code<<":"<<this->description;
        QMessageBox msg;
        msg.setText(this->description);
        msg.exec();
    }
};

#endif // EXCEPTION_H
