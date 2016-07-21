#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->progressBar->setVisible(false);

    connect(&this->timer_check, &QTimer::timeout, this, &MainWindow::slot_checkAvailability);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::saySomething(QString msg) {
    ui->te_output->append(QDateTime::currentDateTime().toString(Qt::SystemLocaleLongDate) + ": " + msg);
}


void MainWindow::on_pb_launch_clicked()
{
    if(ui->pb_launch->text().contains("Go!")) {
        this->timer_check.setInterval(ui->sb_timerInterval->value() * 1000);

        this->timer_check.start();
        ui->pb_launch->setText("Stop it!");
        ui->progressBar->setVisible(true);

        saySomething("Superviser launched!");
        slot_checkAvailability();
    }
    else {
        this->timer_check.stop();
        ui->pb_launch->setText("Go!");
        ui->progressBar->setVisible(false);

        saySomething("Superviser stoped :(");
    }
}


void MainWindow::slot_checkAvailability() {
    saySomething("Checking availability...");

    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QNetworkRequest req( QUrl( QString("https://ws.ovh.com/dedicated/r2/ws.dispatcher/getAvailability2?callback=Request.JSONP.request_map.request_0") ) );
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (reply->error() == QNetworkReply::NoError) {
        //success
        QString strReply = (QString)reply->readAll();
        checkIfAvailable(strReply);

        delete reply;
    }
    else {
        //failure
        saySomething("Last request Failed");
        delete reply;
    }
}


void MainWindow::checkIfAvailable(QString strReply) {
    int wantedRefIndex = strReply.indexOf(ui->le_ref->text());
    if(wantedRefIndex > 0) {
        saySomething("Checking...");

        int startOfMetaZone = strReply.indexOf("[", wantedRefIndex);
        int endOfGeoMetaZone = strReply.indexOf("]", startOfMetaZone);

        int startOfZones = strReply.indexOf("[", endOfGeoMetaZone);
        int endOfZones = strReply.indexOf("]", startOfZones);

        int offsetMetaZone = startOfMetaZone;
        while(offsetMetaZone < endOfGeoMetaZone) {
            int availability = strReply.indexOf("availability", offsetMetaZone);
            int availabilityStart = strReply.indexOf("\":\"", availability) + 3;
            int availabilityLenght = strReply.indexOf(",", availabilityStart) - availabilityStart -1;

            // Unused for now
//                int geoZone = strReply.indexOf("zone", offsetMetaZone);
//                int geoZoneStart = strReply.indexOf("\":\"", geoZone) + 3;
//                int geoZoneLenght = strReply.indexOf(",", geoZoneStart) - geoZoneStart -2;

            QString testString = strReply.mid(availabilityStart, availabilityLenght);

            if(!testString.contains("unknown") && !testString.contains("unavailable")) {
                QString link = "https://www.kimsufi.com/fr/commande/kimsufi.xml?reference=" + ui->le_ref->text();
                QDesktopServices::openUrl(QUrl(link));

                on_pb_launch_clicked();

                return;
            }

            offsetMetaZone = strReply.indexOf("{", availability);
        }

        int offsetZone = startOfZones;
        while(offsetZone < endOfZones) {
            int availability = strReply.indexOf("availability", offsetZone);
            int availabilityStart = strReply.indexOf("\":\"", availability) + 3;
            int availabilityLenght = strReply.indexOf("\"", availabilityStart) - availabilityStart;

            // Unused for now
//                int geoZone = strReply.indexOf("zone", offsetZone);
//                int geoZoneStart = strReply.indexOf("\":\"", geoZone) + 3;
//                int geoZoneLenght = strReply.indexOf("\"", geoZoneStart) - geoZoneStart;

            QString testString = strReply.mid(availabilityStart, availabilityLenght);

            if(!testString.contains("unknown") && !testString.contains("unavailable")) {
                QString link = "https://www.kimsufi.com/fr/commande/kimsufi.xml?reference=" + ui->le_ref->text();
                QDesktopServices::openUrl(QUrl(link));

                on_pb_launch_clicked();

                return;
            }

            offsetZone = strReply.indexOf("{", availability);
        }

        saySomething("None available...");
    }
    else {
        saySomething("Ref not found, please check your ref and try again!");
        on_pb_launch_clicked();
    }
}
