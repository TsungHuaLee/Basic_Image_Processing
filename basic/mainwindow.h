#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include <QtCharts>
#include <vector>
#include <stack>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_1_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_chooseFile_clicked();

    void on_undo_clicked();

private:
    Ui::MainWindow *ui;
    QString str;
    QImage image;
    QImage gray_image;

    int now = -1;
    bool existGray = false;
    bool existColor = false;
    double threshold = 0;
    uchar* sobelResult = NULL;
    std::stack<int> pre_operation;
    std::stack<QImage> pre_image;


    void reset();
    void meanFilter (unsigned char* corrupted, unsigned char* smooth, int width, int height);
    void medianFilter (unsigned char* corrupted, unsigned char* smooth, int width, int height);
    QImage convert2gray (QImage input);
    void plotHistogram(QChartView *graphicview, std::vector<int> data);
    void showGrayImage(QGraphicsView *graphicview, uchar* oneChannel, int width, int height, bool store=false);
    void sobelOperation (unsigned char* origin, int width, int height);
    uchar* scaling (unsigned char* origin, unsigned char* processed, int width, int height, double scale_width, double scale_height);
    void rotation (unsigned char* origin, unsigned char* processed, int width, int height, double theta);
    void rotation_size(int width, int height, double theta, int &new_width, int &new_height);
};

#endif // MAINWINDOW_H
