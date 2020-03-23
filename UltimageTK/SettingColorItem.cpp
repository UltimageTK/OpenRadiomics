/******************************************************** 
* @file    : SettingColorItem.cpp
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-7-9
*********************************************************/
#include <QColorDialog>
#include <QMessageBox>

#include "SettingColorItem.h"

/******************************************************** 
*  @function : SettingColorItem
*  @brief    : 构造函数
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
SettingColorItem::SettingColorItem(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    bool bRet = connect(ui.pBtnColor, &QPushButton::clicked, this, &SettingColorItem::OnSelectColor);
}

/******************************************************** 
*  @function : SettingColorItem
*  @brief    : 构造函数
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
SettingColorItem::SettingColorItem(QColor color,QString strLabel, QWidget *parent /*= Q_NULLPTR*/)
{
    ui.setupUi(this);
    bool bRet = connect(ui.pBtnColor, &QPushButton::clicked, this, &SettingColorItem::OnSelectColor);
    bRet = connect(ui.lineEditLabelName, &QLineEdit::textChanged,this, &SettingColorItem::OnLabelNameChanged);
    QString strStyle = "border: 2px solid black;\
        background-color: #";
    QRgb rgbColor = color.rgb();
    strStyle += QString("%1").arg(rgbColor, 6, 16, QChar('0')) + ";";
    ui.pBtnColor->setStyleSheet(strStyle);
    ui.lineEditLabelName->setText(strLabel);
    m_curColor = color;
    m_strCurLabel = strLabel;
}

SettingColorItem::~SettingColorItem()
{
}


/******************************************************** 
*  @function : GetColorAndLabel
*  @brief    : 获取当前颜色和标签名
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void SettingColorItem::GetColorAndLabel(QColor &color, QString &strLabel)
{
    color = m_curColor;
    strLabel = m_strCurLabel;
}

/******************************************************** 
*  @function : SetWarningState
*  @brief    : 显示警告
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void SettingColorItem::SetWarningState(bool bWarning /*= true*/)
{
    if (bWarning)
    {
        ui.lineEditLabelName->setStyleSheet("border: 2px solid red;");
        ui.lineEditLabelName->setPlaceholderText(QString::fromLocal8Bit("不能为空(No Empty)!"));
    }
    else
    {
        ui.lineEditLabelName->setStyleSheet("");
    }
}

/******************************************************** 
*  @function : OnSelectColor
*  @brief    : 设置颜色
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void SettingColorItem::OnSelectColor()
{
    QColorDialog dlgColor;
    QColor color = dlgColor.getColor();
    QPalette palette;
    QString strStyle = "border: 2px solid black;\
        background-color: #";
    QRgb rgbColor = color.rgb();
    strStyle += QString("%1").arg(rgbColor, 6, 16, QChar('0')) +";";
    ui.pBtnColor->setStyleSheet(strStyle);

    m_curColor = color;
}

/******************************************************** 
*  @function : OnLabelNameChanged
*  @brief    : 更新标签名字
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void SettingColorItem::OnLabelNameChanged(const QString &str)
{
    if (str.isEmpty()|| str.simplified().isEmpty())
    {
        SetWarningState(true);
    }
    else
    {
        SetWarningState(false);
    }
    m_strCurLabel = str;
}
