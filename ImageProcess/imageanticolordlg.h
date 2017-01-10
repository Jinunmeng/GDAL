#ifndef IMAGEANTICOLORDLG_H
#define IMAGEANTICOLORDLG_H

#include <QDialog>
#include "cprogressdlg.h"
namespace Ui {
class ImageAnticolorDlg;
}

class ImageAnticolorDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ImageAnticolorDlg(QWidget *parent = 0);
    ~ImageAnticolorDlg();

private:
    Ui::ImageAnticolorDlg *ui;

private:
    int ImageAnticolor(const char* pszSrcFile, const char* pszDstFile, const char* pszFormat,
                        CProgressBase* pProcess = NULL);

private slots:
    void slotsInput();
    void slotsOutput();
    void slotsApply();



};

#endif // IMAGEANTICOLORDLG_H
