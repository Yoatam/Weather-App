#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "QtNetwork/QNetworkAccessManager"
#include "QtNetwork/QNetworkRequest"

struct WeatherData {
    QString location;
    QString date;
    QString weatherCondition;
    int temperature;
    int pressure;
    int humidity;
    int windSpeed;
    QString windDirection;
    QString sunriseTime;
    QString sunsetTime;
};

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_queryButton_clicked();
    void handleNetworkReply(QNetworkReply *reply);  // Declaration added
    WeatherData parseWeatherData(QString jsonString);  // Declaration added

private:
    Ui::Widget *ui;
    QNetworkAccessManager *manager;
    QNetworkRequest request;

    void displayWeatherData(QString jsonString);
    QString direction(int deg);
    void clearDisplay();
};

#endif // WIDGET_H
