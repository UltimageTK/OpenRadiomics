#pragma once

#include <QtWidgets/QMainWindow>
#include <DataLoad.h>
#include <QTranslator>
#include "ui_UltimageTK.h"
#include "SettingUI.h"
#include "SaveThread.h"

#include "FormatConvert.h"
#include "HistogramMatching.h"
#include "Registration.h"

#include "AboutDlg.h"
class UltimageTK : public QMainWindow
{
    Q_OBJECT

public:
    UltimageTK(QWidget *parent = Q_NULLPTR);

    bool init();
    bool connectSignals();
    bool loadLabel2Combobox(const std::map<std::string, int> &mapLabelProperty);

    void reloadColorMap();
public slots:

    void OnUpadteCurPosAndValue(float nX, float nY, float nZ, int nValue);
    //void OnUpadteCurZoom(int nX, int nY, int nZ, float fZoom);
    void OnUpadteCurZoom(float fZoom);

    void OnLoadFinish(bool bLoadSuccess);

    void OnFitZoom();
    void OnSetPenType();
    void OnSaveResult();
    void OnDelTarget();
    void OnClearTargets();
    void OnPlaneAlone();
    void OnCapture();

    void OnSetting();
    void OnCurColorChanged(QString str);

    void OnAlphaValueChanged(int nNewValue);

    void OnWindowLevelChanged();

    void OnUpdate3DView();

    //�ļ��˵�
    void OnLoad();
    void OnLoadDir();
    
    //Ԥ����˵�
    void OnSaveAs();    //������Ϊ
    void OnFormatConvert();  //ת������
    void OnHistogramMatching(); //ֱ��ͼƥ��
    void OnRegistation();   //��׼

    //�����˵�
    void OnAbout();
	void OnLanguageChanged();

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    Ui::UltimageTKClass ui;
    DataLoad m_stThreadDataLoad;
    SettingUI m_dlgSettingUI;
    SaveThread m_thdSave;

    FormatConvert m_dlgFormatConvert;
    HistogramMatching m_dlgHistogramMatching;
    Registration m_dlgRegistration;
    AboutDlg m_dlgAbout;

	QTranslator m_translator;
};
