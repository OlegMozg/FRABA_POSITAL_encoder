#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFrame>
#include <socket_usage.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    encoder=new fraba_posital_encoder();
    encoder->min_node_num=node_number::min_number;
    encoder->max_node_num=node_number::max_number;
    encoder->min_cycle_time=encoder->MIN_period;
    encoder->max_cycle_time=encoder->MAX_period;
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
   // ui->pushButton_4->setText("Stopped");
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
    ui->label_9->setText("Current values:");
    ui->label_8->setText("Node number:");
    ui->label_7->setText("");
    ui->label_10->setText("Status:");
    ui->label_11->setText("");
    ui->pushButton_6->setText("Reset");
    ui->label_12->setText("Baudrate");
    ui->label_13->setText("");

    QStringList list;
    for(auto el:velocity_to_screen)
        list.append(el+" KBit/s");
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
}

MainWindow::~MainWindow()//деструктор окна
{

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
    if(current_nn>=node_number::min_number && current_nn<=node_number::max_number && current_nn!=0)
    {
        send_rule_msg(socket_handle,func_codes::NMT,current_nn,encoder->set_pre_operational);
    }
    else{
        msg.setText("Некоректный номер узла");
        msg.exec();
        return;
    }
    emit status_pre_operational();
}


void MainWindow::on_pushButton_3_clicked()//перейти в operational
{
    QMessageBox msg;
    uint8_t current_nn=static_cast<uint8_t>(ui->label_7->text().toUInt());
    if(current_nn>=node_number::min_number && current_nn<=node_number::max_number && current_nn!=0)
    {
        send_rule_msg(socket_handle,func_codes::NMT,current_nn,encoder->set_operational);
    }
    else{
        msg.setText("Некоректный номер узла");
        msg.exec();
        return;
    }
    emit status_operational();
}




void MainWindow::on_pushButton_4_clicked()//перейти в stopped
{
//датчик не включен
}


void MainWindow::on_comboBox_currentIndexChanged(int index)//выбор скорости
{

}


void MainWindow::on_radioButton_clicked()//cyclic
{
    ui->spinBox->setEnabled(true);
    if(ui->radioButton->isChecked()){
        ui->radioButton_2->setChecked(false);
        ui->radioButton_3->setChecked(false);
    }
}


void MainWindow::on_radioButton_2_clicked()//sync
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

void MainWindow::program_run(int handle)//пытаемся получить boot-up msg
{
    QMessageBox msg;
    struct can_frame boot_up_frame;
    int nbytes=0;
    try{
        while(nbytes==0){
            nbytes=recv(handle,&boot_up_frame,sizeof(struct can_frame),0);
        }
        check_data(nbytes);
    }
    catch(...)
    {
        //
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
        nbytes=0;
        try
        {
            while(1){
                nbytes=recv(socket_handle,&baud_frame,sizeof(struct can_frame),0);
                if(nbytes!=0)
                    break;
            }
            check_data(nbytes);
        }
        catch(...)
        {
        //
        }
        if(((baud_frame.can_id)&FC_MASK)==func_codes::SDO_rx){
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
    uint32_t FC_MASK=0x00000780;
    //новые настройки можно применять только если они отличаются от старых
    //1.node number
    if(ui->spinBox_2->value()!=ui->label_7->text().toInt())
    {
        CODT::cannode NN=static_cast<CODT::cannode>(ui->spinBox_2->value());
        OpenData open_data;
        open_data.command=
    }
    //2.baudrate
    if((ui->comboBox->itemText(ui->comboBox->currentIndex())).toUInt()!=ui->label_13->text().toUInt())
    {
        int index=ui->comboBox->currentIndex();
        OpenData open_data;
        CODT::canbyte data[1];
        data[0]=rates.find(index)->second;
        open_data.command=encoder->set_param;
        open_data.index=0x3001;
        open_data.subindex=0x0;
        uint8_t len=5;
        send_SDO_msg(socket_handle,func_codes::SDO_tx,encoder->node_num,&open_data,len);
        int nbytes=0;
        struct can_frame baud_frame;
        while(nbytes==0)
            nbytes=recv(socket_handle,&baud_frame,sizeof(struct can_frame),0);
        try{
            check_data(nbytes);
        }
        catch(...){}
        if(((baud_frame.can_id)& FC_MASK)==func_codes::SDO_rx){
            //все ок
        }

    }
}

