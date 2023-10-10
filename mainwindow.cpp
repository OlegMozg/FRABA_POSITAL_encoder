#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFrame>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    encoder=new fraba_posital_encoder();
    QFont font =ui->pushButton->font();
    font.setPixelSize(18);
   // ui->pushButton->setFont(font);
    ui->label->setFont(font);
    ui->label_2->setFont(font);
    ui->pushButton->setText("Quit");
    ui->label->setText("Work mode:");
    ui->pushButton_2->setText("Pre-Operational");
    ui->pushButton_3->setText("Operational");
    ui->pushButton_4->setText("Stopped");
    ui->label_2->setText("Baudrate:");
    ui->label_3->setText("Transmission mode:");
    ui->radioButton->setText("Cyclic");
    ui->radioButton_2->setText("SYNC");
    ui->radioButton_3->setText("Polled");


    QStringList list;
    can_rates rates=encoder->get_rates();
    for(const auto& element:rates)
        list.append(QString::number(element.first)+" kBit/s");
    ui->comboBox->addItems(list);
    ui->comboBox->show();
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

}


void MainWindow::on_pushButton_3_clicked()//перейти в operational
{

}




void MainWindow::on_pushButton_4_clicked()//перейти в stopped
{

}


void MainWindow::on_comboBox_currentIndexChanged(int index)//выбор скорости
{

}


void MainWindow::on_radioButton_clicked()
{
    if(ui->radioButton->isChecked()){
        ui->radioButton_2->setChecked(false);
        ui->radioButton_3->setChecked(false);
    }
}


void MainWindow::on_radioButton_2_clicked()
{
    if(ui->radioButton_2->isChecked()){
        ui->radioButton->setChecked(false);
        ui->radioButton_3->setChecked(false);
    }
}


void MainWindow::on_radioButton_3_clicked()
{
    if(ui->radioButton_3->isChecked()){
        ui->radioButton->setChecked(false);
        ui->radioButton_2->setChecked(false);
    }
}

