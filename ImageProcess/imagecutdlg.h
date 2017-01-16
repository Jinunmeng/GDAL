#ifndef IMAGECUTDLG_H
#define IMAGECUTDLG_H

#include <QDialog>
#include "cprogressdlg.h"
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
private slots:
    void slotsInput();
    void slotsInputRoi();
    void slotsOutput();
    void slotsImageCutRasterIO();
};

#endif // IMAGECUTDLG_H
