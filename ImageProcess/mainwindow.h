#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cpolygonizedlg.h"
#include "imagepyramidsdlg.h"
#include "sievefilterdlg.h"
#include "createcontourdlg.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    CPolygonizeDlg* polygonizeDlg;
    ImagePyramidsDlg* imagegDlg;
    SieveFilterDlg* sieveDlg;
    CreateContourDlg* contourDlg;
public slots:
    void slotsOpen();
    void slotsSave();
    void slotsSaveAs();
    void slotsExit();
    void slotsImagePygonize();
    void slotsCreatePyramids();
    void slotsSieveFilter();
    void slotsContour();
};

#endif // MAINWINDOW_H
