#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include<iostream>
#include <string>
#include <algorithm>    // std::sort
#include <math.h>
#include <QBitmap>
#include <QColor>
#include <QtGui>
#include <QRgb>

#include <QBarSet>
#include <QtCharts>
#include <QBarSeries>
#include <QPen>

#define PI 3.14159265

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->undo->setEnabled(false);
    reset();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_1_clicked()
{
    if(now == 1){
        std::cout<<"checked test 1"<<std::endl;
        return;
    }
    now = 1;
    // reset window
    reset();
    ui->graphicsView_before->setVisible(true);
    ui->graphicsView_1->setVisible(true);
    ui->graphicsView_2->setVisible(true);
    ui->graphicsView_3->setVisible(true);
    ui->graphicsView_u_1->setVisible(true);
    ui->graphicsView_hist->setVisible(false);
    ui->graphicsView_hist_2->setVisible(false);
    // set before
    QGraphicsScene *graphic = new QGraphicsScene(this);
    graphic->addPixmap( QPixmap::fromImage(image));

    if(!existColor)
    {   // create gray image
        image = QImage("../ExampleImage/A_RGB.bmp");
        gray_image = convert2gray(image);
        graphic->addPixmap( QPixmap::fromImage(image));
    }
    else
    {
        graphic->addPixmap( QPixmap::fromImage(image));
    }

    ui->graphicsView_before->setFixedSize(image.size());
    ui->graphicsView_before->setSceneRect(0, 0, image.width(), image.height());
    ui->graphicsView_before->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_before->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_before->setScene(graphic);

    // set label
    ui->label_after_1->setText("R channel");
    ui->label_after_2->setText("G channel");
    ui->label_after_3->setText("B channel");
    ui->label_u_1->setText("Grayscale");
    // ==========================================================
    //    QImage::Format_RGB32    //儲存格式為B,G,R,A 對應0,1,2,3
    int format = 4;
    uchar *imagebits_32;
    uchar* RChannel = (unsigned char*) malloc(image.height()*image.width()*sizeof(unsigned char));
    uchar* GChannel = (unsigned char*) malloc(image.height()*image.width()*sizeof(unsigned char));
    uchar* BChannel = (unsigned char*) malloc(image.height()*image.width()*sizeof(unsigned char));
    uchar* pixelVal = (unsigned char*) malloc(image.height()*image.width()*sizeof(unsigned char));

    // extract RGB
    for(int i = 0; i < image.height(); i++)
    {
        imagebits_32 = image.scanLine(i);
        for(int j = 0; j < image.width(); j++)
        {
            BChannel[i*image.width() + j] = imagebits_32[ j * format + 0];
            GChannel[i*image.width() + j] = imagebits_32[ j * format + 1];
            RChannel[i*image.width() + j] = imagebits_32[ j * format + 2];
            pixelVal[i*image.width() + j] = (double)RChannel[i*image.width() + j]*0.299 + (double)GChannel[i*image.width() + j]*0.587 + (double)BChannel[i*image.width() + j]*0.114;
        }
    }
    // set r channel image
    showGrayImage(ui->graphicsView_1, RChannel, image.width(), image.height());

    // set g channel image
    showGrayImage(ui->graphicsView_2, GChannel, image.width(), image.height());

    // set b channel image
    showGrayImage(ui->graphicsView_3, BChannel, image.width(), image.height());

    // set gray image
    showGrayImage(ui->graphicsView_u_1, pixelVal, image.width(), image.height(), true);

    pre_operation.push(1);
}

void MainWindow::on_pushButton_2_clicked()
{
    if(now == 2){
        std::cout<<"checked test 2"<<std::endl;
        return;
    }
    now = 2;
    // reset window
    reset();
    ui->graphicsView_before->setVisible(true);
    ui->graphicsView_1->setVisible(true);
    ui->graphicsView_2->setVisible(true);
    ui->graphicsView_3->setVisible(false);
    ui->graphicsView_u_1->setVisible(false);
    ui->graphicsView_hist->setVisible(false);
    ui->graphicsView_hist_2->setVisible(false);
    // set before
    QGraphicsScene *graphic = new QGraphicsScene(this);
    if(!existGray)
    {   // create gray image
        image = QImage("../ExampleImage/B_noisy.bmp");
        gray_image = convert2gray(image);
        graphic->addPixmap( QPixmap::fromImage(gray_image));
    }
    else
    {
        graphic->addPixmap( QPixmap::fromImage(gray_image));
    }


    ui->graphicsView_before->setFixedSize(image.size());
    ui->graphicsView_before->setSceneRect(0, 0, image.width(), image.height());
    ui->graphicsView_before->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_before->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_before->setScene(graphic);


    // set label
    ui->label_after_1->setText("mean filter");
    ui->label_after_2->setText("median filter");
    // ==========================================================
    // get pixel from original image
    uchar* grayscale = (unsigned char*) malloc(image.height()*image.width()*sizeof(unsigned char));
    uchar* smooth_mean = (unsigned char*) malloc(image.height()*image.width()*sizeof(unsigned char));
    uchar* smooth_median = (unsigned char*) malloc(image.height()*image.width()*sizeof(unsigned char));
    uchar* imagebits_32;
    for(int i = 0; i < image.height(); i++)
    {
        imagebits_32 = image.scanLine(i);
        for(int j = 0; j < image.width(); j++)
        {
            grayscale[i*image.width() + j] = imagebits_32[ j * 4 + 0];
        }
    }

    // set meanFilter
    meanFilter(grayscale, smooth_mean, image.width(), image.height());
    // show image
    showGrayImage(ui->graphicsView_1, smooth_mean, image.width(), image.height());

    // set medianFileter
    medianFilter(grayscale, smooth_median, image.width(), image.height());
    // show image
    showGrayImage(ui->graphicsView_2, smooth_median, image.width(), image.height(), true);

    pre_operation.push(2);
}

void MainWindow::reset()
{
    // reset label
    ui->label_after_1->setText("");
    ui->label_after_2->setText("");
    ui->label_after_3->setText("");
    ui->label_u_1->setText("");

    // reset image
    QImage empty = QImage(image.width(), image.height(), QImage::Format_Indexed8);

    ui->graphicsView_before->setVisible(false);
    ui->graphicsView_1->setVisible(false);
    ui->graphicsView_2->setVisible(false);
    ui->graphicsView_3->setVisible(false);
    ui->graphicsView_u_1->setVisible(false);
    ui->graphicsView_hist->setVisible(false);
    ui->graphicsView_hist_2->setVisible(false);
    if(!pre_operation.empty())
         ui->undo->setEnabled(true);
}

void MainWindow::meanFilter (unsigned char* corrupted, unsigned char* smooth, int width, int height)
{
    memcpy ( smooth, corrupted, width*height*sizeof(unsigned char) );
    std::cout<<width<<" "<<height<<std::endl;
    for (int i=1;i<height-1;i++)
    {
        for (int j=1;j<width-1;j++)
        {
            int temp = 0;
            for(int k = -1; k < 2; k++)
                for(int u = -1; u < 2; u++)
                {
                    temp += corrupted[(i+k)*width + (j+u)];
                }
            smooth[i*width + j] = temp/9;
        }
    }
}

void MainWindow::medianFilter (unsigned char* corrupted, unsigned char* smooth, int width, int height)
{
    memcpy ( smooth, corrupted, width*height*sizeof(unsigned char) );
    std::vector<uchar> mask;
    for (int i=1;i<height-1;i++)
    {
        for (int j=1;j<width-1;j++)
        {
            mask.clear();
            for(int k = -1; k < 2; k++)
                for(int u = -1; u < 2; u++)
                {
                    mask.push_back(corrupted[(i+k)*width + (j+u)]);
                }
            std::sort(mask.begin(), mask.end());

            smooth [ i*width+j ] = mask[5];
        }
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    if(now == 3){
        std::cout<<"checked test 3"<<std::endl;
        return;
    }
    now = 3;
    // reset window
    reset();
    ui->graphicsView_before->setVisible(true);
    ui->graphicsView_1->setVisible(false);
    ui->graphicsView_2->setVisible(false);
    ui->graphicsView_3->setVisible(false);
    ui->graphicsView_u_1->setVisible(true);
    ui->graphicsView_hist->setVisible(true);
    ui->graphicsView_hist_2->setVisible(true);
    QGraphicsScene *graphic = new QGraphicsScene(this);
    // set before
    if(!existGray)
    {   // create gray image
        image = QImage("../ExampleImage/A_RGB.bmp");
        gray_image = convert2gray(image);
        graphic->addPixmap( QPixmap::fromImage(gray_image));
    }
    else
    {
        graphic->addPixmap( QPixmap::fromImage(gray_image));
    }
    ui->graphicsView_before->setFixedSize(gray_image.size());
    ui->graphicsView_before->setSceneRect(0, 0, gray_image.width(), gray_image.height());
    ui->graphicsView_before->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_before->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_before->setScene(graphic);

    // set label
    ui->label_u_1->setText("Result");
    // ==========================================================
    std::vector<int> gray_level_distribution(300, 0), result_level_distribution(300, 0);
    int total_pixel = gray_image.height() * gray_image.width();

    uchar* gray_value = (unsigned char*) malloc(gray_image.height()*gray_image.width()*sizeof(unsigned char));
    uchar* imagebits_32;
    for(int i = 0; i < gray_image.height(); i++)
    {
        imagebits_32 = gray_image.scanLine(i);
        for(int j = 0; j < gray_image.width(); j++)
        {
            gray_value[i*gray_image.width() + j] = imagebits_32[ j * 4 + 0];
            gray_level_distribution[(int)gray_value[i*gray_image.width() + j]]++;
        }
    }
    plotHistogram(ui->graphicsView_hist,gray_level_distribution);
    // calculate Cumulative Distribution Function
    std::vector<int> cdf(300, 0);
    int accu = 0, cdf_min = 0, cdf_max = 0;
    for(size_t i = 0; i < 256; i++)
    {
        if(cdf_min == 0 && gray_level_distribution[i]!=0)
            cdf_min = gray_level_distribution[i];
        if(cdf_max < gray_level_distribution[i])
            cdf_max = gray_level_distribution[i];
        accu += gray_level_distribution[i];
        cdf[i] = accu;
    }

    // set new image
//    QImage img = QImage(gray_image.width(), gray_image.height(), QImage::Format_ARGB32);

    uchar* histogram_equal = (unsigned char*) malloc(image.height()*image.width()*sizeof(unsigned char));

    for(int i = 0; i < gray_image.height(); i++)
    {
        for(int j = 0; j < gray_image.width(); j++)
        {
            size_t pixel_val = (size_t)gray_value[i*gray_image.width() + j];
            int histogram_equal_value = (int)round( (double)(cdf[pixel_val] - cdf_min)/(double)(total_pixel - cdf_min) * 255 );
            histogram_equal[i*gray_image.width()+j] = histogram_equal_value;
//            img.setPixel(j, i, QColor(histogram_equal,histogram_equal,histogram_equal,0).rgb());
            result_level_distribution[histogram_equal_value]++;
        }
    }
    // update grayscale image
//    gray_image = img;
    plotHistogram(ui->graphicsView_hist_2,result_level_distribution);
//    QGraphicsScene *graphic_result = new QGraphicsScene(this);
//    graphic_result->addPixmap( QPixmap::fromImage(img));

//    ui->graphicsView_u_1->setFixedSize(image.size());
//    ui->graphicsView_u_1->setSceneRect(0, 0, image.width(), image.height());
//    ui->graphicsView_u_1->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    ui->graphicsView_u_1->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    ui->graphicsView_u_1->setScene(graphic_result);

    showGrayImage(ui->graphicsView_u_1, histogram_equal, image.width(), image.height(), true);

    pre_operation.push(3);
}

QImage MainWindow::convert2gray(QImage input)
{
    uchar* pixelVal = (unsigned char*) malloc(input.height()*input.width()*sizeof(unsigned char));
    uchar* imagebits_32;
    uchar BChannel, GChannel, RChannel;
    // extract RGB
    for(int i = 0; i < input.height(); i++)
    {
        imagebits_32 = input.scanLine(i);
        for(int j = 0; j < input.width(); j++)
        {
            BChannel = imagebits_32[ j * 4 + 0];
            GChannel = imagebits_32[ j * 4 + 1];
            RChannel = imagebits_32[ j * 4 + 2];
            pixelVal[i*input.width() + j] = (double)RChannel*0.299 + (double)GChannel*0.587 + (double)BChannel*0.114;
        }
    }

    QImage gray(input.width(), input.height(), QImage::Format_ARGB32);

    for(int i = 0; i < input.height(); i++)
    {
        for(int j = 0; j < input.width(); j++)
        {
            int one_pixel = pixelVal[i*input.width() + j];
            gray.setPixel(j, i, QColor(one_pixel,one_pixel,one_pixel,0).rgb());
        }
    }
    return gray;
}

void MainWindow::plotHistogram(QChartView *graphicview, std::vector<int> data)
{
    // create one set of bar in bar chart
    QBarSet *set = new QBarSet("hist1");
    // setting color of bar
    QPen pen = set->pen();
    pen.setWidth(1);
    pen.setBrush(QBrush("blue"));
    set->setPen(pen);
    // setting value of bar
    for(int i = 0; i < 256; i++)
        *set<<data[i];
    // series groups the data from sets to categories
    QBarSeries *series = new QBarSeries();
    series->append(set);
    // append barseries to chart, and set chart
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Histogram of gray level");
    // set x-axis
    QValueAxis *axisX = new QValueAxis; //定义X轴
    axisX->setRange(0, 255); //设置范围
    axisX->setLabelFormat("%g"); //设置刻度的格式
    axisX->setGridLineVisible(true); //设置是否显示网格线
    axisX->setMinorTickCount(4); //设置小刻度线的数目
    chart->addAxis(axisX, Qt::AlignBottom);

    // setting on graphics view
    graphicview->setChart(chart);
    graphicview->setRenderHint(QPainter::Antialiasing);
    graphicview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    return;
}

void MainWindow::showGrayImage(QGraphicsView *graphicview, uchar* oneChannel, int width, int height, bool store)
{
    // set r channel image
    QImage img = QImage(width, height, QImage::Format_ARGB32);

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            int one_pixel = oneChannel[i*width + j];
            img.setPixel(j, i, QColor(one_pixel,one_pixel,one_pixel,0).rgb());
        }
    }

    QGraphicsScene *graphic = new QGraphicsScene(this);
    graphic->addPixmap( QPixmap::fromImage(img));

    graphicview->setFixedSize(img.size());
    graphicview->setSceneRect(0, 0, width, height);
    graphicview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicview->setScene(graphic);

    if(store)
    {
        gray_image = img;
        existGray = true;
        pre_image.push(img);
    }
}

void MainWindow::on_pushButton_4_clicked()
{

    if(now == 4 && (int)threshold == ui->threshold->value()){
        std::cout<<"checked test 4 "<<threshold<<" "<<ui->threshold->value()<<std::endl;
        return;
    }
    now = 4;
    // reset window
    reset();
    ui->graphicsView_before->setVisible(true);
    ui->graphicsView_1->setVisible(false);
    ui->graphicsView_2->setVisible(false);
    ui->graphicsView_3->setVisible(false);
    ui->graphicsView_u_1->setVisible(true);
    ui->graphicsView_hist->setVisible(false);
    ui->graphicsView_hist_2->setVisible(false);
    // set before
    QGraphicsScene *graphic = new QGraphicsScene(this);
    if(!existGray)
    {   // create gray image
        image = QImage("../ExampleImage/A_RGB.bmp");
        gray_image = convert2gray(image);
        graphic->addPixmap( QPixmap::fromImage(gray_image));
    }
    else
    {
        graphic->addPixmap( QPixmap::fromImage(gray_image));
    }
    ui->graphicsView_before->setFixedSize(gray_image.size());
    ui->graphicsView_before->setSceneRect(0, 0, gray_image.width(), gray_image.height());
    ui->graphicsView_before->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_before->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_before->setScene(graphic);

    // set label
    ui->label_u_1->setText("Result");
    // ==========================================================
    // get threshold from spinbox
    threshold = ui->threshold->value();
    uchar* pixelVal = (unsigned char*) malloc(gray_image.height()*gray_image.width()*sizeof(unsigned char));

    uchar* imagebits_32;
    for(int i = 0; i < gray_image.height(); i++)
    {
        imagebits_32 = gray_image.scanLine(i);
        for(int j = 0; j < gray_image.width(); j++)
        {
            pixelVal[i*gray_image.width() + j] = (imagebits_32[ j * 4 + 0]>=threshold)?255:0;
        }
    }
    showGrayImage(ui->graphicsView_u_1, pixelVal, gray_image.width(), gray_image.height(), true);

    pre_operation.push(4);
}

void MainWindow::on_pushButton_5_clicked()
{
    if(now == 5 && (int)threshold == ui->threshold_sobel->value()){
        std::cout<<"checked test 5 "<<threshold<<" "<<ui->threshold_sobel->value()<<std::endl;
        return;
    }
    now = 5;
    // reset window
    reset();
    ui->graphicsView_before->setVisible(true);
    ui->graphicsView_1->setVisible(true);
    ui->graphicsView_2->setVisible(true);
    ui->graphicsView_3->setVisible(true);
    ui->graphicsView_u_1->setVisible(true);
    ui->graphicsView_hist->setVisible(false);
    ui->graphicsView_hist_2->setVisible(false);
    // set before
    QGraphicsScene *graphic = new QGraphicsScene(this);
    if(!existGray)
    {   // create gray image
        image = QImage("../ExampleImage/A_RGB.bmp");
        gray_image = convert2gray(image);
        graphic->addPixmap( QPixmap::fromImage(gray_image));
    }
    else
    {
        graphic->addPixmap( QPixmap::fromImage(gray_image));
    }

    ui->graphicsView_before->setFixedSize(gray_image.size());
    ui->graphicsView_before->setSceneRect(0, 0, gray_image.width(), gray_image.height());
    ui->graphicsView_before->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_before->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_before->setScene(graphic);

    // set label
    ui->label_after_1->setText("Vertical");
    ui->label_after_2->setText("Horizontal");
    ui->label_after_3->setText("Conbined");
    ui->label_u_1->setText("Overlap");
    // ==========================================================
    uchar* grayscale = (unsigned char*) malloc(gray_image.height()*gray_image.width()*sizeof(unsigned char));
    uchar* imagebits_32;
    for(int i = 0; i < gray_image.height(); i++)
    {
        imagebits_32 = gray_image.scanLine(i);
        for(int j = 0; j < gray_image.width(); j++)
        {
            grayscale[i*gray_image.width() + j] = imagebits_32[ j * 4 + 0];
        }
    }
    sobelOperation(grayscale, gray_image.width(), gray_image.height());

    pre_operation.push(5);
}

void MainWindow::sobelOperation (unsigned char* origin, int width, int height)
{
    threshold = ui->threshold_sobel->value();

    uchar* H_masked = (uchar*) malloc(width*height*sizeof(uchar));
    uchar* V_masked = (uchar*) malloc(width*height*sizeof(uchar));
    uchar* combine_masked = (uchar*) malloc(width*height*sizeof(uchar));

    //calculate approximations of the derivatives – horizontal changes
    const int H_mask[3][3] = {
      {-1, 0, 1},
      {-2, 0, 2},
      {-1, 0, 1}
    };
    //calculate approximations of the derivatives – vertical changes
    const int V_mask[3][3] = {
        {-1,-2,-1},
        {0 , 0, 0},
        {1 , 2, 1}
    };

    // convolution on sobel operation
    for (int i=1;i<height-1;i++)
    {
        for (int j=1;j<width-1;j++)
        {
            double tempH = 0, tempV = 0, total = 0;
            for(int k = -1; k < 2; k++)
                for(int u = -1; u < 2; u++)
                {
                     tempH += (int)origin[(i+k)*width + (j+u)]*H_mask[1+k][1+u];
                     tempV += (int)origin[(i+k)*width + (j+u)]*V_mask[1+k][1+u];
                }

            total = sqrt(tempH*tempH + tempV*tempV);

            combine_masked[i*width+j] = (total >= threshold)?255:0;
            H_masked[i*width + j] = (tempH >= threshold)?255:0;
            V_masked[i*width + j] = (tempV >= threshold)?255:0;
        }
    }
    showGrayImage(ui->graphicsView_3, combine_masked, width, height);
    showGrayImage(ui->graphicsView_1, H_masked, width, height);
    showGrayImage(ui->graphicsView_2, V_masked, width, height);

    // set r channel image
    QImage img = QImage(width, height, QImage::Format_ARGB32);

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            uchar pixel_value = origin[i*width+j];
            if(combine_masked[i*width+j] == 255)
                img.setPixel(j, i, QColor(0,255,0,0).rgb());
            else
                img.setPixel(j, i, QColor(pixel_value,pixel_value,pixel_value,0).rgb());
        }
    }

    QGraphicsScene *graphic = new QGraphicsScene(this);
    graphic->addPixmap( QPixmap::fromImage(img));

    ui->graphicsView_u_1->setFixedSize(image.size());
    ui->graphicsView_u_1->setSceneRect(0, 0, width, height);
    ui->graphicsView_u_1->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_u_1->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_u_1->setScene(graphic);

}


void MainWindow::on_pushButton_6_clicked()
{
//    if(now == 6){
//        std::cout<<"checked test 6 "<<threshold<<" "<<ui->threshold_sobel->value()<<std::endl;
//        return;
//    }
    now = 6;
    // reset window
    reset();
    ui->graphicsView_before->setVisible(true);
    ui->graphicsView_1->setVisible(false);
    ui->graphicsView_2->setVisible(false);
    ui->graphicsView_3->setVisible(false);
    ui->graphicsView_u_1->setVisible(true);
    ui->graphicsView_hist->setVisible(false);
    ui->graphicsView_hist_2->setVisible(false);
    // set before
    QGraphicsScene *graphic = new QGraphicsScene(this);
    if(!existGray)
    {   // create gray image
        image = QImage("../ExampleImage/A_RGB.bmp");
        gray_image = convert2gray(image);
        graphic->addPixmap( QPixmap::fromImage(gray_image));
    }
    else
    {
        graphic->addPixmap( QPixmap::fromImage(gray_image));
    }

    ui->graphicsView_before->setFixedSize(gray_image.size());
    ui->graphicsView_before->setSceneRect(0, 0, gray_image.width(), gray_image.height());
    ui->graphicsView_before->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_before->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_before->setScene(graphic);

    // set label
    ui->label_u_1->setText("after");
    // ==========================================================
    uchar* grayscale = (unsigned char*) malloc(gray_image.height()*gray_image.width()*sizeof(unsigned char));
    uchar* imagebits_32;
    for(int i = 0; i < gray_image.height(); i++)
    {
        imagebits_32 = gray_image.scanLine(i);
        for(int j = 0; j < gray_image.width(); j++)
        {
            grayscale[i*gray_image.width() + j] = imagebits_32[ j * 4 + 0];
        }
    }
    unsigned char* processed = NULL;
    processed = scaling(grayscale, processed, gray_image.width(), gray_image.height(), ui->scaleX_value->value(), ui->scaleY_value->value());


    pre_operation.push(6);
//    unsigned char* processed_ro;
//    rotation(processed, processed_ro,  gray_image.width(), gray_image.height(), ui->theta_value->value());
}

uchar* MainWindow::scaling (unsigned char* origin, unsigned char* processed, int width, int height, double scale_width, double scale_height)
{
    int new_width = (int)(width * scale_width + 0.5);
    int new_height = (int)(height * scale_height + 0.5);

    processed = new uchar[new_width*new_height]();

    int nx = 0, ny = 0, x = 0, y = 0;
    // forword mapping
//    for(y = 0; y < height; y++)
//        for(x = 0; x < width; x++)
//        {
//            nx = (int)(x*scale_width+0.5);
//            ny = (int)(y*scale_height+0.5);
//            processed[ny*new_width+nx] = origin[y*width+x];
//            std::cout<<"("<<nx<<","<<ny<<")"<<std::endl;
//        }
    // inverse mapping
    for(ny = 0; ny < new_height; ny++)
        for(nx = 0; nx < new_width; nx++)
        {
            y = (int)(ny/scale_height + 0.5);
            x = (int)(nx/scale_width + 0.5);
            processed[ny * new_width + nx] = origin[y * width + x];
        }
     std::cout<<new_width<<" "<<new_height<<std::endl;
//    showGrayImage(ui->graphicsView_u_1, processed, new_width, new_height);
     unsigned char* processed_ro = NULL;
     rotation(processed, processed_ro, new_width, new_height, ui->theta_value->value());
     return processed;

}

void MainWindow::rotation(unsigned char* origin, unsigned char* processed, int width, int height, double theta)
{
    int nx = 0, ny = 0, x = 0, y = 0, shiftX = 0, shiftY = 0;
    int new_width = 0, new_height = 0;
    rotation_size(width, height, theta, new_width, new_height);
    std::cout<<new_width<<" "<<new_height<<std::endl;
    int centerX = (width - 1) / 2;
    int centerY = (height - 1) / 2;
    int new_centerX = (new_width - 1) / 2;
    int new_centerY = (new_height - 1) / 2;


    processed = new uchar[new_width*new_width]();
    for(y = 0; y < new_height; y++)
        for(x = 0; x < new_width; x++)
        {
            processed[y * new_width + x] = 0;
        }

    double sinValue = std::sin(theta*PI/180), cosValue = std::cos(theta*PI/180);

    // distance between new graph center and old graph center
    shiftX = (new_centerX * cosValue + new_centerY * sinValue) - centerX;
    shiftY = (-new_centerX * sinValue + new_centerY * cosValue) - centerY;

    for(ny = 0; ny < new_height; ny++)
        for(nx = 0; nx < new_width; nx++)
        {

            x = (int)(nx * cosValue + ny * sinValue + 0.5 - shiftX);
            y = (int)(-nx * sinValue + ny * cosValue + 0.5 - shiftY);

            if(y >= 0 && y < height && x >= 0 && x < width){
                processed[ny * new_width + nx] = origin[y * width + x];
            }
            else
                processed[ny * new_width + nx] = 0;
        }
    showGrayImage(ui->graphicsView_u_1, processed, new_width, new_height);
}

void MainWindow::rotation_size(int width, int height, double theta,
                               int &new_width, int &new_height)
{
    std::vector<int> x_coor;
    std::vector<int> y_coor;
    double sinValue = std::sin(theta*PI/180), cosValue = std::cos(theta*PI/180);

    int tempX, tempY;
    // left top
    tempX = (int)(0*cosValue + -0*sinValue);
    tempY = (int)(0*sinValue + 0*cosValue);
    x_coor.push_back(tempX);
    y_coor.push_back(tempY);
    // right top
    tempX = (int)(width*cosValue + -0*sinValue);
    tempY = (int)(width*sinValue + 0*cosValue);
    x_coor.push_back(tempX);
    y_coor.push_back(tempY);
    // left bottom
    tempX = (int)(0*cosValue + -height*sinValue);
    tempY = (int)(0*sinValue + height*cosValue);
    x_coor.push_back(tempX);
    y_coor.push_back(tempY);
    // right bottom
    tempX = (int)(width*cosValue + -height*sinValue);
    tempY = (int)(width*sinValue + height*cosValue);
    x_coor.push_back(tempX);
    y_coor.push_back(tempY);

    std::sort(x_coor.begin(), x_coor.end());
    std::sort(y_coor.begin(), y_coor.end());

    new_width = x_coor[x_coor.size() - 1] - x_coor[0];
    new_height = y_coor[y_coor.size() - 1] - y_coor[0];
}

void MainWindow::on_chooseFile_clicked()
{
    reset();
    now = 0;
    QString file_name = QFileDialog::getOpenFileName(this, "Open file", "../ExampleImage/");
    std::cout<<file_name.toStdString()<<std::endl;

    image = QImage(file_name);
    QGraphicsScene *graphic = new QGraphicsScene(this);
    graphic->addPixmap( QPixmap::fromImage(image));
    ui->graphicsView_before->setVisible(true);

    ui->graphicsView_before->setFixedSize(image.size());
    ui->graphicsView_before->setSceneRect(0, 0, image.width(), image.height());
    ui->graphicsView_before->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_before->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView_before->setScene(graphic);

    int format = 4;
    uchar *imagebits_32;
    uchar* RChannel = (unsigned char*) malloc(image.height()*image.width()*sizeof(unsigned char));
    uchar* GChannel = (unsigned char*) malloc(image.height()*image.width()*sizeof(unsigned char));
    uchar* BChannel = (unsigned char*) malloc(image.height()*image.width()*sizeof(unsigned char));
    uchar* pixelVal = (unsigned char*) malloc(image.height()*image.width()*sizeof(unsigned char));

    // extract RGB
    for(int i = 0; i < image.height(); i++)
    {
        imagebits_32 = image.scanLine(i);
        for(int j = 0; j < image.width(); j++)
        {
            BChannel[i*image.width() + j] = imagebits_32[ j * format + 0];
            GChannel[i*image.width() + j] = imagebits_32[ j * format + 1];
            RChannel[i*image.width() + j] = imagebits_32[ j * format + 2];
            pixelVal[i*image.width() + j] = (double)RChannel[i*image.width() + j]*0.299 + (double)GChannel[i*image.width() + j]*0.587 + (double)BChannel[i*image.width() + j]*0.114;
        }
    }
    existGray = true;
    existColor = true;
    // set gray image
    showGrayImage(ui->graphicsView_u_1, pixelVal, image.width(), image.height(), true);

    while(!pre_image.empty())
        pre_image.pop();
    while(!pre_operation.empty())
        pre_operation.pop();

}

void MainWindow::on_undo_clicked()
{
    // pop current state
    pre_operation.pop();
    int pre = pre_operation.top();
    pre_operation.pop();
    // pop current state
    pre_image.pop();
    gray_image = pre_image.top();
    pre_image.pop();

    std::cout<<pre<<"\n";
    switch(pre){
        case 1:
            on_pushButton_1_clicked();
            break;

        case 2:
            on_pushButton_2_clicked();
            break;

        case 3:
            on_pushButton_3_clicked();
            break;

        case 4:
            on_pushButton_4_clicked();
            break;

        case 5:
            on_pushButton_5_clicked();
            break;

        case 6:
            on_pushButton_6_clicked();
            break;
        default:
            std::cout<<"something wrong!!!\n";
    }

}
