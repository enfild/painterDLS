#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pathToJson = getPath();

    QGraphicsScene *scene = new QGraphicsScene(ui->graphicsView);
    QPen pen(Qt::black);
    scene->addLine(0,90,180,90,pen);//x
    scene->addLine(90,0,90,180,pen);//y


    if(pathToJson.isEmpty())
        return;
    QFile jsonFile(pathToJson);

    if (!jsonFile.open(QIODevice::ReadOnly))
    {
        return;
    }
    QByteArray saveData = jsonFile.readAll();
    jsonFile.close();
    QJsonDocument doc(QJsonDocument::fromJson(saveData));
    QJsonArray jsonArray(doc.array());

    QVector<double> x(jsonArray.size()) , y(jsonArray.size());

    for(int i =0; i < jsonArray.size(); i++)
    {
        QJsonObject jsonObj = jsonArray.at(i).toObject();

        double depth = jsonObj.find("depth").value().toDouble();
        double azimuth = jsonObj.find("azimuth").value().toDouble();
        double inclination = jsonObj.find("inclination").value().toDouble();

        qDebug() << "DAI" << depth << azimuth << inclination;


    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getPath(){
    return QFileDialog::getOpenFileName(this,tr("Open Json File"),QString(),tr("JSON (*.json)"));
}
