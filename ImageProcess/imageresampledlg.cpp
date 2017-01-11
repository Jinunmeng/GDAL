#include "imageresampledlg.h"
#include "ui_imageresampledlg.h"
#include "QMessageBox"
#include "QFileDialog"

ImageResampleDlg::ImageResampleDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageResampleDlg)
{
    ui->setupUi(this);

    QObject::connect(ui->pushButtonInput, &QPushButton::clicked, this, &ImageResampleDlg::slotsInput);
    QObject::connect(ui->pushButtonOutput, &QPushButton::clicked, this, &ImageResampleDlg::slotsOutput);


}

ImageResampleDlg::~ImageResampleDlg()
{
    delete ui;
}


void ImageResampleDlg::slotsInput()
{
    QString qInputFilePath = QFileDialog::getOpenFileName(this,
                                                     tr("打开图像"),
                                                     ".",
                                                     "Image files(*.tif)");
    if(qInputFilePath == NULL)
    {
        QMessageBox::information(this,"Tips","Can not Open the File");
        return;
    }
    ui->lineEditInput->setText(qInputFilePath);
}

void ImageResampleDlg::slotsOutput()
{
    QString qOutputFilePath = QFileDialog::getSaveFileName(this,
                                                           tr("保存图像"),
                                                           ".",
                                                           "Image file(*.tif)");
    if(qOutputFilePath == NULL)
    {
        QMessageBox::information(this, "Tips", "Select the Save File");
        return;
    }
    ui->lineEditOuput->setText(qOutputFilePath);
}
void ImageResampleDlg::slotsRasterIO()
{
    QString pszSrcFile;
    pszSrcFile = ui->lineEditInput->text();
    QString pszDstFile;
    pszDstFile = ui->lineEditOuput->text();

    ///矢量化实例
    CProgressDlg* pProcess = new CProgressDlg();
    pProcess->setWindowTitle(tr("Polygonize"));
    pProcess->show();

    ImageResampleRasterIO(pszSrcFile.toStdString().c_str(), pszDstFile.toStdString().c_str(),
                          0.5, 0.5, "GTiff", pProcess);

    delete pProcess;

}

void ImageResampleDlg::slotsGDALWarp()
{

}


void ImageResampleDlg::ImageResampleRasterIO(const char *pszSrcFile, const char *pszDstFile,
                                             double dResX, double dResY,
                                             const char* pszFormat, CProgressBase* pProcess)
{
    if(pProcess != NULL)
    {
        pProcess->ReSetProcess();
        pProcess->SetProgressTip("Running Resample...");
    }
    GDALAllRegister();
    GDALDataset* poSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
    GDALDataType eGT = poSrcDS->GetRasterBand(1)->GetRasterDataType();

    int iBandCount = poSrcDS->GetRasterCount();
    int iSrcWidth = poSrcDS->GetRasterXSize();
    int iSrcHeight = poSrcDS->GetRasterYSize();

    //根据采样比例计算采样后图像的大小
    int iDstWidth = static_cast<int>(iSrcWidth * dResX + 0.5);
    int iDstHeight = static_cast<int>(iSrcHeight * dResY + 0.5);

    double adfGeoTransform[6] = {0};
    poSrcDS->GetGeoTransform(adfGeoTransform);
    //计算采样后图像的分辨率
    adfGeoTransform[1] = adfGeoTransform[1] / dResX;
    adfGeoTransform[5] = adfGeoTransform[5] / dResY;

    //创建输出文件并设置空间参考和坐标信息
    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
    GDALDataset* poDstDS = poDriver->Create(pszDstFile, iDstWidth, iDstHeight, iBandCount, eGT, NULL);
    poDstDS->SetGeoTransform(adfGeoTransform);
    poDstDS->SetProjection(poSrcDS->GetProjectionRef());

    int* pBandMap = new int[iBandCount];
    for(int i =0 ; i < iBandCount; i++)
        pBandMap[i] = i+1;
    //根据数据的类型，申请不同的数据缓存并进行处理
    if( eGT == GDT_Byte)
    {
        unsigned char* pDataBuf = new unsigned char[iDstWidth * iDstHeight * iBandCount];
        poSrcDS->RasterIO(GF_Read,0, 0, iSrcWidth, iSrcHeight,pDataBuf,iDstWidth, iDstHeight,
                          eGT, iBandCount, pBandMap, 0, 0, 0);
        poDstDS->RasterIO(GF_Write,0, 0, iDstWidth, iSrcHeight, pDataBuf, iDstWidth, iDstHeight,
                          eGT,iBandCount, pBandMap,0,0,0);
        delete pDataBuf;
    }
    else
    {

    }

    GDALClose((GDALDatasetH)poSrcDS);
    GDALClose((GDALDatasetH)poDstDS);
    delete pBandMap;









}

