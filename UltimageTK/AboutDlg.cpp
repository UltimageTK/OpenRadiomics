/******************************************************** 
* @file    : AboutDlg.cpp
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-7-9
*********************************************************/
#include "AboutDlg.h"

AboutDlg::AboutDlg(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
}

AboutDlg::~AboutDlg()
{
}

/********************************************************
*  @function : changeLanguage
*  @brief    : ±‰∏¸”Ô—‘
*  @input    :
*  @output   :
*  @return   :
*********************************************************/
void AboutDlg::changeLanguage()
{
	ui.retranslateUi(this);
}