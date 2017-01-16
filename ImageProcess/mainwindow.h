#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cpolygonizedlg.h"
#include "imagepyramidsdlg.h"
#include "sievefilterdlg.h"
#include "createcontourdlg.h"
#include "imageanticolordlg.h"
#include "imageresampledlg.h"
#include "imagecutdlg.h"
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
    ImageAnticolorDlg* imageAnticolorDlg;
    ImageResampleDlg* imageResampleDlg;
    ImageCutDlg* imageCutDlg;


private slots:
    void slotsOpen();
    void slotsSave();
    void slotsSaveAs();
    void slotsExit();
    void slotsImagePygonize();
    void slotsCreatePyramids();
    void slotsSieveFilter();
    void slotsContour();
    void slotsImageAnticolor();
    void slotsImageResample();
    void slotsImageCut();
};

#endif // MAINWINDOW_H
