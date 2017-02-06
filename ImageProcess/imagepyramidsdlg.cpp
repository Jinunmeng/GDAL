#include "imagepyramidsdlg.h"
#include "ui_imagepyramidsdlg.h"
#include "cprogressdlg.h"
#include "QMessageBox"
#include "QFileDialog"
#include "imagepyramidsdlg.h"
enum
{
    RE_SUCCESS = 0,
    RE_FAILD = 1,
    RE_NOFILE = 2,
    RE_PARAMERRORS = 3,
    RE_FILEENOTEXIST = 4,
    RE_FILENOTSUPPORT = 5,
    RE_CREATEFALIED = 6,
    RE_USERCANCLE = 7

};


ImagePyramidsDlg::ImagePyramidsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImagePyramidsDlg)
{
    ui->setupUi(this);

    QObject::connect(ui->pushButtonInput, &QPushButton::clicked, this, &ImagePyramidsDlg::slotsInput);
    QObject::connect(ui->pushButtonPyramids, &QPushButton::clicked, this, &ImagePyramidsDlg::slotsCreatePyramids);

}

ImagePyramidsDlg::~ImagePyramidsDlg()
{
    delete ui;
}

void ImagePyramidsDlg::slotsInput()
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

void ImagePyramidsDlg::slotsCreatePyramids()
{
    QString pszSrcFile;
    pszSrcFile = ui->lineEditInput->text();
    ///创建金字塔实例
    CProgressDlg *pPro = new CProgressDlg();
    pPro->setWindowTitle("CreatePyramids");
    pPro->show();
    int irev=CreatePyramids(pszSrcFile.toStdString().c_str(),pPro);
    delete pPro;
}


///创建金字塔
int ImagePyramidsDlg::CreatePyramids(const char* pszFileName,CProgressBase *pProgress)
{
    if (pProgress != NULL)
    {
        //pProgress->SetProgressCaption("Create Pyramid");
        pProgress->SetProgressTip("Createing Pyramid...");
    }

    GDALAllRegister();
    CPLSetConfigOption("USE_RRD","YES");    //创建Erdas格式的字塔文件

    /* -------------------------------------------------------------------- */
    /*      Open data file.                                                 */
    /* -------------------------------------------------------------------- */

    GDALDatasetH     hDataset;
    hDataset = GDALOpen( pszFileName, GA_ReadOnly );

    GDALDriverH hDriver = GDALGetDatasetDriver(hDataset);
    const char* pszDriver = GDALGetDriverShortName(hDriver);
    if (EQUAL(pszDriver, "HFA") || EQUAL(pszDriver, "PCIDSK"))
    {
        GDALClose(hDataset);    //如果文件是Erdas的img或者PCI的pix格式，创建内金字塔，其他的创建外金字塔
        hDataset = GDALOpen( pszFileName, GA_Update );
    }

    if( hDataset == NULL )
    {
        if (pProgress != NULL)
            pProgress->SetProgressTip("打开图像失败，请检查图像是否存在或文件是否是图像文件！");

        return RE_NOFILE;
    }

    /* -------------------------------------------------------------------- */
    /*      Get File basic infomation                                       */
    /* -------------------------------------------------------------------- */
    int iWidth = GDALGetRasterXSize(hDataset);
    int iHeigh = GDALGetRasterYSize(hDataset);

    int iPixelNum = iWidth * iHeigh;    //图像中的总像元个数
    int iTopNum = 4096;                 //顶层金字塔大小，64*64
    int iCurNum = iPixelNum / 4;

    int anLevels[1024] = { 0 };
    int nLevelCount = 0;                //金字塔级数

    do    //计算金字塔级数，从第二级到顶层
    {
        anLevels[nLevelCount] = static_cast<int>(pow(2.0, nLevelCount+2));
        nLevelCount ++;
        iCurNum /= 4;
    } while (iCurNum > iTopNum);

    const char  *pszResampling = "NEAREST"; //采样方式
    //"NEAREST", "GAUSS", "CUBIC", "AVERAGE", "MODE", "AVERAGE_MAGPHASE" or "NONE"

    MyGDALProgressFunc pfnProgress = ALGTermProgress;
    /* -------------------------------------------------------------------- */
    /*      Generate overviews.                                             */
    /* -------------------------------------------------------------------- */
    if (nLevelCount > 0 &&
        GDALBuildOverviews( hDataset,pszResampling, nLevelCount, anLevels,
        0, NULL, pfnProgress, pProgress ) != CE_None )
    {
        if (pProgress != NULL)
            pProgress->SetProgressTip("Create Pyramid Failed!");
        return RE_FAILD;
    }
    /* -------------------------------------------------------------------- */
    /*      Clean up                                                         */
    /* -------------------------------------------------------------------- */
    GDALClose(hDataset);
    GDALDestroyDriverManager();
    if (pProgress != NULL)
        pProgress->SetProgressTip("Create Pyramid Success!");
    return RE_SUCCESS;
}
