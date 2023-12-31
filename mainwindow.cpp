#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFrame>
#include <socket_usage.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <QMessageBox>
#include <QInputDialog>
#include <QProcess>
#include <unistd.h>
#include <exception.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    encoder=new fraba_posital_encoder();
    encoder->status=encoder->Stopped;
    emit status_stopped();
    password="";

    QFont font =ui->pushButton->font();
    font.setPixelSize(18);
    ui->label->setFont(font);
    ui->label_2->setFont(font);
    ui->label_3->setFont(font);
    ui->label_4->setFont(font);
    ui->label_5->setFont(font);
    ui->label_6->setFont(font);
    ui->label_7->setFont(font);
    ui->label_8->setFont(font);
    ui->label_10->setFont(font);
    ui->label_11->setFont(font);
    ui->label_12->setFont(font);
    ui->label_13->setFont(font);
    ui->label_14->setFont(font);
    ui->label_15->setFont(font);
    ui->label_9->setFont(font);
    ui->label_16->setFont(font);
    ui->label_17->setFont(font);
    ui->label_18->setFont(font);
    ui->radioButton->setFont(font);
    ui->radioButton_2->setFont(font);
    ui->radioButton_3->setFont(font);
    ui->comboBox->setFont(font);
    ui->comboBox_2->setFont(font);
    ui->spinBox->setFont(font);
    ui->spinBox_2->setFont(font);
    ui->pushButton->setText("Quit");
    ui->label->setText("Work mode:");
    ui->pushButton_2->setText("Pre-Operational");
    ui->pushButton_3->setText("Operational");
    ui->pushButton_5->setText("Configure");
    ui->label_2->setText("Baudrate:");
    ui->label_3->setText("Transmission mode:");
    ui->radioButton->setText("Cyclic");
    ui->radioButton_2->setText("SYNC");
    ui->radioButton_3->setText("Polled");
    ui->groupBox->setTitle("Settings");
    ui->label_4->setText("Period:");
    ui->label_5->setText("Resolution:");
    ui->label_6->setText("Node number:");
    ui->spinBox->setMinimum(encoder->min_cycle_time);
    ui->spinBox->setMaximum(encoder->max_cycle_time);
    ui->spinBox->setValue(encoder->default_value);
    ui->spinBox->setSingleStep(1);
    ui->spinBox->setSuffix("ms");
    ui->spinBox->setEnabled(false);
    ui->spinBox_2->setMinimum(encoder->min_node_num);
    ui->spinBox_2->setMaximum(encoder->max_node_num);
    ui->label_8->setText("Node number:");
    ui->label_7->setText("NO INFO");
    ui->label_10->setText("Status:");
    ui->label_11->setText("NO INFO");
    ui->pushButton_6->setText("Reset");
    ui->pushButton_7->setText("Start receiving/Issue values");
    ui->label_12->setText("Baudrate:");
    ui->label_13->setText("NO INFO");
    ui->label_14->setText("Resolution:");
    ui->label_15->setText("NO INFO");
    ui->groupBox_2->setTitle("Current values");
    ui->pushButton_4->setText("Apply");
    ui->groupBox_3->setTitle("Data transmission");
    ui->label_9->setText("Rotation Angle");
    ui->label_16->setText("Revolutions");
    ui->label_17->setText("NO INFO");
    ui->label_18->setText("NO INFO");


    QStringList list;
    for(auto el:velocity_to_screen){
        list.append(el+" Kbit/s");
    }
    ui->comboBox->addItems(list);
    ui->comboBox->show();
    list.clear();
    for(auto el:resolutions)
        list.append(el+QChar(176));
    ui->comboBox_2->addItems(list);
    ui->comboBox_2->show();

    connect(this,SIGNAL(start_program(int)),this,SLOT(program_run(int)));
    connect(this,SIGNAL(status_stopped()),this,SLOT(is_stopped()));
    connect(this,SIGNAL(status_pre_operational()),this,SLOT(is_pre_operational()));
    connect(this,SIGNAL(status_operational()),this,SLOT(is_pre_operational()));

    try
    {
        int handle=create_can_socket(can_socket_type::RAW_SOCKET);
        socket_handle=handle;
        bind_can_sock_with_ifs(if_name,can_socket_type::RAW_SOCKET);
    }
    catch (const exception& ex)
    {
        qDebug()<<"Исключение с кодом "<<ex.code<<":"<<ex.description;
            if(ex.is_fatal)
            exit(ex.code);
    }
    emit start_program(socket_handle);
}

MainWindow::~MainWindow()//деструктор окна
{
    int is_closed=::close(socket_handle);
    if(!is_closed)
        qDebug()<<"Соединение разорвано";
    else{
        qDebug()<<"Ошибка закрытия сокета:"<<errno;
    }
    delete encoder;
    delete ui;
}

void MainWindow::on_pushButton_clicked()//кнопка quit
{
    this->close();
}

void MainWindow::on_pushButton_2_clicked()//перейти в pre-operational
{
    QMessageBox msg;
    uint8_t current_nn=static_cast<uint8_t>(ui->label_7->text().toUInt());
  try
    {
        send_rule_msg(socket_handle,func_codes::NMT,current_nn,encoder->set_pre_operational);
    }
  catch(const exception& ex)
    {
        ex.standart_exception_info();
        if(ex.is_fatal)
            exit(ex.code);
        return;
    }
    catch(const std::exception& ex){
        qDebug()<<ex.what();
        exit(unknown);
    }

    emit status_pre_operational();
}

void MainWindow::on_pushButton_3_clicked()//перейти в operational
{
    QMessageBox msg;
    uint8_t current_nn=static_cast<uint8_t>(ui->label_7->text().toUInt());
    try
    {
        send_rule_msg(socket_handle,func_codes::NMT,current_nn,encoder->set_operational);
    }
    catch(const exception& ex)
    {
        ex.standart_exception_info();
        if(ex.is_fatal)
            exit(ex.code);
        return;
    }
    catch(const std::exception& ex){
        qDebug()<<ex.what();
        exit(unknown);
    }

    emit status_operational();
}

void MainWindow::on_radioButton_clicked()//cyclic
{
    ui->spinBox->setEnabled(true);
    if(ui->radioButton->isChecked()){
        ui->radioButton_2->setChecked(false);
        ui->radioButton_3->setChecked(false);
    }
}

void MainWindow::on_radioButton_2_clicked()//sync//temporary is not available
{
    ui->spinBox->setEnabled(false);
    if(ui->radioButton_2->isChecked()){
        ui->radioButton->setChecked(false);
        ui->radioButton_3->setChecked(false);
    }
}

void MainWindow::on_radioButton_3_clicked()//polled
{
    ui->spinBox->setEnabled(false);
    if(ui->radioButton_3->isChecked()){
        ui->radioButton->setChecked(false);
        ui->radioButton_2->setChecked(false);
    }
}

void MainWindow::program_run(int handle)//пытаемся получить boot-up msg и задать скорость can-контроллеру
{
    try
    {
        QString rate=(encoder->default_rate);
        reconfigure_interface(rate);
    }
    catch (const exception& ex)
    {
        ex.standart_exception_info();
        if(ex.is_fatal)
            exit(ex.code);
        return;
    }
    catch (const std::exception& ex){
        qDebug()<<ex.what();
        exit(unknown);
    }

    QMessageBox msg;
    struct can_frame boot_up_frame;
    int nbytes=0;
    try{
        while(nbytes==0){
            nbytes=recv(handle,&boot_up_frame,sizeof(struct can_frame),0);
        }
        check_data_SDO(nbytes);
    }
    catch(const exception& ex)
    {
        ex.standart_exception_info();
        QMessageBox msg;
        msg.setText("Boot-up сообщение не получено");
        msg.exec();
        exit(boot_up_error);
    }

    uint32_t FC_MASK=0x00000780;
    uint32_t NN_MASK=0x7F;
    try
    {
    if((((boot_up_frame.can_id)&FC_MASK)==0x700) && boot_up_frame.len==1){
        encoder->node_num=(boot_up_frame.can_id)&NN_MASK;
        ui->label_7->setText(QString::number(encoder->node_num));
        emit status_pre_operational();
        msg.setText("Датчик активен");
        msg.exec();
        qDebug()<<"boot-up frame получен"<<",номер узла:"<<encoder->node_num;
        OpenData data;
        data.command=encoder->get_param;
        data.index=0x3001;
        data.subindex=0x00;
        uint8_t len=4;
        data.SP_data_buff=nullptr;
        struct can_frame baud_frame;

        send_SDO_msg(socket_handle,func_codes::SDO_tx,encoder->node_num,&data,len);//хотим узнать скорость передачи
        baud_frame=recv_SDO_msg(socket_handle);

        if((((baud_frame.can_id)&FC_MASK)==func_codes::SDO_rx) && (baud_frame.data[0]==encoder->recv_param_u8)){
            encoder->baudrate=baud_frame.data[4];
            QString rate_to_screen="";
            for(const auto& pair : rates){
                if(pair.second==encoder->baudrate){
                    rate_to_screen=pair.first;
                    break;
                }
            }
            ui->label_13->setText(rate_to_screen);
            qDebug()<<"Скорость по умолчанию установлена:"<<encoder->baudrate;
        }
        else
        {
            qDebug()<<"Ответ от датчика следующий при попытке узнать скорость:"<<"CAN-ID:"<<baud_frame.can_id<<"Код команды:"<<baud_frame.data[0];
            exit(unknown_frame);
        }
        OpenData open_data;
        open_data.command=encoder->get_param;
        open_data.index=0x6000;
        open_data.subindex=0x00;
        len=4;
        open_data.SP_data_buff=nullptr;
        struct can_frame res_frame;

        send_SDO_msg(socket_handle,func_codes::SDO_tx,encoder->max_node_num,&open_data,len);//хотим узнать разрешение выдачи угла поворота
        res_frame=recv_SDO_msg(socket_handle);
        if((((res_frame.can_id)&FC_MASK)==func_codes::SDO_rx) && (res_frame.data[0]==encoder->recv_param_u16)){
            uint8_t resolution_low=res_frame.data[4];
            uint8_t resolution_high=res_frame.data[5];
            uint16_t hex_resolution=create_from_low_and_high_bite(resolution_low,resolution_high);
            encoder->resolution=hex_resolution;
            float res_to_screen=0.0;
            for(const auto& pair : resolutions_dict){
                if(pair.second==encoder->resolution){
                    res_to_screen=pair.first;
                    break;
                }
            }
            ui->label_15->setText(QString::number(res_to_screen));
        }
        else{
            qDebug()<<"Ответ от датчика следующий при попытке узнать разрешение выдачи:"<<"CAN-ID:"<<res_frame.can_id<<"Код команды:"<<res_frame.data[0];
            exit(unknown_frame);
        }
    }
    else
    {
        qDebug()<<"При попытке получить boot-up frame получено:"<<"CAN-ID"<<boot_up_frame.can_id<<"len:"<<boot_up_frame.len;
        exit(unknown_frame);
    }
    }
    catch(const exception& ex)
    {
        qDebug()<<"Попытка узнать скорость передачи или разрешение выдачи от узла с номером:"<<encoder->node_num<<" провалена!";
        ex.standart_exception_info();
        exit(ex.code);
    }
    catch(const std::exception& ex)
    {
        qDebug()<<ex.what();
        exit(unknown);
    }
}

void MainWindow::is_stopped(){
    ui->label_11->setText("Stopped");
    ui->pushButton_5->setHidden(true);
}

void MainWindow::is_pre_operational(){
    ui->label_11->setText("Pre-Operational");
    ui->pushButton_5->setHidden(false);
}

void MainWindow::is_operational(){
    ui->label_11->setText("Operational");
    ui->pushButton_5->setHidden(true);
}

void MainWindow::on_pushButton_6_clicked()//reset encoder
{
    QMessageBox msg;
    uint8_t current_nn=static_cast<uint8_t>(ui->label_7->text().toUInt());
    try
    {
        send_rule_msg(socket_handle,func_codes::NMT,current_nn,encoder->reset);
    }
    catch(const exception& ex){
        ex.standart_exception_info();
        if(ex.is_fatal)
            exit(ex.code);
        return;
    }
    emit status_stopped();
    emit start_program(socket_handle);
}

void MainWindow::on_pushButton_5_clicked()//сконфигурировать датчик с новыми настройками
{
    ui->pushButton_7->setEnabled(false);
    if(encoder->status!=encoder->Pre_Operational){
        QMessageBox msg;
        msg.setText("Перейдите в режим Pre-Operational");
        msg.exec();
        return;
    }
    ui->pushButton_5->setHidden(true);
    uint32_t FC_MASK=0x00000780;
    uint32_t NN_MASK=0x7F;
    //новые настройки можно применять только если они отличаются от старых
    double choosen_resolution;
    QString baudrate_key;
    int value;
    int combo_index;
    //1.node number (?)
    try
    {
    value=ui->spinBox_2->value();
    if(value!=ui->label_7->text().toInt())
    {
        OpenData open_data;
        open_data.command=encoder->set_param;
        open_data.index=0x3000;
        open_data.subindex=0x0;
        CODT::canbyte data[1];
        data[0]=value;
        open_data.SP_data_buff=data;
        uint8_t len=5;
        struct can_frame node_frame;

        send_SDO_msg(socket_handle,func_codes::SDO_tx,encoder->node_num,&open_data,len,true);
        node_frame=recv_SDO_msg(socket_handle);

        if((((node_frame.can_id)&FC_MASK)!=func_codes::SDO_rx) || node_frame.data[0]!=encoder->set_param_confirm){
            qDebug()<<"Попытка настроить номер узла ,ответ от датчика:"<<"CAN_ID:"<<node_frame.can_id<<"Код команды:"<<node_frame.data[0];
            exit(unknown_frame);
        }
    }
    //2.baudrate
    combo_index=ui->comboBox->currentIndex();
    if((ui->comboBox->itemText(combo_index)).toUInt()!=ui->label_13->text().toUInt())
    {
        OpenData open_data;
        CODT::canbyte data[1];
        baudrate_key=ui->comboBox->itemText(combo_index);
        baudrate_key.remove(" Kbit/s");
        data[0]=rates[baudrate_key];
        open_data.command=encoder->set_param;
        open_data.index=0x3001;
        open_data.subindex=0x00;
        uint8_t len=5;
        open_data.SP_data_buff=data;
        struct can_frame baud_frame;

        send_SDO_msg(socket_handle,func_codes::SDO_tx,encoder->node_num,&open_data,len);
        baud_frame=recv_SDO_msg(socket_handle);

        if((((baud_frame.can_id)& FC_MASK)!=func_codes::SDO_rx) || baud_frame.data[0]!=encoder->set_param_confirm){
            qDebug()<<"Попытка настроить скорость узла ,ответ от датчика:"<<"CAN_ID:"<<baud_frame.can_id<<"Код команды:"<<baud_frame.data[0];
            exit(unknown_frame);
        }
    }
    //3.Resolution
    int combo_index_2=ui->comboBox_2->currentIndex();
    choosen_resolution=(ui->comboBox_2->itemText(combo_index_2)).toFloat();
    if(choosen_resolution!=ui->label_15->text().toFloat()){
        OpenData open_data;
        BYTES bytes;
        uint16_t hex_resolution=resolutions_dict[choosen_resolution];
        find_low_and_high_byte(hex_resolution,bytes);
        CODT::canbyte data[2];
        data[0]=bytes.low;
        data[1]=bytes.high;
        open_data.command=encoder->set_param;
        open_data.index=0x6000;
        open_data.subindex=0x00;
        open_data.SP_data_buff=data;
        uint8_t len=6;
        struct can_frame res_frame;

        send_SDO_msg(socket_handle,func_codes::SDO_tx,encoder->node_num,&open_data,len);
        res_frame=recv_SDO_msg(socket_handle);

        if((((res_frame.can_id)&FC_MASK)!=func_codes::SDO_rx) || res_frame.data[0]!=encoder->set_param_confirm){
            qDebug()<<"Попытка настроить разрешение выдачи,ответ от датчика:"<<"CAN_ID:"<<res_frame.can_id<<"Код команды:"<<res_frame.data[0];
            exit(unknown_frame);
        }
     }
    }
    catch(const exception& ex)
    {
        ex.standart_exception_info();
        exit(ex.code);
    }
    catch(const std::exception& ex)
    {
        qDebug()<<ex.what();
        exit(unknown);
    }
    //save in flash-memory
    QString out,command;
    OpenData open_data;
    CODT::canbyte data[4];
    data[0]=0x55;
    data[1]=0xAA;
    data[2]=0xAA;
    data[3]=0x55;
    open_data.command=encoder->set_param;
    open_data.index=0x2300;
    open_data.subindex=0x00;
    try
    {
    send_SDO_msg(socket_handle,func_codes::SDO_tx,encoder->node_num,&open_data);
    //переключить на новую скорость CAN-контроллер и принять 2 мистических сообщения???
    QString rate=ui->comboBox->itemText(combo_index)+"000";


    reconfigure_interface(rate);

    struct can_frame flash_answer_1, flash_answer_2;

        flash_answer_1=recv_SDO_msg(socket_handle);
       //if((((flash_answer_1.can_id)&FC_MASK)==0x80) && flash_answer_1.len==1){
       //   qDebug()<<"Мистическое сообщение 1 принято";
        qDebug()<<"Мистическое сообщение 1:"<<"can_id:"<<flash_answer_1.can_id<<"len:"<<flash_answer_1.len;

        flash_answer_2=recv_SDO_msg(socket_handle);
      //  if((((flash_answer_1.can_id)&FC_MASK)==0x80) && flash_answer_1.len==8){
      //     qDebug()<<"Мистическое сообщение 2 принято";
       // }

        qDebug()<<"Мистическое сообщение 2:"<<"can_id:"<<flash_answer_2.can_id<<"len:"<<flash_answer_2.len;
    }
    catch(const exception& ex)
    {
        ex.standart_exception_info();
        if(ex.is_fatal || ex.code==send_sdo_error)
            exit(ex.code);
    }
    catch(const std::exception& ex)
    {
        qDebug()<<ex.what();
        exit(unknown);
    }
    //выключить питание или нет????
    QMessageBox msg;
    msg.setText("Снимите питание с датчика и нажмите ОК!");
    int code=msg.exec();
    if(code!=0x400){//не ок                                            //отладка
        OpenData data;
        data.command=encoder->get_param;
        data.index=0x3001;
        data.subindex=0x0;
        send_SDO_msg(socket_handle,func_codes::SDO_tx,value,&data,4);
        struct can_frame baud_frame;

        try
        {
           baud_frame=recv_SDO_msg(socket_handle);
        }
        catch(const exception & ex)
        {
           qDebug()<<"Исключение с кодом "<<ex.code<<":"<<ex.description;
           qDebug()<<"Ошибка получения скорости узла без выкл питания после записи во flash";
           exit(ex.code);
        }
        if((((baud_frame.can_id)&FC_MASK)==func_codes::SDO_rx) && (baud_frame.data[0]==encoder->recv_param_u8)){
           qDebug()<<"Питание не снято, но параметры установлены!?!";
           qDebug()<<"Реальная скорость"<<baud_frame.data[4];
        }
        else{
           qDebug()<<"Пытались получить скорость узла без выключения питания";
           qDebug()<<"Ответ датчика:"<<"CAN_ID:"<<baud_frame.can_id<<"Код команды:"<<baud_frame.data[0];
           exit(unknown_frame);
        }
    }
    //включение питания
    struct can_frame boot_frame;
    try{
        boot_frame=recv_SDO_msg(socket_handle);
    }
    catch(const exception& ex)
    {
        qDebug()<<"Исключение с кодом "<<ex.code<<":"<<ex.description;
        qDebug()<<"При попытке включения питания произошла ошибка приема кадра";
        exit(ex.code);
    }

    if((((boot_frame.can_id)&FC_MASK)==0x700) && (((boot_frame.can_id)&NN_MASK)==value)){
        encoder->node_num=value;
        ui->label_7->setText(QString::number(value));
        encoder->status=encoder->Pre_Operational;
        encoder->baudrate=rates[baudrate_key];
        ui->label_13->setText(baudrate_key);
        encoder->resolution=resolutions_dict[choosen_resolution];
        ui->label_15->setText(QString::number(choosen_resolution));
        qDebug()<<"boot-up frame получен и конфигурация завершена";
        emit status_pre_operational();
    }
    else{
        qDebug()<<"Пытались получить boot-up frame после включения питания";
        qDebug()<<"Ответ датчика:"<<"CAN_ID:"<<boot_frame.can_id<<"Код команды:"<<boot_frame.data[0];
        exit(unknown_frame);
    }
    ui->pushButton_5->setHidden(false);//снова можно конфигурировать датчик
    ui->pushButton_7->setEnabled(true);
}

void MainWindow::ask_for_password(){
    bool ok;
    QString password=QInputDialog::getText(this,"Доступ к контроллеру","Введите пароль:",QLineEdit::Password,"",&ok,Qt::Window);
    if(ok && password!=""){
        this->password=password;
        return;
    }
    else{
        QMessageBox MSG;
        MSG.setText("Доступ запрещен.Повторите попытку или воспользуйтесь терминалом!");
        MSG.exec();
        this->password="";
    }
}

QString MainWindow::executeSudoCommand(const QString& command){

    if(password=="")
        ask_for_password();

    QStringList sudoArgs;
    sudoArgs<<"-k"<<"-S"<<"sh"<<"-c"<<command;
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start("sudo",sudoArgs);
    process.write(password.toUtf8());
    process.closeWriteChannel();
    process.waitForFinished();
    QString out=process.readAll();
    return out;
}

void MainWindow::reconfigure_interface(const QString& rate){//сконфигурировать физический интерфейс
    QString command,out;
    command="ip link set can0 down";
    out=executeSudoCommand(command);
    if(out.contains("cannot find device",Qt::CaseInsensitive))
    {
        exception ex(if_not_found,"CAN-устройство не обнаружено",true);
        throw ex;
    }

    command="ip link set can0 up type can bitrate "+rate+" loopback off";
    out=executeSudoCommand(command);
    if(out.contains("cannot find device",Qt::CaseInsensitive))
    {
        exception ex(if_not_found,"CAN-устройство не обнаружено",true);
        throw ex;
    }
}

void MainWindow::on_pushButton_4_clicked()//выбрать режим передачи сообщений
{
    ui->pushButton_7->setEnabled(false);
    uint32_t FC_MASK=0x00000780;
    uint32_t NN_MASK=0x7F;
    if(encoder->status!=encoder->Pre_Operational){
        QMessageBox msg;
        msg.setText("Перейдите в режим Pre-Operational");
        msg.exec();
        return;
    }

    try
    {
        if(ui->radioButton->isChecked()){//режим циклический

            if(encoder->transmit_mode!=encoder->Cyclic){
            //перейти в циклический
                OpenData open_data;
                open_data.command=encoder->set_param;
                open_data.index=0x1800;
                open_data.subindex=0x01;//or 0x02
                CODT::canbyte data[4];
                data[0]=0x00;
                data[1]=0x00;
                data[2]=0x00;
                data[3]=0xFF;//or 0x00
                open_data.SP_data_buff=data;
                send_SDO_msg(socket_handle,func_codes::SDO_tx,encoder->node_num,&open_data);
                struct can_frame mode_frame;
                mode_frame=recv_SDO_msg(socket_handle);
                if((((mode_frame.can_id)& FC_MASK)!=func_codes::SDO_rx) || mode_frame.data[0]!=encoder->set_param_confirm){
                    qDebug()<<"Циклический режим не настроен"<<"CAN_ID:"<<mode_frame.can_id<<"Код команды:"<<mode_frame.data[0];
                    return;
                }
            }
            uint16_t cycle_time=static_cast<uint16_t>(ui->spinBox->value());
            BYTES bytes;
            find_low_and_high_byte(cycle_time,bytes);
            OpenData open_data;
            open_data.command=encoder->set_param;
            open_data.index=0x2200;
            open_data.subindex=0x00;
            CODT::canbyte data[2];
            data[0]=bytes.low;
            data[1]=bytes.high;
            open_data.SP_data_buff=data;
            uint8_t len=6;
            send_SDO_msg(socket_handle,func_codes::SDO_tx,encoder->node_num,&open_data,len);
            struct can_frame cycle_frame;
            cycle_frame=recv_SDO_msg(socket_handle);

            if((((cycle_frame.can_id)& FC_MASK)!=func_codes::SDO_rx) || cycle_frame.data[0]!=encoder->set_param_confirm){
               qDebug()<<"Параметр времени цикла не настроен"<<"CAN_ID:"<<cycle_frame.can_id<<"Код команды:"<<cycle_frame.data[0];
               return;
            }
            encoder->transmit_mode=encoder->Cyclic;
            return;
        }

        else if(ui->radioButton_2->isChecked()){//пока без режима SYNC
            QMessageBox msg;
            msg.setText("Данный режим пока не поддерживается");
            msg.exec();
            return;
            encoder->transmit_mode=encoder->Syncronius;
        }

        else if(ui->radioButton_3->isChecked()){//режим по запросу
                if(encoder->transmit_mode!=encoder->Polled){
                    //перейти в этот режим
                    OpenData open_data;
                    open_data.command=encoder->set_param;
                    open_data.index=0x1800;//or 0x1801
                    open_data.subindex=0x01;//or 0x02
                    uint8_t len=4;
                     open_data.SP_data_buff=nullptr;
                    send_SDO_msg(socket_handle,func_codes::SDO_tx,encoder->node_num,&open_data,len);
                    struct can_frame mode_frame;
                    mode_frame=recv_SDO_msg(socket_handle);

                    if((((mode_frame.can_id)& FC_MASK)!=func_codes::SDO_rx) || mode_frame.data[0]!=encoder->set_param_confirm){
                        qDebug()<<"Режим ПО ЗАПРОСУ не настроен"<<"CAN_ID:"<<mode_frame.can_id<<"Код команды:"<<mode_frame.data[0];
                        return;
                    }
                }
            encoder->transmit_mode=encoder->Polled;
            return;
        }
        else{
            QMessageBox msg;
            msg.setText("Режим передачи сообщений не выбран!");
            msg.exec();
            return;
        }
    }
    catch(const exception& ex)
    {
        ex.standart_exception_info();
        if(ex.is_fatal)
            exit(ex.code);
    }
    catch(const std::exception& ex)
    {
        qDebug()<<ex.what();
        exit(unknown);
    }
    ui->pushButton_7->setEnabled(true);
}


void MainWindow::on_pushButton_7_clicked()//начать циклический прием сообщений или выдать 1 сообщение
{
    uint32_t FC_MASK=0x00000780;
    uint32_t NN_MASK=0x7F;
    if(encoder->status!=encoder->Operational){
        QMessageBox msg;
        msg.setText("Перейдите в режим Operational!");
        msg.exec();
        return;
    }
    static char cyclic_run=1;

    try
    {
        if(encoder->transmit_mode==encoder->Cyclic)
        {
            cyclic_run++;
            if(ui->pushButton_4->isHidden()==false)
                ui->pushButton_4->setHidden(true);
            if(ui->pushButton_5->isHidden()==false)
                ui->pushButton_5->setHidden(true);
            if(cyclic_run%2==0)
                ui->pushButton_7->setText("Stop cyclic transmission");
            struct can_frame positional_frame;
            while(cyclic_run%2==0){
                    //остановка через static переменную!
                positional_frame=recv_PDO_msg(socket_handle);
                if((((positional_frame.can_id)&FC_MASK)==func_codes::PDO_rx) && (((positional_frame.can_id)&NN_MASK)==encoder->node_num))
                    display_positional_data(positional_frame);

                else{
                    qDebug()<<"Ошибка циклического приема"<<", получено CAN_ID:"<<positional_frame.can_id<<" Код команды:"<<positional_frame.data[0];
                    QMessageBox msg;
                    msg.setText("Циклический прием остановлен");
                    msg.exec();
                    break;
                }
            }
            ui->pushButton_7->setText("Start receiving/Issue values");
        }
        else if(encoder->transmit_mode==encoder->Syncronius)
        {//пока не поддерживается и надо ли вообще
            return;
        }
        else if(encoder->transmit_mode==encoder->Polled){
            OpenData* data=nullptr;
            send_PDO_msg(socket_handle,func_codes::PDO_tx,encoder->node_num,data);
            struct can_frame positional_frame;
            positional_frame=recv_PDO_msg(socket_handle);
            if((((positional_frame.can_id)&FC_MASK)==func_codes::PDO_rx) && (((positional_frame.can_id)&NN_MASK)==encoder->node_num))
                display_positional_data(positional_frame);

            else{
                QMessageBox msg;
                msg.setText("Позиционные значения не получены");
                msg.exec();
                qDebug()<<"Ответ от датчика:"<<"CAN_ID:"<<positional_frame.can_id<<"Код команды:"<<positional_frame.data[0];
                return;
            }
        }
    }
    catch(const exception& ex)
    {
        ui->pushButton_7->setText("Start receiving/Issue values");
        ex.standart_exception_info();
        if(ex.is_fatal)
            exit(ex.code);
    }
    catch(const std::exception& ex)
    {
        qDebug()<<ex.what();
        exit(unknown);
    }
    if(ui->pushButton_4->isHidden())
        ui->pushButton_4->setHidden(false);
    if(ui->pushButton_5->isHidden())
        ui->pushButton_5->setHidden(false);
}

void MainWindow::display_positional_data(const can_frame& positional_frame){//отобразить угол поворота и число оборотов на экране
    //ВАРИАНТ_1:ЕСЛИ ДАТЧИК ПЕРЕДАЕТ МЛАДШИЕ И СТАРШИЕ БИТЫ ЧИСЛА С ПЛАВАЮЩЕЙ ТОЧКОЙ
    //           unsigned char bytes[]={positional_frame.data[0],positional_frame.data[1]};
    //           double angle=*(double*)bytes;
    //           ui->label_17->setText(QString::number(angle)+QChar(176));
    //ВАРИАНТ_2:ЕСЛИ ДАТЧИК ПЕРЕДАЕТ ЦЕЛУЮ И ДРОБНУЮ ЧАСТЬ ЧИСЛА
    //               uint8_t integer_part=positional_frame.data[0];
    //               uint8_t fractional_part=positional_frame.data[1];
    //               ui->label_17->setText(QString::number(integer_part)+"."+QString::number(fractional_part));

    //ВАРИАНТ_3:ЕСЛИ ДАТЧИК ПЕРЕДАЕТ ДАННЫЕ В ВИДЕ СТАРШЕГО И МЛАДШЕГО БАЙТА ЦЕЛОГО ЧИСЛА ЭКВИВАЛЛЕНТНОГО ДРОБНОМУ С УЧЕТОМ ЗАДАННОЙ ТОЧНОСТИ
    uint8_t low_angle=positional_frame.data[0];
    uint8_t high_angle=positional_frame.data[1];
    uint16_t current_angle=create_from_low_and_high_bite(low_angle,high_angle);
    float angle_deg=current_angle*(ui->label_15->text().toFloat());
    ui->label_17->setText(QString::number(angle_deg)+QChar(176));

    uint8_t low_revolutions=positional_frame.data[2];
    uint8_t high_revolutions=positional_frame.data[3];
    uint16_t current_revolutions=create_from_low_and_high_bite(low_revolutions,high_revolutions);
    ui->label_18->setText(QString::number(current_revolutions));
}
