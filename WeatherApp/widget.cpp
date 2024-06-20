#include "widget.h"
#include "ui_widget.h"
#include "QtNetwork/QNetworkReply"
#include "QUrl"
#include "QUrlQuery"
#include "QtNetwork/QNetworkAccessManager"
#include "QtNetwork/QNetworkRequest"
#include "QJsonDocument"
#include "QJsonArray"
#include "QJsonObject"
#include "QDateTime"


Widget::Widget(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::Widget),
    manager(new QNetworkAccessManager())
{
    ui->setupUi(this);
    clearDisplay();

    QObject::connect(manager, &QNetworkAccessManager::finished, this, &Widget::handleNetworkReply);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::handleNetworkReply(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        clearDisplay();
        ui->location->setText("Not Found");
        return;
    }

    QString jsonString = reply->readAll();
    qDebug() << jsonString;
    displayWeatherData(jsonString);
}

void Widget::on_queryButton_clicked()
{
    QString query = ui->query->text();
    QUrl url("http://api.openweathermap.org/data/2.5/weather");
    QUrlQuery attributes;
    attributes.addQueryItem("q", query);
    attributes.addQueryItem("appid", "8303e012321f9c71afb4bee2c6bd85d1");
    attributes.addQueryItem("units", "metric");
    url.setQuery(attributes);
    request.setUrl(url);
    manager->get(request);
}

void Widget::displayWeatherData(QString jsonString){
    WeatherData data = parseWeatherData(jsonString);

    ui->query->setText(data.location);
    ui->location->setText(data.location);
    ui->date->setText(data.date);
    ui->weatherlabel->setText(data.weatherCondition);
    ui->temperature->setText(QString::number(data.temperature));
    ui->pressurevalue->setText(QString::number(data.pressure).append(" hPa"));
    ui->humidityValue->setText(QString::number(data.humidity).append("%"));
    ui->windSpeedValue->setText(QString::number(data.windSpeed).append(" m/s"));
    ui->windDirValue->setText(data.windDirection);
    ui->sunriseValue->setText(data.sunriseTime);
    ui->sunsetValue->setText(data.sunsetTime);
}

WeatherData Widget::parseWeatherData(QString jsonString) {
    WeatherData data;

    QJsonDocument jsonResponse = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonObject rootObject = jsonResponse.object();
    QJsonObject mainObject = rootObject["main"].toObject();
    QJsonObject weatherObject = rootObject["weather"].toArray().at(0).toObject();
    QJsonObject windObject = rootObject["wind"].toObject();
    QJsonObject sysObject = rootObject["sys"].toObject();

    data.location = rootObject["name"].toString() + ", " + sysObject["country"].toString();
    data.date = QDateTime::fromSecsSinceEpoch(sysObject["sunrise"].toInt()).date().toString();
    data.weatherCondition = weatherObject["main"].toString();
    data.temperature = mainObject["temp"].toDouble();
    data.pressure = mainObject["pressure"].toInt();
    data.humidity = mainObject["humidity"].toInt();
    data.windSpeed = windObject["speed"].toDouble();
    data.windDirection = direction(windObject["deg"].toInt());
    data.sunriseTime = QDateTime::fromSecsSinceEpoch(sysObject["sunrise"].toInt()).time().toString().remove(5, 3);
    data.sunsetTime = QDateTime::fromSecsSinceEpoch(sysObject["sunset"].toInt()).time().toString().remove(5, 3);

    return data;
}



QString Widget::direction(int deg) {
    deg += 23;
    deg %= 360;

    QString direction;

    switch ((deg / 45) % 8) {
    case 0:
        direction = "North"; break;
    case 1:
        direction = "North-East"; break;
    case 2:
        direction = "East"; break;
    case 3:
        direction = "South-East"; break;
    case 4:
        direction = "South"; break;
    case 5:
        direction = "South-West"; break;
    case 6:
        direction = "West"; break;
    case 7:
        direction = "North-West"; break;
    default:
        direction = "Unknown"; break;
    }

    return direction;
}

void Widget::clearDisplay(){
    ui->location->setText("Location");
    ui->date->setText("Date");
    ui->weatherlabel->setText("Partly sunny");
    ui->temperature->setText(QString::number(0));
    ui->pressurevalue->setText("0 hPa");
    ui->humidityValue->setText(" 0 %");
    ui->windSpeedValue->setText(QString::number(0).append(" m/s"));
    ui->windDirValue->setText(direction(0));
    ui->sunriseValue->setText("00:00");
    ui->sunsetValue->setText("00:00");
}

