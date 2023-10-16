#ifndef EXTERNAL_DEVICE_ACCESS_H
#define EXTERNAL_DEVICE_ACCESS_H

#include <QString>
#include <QInputDialog>
#include <QMessageBox>
#include <QProcess>

static void ask_for_password(QWidget* parent){
    bool ok;
    QString password=QInputDialog::getText(parent,"Доступ к контроллеру","Введите пароль:",QLineEdit::Password,"",&ok,Qt::Window);
    if(ok && password!=""){
        parent->pa=password;
        return;
    }
    else{
        QMessageBox MSG;
        MSG.setText("Доступ запрещен.Повторите попытку или воспользуйтесь терминалом!");
        MSG.exec();
        parent->password="";
    }
}

static QString executeSudoCommand(QWidget* parent,const QString& command,QString& password){

    if(password=="")
        ask_for_password(parent);

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

#endif // EXTERNAL_DEVICE_ACCESS_H
