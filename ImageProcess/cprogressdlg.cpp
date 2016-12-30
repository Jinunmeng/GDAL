#include "cprogressdlg.h"

CProgressDlg::CProgressDlg(QWidget *parent)
:QProgressDialog(parent)
{
    m_dPosition = 0.0;
    m_iStepCount = 100;
    m_iCurStep = 0;

    setModal(true);
    setLabelText(tr("Running..."));
    setAutoClose(false);
    setAutoReset(false);

    //禁用关闭按钮
    setWindowFlags(Qt::WindowTitleHint | Qt::WindowStaysOnTopHint);

}

/**
* @brief 析构函数
*/
CProgressDlg::~CProgressDlg()
{
};
/**
* @brief 设置进度信息
* @param pszMsg         进度信息
*/
void CProgressDlg::SetProgressTip(const char* pszMsg)
{
    if (pszMsg != NULL)
    {
        m_strMessage = pszMsg;
        setLabelText(QString(pszMsg));
    }
}

/**
* @brief 设置进度值
* @param dPosition      进度值
*/
bool CProgressDlg::SetPosition(double dPosition)
{
    m_dPosition = dPosition;

    setValue( std::min( 100u, ( uint )( m_dPosition*100.0 ) ) );

    QCoreApplication::instance()->processEvents();

    if(this->wasCanceled())
        return false;

    return true;
}
/**
* @brief 进度条前进一步,返回false表示终止操作
*/
bool CProgressDlg::StepIt()
{
    m_iCurStep ++;
    m_dPosition = m_iCurStep*1.0 / m_iStepCount;

    setValue( std::min( 100u, ( uint )( m_dPosition*100.0 ) ) );

    QCoreApplication::instance()->processEvents();

    if(this->wasCanceled())
        return false;

    return true;
}

void CProgressDlg::updateProgress(int step)
{
    this->setValue(step);
    QCoreApplication::instance()->processEvents();
}
//该函数用于将GDAL算法中的进度信息导出到CProcessBase基类中，供给界面显示
int  ALGTermProgress( double dfComplete, const char *pszMessage, void * pProgressArg )
{
    if(pProgressArg != NULL)
    {
        CProgressBase * pProcess = (CProgressBase*) pProgressArg;
        pProcess->m_bIsContinue = pProcess->SetPosition(dfComplete);

        if(pProcess->m_bIsContinue)
            return TRUE;
        else
            return FALSE;
    }
    else
        return TRUE;
}
