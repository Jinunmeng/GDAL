#ifndef CPOLYGONIZEDLG_H
#define CPOLYGONIZEDLG_H

#include <QDialog>
#include "cprogressdlg.h"
namespace Ui {
class CPolygonizeDlg;
}

class CPolygonizeDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CPolygonizeDlg(QWidget *parent = 0);
    ~CPolygonizeDlg();

private:
    Ui::CPolygonizeDlg *ui;
private:
    //矢量化函数
    int ImagePolygonize(QString pszSrcFile, QString pszDstFile,
                        int iBandIndex, bool bIs8Con,
                        const char* pszFormat, CProgressBase* pPro);


public slots:
    void slotsInput();
    void slotsOutput();
    void slotsImagePolygonize();

};

#endif // CPOLYGONIZEDLG_H
