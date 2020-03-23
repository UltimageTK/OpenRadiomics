/******************************************************** 
* @file    : AboutDlg.h
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-7-9
*********************************************************/
#pragma once

#include <QDialog>
#include "ui_AboutDlg.h"

class AboutDlg : public QDialog
{
    Q_OBJECT

public:
    AboutDlg(QWidget *parent = Q_NULLPTR);
    ~AboutDlg();
	//±‰∏¸”Ô—‘
	void changeLanguage();
private:
    Ui::AboutDlg ui;
};
