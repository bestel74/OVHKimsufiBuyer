#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDesktopServices>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void checkIfAvailable(QString strReply);

private slots:
    void on_pb_launch_clicked();
    void saySomething(QString msg);
    void slot_checkAvailability();

private:
    Ui::MainWindow *ui;
    QTimer timer_check;
};

#endif // MAINWINDOW_H
