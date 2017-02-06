                                                 #include "createimagethumbdlg.h"
#include "ui_createimagethumbdlg.h"
#include "QFileDialog"
#include "QMessageBox"
CreateImageThumbDlg::CreateImageThumbDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateImageThumbDlg)
{
    ui->setupUi(this);

    QObject::connect(ui->pushButtonInput, &QPushButton::clicked, this, &CreateImageThumbDlg::slotsInput);
    QObject::connect(ui->pushButtonOutputLL, &QPushButton::clicked, this, &CreateImageThumbDlg::slotsOutputLL);
    QObject::connect(ui->pushButtonOutputMZ, &QPushButton::clicked, this, &CreateImageThumbDlg::slotsOutputMZ);
    QObject::connect(ui->pushButtonApply, &QPushButton::clicked, this, &CreateImageThumbDlg::slotsApply);

}

CreateImageThumbDlg::~CreateImageThumbDlg()
{
    delete ui;
}


void CreateImageThumbDlg::slotsInput()
{
    QString inputFile = QFileDialog::getOpenFileName(this,
                                                     tr("打开文件"),
                                                     ".",
                                                     "Image Files(*.tif)");

    if(inputFile == NULL)
    {
        QMessageBox::information(this, "Tips", "Please Select a file...");
        return;
    }
    ui->lineEditInput->setText(inputFile);


}
void CreateImageThumbDlg::slotsOutputLL()
{
    QString outputFileLL = QFileDialog::getSaveFileName(this,
                                                        tr("保存"),
                                                        ".",
                                                        "Image Files(*.jpg)");

    if(outputFileLL == NULL)
    {
        QMessageBox::information(this,"Tips", "Select a Save Path");
        return;
    }
    ui->lineEditOutputLL->setText(outputFileLL);

}
void CreateImageThumbDlg::slotsOutputMZ()
{
    QString outputFileMZ = QFileDialog::getSaveFileName(this,
                                                        tr("保存"),
                                                        ".",
                                                        "Image Files(*.jpg)");
    if(outputFileMZ == NULL)
    {
        QMessageBox::information(this, "Tips", "Select a Save Path");
        return;
    }
    ui->lineEditOutputMZ->setText(outputFileMZ);
}
void CreateImageThumbDlg::slotsApply()
{
     QString inputFile;
     inputFile = ui->lineEditInput->text();
     QString outputFileLL;
     outputFileLL = ui->lineEditOutputLL->text();

     CProgressDlg* pProcess = new CProgressDlg();
     pProcess->setWindowTitle(tr("Create Thumb"));
     pProcess->show();

     CreateThumbnail(inputFile.toStdString().c_str(), outputFileLL.toStdString().c_str(),1, 0.2f, pProcess);


     delete pProcess;
}

//生成单波段灰度浏览图
bool CreateImageThumbDlg::CreateThumbnail(const char *input, const char *output, int bandID, float scale, CProgressBase* pProcess)
{
    if(pProcess != NULL)
    {
        pProcess->ReSetProcess();
        pProcess->SetProgressTip("Running...");
    }
    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");   //设置支持中文路径
    GDALDataset *pDataset = (GDALDataset *)GDALOpen(input, GA_ReadOnly);

    if (pDataset == NULL)
        return false;
    int nSamples = ((GDALDataset *)pDataset)->GetRasterXSize();
    int nLines = ((GDALDataset *)pDataset)->GetRasterYSize();
    int nBands = ((GDALDataset *)pDataset)->GetRasterCount();
    if (bandID > nBands)
    {
        GDALClose((GDALDatasetH)pDataset);
        return false;
    }

    int i, j;
    float *pBuffer = new float[nSamples];

    //查找该波段的最值
    GDALRasterBand *pBandRead = ((GDALDataset *)pDataset)->GetRasterBand(bandID);
    float minV, maxV;
    for (i = 0; i < nLines; i++)
    {
        if (pBandRead->RasterIO(GF_Read, 0, i, nSamples, 1, pBuffer, nSamples, 1, GDT_Float32, 0, 0) != CE_None)
        {
            //cout << "Fail to read image!" << endl;
            pProcess->SetProgressTip("Fail to read image!");
            delete[] pBuffer;
            GDALClose((GDALDatasetH)pDataset);
            return false;
        }

        for (j = 0; j < nSamples; j++)
        {
            if ((i == 0) && (j == 0)) maxV = minV = pBuffer[0];
            if (pBuffer[j] < minV) minV = pBuffer[j];
            if (pBuffer[j] > maxV) maxV = pBuffer[j];
        }
    }

    //打开写入图像
    GDALDriver * pDriver = (GDALDriver*)GDALGetDriverByName("BMP");
    if (pDriver == NULL)
    {
        //cout << "Fail to create bmp fiel driver!" << endl;
        pProcess->SetProgressTip("Fail to create bmp fiel driver!");
        delete[] pBuffer;
        GDALClose((GDALDatasetH)pDataset);
        return false;
    }

    int stepSize = int(1.0f / scale);
    int outSample = int(nSamples / stepSize);
    int outLine = int(nLines / stepSize);

    char tmpFile[512];
    strcpy(tmpFile, output);
    strcat(tmpFile, ".bmp");
    GDALDataset* pDSOut = pDriver->Create(tmpFile, outSample, outLine, 1, GDT_Byte, NULL);
    if (pDSOut == NULL)
    {
        delete[] pBuffer;
        GDALClose((GDALDatasetH)pDataset);
        return false;
    }

    unsigned char* pBufferOut = new unsigned char[outSample];//输出缓存
    GDALRasterBand *pBandWrite = ((GDALDataset *)pDSOut)->GetRasterBand(1);

    int offset;
    int offsetY = 0;
    //写入图像

    for (i = 0; i < nLines; i += stepSize)
    {
        if (pBandRead->RasterIO(GF_Read, 0, i, nSamples, 1, pBuffer, nSamples, 1, GDT_Float32, 0, 0) != CE_None)
        {
            //cout << "error read image!" << endl;
            pProcess->SetProgressTip("error read image!");
            delete[] pBuffer;
            delete[] pBufferOut;
            GDALClose((GDALDatasetH)pDataset);
            GDALClose((GDALDatasetH)pDSOut);
            return false;
        }

        offset = 0;
        for (j = 0; j < nSamples; j += stepSize)
        {
            if (offset < outSample)
                pBufferOut[offset] = (unsigned char)((pBuffer[j] - minV) * 256 / (maxV - minV + 1));//归一化
            offset++;
        }
        pBandWrite->WriteBlock(0, offsetY, pBufferOut);
        offsetY++;
    }

    delete[] pBuffer;
    delete[] pBufferOut;

    GDALDriver * poDriver = GetGDALDriverManager()->GetDriverByName("JPEG");
    if (poDriver == NULL)
    {
        GDALClose((GDALDatasetH)pDataset);
        GDALClose((GDALDatasetH)pDSOut);
        return false;
    }
    GDALDataset * poDataset3 = poDriver->CreateCopy(output, pDSOut, false, NULL, NULL, NULL);

    //reject to save ".aux.xml" file
    GDALPamDataset *pamDs = dynamic_cast<GDALPamDataset*>(poDataset3);
    if (pamDs)
    {
        int pamFlags = pamDs->GetPamFlags();
        pamFlags |= GPF_NOSAVE;                  // |= 位或标志； 转为二进制，如果有一个为1 就为1；
        pamDs->SetPamFlags(pamFlags);
    }

    if (pDSOut != NULL)
        GDALClose((GDALDatasetH)pDSOut);
    pDriver->Delete(tmpFile);
    if (pDataset != NULL)
        GDALClose((GDALDatasetH)pDataset);
    if (poDataset3 != NULL)
        GDALClose((GDALDatasetH)poDataset3);
    return true;
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------/
* Function Name:bool CreateThumbnail(const char* input,const char* output,int redBand,int greenBand,int blueBand,float scale,bool IsENVIStretch)
* Parameters:
*  const char* input              //输入影像路径
*  const char *output             //输出浏览图路径（JPG格式）
*  int redBand                    //红波段ID，此三个波段没必要非要是RGB，只是看需求，一般而言生成RGB真彩色浏览图
*  int greenBand                  //绿波段ID
*  int blueBand                   //蓝波段ID
*  float scale                    //拉伸尺度，该值表示浏览图是相对与原图的缩小比例
*  bool IsENVIStretch             //是否按ENVI标准拉伸,默认拉伸
*  bool IsENVIOptimizedLinear     //按ENVI最优拉伸算法拉伸
* Returns: true or false
* Function:生成彩色浏览图，指定三个RGB波段即可。(该函数按ENVI截掉累积直方图2%后进行线性拉伸)
* Date:2014.11.11(update)
*-----------------------------------------------------------------------------------------------------------------------------------------------*/
bool CreateImageThumbDlg::CreateThumbnail(const char *input, const char *output, int redBand, int greenBand,
                                          int blueBand, float scale, bool IsENVIStretch, bool IsENVIOptimizedLinear, CProgressBase* pProcess)
{
    if(pProcess != NULL)
    {
        pProcess->ReSetProcess();
        pProcess->SetProgressTip("Running...");
    }

    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    GDALDataset *pDataset = (GDALDataset *)GDALOpen(input, GA_ReadOnly);
    if (pDataset == NULL)
        return false;
    int nSamples = ((GDALDataset *)pDataset)->GetRasterXSize();
    int nLines = ((GDALDataset *)pDataset)->GetRasterYSize();
    int nBands = ((GDALDataset *)pDataset)->GetRasterCount();
    int nThumbBands = nBands >= 3 ? 3 : 1;
    int rgbBand[] = { redBand, greenBand, blueBand };  //要生成浏览图的三个波段顺序
    if (nThumbBands == 1)
    {
        rgbBand[0] = 1;
    }
    for (int i = 0; i < nThumbBands; i++)
    {
        if (rgbBand[i] > nBands)
        {
            printf(" => Input bandID error, bandID > BandNums.\n");
            GDALClose((GDALDatasetH)pDataset);
            return false;
        }
    }
    unsigned short *pBuffer = new unsigned short[nSamples];
    int Low[3] = { 0 }, High[3] = { 0 };
    int minV[3] = { 0 }, maxV[3] = { 0 };
    for (int k = 0; k < nThumbBands; k++)
    {
        int Histo[1024] = { 0 };
        unsigned long long TotalPixel = 0;
        GDALRasterBand *pBandRead = ((GDALDataset *)pDataset)->GetRasterBand(rgbBand[k]);
        for (int i = 0; i < nLines; i++)
        {
            //按行读取，然后统计最大值最小值
            if (pBandRead->RasterIO(GF_Read, 0, i, nSamples, 1, pBuffer, nSamples, 1, GDT_UInt16, 0, 0) != CE_None)
            {
                //cout << " => Fail to read image!" << endl;
                pProcess->SetProgressTip("=> Fail to read image!");
                delete[] pBuffer;
                pBuffer = NULL;
                GDALClose((GDALDatasetH)pDataset);
                return false;
            }
            maxV[k] = minV[k] = pBuffer[0];
            for (int j = 0; j < nSamples; j++)
            {
                int px = pBuffer[j];
                px = px > 1023 ? 1023 : px;
                px = px < 0 ? 0 : px;
                if (px != 0)
                {
                    Histo[px]++;
                    TotalPixel++;
                }
                if (px < minV[k])
                    minV[k] = px;
                if (px > maxV[k])
                    maxV[k] = px;
            }
        }
        //是否使用ENVI的2%的拉伸,默认使用
        if (IsENVIStretch)
        {
            //计算累积直方图
            double *Res = new double[1024];
            Res[0] = 0;
            double Min = 99999, Max = 99999;
            for (int i = 1; i < 1024; i++)
            {
                Histo[i] += Histo[i - 1];
                Res[i] = 1.0*Histo[i] / TotalPixel;
            }
            for (int i = 1; i < 1024; i++)
            {
                if (abs(Res[i] - 0.05) < Min)
                {
                    Min = abs(Res[i] - 0.05);
                    Low[k] = i;
                }
                if (abs(Res[i] - 0.95) < Max)
                {
                    Max = abs(Res[i] - 0.95);
                    High[k] = i;
                }
            }
            delete[] Res; Res = NULL;
            //printf("Band%d,Low = %d,High = %d\n", k + 1, Low[k], High[k]);
        }

        if (IsENVIOptimizedLinear)
        {
            //计算累积直方图
            double *Res = new double[1024];
            Res[0] = 0;
            double Min = 99999, Max = 99999;
            int a = 0, b = 65535, c = 0, d = 65535;
            for (int i = 1; i < 1024; i++)
            {
                Histo[i] += Histo[i - 1];
                Res[i] = 1.0*Histo[i] / TotalPixel;
            }
            for (int i = 1; i < 1024; i++)
            {
                if (abs(Res[i] - 0.025) < Min)
                {
                    Min = abs(Res[i] - 0.025);
                    a = i;
                }
                if (abs(Res[i] - 0.99) < Max)
                {
                    Max = abs(Res[i] - 0.99);
                    b = i;
                }
            }
            delete[] Res; Res = NULL;

            c = a - 0.1*(b - a);
            d = b + 0.5*(b - a);

            Low[k] = c;
            High[k] = d;

            //printf("Band%d,Low = %d,High = %d\n", k + 1, Low[k], High[k]);
        }
    }

    //打开写入图像
    GDALDriver * pDriver = (GDALDriver*)GDALGetDriverByName("BMP");
    if (pDriver == NULL)
    {
        //cout << "Fail to create bmp filed driver!" << endl;
        pProcess->SetProgressTip("Fail to create bmp filed driver!");
        delete[] pBuffer; pBuffer = NULL;
        GDALClose((GDALDatasetH)pDataset);
        return false;
    }

    int stepSize = int(1.0f / scale);
    int outSample = int(nSamples / stepSize) + ((nSamples % stepSize) == 0 ? 0 : 1);
    int outLine = int(nLines / stepSize) + ((nLines % stepSize) == 0 ? 0 : 1);

    char tmpFile[512];
    strcpy(tmpFile, output);
    strcat(tmpFile, ".bmp");
    GDALDataset* pDSOut = pDriver->Create(tmpFile, outSample, outLine, nThumbBands, GDT_Byte, NULL);
    if (pDSOut == NULL)
    {
        delete[] pBuffer; pBuffer = NULL;
        GDALClose((GDALDatasetH)pDataset);
        return false;
    }

    //写入图像
    unsigned char* pBufferOut = new unsigned char[outSample];//输出缓存
    for (int k = 0; k < nThumbBands; k++)
    {
        GDALRasterBand *pBandWrite = ((GDALDataset *)pDSOut)->GetRasterBand(k + 1);
        GDALRasterBand *pBandRead = ((GDALDataset *)pDataset)->GetRasterBand(rgbBand[k]);
        int offset;
        int offsetY = 0;

        for (int i = 0; i < nLines; i += stepSize)
        {
            if (pBandRead->RasterIO(GF_Read, 0, i, nSamples, 1, pBuffer, nSamples, 1, GDT_UInt16, 0, 0) != CE_None)
            {
                //cout << "error read image!" << endl;
                pProcess->SetProgressTip("error read image!");
                delete[] pBuffer; pBuffer = NULL;
                delete[] pBufferOut; pBufferOut = NULL;
                GDALClose((GDALDatasetH)pDataset);
                GDALClose((GDALDatasetH)pDSOut);
                return false;
            }

            offset = 0;
            for (int j = 0; j < nSamples; j += stepSize)
            {
                if (IsENVIStretch || IsENVIOptimizedLinear)
                {
                    if (pBuffer[j] >= Low[k] && pBuffer[j] <= High[k])
                    {
                        pBufferOut[offset] = (unsigned char)((pBuffer[j] - Low[k])*255.0 / (High[k] - Low[k] + 1));
                    }
                    else if (pBuffer[j] < Low[k])
                    {
                        pBufferOut[offset] = 16; // 16
                    }
                    else if (pBuffer[j] > High[k])
                    {
                        pBufferOut[offset] = 242; //242
                    }
                }
                else
                {
                    if (pBuffer[j] < minV[k])
                    {
                        pBufferOut[offset] = 16;
                    }
                    else if (pBuffer[j] > maxV[k])
                    {
                        pBufferOut[offset] = 242;
                    }
                    else
                        pBufferOut[offset] = (unsigned char)((pBuffer[j] - minV[k]) * 256.0 / (maxV[k] - minV[k] + 1));
                }
                offset++;
            }
            pBandWrite->WriteBlock(0, offsetY, pBufferOut);
            offsetY++;
        }
    }

    delete[] pBuffer; pBuffer = NULL;
    delete[] pBufferOut; pBufferOut = NULL;

    GDALDriver * poDriver = GetGDALDriverManager()->GetDriverByName("JPEG");
    if (poDriver == NULL)
    {
        GDALClose((GDALDatasetH)pDataset);
        GDALClose((GDALDatasetH)pDSOut);
        return false;
    }
    GDALDataset * poDataset3 = poDriver->CreateCopy(output, pDSOut, false, NULL, NULL, NULL);

    //reject to save ".aux.xml" file
    GDALPamDataset *pamDs = dynamic_cast<GDALPamDataset*>(poDataset3);
    if (pamDs)
    {
        int pamFlags = pamDs->GetPamFlags();
        pamFlags |= GPF_NOSAVE;
        pamDs->SetPamFlags(pamFlags);
    }

    if (pDSOut != NULL)
        GDALClose((GDALDatasetH)pDSOut);
    pDriver->Delete(tmpFile);
    if (pDataset != NULL)
        GDALClose((GDALDatasetH)pDataset);
    if (poDataset3 != NULL)
        GDALClose((GDALDatasetH)poDataset3);
    return true;
}

