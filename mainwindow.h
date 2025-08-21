#pragma once
#include <QMainWindow>
#include <QVector>
#include <QString>
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
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
    void on_next_slide_clicked();
    void on_cat_clicked();
    void on_dog_clicked();

private:
    Ui::MainWindow *ui;
    QVector<QString> album;
    int currentIndex;
    int selected;
    QSqlDatabase db;
    void showCurrentImage();
    void classifyCurrentImage();
    void saveResult(const QString &img, int userChoice, const QString &aiAnswer);
    cv::dnn::Net net;
    std::vector<std::string> labels;
};
