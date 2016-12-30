#include "cpolygonizedlg.h"
#include "ui_cpolygonizedlg.h"
#include "QMessageBox"
#include "QFileDialog"
#include "cprogressdlg.h"
CPolygonizeDlg::CPolygonizeDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CPolygonizeDlg)
{
    ui->setupUi(this);
    setModal(true);


    QObject::connect(ui->pushButtonInput, &QPushButton::clicked, this, &CPolygonizeDlg::slotsInput);
    QObject::connect(ui->pushButtonVector, &QPushButton::clicked, this, &CPolygonizeDlg::slotsOutput);
    QObject::connect(ui->pushButtonApply, &QPushButton::clicked, this, &CPolygonizeDlg::slotsImagePolygonize);


}

CPolygonizeDlg::~CPolygonizeDlg()
{
    delete ui;
}

void CPolygonizeDlg::slotsInput()
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
     ui->lineEditInputImage->setText(strInputImage);
}

void CPolygonizeDlg::slotsOutput()
{
    QString strOutputVector = QFileDialog::getSaveFileName(this,
                                                           tr("Save File"),
                                                           ".",
                                                           tr("ESRI Shapfile (*.shp)"));
    if (strOutputVector == NULL)
    {
       QMessageBox::information(this, "Tips", "Please Select Save File!");
       return ;
    }
     ui->lineEditVector->setText(strOutputVector);
}

void CPolygonizeDlg::slotsImagePolygonize()
{
    QString pszSrcFile;
    pszSrcFile = ui->lineEditInputImage->text();
    QString pszDstFile;
    pszDstFile = ui->lineEditVector->text();

    ///矢量化实例
    CProgressDlg* pProcess = new CProgressDlg();
    pProcess->setWindowTitle(tr("Polygonize"));
    pProcess->show();
    int re = ImagePolygonize(pszSrcFile, pszDstFile, 1, true, "ESRI Shapefile", pProcess);
    delete pProcess;


}

///栅格矢量化
int CPolygonizeDlg::ImagePolygonize(QString pszSrcFile, QString pszDstFile,
                    int iBandIndex , bool bIs8Con ,
                    const char* pszFormat, CProgressBase* pProgress)
{

        if (pProgress != NULL)
        {
            pProgress->SetProgressTip("Running...");
        }


       GDALAllRegister();
       OGRRegisterAll();
       GDALDataset* poSrcDS = (GDALDataset*) GDALOpen(pszSrcFile.toStdString().c_str(), GA_ReadOnly);    //打开栅格图像
       if (poSrcDS == NULL)
       {
            if(pProgress != NULL)
               pProgress->SetProgressTip("Can not open the file!");
            return 0;
       }

       //构造矢量文件驱动并创建矢量图层
       GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
       if (poDriver == NULL)
       {
           if (pProgress == NULL)
               pProgress->SetProgressTip("Cannot create this type File, please check GDAL cupports it.");
            GDALClose((GDALDatasetH)poSrcDS);

            return 0;
        }

        GDALDataset* poDS = poDriver->Create(pszDstFile.toStdString().c_str(), 0, 0, 0, GDT_Unknown, NULL);
        if (poDS == NULL)
        {
            if (pProgress != NULL)
                pProgress->SetProgressTip("Create vec File falied!");
            GDALClose((GDALDatasetH)poSrcDS);

            return 0;
        }

        OGRSpatialReference SpatialRef(poSrcDS->GetProjectionRef());

        //构造输出图层名称
        char* pszFBasename = CPLStrdup(CPLGetBasename(pszDstFile.toStdString().c_str()));
        string strLayerName = pszFBasename;
        CPLFree( pszFBasename);
        OGRLayer* poLayer = poDS->CreateLayer(strLayerName.c_str(), &SpatialRef, wkbPolygon, NULL);
        if (poLayer == NULL)
        {
            if (pProgress != NULL)
                pProgress->SetProgressTip("Create vec layer falied!");
            GDALClose((GDALDatasetH)poSrcDS);
            return 0;
        }
        //构造一个属性表，用于保存图像像元值
        OGRFieldDefn oFieldValue("DN", OFTInteger);    //创建属性表，只有一个字段即“DN”，里面保存对应的栅格的像元值
         if (poLayer->CreateField(&oFieldValue) != OGRERR_NONE)
        {
             if (pProgress != NULL)
                 pProgress->SetProgressTip("Create vec File falied!");
            GDALClose((GDALDatasetH)poSrcDS);

            return 0;
        }

         GDALRasterBandH hSrcBand = (GDALRasterBandH)poSrcDS->GetRasterBand(iBandIndex);
         //设置连通方式
         char** papszOptions = NULL;
         if (bIs8Con)
             papszOptions = CSLSetNameValue(papszOptions, "8CONNECTED", "8");
         else
             papszOptions = CSLSetNameValue(papszOptions, "8CONNECTED", "4");

         //进行矢量化处理，如果是浮点数类型，使用函数GDALFPolygonize
         MyGDALProgressFunc pfnProgress = ALGTermProgress;
         if (GDALPolygonize(hSrcBand, hSrcBand, (OGRLayerH)poLayer, 0, papszOptions,
                            pfnProgress, pProgress ) != CE_None)
        {
             if ( pProgress != NULL)
             {
                 CPLFree(papszOptions);
                 if (!pProgress->m_bIsContinue)
                 {
                     pProgress->SetProgressTip("Compute is cancle!");
                     return 0;
                 }
                 else
                 {
                     pProgress->SetProgressTip("Compute is failed!");
                     return 0;
                 }
             }
            GDALClose((GDALDatasetH)poSrcDS);
            return 0;
        }

        GDALClose((GDALDatasetH)poSrcDS);    //关闭文件
         CPLFree(papszOptions);


    //  OGRSpatialReference::DestroySpatialReference(poSpatialRef);
    /*  if(poSpatialRef != NULL)
        {
            delete poSpatialRef;
            poSpatialRef = NULL;
        }
    */
        return 1;
}
