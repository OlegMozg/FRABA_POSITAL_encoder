#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFrame>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFont font =ui->pushButton->font();
    font.setPixelSize(20);
   // ui->pushButton->setFont(font);
    ui->label->setFont(font);
    ui->pushButton->setText("Quit");
    ui->label->setText("Mode:");
    ui->pushButton_2->setText("Pre-Operational");
    ui->pushButton_3->setText("Operational");
    ui->pushButton_4->setText("Stopped");

}

MainWindow::~MainWindow()//деструктор окна
{
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

