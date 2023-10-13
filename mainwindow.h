#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <fraba_encoder.h>
#include <canopen_local.h>
#include <map>

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

    void on_pushButton_4_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

    void program_run(int);

    void is_stopped();
    void is_pre_operational();
    void is_operational();

    void on_pushButton_6_clicked();

    void on_pushButton_5_clicked();

public:
signals:
    void start_program(int);
    void status_stopped();
    void status_pre_operational();
    void status_operational();

private:
    const QStringList velocity_to_screen={"20","50","100","125","250","500","800","1000"};
    const QStringList resolutions={"0.01","0.1","1"};

    std::map<int,uint8_t> rates={
        {0,boudrates::_20},
        {1,boudrates::_50},
        {2,boudrates::_100},
        {3,boudrates::_125},
        {4,boudrates::_250},
        {5,boudrates::_500},
        {6,boudrates::_800},
        {7,boudrates::_1000}
    };

    std::map<double,uint16_t> resolutions_dict={
        {0.001,fraba_posital_encoder::_0_001},
        {0.01,fraba_posital_encoder::_0_01},
        {0.1,fraba_posital_encoder::_0_1},
        {1,fraba_posital_encoder::_1}
    };

    fraba_posital_encoder* encoder;
    const char* if_name="can0";
    int socket_handle;

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
