#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->spinBox->setRange(-180, 360);
    ui->spinBox->setValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getPath(){
    return QFileDialog::getOpenFileName(this,tr("Open Json File"),QString(),tr("JSON (*.json)"));
}

void MainWindow::calculateDLS(QString pathToJson){

    QChart *chart = new QChart();
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
    QVector<double> depth (jsonArray.size()), azimuth(jsonArray.size()), inclination(jsonArray.size()), DLS(jsonArray.size()), rotatedDLS(jsonArray.size());

    QLineSeries *series = new QLineSeries();
    QLineSeries *series2 = new QLineSeries();

    for(int i = 1; i < jsonArray.size(); i++)
    {
        QJsonObject jsonObj = jsonArray.at(i).toObject();

        depth[i] = jsonObj.find("depth").value().toDouble();
        azimuth[i] = jsonObj.find("azimuth").value().toDouble();
        inclination[i] = jsonObj.find("inclination").value().toDouble();
        // Ищем угол
        DLS[i] = (1 / cos((cos(inclination[i - 1]) * cos(inclination[i])) + (sin(inclination[i - 1]) * sin(inclination[i])) * cos(azimuth[i] + (-azimuth[i - 1])))) * (100 / (depth[i] - depth[i - 1]));
        //методом прямоугольного треугольника ищем смещение по горизонтали
        auto b = tan(DLS[i]) * (depth[i] - depth[i - 1]);
        DLS[i] = DLS[i - 1] + b;
        // ищем смещенный угол
        rotatedDLS[i] = (1 / cos((cos(inclination[i - 1]) * cos(inclination[i])) + (sin(inclination[i - 1]) * sin(inclination[i])) * cos(azimuth[i] + sin(ui->spinBox->value()) + (-azimuth[i - 1] + sin(ui->spinBox->value()) ) ))) * (100 / (depth[i] - depth[i - 1]));
        //методом прямоугольного треугольника ищем смещение по горизонтали
        auto c = tan(rotatedDLS[i]) * (depth[i] - depth[i - 1]);
        rotatedDLS[i] = rotatedDLS[i - 1] + c;

        series->append(DLS[i], ( -depth[i]));
        series2->append(rotatedDLS[i], ( -depth[i]));
    }

    series->setName("Trajectory");
    series2->setName("rotated Trajectory");

    chart->addSeries(series);
    chart->addSeries(series2);

    chart->legend()->setAlignment(Qt::AlignBottom);

    chart->setTitle("Trajectory DEMO");

    QValueAxis *axisX = new QValueAxis;
    axisX->setTitleText("Metrs");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText("Depth");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->show();
}

void MainWindow::on_pushButton_clicked()
{
    pathToJson = getPath();
    calculateDLS(pathToJson);
}
