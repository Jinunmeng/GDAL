#ifndef CPROGRESSDLG_H
#define CPROGRESSDLG_H

#include <QWidget>
#include "QProgressDialog"
#include "progressbase.h"
#include "QCoreApplication"

/**
*全局函数，在类之外声明
* \brief 调用GDAL进度条接口
*
* 该函数用于将GDAL算法中的进度信息导出到CProcessBase基类中，供给界面显示
*
* @param dfComplete 完成进度值，其取值为 0.0 到 1.0 之间
* @param pszMessage 进度信息
* @param pProgressArg   CProcessBase的指针
*
* @return 返回TRUE表示继续计算，否则为取消
*/
int  ALGTermProgress( double dfComplete, const char *pszMessage, void * pProgressArg );
typedef int ( *MyGDALProgressFunc)(double dfComplete, const char *pszMessage, void *pProgressArg);


class CProgressDlg :
        public QProgressDialog,
        public CProgressBase
{
    Q_OBJECT

public:
    /**
        * @brief 构造函数
        */
    CProgressDlg(QWidget *parent = 0);

    /**
        * @brief 析构函数
        */
    ~CProgressDlg();

    /**
        * @brief 设置进度信息
        * @param pszMsg			进度信息
        */
    void SetProgressTip(const char* pszMsg);

    /**
        * @brief 设置进度值
        * @param dPosition		进度值
        */
    bool SetPosition(double dPosition);

    /**
        * @brief 进度条前进一步
        */
    bool StepIt();

public slots:
    void updateProgress(int);
};

#endif // CPROGRESSDLG_H
