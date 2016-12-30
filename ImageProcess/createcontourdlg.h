#ifndef CREATECONTOURDLG_H
#define CREATECONTOURDLG_H

#include <QDialog>
#include "cprogressdlg.h"
namespace Ui {
class CreateContourDlg;
}

class CreateContourDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CreateContourDlg(QWidget *parent = 0);
    ~CreateContourDlg();

private:
    Ui::CreateContourDlg *ui;
private:
    //生成等高线
    int DEM2Contour(const char* pszSrcDEM, const char* pszDstShp, int iBandIndex = 1, double dInterval = 10.0,
            const char * pszFormat = "ESRI Shapefile", CProgressBase* pProcess = NULL);
    //生成等高线
    int DEM2Contour2(const char* pszSrcDEM, const char* pszDstShp, int iBandIndex = 1, double dInterval = 10.0,
            const char * pszFormat = "ESRI Shapefile", CProgressBase* pProcess = NULL);
private slots:
    void slotsInput();
    void slotsOutput();
    void slotsApply();

};

#endif // CREATECONTOURDLG_H
