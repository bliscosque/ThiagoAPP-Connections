#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QList>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QStringConverter>

// #include "qtcsv/stringdata.h"
#include "qtcsv/reader.h"
// #include "qtcsv/writer.h"

QList<QList<QString>> readData;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //TO-DO: save in QSettings
    ui->edt_file->setText("/home/thiago/obsidian/3-Ericsson/connections.csv");
    ui->edt_dest->setText("/home/thiago/connection_scripts");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btn_load_clicked()
{
    ui->table_connections->clear();
    ui->table_connections->setRowCount(0);
    QString filePath=ui->edt_file->text();
    readData = QtCSV::Reader::readToList(filePath);
    ui->table_connections->setColumnCount(3);
    ui->table_connections->setHorizontalHeaderItem(0,new QTableWidgetItem(readData[0][0]));
    ui->table_connections->setHorizontalHeaderItem(1,new QTableWidgetItem(readData[0][1]));
    ui->table_connections->setHorizontalHeaderItem(2,new QTableWidgetItem(readData[0][6]));
    for (auto i = 1; i < readData.size(); i++)
    {
        QString cliente = readData[i][0]; // cliente
        QString pais = readData[i][1]; // pais
        QString serverName = readData[i][6];
        ui->table_connections->insertRow(i-1);
        ui->table_connections->setItem(i-1,0,new QTableWidgetItem(cliente));
        ui->table_connections->setItem(i-1,1,new QTableWidgetItem(pais));
        ui->table_connections->setItem(i-1,2,new QTableWidgetItem(serverName));
    }
}


void generateConnection(int item, QDir dir) {
    QMap<QString,QString> map;
    for (int i=0;i<readData[0].length();i++) {
        map[readData[0][i]]=readData[item][i];
    }
    qInfo() << map;
    // qInfo() << dir.absolutePath();
    dir.mkdir(map["Cliente"]);
    dir.cd(map["Cliente"]);
    dir.mkdir(map["Pais"]);
    dir.cd(map["Pais"]);

    QFile file(dir.absolutePath()+"/"+map["ServerName"]+".sh");
    qInfo() << file.fileName();

    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << "#!/usr/bin/expect -f\n";
        stream << "set OTP [lindex $argv 0]\n";
        stream << "set timeout -1\n";
        stream << "spawn ssh " + map["UID"] << "@" << map["RSG"] << "\n";

    }
    else {
        qInfo() << "Error opening file";
    }
}



void MainWindow::on_btn_connect_clicked()
{
    QDir dir(ui->edt_dest->text());

    int row = ui->table_connections->currentRow();
    qInfo() << "selected row: " << row;
    int csvItem=row+1;
    generateConnection(csvItem, dir);


}

// exemplo de comando
// Default WSL directory: \\wsl$\Ubuntu-22.04
// tbm ?? possivel chamar explorer.exe . de dentro do WSL
// wsl -e bash -c "/mnt/c/Users/egonthi/SYNC_WORK_PI4/clientes/general.sh /mnt/c/Users/egonthi/SYNC_WORK_PI4/clientes/Entel/entel_chile_DR_Enabler_chi3drmmenapf02.sh"

