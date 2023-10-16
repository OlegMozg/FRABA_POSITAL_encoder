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

    QFont font =ui->pushButton->font();
    font.setPixelSize(18);
    ui->label->setFont(font);
    ui->label_2->setFont(font);
    ui->label_3->setFont(font);
    ui->label_4->setFont(font);
    ui->label_5->setFont(font);
    ui->label_6->setFont(font);
    ui->label_8->setFont(font);
    ui->label_10->setFont(font);
    ui->label_12->setFont(font);
    ui->label_14->setFont(font);
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
    ui->label_7->setText("");
    ui->label_10->setText("Status:");
    ui->label_11->setText("");
    ui->pushButton_6->setText("Reset");
    ui->label_12->setText("Baudrate:");
    ui->label_13->setText("");
    ui->label_14->setText("Resolution:");
    ui->label_15->setText("");
    ui->groupBox_2->setTitle("Current values");
    ui->pushButton_4->setText("Apply");
    ui->groupBox_3->setTitle("Data transmission");


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

    int handle=create_can_socket(can_socket_type::RAW_SOCKET);
    socket_handle=handle;
    bind_can_sock_with_ifs(if_name,can_socket_type::RAW_SOCKET);
    emit start_program(socket_handle);

    password="";
}

MainWindow::~MainWindow()//деструктор окна
{
    ::close(socket_handle);
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
        qDebug()<<"Исключение с кодом "<<ex.code<<":"<<ex.description;
        QMessageBox msg;
        msg.setText(ex.description);
        msg.exec();
        if(ex.is_fatal){
            exit(ex.code);
        }
        else
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
        qDebug()<<"Исключение с кодом "<<ex.code<<":"<<ex.description;
        QMessageBox msg;
        msg.setText(ex.description);
        msg.exec();
        if(ex.is_fatal){
            exit(ex.code);
        }
        else
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
        qDebug()<<"Исключение с кодом "<<ex.code<<":"<<ex.description;
        QMessageBox msg;
        msg.setText(ex.description);
        msg.exec();
        if(ex.is_fatal){
            exit(ex.code);
        }
        else
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
    catch(...)
    {
        //do smth
    }

    uint32_t FC_MASK=0x00000780;
    uint32_t NN_MASK=0x7F;
    if((((boot_up_frame.can_id)&FC_MASK)==0x700) && boot_up_frame.len==1){
        encoder->node_num=(boot_up_frame.can_id)&NN_MASK;
        ui->label_7->setText(QString::number(encoder->node_num));
        emit status_pre_operational();
        msg.setText("Датчик активен");
        msg.exec();

        OpenData data;
        data.command=encoder->get_param;
        data.index=0x3001;
        data.subindex=0x0;
        data.SP_data_buff=nullptr;
        send_SDO_msg(socket_handle,func_codes::SDO_tx,encoder->node_num,&data,4);//хотим узнать скорость передачи
        struct can_frame baud_frame;
        try
        {
            baud_frame=recv_SDO_msg(socket_handle);
        }
        catch(...){}
        if((((baud_frame.can_id)&FC_MASK)==func_codes::SDO_rx) && (baud_frame.data[0]==encoder->recv_param_u8)){
            encoder->boudrate=baud_frame.data[4];
            ui->label_13->setText(QString::number(encoder->boudrate));
        }
        else
            throw "";
    }
    else
        throw "";
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
    if(current_nn>=node_number::min_number && current_nn<=node_number::max_number && current_nn!=0)
    {
        send_rule_msg(socket_handle,func_codes::NMT,current_nn,encoder->reset);
    }
    else{
        msg.setText("Некоректный номер узла");
        msg.exec();
        return;
    }
    emit status_stopped();
    emit start_program(socket_handle);
}


void MainWindow::on_pushButton_5_clicked()//сконфигурировать датчик с новыми настройками
{
    ui->pushButton_5->setHidden(true);
    uint32_t FC_MASK=0x00000780;
    uint32_t NN_MASK=0x7F;
    //новые настройки можно применять только если они отличаются от старых
    //1.node number
    int value=ui->spinBox_2->value();
    if(value!=ui->label_7->text().toInt())
    {
        OpenData open_data;
        open_data.command=encoder->set_param;
        open_data.index=0x3000;
        open_data.subindex=0x0;
        CODT::canbyte data[1];
        data[0]=value;
        uint8_t len=5;
        send_SDO_msg(socket_handle,func_codes::SDO_tx,encoder->node_num,&open_data,len,true);
        struct can_frame node_frame;
        try{
            node_frame=recv_SDO_msg(socket_handle);
        }
        catch(...){}
        if(((node_frame.can_id)&FC_MASK)==func_codes::SDO_rx){
            //все ок
        }
    }
    //2.baudrate
    int combo_index=ui->comboBox->currentIndex();
    if((ui->comboBox->itemText(combo_index)).toUInt()!=ui->label_13->text().toUInt())
    {
        OpenData open_data;
        CODT::canbyte data[1];
        data[0]=rates[ui->comboBox->itemText(combo_index)];
        open_data.command=encoder->set_param;
        open_data.index=0x3001;
        open_data.subindex=0x00;
        uint8_t len=5;
        send_SDO_msg(socket_handle,func_codes::SDO_tx,encoder->node_num,&open_data,len);
        struct can_frame baud_frame;
        try{
           baud_frame=recv_SDO_msg(socket_handle);
        }
        catch(...){}
        if(((baud_frame.can_id)& FC_MASK)==func_codes::SDO_rx){
            //все ок
        }
    }
    //3.Resolution
    int combo_index_2=ui->comboBox_2->currentIndex();
    double choosen_resolution=(ui->comboBox_2->itemText(combo_index_2)).toDouble();
    if(choosen_resolution!=ui->label_15->text().toDouble()){
        OpenData open_data;
        BYTES bytes;
        find_low_and_high_byte(resolutions_dict[choosen_resolution],bytes);
        CODT::canbyte data[2];
        data[0]=bytes.low;
        data[1]=bytes.high;
        open_data.command=encoder->set_param;
        open_data.index=0x6000;
        open_data.subindex=0x00;
        uint8_t len=6;
        send_SDO_msg(socket_handle,func_codes::SDO_tx,encoder->node_num,&open_data,len);
        struct can_frame res_frame;
        try{
           res_frame=recv_SDO_msg(socket_handle);
        }
        catch(...){}
        if(((res_frame.can_id)&FC_MASK)==func_codes::SDO_rx){
           //все ок
        }
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
    send_SDO_msg(socket_handle,func_codes::SDO_tx,encoder->node_num,&open_data);
    //переключить на новую скорость CAN-контроллер и принять 2 мистических сообщения???
    QString rate=ui->comboBox->itemText(combo_index)+"000";
    try
    {
        reconfigure_interface(rate);
    }
    catch(...){}
    struct can_frame flash_answer_1, flash_answer_2;

    try{
        flash_answer_1=recv_SDO_msg(socket_handle);
       /// if((((flash_answer_1.can_id)&FC_MASK)==0x80) && flash_answer_1.len==1){
       //   qDebug()<<"Мистическое сообщение 1 принято";
           qDebug()<<"Мистическое сообщение 1:"<<"can_id:"<<flash_answer_1.can_id<<"len:"<<flash_answer_1.len;

        flash_answer_2=recv_SDO_msg(socket_handle);
      //  if((((flash_answer_1.can_id)&FC_MASK)==0x80) && flash_answer_1.len==8){
      //     qDebug()<<"Мистическое сообщение 2 принято";
       // }

        qDebug()<<"Мистическое сообщение 2:"<<"can_id:"<<flash_answer_2.can_id<<"len:"<<flash_answer_2.len;
    }
    catch(...){}
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
        catch(...){}
        if((((baud_frame.can_id)&FC_MASK)==func_codes::SDO_rx) && (baud_frame.data[0]==encoder->recv_param_u8)){
           qDebug()<<"Питание не снято, но новые параметры установлены";
           qDebug()<<"Реальная скорость"<<baud_frame.data[4];
        }
    }
    //включение питания
    struct can_frame boot_frame;
    try{
        boot_frame=recv_SDO_msg(socket_handle);
    }
    catch(...){}

    if((((boot_frame.can_id)&FC_MASK)==0x700) && (((boot_frame.can_id)&NN_MASK)==value)){
        encoder->node_num=value;
        encoder->status=encoder->Pre_Operational;
        encoder->resolution=choosen_resolution;
        encoder->boudrate=rates[ui->comboBox->itemText(combo_index)];
        qDebug()<<"boot frame получен и конфигурация завершена";
        emit status_pre_operational();
    }
    ui->pushButton_5->setHidden(false);//снова можно конфигурировать датчик
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
    QString output=process.readAll();
    return output;
}

void MainWindow::reconfigure_interface(const QString& rate){
    QString command,out;
    command="ip link set can0 down";
    out=executeSudoCommand(command);
    if(out.contains("cannot find device",Qt::CaseInsensitive))
    {
        exception ex;
        ex.code=if_not_found;
        ex.description="CAN-устройство не обнаружено";
        ex.is_fatal=true;
        throw(ex);
    }

    command="ip link set can0 up type can bitrate "+rate+" loopback off";
    out=executeSudoCommand(command);
    if(out.contains("cannot find device",Qt::CaseInsensitive))
    {
        exception ex;
        ex.code=if_not_found;
        ex.description="CAN-устройство не обнаружено";
        ex.is_fatal=true;
        throw(ex);
    }

}
