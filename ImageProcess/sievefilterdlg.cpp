#include "sievefilterdlg.h"
#include "ui_sievefilterdlg.h"
#include "QMessageBox"
#include "QFileDialog"
#include "cprogressdlg.h"
#include "ogrsf_frmts.h"

SieveFilterDlg::SieveFilterDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SieveFilterDlg)
{
    ui->setupUi(this);

    QObject::connect(ui->pushButtonInput, &QPushButton::clicked, this, &SieveFilterDlg::slotsInput);
    QObject::connect(ui->pushButtonOutput, &QPushButton::clicked, this, &SieveFilterDlg::slotsOutput);
    QObject::connect(ui->pushButtonApply, &QPushButton::clicked, this, &SieveFilterDlg::slotsApply);

}

SieveFilterDlg::~SieveFilterDlg()
{
    delete ui;
}

void SieveFilterDlg::slotsInput()
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
     ui->lineEditInput->setText(strInputImage);
}
void SieveFilterDlg::slotsOutput()
{
    QString strOutputFile = QFileDialog::getSaveFileName(this,
                                                           tr("保存原始图像"),
                                                           ".",
                                                           "Image files (*.bmp *.jpg *.tif *.png );;ESRI Shapefile (*.shp);;All files (*.*)");
    if (strOutputFile == NULL)
    {
       QMessageBox::information(this, "Tips", "Please Select Save File!");
       return ;
    }
     ui->lineEditOutput->setText(strOutputFile);
}
void SieveFilterDlg::slotsApply()
{
    QString pszSrcFile;
    pszSrcFile = ui->lineEditInput->text();
    QString pszDstFile;
    pszDstFile = ui->lineEditOutput->text();

    ///矢量化实例
    CProgressDlg* pProcess = new CProgressDlg();
    pProcess->setWindowTitle(tr("SieveFilter"));
    pProcess->show();

    SieveFilter(pszSrcFile.toStdString().c_str(),pszDstFile.toStdString().c_str(),
                300, 8, "GTiff", pProcess);

    delete pProcess;
}


//小图斑除去
int SieveFilterDlg::SieveFilter(const char *pszSrcFile, const char *pszDstFile,
                                int nSizeThreshold, int nConnectedness, const char *pszFormat, CProgressBase *pProcess)
{
    if(pProcess != NULL)
        {
            pProcess->ReSetProcess();
            pProcess->SetProgressTip("Running...");
        }

        if(pszSrcFile == NULL || pszDstFile == NULL)
            return 0;

        GDALAllRegister();

        GDALDataset *pSrcDS = (GDALDataset *) GDALOpen( pszSrcFile, GA_ReadOnly );
        if( pSrcDS == NULL )
        {
            if(pProcess != NULL)
                pProcess->SetProgressTip("Can not Open the File!");

            return 0;
        }

        GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
        if( poDriver == NULL )
        {
            if(pProcess != NULL)
                pProcess->SetProgressTip("Can not Create the Type File!");
            GDALClose( (GDALDatasetH) pSrcDS );
            return 0;
        }

        // 获取输入图像信息
        int iBandCount = pSrcDS->GetRasterCount();
        int iWidth  = pSrcDS->GetRasterXSize();
        int iHeight = pSrcDS->GetRasterYSize();
        GDALDataType eDT = pSrcDS->GetRasterBand(1)->GetRasterDataType();

        // 创建输出图像
        GDALDataset *pDstDS = poDriver->Create(pszDstFile, iWidth, iHeight, iBandCount, eDT, NULL);

        double dGeoTrans[6] = {0};
        pSrcDS->GetGeoTransform(dGeoTrans);
        pDstDS->SetGeoTransform(dGeoTrans);
        pDstDS->SetProjection(pSrcDS->GetProjectionRef());


        for (int i=0; i<iBandCount; i++)
        {
            GDALRasterBandH hSrcBand = (GDALRasterBandH)pSrcDS->GetRasterBand(i+1);
            GDALRasterBandH hDstBand = (GDALRasterBandH)pDstDS->GetRasterBand(i+1);

            GDALSetRasterColorTable(hDstBand, GDALGetRasterColorTable(hSrcBand));	//设置颜色表

            // 调用GDAL算法进行处理
            MyGDALProgressFunc pfnProgress = ALGTermProgress;
            if (GDALSieveFilter(hSrcBand, NULL, hDstBand, nSizeThreshold, nConnectedness, NULL,
                pfnProgress, pProcess)!= CE_None)
            {
                if(pProcess != NULL)
                    pProcess->SetProgressTip("Running Falied!");

                if( pDstDS != NULL )
                    GDALClose( (GDALDatasetH) pDstDS );
                GDALClose( (GDALDatasetH) pSrcDS );
                return 0;
            }
        }

        GDALClose( (GDALDatasetH) pDstDS );
        GDALClose( (GDALDatasetH) pSrcDS );

        if(pProcess != NULL)
            pProcess->SetProgressTip("Running Succeed!");
        return 1;
}


