#ifndef SIEVEFILTERDLG_H
#define SIEVEFILTERDLG_H

#include <QDialog>
#include "cprogressdlg.h"



namespace Ui {
class SieveFilterDlg;
}

class SieveFilterDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SieveFilterDlg(QWidget *parent = 0);
    ~SieveFilterDlg();

private:
    Ui::SieveFilterDlg *ui;

private:
    //小斑除去
    int SieveFilter(const char* pszSrcFile, const char* pszDstFile, int nSizeThreshold, int nConnectedness,
                    const char* pszFormat, CProgressBase* pProcess = NULL);

private slots:
    void slotsInput();
    void slotsOutput();
    void slotsApply();

};

#endif // SIEVEFILTERDLG_H
