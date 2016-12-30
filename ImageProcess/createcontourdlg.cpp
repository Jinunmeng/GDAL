#include "createcontourdlg.h"
#include "ui_createcontourdlg.h"
#include "QMessageBox"
#include "QFileDialog"
#include "cprogressdlg.h"

CreateContourDlg::CreateContourDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateContourDlg)
{
    ui->setupUi(this);

    QObject::connect(ui->pushButtonInput, &QPushButton::clicked, this, &CreateContourDlg::slotsInput);
    QObject::connect(ui->pushButtonOutput, &QPushButton::clicked, this, &CreateContourDlg::slotsOutput);
    QObject::connect(ui->pushButtonApply, &QPushButton::clicked, this, &CreateContourDlg::slotsApply);

}

CreateContourDlg::~CreateContourDlg()
{
    delete ui;
}


void CreateContourDlg::slotsInput()
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
void CreateContourDlg::slotsOutput()
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
     ui->lineEditOutput->setText(strOutputVector);
}
void CreateContourDlg::slotsApply()
{
    QString pszSrcFile;
    pszSrcFile = ui->lineEditInput->text();
    QString pszDstFile;
    pszDstFile = ui->lineEditOutput->text();

    ///矢量化实例
    CProgressDlg* pProcess = new CProgressDlg();
    pProcess->setWindowTitle(tr("Polygonize"));
    pProcess->show();

    DEM2Contour2(pszSrcFile.toStdString().c_str(),pszDstFile.toStdString().c_str(),
                1, 100.0, "ESRI Shapefile", pProcess);


    delete pProcess;

}

//生成等高线
///C
int CreateContourDlg::DEM2Contour(const char* pszSrcDEM, const char* pszDstShp, int iBandIndex, double dInterval,
       const char * pszFormat, CProgressBase* pProcess)
{
    if(pProcess != NULL)
    {
        pProcess->ReSetProcess();
        pProcess->SetProgressTip("开始DEM生成等高线...");
    }

    GDALAllRegister();
    OGRRegisterAll();

    GDALDatasetH hSrcDS = GDALOpen( pszSrcDEM, GA_ReadOnly );
    GDALRasterBandH hBand = GDALGetRasterBand( hSrcDS, iBandIndex );

    // 获取原始DEM数据中的NODATA值
    int bNoDataSet = FALSE, bIgnoreNoData = FALSE;
    double dfNoData = 0.0;
    if( !bNoDataSet && !bIgnoreNoData )
        dfNoData = GDALGetRasterNoDataValue( hBand, &bNoDataSet );

    // 从输入的DEM数据中获取空间参考信息
    OGRSpatialReferenceH hSRS = NULL;
    const char *pszWKT = GDALGetProjectionRef( hSrcDS );

    if( pszWKT != NULL && strlen(pszWKT) != 0 )
        hSRS = OSRNewSpatialReference( pszWKT );

    // 创建输出等高线矢量文件
    OGRSFDriverH hDriver = OGRGetDriverByName( pszFormat );
    OGRDataSourceH hDS = OGR_Dr_CreateDataSource( hDriver, pszDstShp, NULL );
    OGRLayerH hLayer = OGR_DS_CreateLayer( hDS, "Contour", hSRS, wkbLineString, NULL );

    // 创建保存等高线ID的属性字段
    OGRFieldDefnH hFld;
    hFld = OGR_Fld_Create( "ID", OFTInteger );
    OGR_Fld_SetWidth( hFld, 8 );
    OGR_L_CreateField( hLayer, hFld, FALSE );
    OGR_Fld_Destroy( hFld );

    // 创建保存等高线高程值的属性字段
    hFld = OGR_Fld_Create( "Elevation", OFTReal );
    OGR_Fld_SetWidth( hFld, 12 );
    OGR_Fld_SetPrecision( hFld, 3 );
    OGR_L_CreateField( hLayer, hFld, FALSE );
    OGR_Fld_Destroy( hFld );
    int nElevField = 1;

    // 调用GDAL库中的函数生成等高线
    MyGDALProgressFunc pfnProgress = ALGTermProgress;
    CPLErr eErr = GDALContourGenerate( hBand, dInterval, 0.0,
                                       0, NULL,
                                       bNoDataSet, dfNoData,
                                       hLayer, 0, nElevField,
                                       pfnProgress, pProcess );
    OGR_DS_Destroy( hDS );
    GDALClose( hSrcDS );

    GDALDestroyDriverManager();
    OGRCleanupAll();

    return 1;

}


//生成等高线
///C++
int CreateContourDlg::DEM2Contour2(const char* pszSrcDEM, const char* pszDstShp, int iBandIndex, double dInterval,
       const char * pszFormat, CProgressBase* pProcess)
{
    if(pProcess != NULL)
    {
        pProcess->ReSetProcess();
        pProcess->SetProgressTip("CreateContour...");
    }

    GDALAllRegister();
    OGRRegisterAll();

    GDALDataset* poDataset;
    poDataset = (GDALDataset*) GDALOpen(pszSrcDEM, GA_ReadOnly);
    GDALRasterBand* poBand;
    poBand = poDataset->GetRasterBand(iBandIndex);
    // 获取原始DEM数据中的NODATA值
    int bNoDataSet = FALSE, bIgnoreNoData = FALSE;
    double dfNoData = 0.0;
    if( !bNoDataSet && !bIgnoreNoData )
        dfNoData = poBand->GetNoDataValue(&bNoDataSet);

    OGRSpatialReference oSRS;
    const char* pszWKT;
    pszWKT = poDataset->GetProjectionRef();

    char* psz;
    psz  = 	const_cast<char*>(pszWKT);
    if( pszWKT != NULL && strlen(pszWKT) != 0 )
       oSRS.importFromWkt(&psz);

    //OGRSpatialReference oSRS(pszWKT);


    GDALDriver* poDriverDem;
    poDriverDem = GetGDALDriverManager()->GetDriverByName(pszFormat);
    GDALDataset* poDS;
    poDS = poDriverDem->Create(pszDstShp, 0, 0, 0, GDT_Unknown, 0);
    OGRLayer* poLayer;
    poLayer = poDS->CreateLayer("Contour", &oSRS, wkbLineString ,NULL);

    OGRFieldDefn oFieldID( "ID", OFTInteger);
    oFieldID.SetWidth(8);
    poLayer->CreateField(&oFieldID, false);
    OGRFieldDefn oFieldEle( "Elevation", OFTReal);
    oFieldEle.SetWidth(12);
    oFieldEle.SetPrecision(3);
    poLayer->CreateField(&oFieldEle, false);

    int nElevField = 1;


    // 调用GDAL库中的函数生成等高线
    MyGDALProgressFunc pfnProgress = ALGTermProgress;
    CPLErr eErr = GDALContourGenerate( (GDALRasterBandH)poBand, dInterval, 0.0,
                                       0, NULL,
                                       bNoDataSet, dfNoData,
                                       poLayer, 0, nElevField,
                                       pfnProgress, pProcess );

    oSRS.Release();
    GDALClose(poDataset);
    return 1;
}
