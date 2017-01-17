#ifndef IMAGECUTDLG_H
#define IMAGECUTDLG_H

#include <QDialog>
#include "cprogressdlg.h"
#include "ogr_geometry.h"
namespace Ui {
class ImageCutDlg;
}

class ImageCutDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ImageCutDlg(QWidget *parent = 0);
    ~ImageCutDlg();

private:
    Ui::ImageCutDlg *ui;

private:
    void ImageCutRasterIO(const char* pszSrcFile, const char* pszDstFile,
                          int iStartX, int iStartY, int iSizeX, int iSizeY,
                          const char* pszFormat, CProgressBase* pProgress = NULL);

    void ImageCutWarp(const char* pszSrcFile, const char* pszDstFile,const char* pszAOIWKT,
                      const char* pszFormat, CProgressBase* pProgress = NULL);

private slots:
    void slotsInput();
    void slotsInputRoi();
    void slotsOutput();
    void slotsImageCutRasterIO();
    void slotsImageCutWarp();
};

#endif // IMAGECUTDLG_H
