#ifndef CREATEIMAGETHUMBDLG_H
#define CREATEIMAGETHUMBDLG_H

#include <QDialog>
#include "cprogressdlg.h"
#include "gdal_pam.h"
namespace Ui {
class CreateImageThumbDlg;
}

class CreateImageThumbDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CreateImageThumbDlg(QWidget *parent = 0);
    ~CreateImageThumbDlg();

private:
    Ui::CreateImageThumbDlg *ui;

private:
    //生成单波段灰度缩略图
    bool CreateThumbnail(const char* input, const char* output, int bandID, float scale,CProgressBase* pProcess = NULL);

    //生成RGB彩色缩略图
    bool CreateThumbnail(const char* input, const char* output, int redBand, int greenBand, int blueBand, float scale,
                         bool IsENVIStretch, bool IsENVIOptimizedLinear,CProgressBase* pProcess = NULL);


private slots:
    void slotsInput();
    void slotsOutputLL();
    void slotsOutputMZ();
    void slotsApply();

};

#endif // CREATEIMAGETHUMBDLG_H
