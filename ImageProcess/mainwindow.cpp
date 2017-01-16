#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include "QFileDialog"

#include "QDialog"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->actionExit, &QAction::triggered, this, &MainWindow::slotsExit);
    QObject::connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::slotsOpen);
    QObject::connect(ui->actionSave, &QAction::triggered, this, &MainWindow::slotsSave);
    QObject::connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::slotsSaveAs);
    QObject::connect(ui->actionPoygonize, &QAction::triggered, this, &MainWindow::slotsImagePygonize);
    QObject::connect(ui->actionCreatePyramids, &QAction::triggered, this, &MainWindow::slotsCreatePyramids);
    QObject::connect(ui->actionSieveFilter, &QAction::triggered, this, &MainWindow::slotsSieveFilter);
    QObject::connect(ui->actionContour, &QAction::triggered, this, &MainWindow::slotsContour);
    QObject::connect(ui->actionImageAnticolor, &QAction::triggered, this, &MainWindow::slotsImageAnticolor);
    QObject::connect(ui->actionImageResample, &QAction::triggered, this, &MainWindow::slotsImageResample);
    QObject::connect(ui->actionImageCut, &QAction::triggered, this, &MainWindow::slotsImageCut);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotsOpen()
{
    QString strInputImage = QFileDialog::getOpenFileName(this,
                                                        tr("打开原始图像"),
                                                        ".",
                                                        "Image files (*.bmp *.jpg *.tif *.png );;All files (*.*)");
    if (strInputImage == NULL)
    {
       QMessageBox::information(this, "Tips", "Please Select Image File!");
       return ;
    }
}
void MainWindow::slotsSave()
{

}

void MainWindow::slotsSaveAs()
{

}

void MainWindow::slotsExit()
{
    this->close();
}

void MainWindow::slotsImagePygonize()
{
    polygonizeDlg = new CPolygonizeDlg(this);
    polygonizeDlg->show();
}

void MainWindow::slotsCreatePyramids()
{
    imagegDlg = new ImagePyramidsDlg(this);
    imagegDlg->show();
}
void MainWindow::slotsSieveFilter()
{
    sieveDlg = new SieveFilterDlg(this);
    sieveDlg->show();
}
void MainWindow::slotsContour()
{
    contourDlg = new CreateContourDlg(this);
    contourDlg->show();
}

void MainWindow::slotsImageAnticolor()
{
    imageAnticolorDlg = new ImageAnticolorDlg(this);
    imageAnticolorDlg->show();
}

void MainWindow::slotsImageResample()
{
    imageResampleDlg = new ImageResampleDlg(this);
    imageResampleDlg->show();
}
void MainWindow::slotsImageCut()
{
    imageCutDlg = new ImageCutDlg(this);
    imageCutDlg->show();
}
