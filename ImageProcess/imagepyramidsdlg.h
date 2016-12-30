#ifndef IMAGEPYRAMIDSDLG_H
#define IMAGEPYRAMIDSDLG_H

#include <QDialog>
#include "cprogressdlg.h"
namespace Ui {
class ImagePyramidsDlg;
}

class ImagePyramidsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ImagePyramidsDlg(QWidget *parent = 0);
    ~ImagePyramidsDlg();

private:
    Ui::ImagePyramidsDlg *ui;


private:
    //栅格化函数
    int CreatePyramids(const char* pszFileName,CProgressBase *pProcess=NULL);


private slots:
    void slotsInput();
    void slotsCreatePyramids();
};

#endif // IMAGEPYRAMIDSDLG_H
