#include "imagecutdlg.h"
#include "ui_imagecutdlg.h"
#include "QMessageBox"
#include "QFileDialog"
#include "gdalwarper.h"
ImageCutDlg::ImageCutDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageCutDlg)
{
    ui->setupUi(this);
    QObject::connect(ui->pushButtonInput, &QPushButton::clicked, this, &ImageCutDlg::slotsInput);
    QObject::connect(ui->pushButtonROI, &QPushButton::clicked, this, &ImageCutDlg::slotsInputRoi);
    QObject::connect(ui->pushButtonOutput, &QPushButton::clicked, this, &ImageCutDlg::slotsOutput);
    QObject::connect(ui->pushButtonRasterIO, &QPushButton::clicked, this, &ImageCutDlg::slotsImageCutRasterIO);
    QObject::connect(ui->pushButtonGDALWarp, &QPushButton::clicked, this, &ImageCutDlg::slotsImageCutWarp);



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

void ImageCutDlg::slotsImageCutWarp()
{
    QString pszSrcFile;
    pszSrcFile = ui->lineEditInput->text();
    QString pszDstFile;
    pszDstFile = ui->lineEditOutput->text();

    ///矢量化实例
    CProgressDlg* pProcess = new CProgressDlg();
    pProcess->setWindowTitle(tr("ImageCut"));
    pProcess->show();

    const char* pszAOI = "POLYGON((400 500, 1400 200, 2000 1600, 800 1200, 400 500))";
    ImageCutWarp(pszSrcFile.toStdString().c_str(), pszDstFile.toStdString().c_str(), pszAOI, "GTiff", pProcess);

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


void ImageCutDlg::ImageCutWarp(const char *pszSrcFile, const char *pszDstFile, const char *pszAOIWKT,
                               const char *pszFormat, CProgressBase *pProgress)
{
    if(pProgress != NULL)
    {
        pProgress->ReSetProcess();
        pProgress->SetProgressTip("Running...");
    }

    GDALAllRegister();
    GDALDataset* poSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
    GDALDataType eDT = poSrcDS->GetRasterBand(1)->GetRasterDataType();

    int iBandCount = poSrcDS->GetRasterCount();
    int iSrcWidth = poSrcDS->GetRasterXSize();
    int iSrcHeight = poSrcDS->GetRasterYSize();

    double pSrcGeoTransform[6] = {0};
    double pDstGeoTransform[6] = {0};
    poSrcDS->GetGeoTransform(pSrcGeoTransform);
    //内存空间拷贝
    memcpy(pDstGeoTransform, pSrcGeoTransform, sizeof(double) * 6);

    //将wkt转换为ORGGeometry类型
    char* pszWKT = (char*)pszAOIWKT;

    OGRGeometry* pAOIGeometry = OGRGeometryFactory::createGeometry(wkbPolygon);
    pAOIGeometry->importFromWkt(&pszWKT);

    OGREnvelope eRect;
    pAOIGeometry->getEnvelope(&eRect);

    GDALApplyGeoTransform(pSrcGeoTransform, eRect.MinX, eRect.MinY, (&pDstGeoTransform[0]), &(pDstGeoTransform[3]));

    int iDstWidth = static_cast<int>(eRect.MaxX - eRect.MinX);
    int iDstHeight = static_cast<int>(eRect.MaxY - eRect.MinY);

    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
    GDALDataset* poDstDS = poDriver->Create(pszDstFile, iDstWidth, iDstHeight, iBandCount, eDT, NULL);

    poDstDS->SetGeoTransform(pDstGeoTransform);
    poDstDS->SetProjection(poSrcDS->GetProjectionRef());


    void* hTransformArg = GDALCreateGenImgProjTransformer2((GDALDatasetH)poSrcDS,(GDALDatasetH)poDstDS, NULL);
    GDALTransformerFunc pfnTransformer = GDALGenImgProjTransform;

    //构造GDALWarp的变换选项
    GDALWarpOptions* psWO = GDALCreateWarpOptions();
    psWO->papszWarpOptions = CSLDuplicate(NULL);
    psWO->eWorkingDataType = eDT;
    psWO->eResampleAlg = GRA_NearestNeighbour;

    psWO->hSrcDS = (GDALDatasetH)poSrcDS;
    psWO->hDstDS = (GDALDatasetH)poDstDS;

    psWO->pfnTransformer = pfnTransformer;
    psWO->pTransformerArg = hTransformArg;

    psWO->pfnProgress = ALGTermProgress;
    psWO->pProgressArg = pProgress;

    psWO->nBandCount = iBandCount;
    psWO->panSrcBands = (int*)CPLMalloc(psWO->nBandCount * sizeof(int));
    psWO->panDstBands = (int*)CPLMalloc(psWO->nBandCount * sizeof(int));

    for(int i = 0; i< iBandCount; i++)
    {
        psWO->panSrcBands[i] = i + 1;
        psWO->panDstBands[i] = i + 1;
    }
    //设置剪切AOI
    psWO->hCutline = (void*)pAOIGeometry;
    psWO->papszWarpOptions = CSLSetNameValue(psWO->papszWarpOptions, "CUTLINE", pszAOIWKT);

    //创建GDALWarp执行对象，并使用GDALWarpOptions来进行初始化
    GDALWarpOperation oWO;
    oWO.Initialize(psWO);
    //执行处理
    oWO.ChunkAndWarpImage(0, 0, iDstWidth, iDstHeight);

    //释放资源及关闭文件
    GDALDestroyGenImgProjTransformer(psWO->pTransformerArg);
    GDALDestroyWarpOptions(psWO);
    GDALClose((GDALDatasetH)poSrcDS);
    GDALClose((GDALDatasetH)poDstDS);


//    GDALAllRegister();

//    // 打开原始图像并计算图像信息
//    GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
//    GDALDataType eDT = pSrcDS->GetRasterBand(1)->GetRasterDataType();

//    int iBandCount = pSrcDS->GetRasterCount();
//    int iSrcWidth  = pSrcDS->GetRasterXSize();
//    int iSrcHeight = pSrcDS->GetRasterYSize();

//    double pSrcGeoTransform[6] = {0};
//    double pDstGeoTransform[6] = {0};
//    pSrcDS->GetGeoTransform(pSrcGeoTransform);			//图像的仿射变换信息
//    memcpy(pDstGeoTransform, pSrcGeoTransform, sizeof(double)*6);

//    // 将传入的AOI的WKT处理为一个OGRGeometry类型，用于后续处理
//    char *pszWKT = (char*) pszAOIWKT;
//    OGRGeometry* pAOIGeometry = OGRGeometryFactory::createGeometry(wkbPolygon);
//    pAOIGeometry->importFromWkt(&pszWKT);

//    OGREnvelope eRect;
//    pAOIGeometry->getEnvelope(&eRect);

//    // 设置输出图像的左上角坐标
//    GDALApplyGeoTransform(pSrcGeoTransform, eRect.MinX, eRect.MinY, (&pDstGeoTransform[0]), &(pDstGeoTransform[3]));

//    // 根据裁切范围确定裁切后的图像宽高
//    int iDstWidth  = static_cast<int>(eRect.MaxX - eRect.MinX);
//    int iDstHeight = static_cast<int>(eRect.MaxY - eRect.MinY);

//    // 创建输出图像
//    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
//    GDALDataset *pDstDS = poDriver->Create(pszDstFile, iDstWidth, iDstHeight, iBandCount, eDT, NULL);
//    pDstDS->SetGeoTransform(pDstGeoTransform);
//    pDstDS->SetProjection(pSrcDS->GetProjectionRef());

//    // 构造坐标转换关系
//    void *hTransformArg = GDALCreateGenImgProjTransformer2((GDALDatasetH)pSrcDS, (GDALDatasetH)pDstDS, NULL );
//    GDALTransformerFunc pfnTransformer = GDALGenImgProjTransform;

//    // 构造GDALWarp的变换选项
//    GDALWarpOptions *psWO = GDALCreateWarpOptions();

//    psWO->papszWarpOptions = CSLDuplicate(NULL);
//    psWO->eWorkingDataType = eDT;
//    psWO->eResampleAlg = GRA_NearestNeighbour;

//    psWO->hSrcDS = (GDALDatasetH)pSrcDS;
//    psWO->hDstDS = (GDALDatasetH)pDstDS;

//    psWO->pfnTransformer = pfnTransformer;
//    psWO->pTransformerArg = hTransformArg;

//    psWO->pfnProgress = ALGTermProgress;
//    psWO->pProgressArg = pProgress;

//    psWO->nBandCount = iBandCount;
//    psWO->panSrcBands = (int *) CPLMalloc(psWO->nBandCount*sizeof(int));
//    psWO->panDstBands = (int *) CPLMalloc(psWO->nBandCount*sizeof(int));
//    for(int i = 0; i < iBandCount; i++ )
//    {
//        psWO->panSrcBands[i] = i+1;
//        psWO->panDstBands[i] = i+1;
//    }

//    // 设置裁切AOI，AOI中的坐标必须是图像的行列号坐标，否则不能进行裁切
//    psWO->hCutline = (void*)pAOIGeometry;
//    // 设置上面的hCutline的值和使用下面的CUTLINE配置项的效果一样，两个选择一个即可
//    psWO->papszWarpOptions = CSLSetNameValue( psWO->papszWarpOptions, "CUTLINE", pszAOIWKT );

//    // 创建GDALWarp执行对象，并使用GDALWarpOptions来进行初始化
//    GDALWarpOperation oWO;
//    oWO.Initialize( psWO );

//    // 执行处理
//    oWO.ChunkAndWarpImage( 0, 0, iDstWidth, iDstHeight);

//    // 释放资源和关闭文件
//    GDALDestroyGenImgProjTransformer( psWO->pTransformerArg );
//    GDALDestroyWarpOptions( psWO );

//    GDALClose((GDALDatasetH) pSrcDS );
//    GDALClose((GDALDatasetH) pDstDS );
    if(pProgress != NULL)
    {
        pProgress->SetProgressTip("Finished!");
    }

}
