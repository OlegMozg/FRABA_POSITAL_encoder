#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <fraba_encoder.h>
#include <canopen_local.h>
#include <map>
#include <exception>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();


    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

    void program_run(int);

    void is_stopped();
    void is_pre_operational();
    void is_operational();

    void on_pushButton_6_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_7_clicked();

public:
signals:
    void start_program(int);
    void status_stopped();
    void status_pre_operational();
    void status_operational();

private:
    Ui::MainWindow *ui;
    const QStringList velocity_to_screen={"20","50","100","125","250","500","800","1000"};
    const QStringList resolutions={"0.01","0.1","1"};

    std::map<QString,uint8_t> rates={
        {"20",boudrates::_20},
        {"50",boudrates::_50},
        {"100",boudrates::_100},
        {"125",boudrates::_125},
        {"250",boudrates::_250},
        {"500",boudrates::_500},
        {"800",boudrates::_800},
        {"1000",boudrates::_1000}
    };

    std::map<double,uint16_t> resolutions_dict={
        {0.001,fraba_posital_encoder::_0_001},
        {0.01,fraba_posital_encoder::_0_01},
        {0.1,fraba_posital_encoder::_0_1},
        {1,fraba_posital_encoder::_1}
    };

    QString password;
    fraba_posital_encoder* encoder;
    const char* if_name="can0";
    int socket_handle;


    void ask_for_password();
    QString executeSudoCommand(const QString& command);
    void reconfigure_interface(const QString& rate);
};
#endif // MAINWINDOW_H
