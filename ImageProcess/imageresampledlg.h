#ifndef IMAGERESAMPLEDLG_H
#define IMAGERESAMPLEDLG_H

#include <QDialog>
#include "cprogressdlg.h"
#include "gdalwarper.h"
namespace Ui {
class ImageResampleDlg;
}

class ImageResampleDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ImageResampleDlg(QWidget *parent = 0);
    ~ImageResampleDlg();

private:
    Ui::ImageResampleDlg *ui;

    void ImageResampleRasterIO(const char* pszSrcFile, const char* pszDstFile,
                               double dResX, double dResY,
                               const char* pszFormat, CProgressBase* pProcess = NULL);
    void ImageResampleGDALWarp(const char* pszSrcFile, const char* pszDstFile,double dResX, double dResY,
                               GDALResampleAlg eResampleMethod = GRA_NearestNeighbour,
                               const char* pszFormat = "GTiff",CProgressBase* pProcess = NULL);
private slots:
    void slotsInput();
    void slotsOutput();
    void slotsRasterIO();
    void slotsGDALWarp();
};

#endif // IMAGERESAMPLEDLG_H
