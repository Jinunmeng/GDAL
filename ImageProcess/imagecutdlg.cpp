#include "imagecutdlg.h"
#include "ui_imagecutdlg.h"
#include "QMessageBox"
#include "QFileDialog"
ImageCutDlg::ImageCutDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageCutDlg)
{
    ui->setupUi(this);
    QObject::connect(ui->pushButtonInput, &QPushButton::clicked, this, &ImageCutDlg::slotsInput);
    QObject::connect(ui->pushButtonROI, &QPushButton::clicked, this, &ImageCutDlg::slotsInputRoi);
    QObject::connect(ui->pushButtonOutput, &QPushButton::clicked, this, &ImageCutDlg::slotsOutput);
    QObject::connect(ui->pushButtonRasterIO, &QPushButton::clicked, this, &ImageCutDlg::slotsImageCutRasterIO);



}

ImageCutDlg::~ImageCutDlg()
{
    delete ui;
}


void ImageCutDlg::slotsInput()
{
    QString inputFile = QFileDialog::getOpenFileName(this,
                                                     tr("打开图像"),
                                                     ".",
                                                     "Image Files(*.tif *.jpg)");
    if(inputFile == NULL)
    {
        QMessageBox::information(this,"Tips", "please select a image!");
        return;
    }
    ui->lineEditInput->setText(inputFile);

}
void ImageCutDlg::slotsInputRoi()
{
    QString inputRoiFile = QFileDialog::getOpenFileName(this,
                                                        tr("打开Roi文件"),
                                                        ".",
                                                        "Roi Files(*.roi);;Shpfiles(*.shp);;All files(*.*)");
    if(inputRoiFile == NULL)
    {
        QMessageBox::information(this, "Tips", "Please Select a file");
        return;
    }
    ui->lineEditROI->setText(inputRoiFile);
}
void ImageCutDlg::slotsOutput()
{
    QString outPutFile = QFileDialog::getSaveFileName(this,
                                                      tr("保存图像"),
                                                      ".",
                                                      "ImageFile(*.tif)");
    if(outPutFile == NULL)
    {
        QMessageBox::information(this, "Tips", "Please Save File");
    }
    ui->lineEditOutput->setText(outPutFile);
}


void ImageCutDlg::slotsImageCutRasterIO()
{
    QString pszSrcFile;
    pszSrcFile = ui->lineEditInput->text();
    QString pszDstFile;
    pszDstFile = ui->lineEditOutput->text();

    ///矢量化实例
    CProgressDlg* pProcess = new CProgressDlg();
    pProcess->setWindowTitle(tr("Polygonize"));
    pProcess->show();

    ImageCutRasterIO(pszSrcFile.toStdString().c_str(), pszDstFile.toStdString().c_str(),
                     800, 800, 1000, 1000, "GTiff", pProcess);

    delete pProcess;
}


void ImageCutDlg::ImageCutRasterIO(const char *pszSrcFile, const char *pszDstFile,
                                   int iStartX, int iStartY, int iSizeX, int iSizeY,
                                   const char *pszFormat, CProgressBase *pProgress)
{
    if(pProgress != NULL)
    {
        pProgress->ReSetProcess();
        pProgress->SetProgressTip("Cut Running...");
    }
    GDALAllRegister();
    GDALDataset* pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
    GDALDataType eDT = pSrcDS->GetRasterBand(1)->GetRasterDataType();


    int iBandCount = pSrcDS->GetRasterCount();
    int iDstWidth = iSizeX;
    int iDstHeight = iSizeY;
    double adfGeoTransform[6] = {0};
    pSrcDS->GetGeoTransform(adfGeoTransform);

    adfGeoTransform[0] = adfGeoTransform[0] + iStartX * adfGeoTransform[1] +
            iStartY * adfGeoTransform[2];
    adfGeoTransform[3] = adfGeoTransform[3] + iStartX * adfGeoTransform[4] +
            iStartY * adfGeoTransform[5];

    //创建输出文件并设置空间参考及坐标信息

    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
    GDALDataset* pDstDS = poDriver->Create(pszDstFile, iDstWidth, iDstHeight,iBandCount, eDT, NULL);
    pDstDS->SetGeoTransform(adfGeoTransform);
    pDstDS->SetProjection(pSrcDS->GetProjectionRef());

    int* pBandMap = new int[iBandCount];

    for(int i  = 0; i < iBandCount; i++)
    {
        pBandMap[i] = i + 1;
    }

    if(eDT == GDT_Byte)
    {
       unsigned char* pDataBuff = new unsigned char[iDstHeight * iDstWidth * iBandCount];

       pSrcDS->RasterIO(GF_Read, iStartX, iStartY, iSizeX, iSizeY, pDataBuff, iSizeX, iSizeY, eDT,
                        iBandCount, pBandMap, 0, 0, 0);

       pDstDS->RasterIO(GF_Write, 0, 0, iSizeX, iSizeY, pDataBuff, iSizeX, iSizeY, eDT,
                        iBandCount, pBandMap, 0, 0, 0);
       delete pDataBuff;
    }
    else
    {

    }

    delete pBandMap;
    GDALClose((GDALDatasetH)pSrcDS);
    GDALClose((GDALDatasetH)pDstDS);

    if(pProgress != NULL)
    {
        pProgress->SetProgressTip("Finished!");
    }




}
