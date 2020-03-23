/******************************************************** 
* @file    : SettingUI.cpp
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-7-9
*********************************************************/
#include <QMessageBox>
#include "SettingUI.h"
#include "SettingColorItem.h"
#include "ImageDataManager.h"

/******************************************************** 
*  @function : SettingUI
*  @brief    : 构造函数
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
SettingUI::SettingUI(QDialog *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
	QIcon icon(":/UltimageTK/Resources/icon.ico");
	this->setWindowIcon(icon);
}

SettingUI::~SettingUI()
{
}

/******************************************************** 
*  @function : Init
*  @brief    : 初始化
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool SettingUI::Init(std::map<std::string, int> mapLabelProperty)
{
    bool bRet = connect(ui.pBtnAddColorLabel, &QPushButton::clicked, this, &SettingUI::OnAddColorLabel);
    bRet = connect(ui.pBtnDelColorLabel, &QPushButton::clicked, this, &SettingUI::OnDelColorLabel);
    
    reload(mapLabelProperty);
	return true;
}

/******************************************************** 
*  @function : AddItem
*  @brief    : 增加一个标签
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void SettingUI::AddItem(QColor color, QString strLabel)
{
    SettingColorItem *pSettingColorItem = new SettingColorItem(color, strLabel);
    //将widget作为列表的item
    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(QSize(item->sizeHint().width(), 30));
    ui.listWidget->addItem(item);
    pSettingColorItem->setSizeIncrement(item->sizeHint().width(), 30);
    ui.listWidget->setItemWidget(item, pSettingColorItem);
}

/******************************************************** 
*  @function : reload
*  @brief    : 重新载入标签颜色表
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void SettingUI::reload(std::map<std::string, int> mapLabelProperty)
{

    while (ui.listWidget->count()>0)
    {
        QListWidgetItem *pListWidgetItem = ui.listWidget->item(0);
        SettingColorItem *pSettingColorItem = (SettingColorItem*)ui.listWidget->itemWidget(pListWidgetItem);
        if (pSettingColorItem!=nullptr)
        {
            delete pSettingColorItem;
            pSettingColorItem = nullptr;
        }
        if (pListWidgetItem != nullptr)
        {
            delete pListWidgetItem;
            pListWidgetItem = nullptr;
        }
    }
    for each (auto var in mapLabelProperty)
    {
        QColor color(var.second);
        QString strLabel(QString::fromLocal8Bit(var.first.c_str()));
        AddItem(color, strLabel);
    }
}

/******************************************************** 
*  @function : changeLanguage
*  @brief    : 变更语言
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void SettingUI::changeLanguage()
{
	ui.retranslateUi(this);
}

/******************************************************** 
*  @function : closeEvent
*  @brief    : 
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void SettingUI::closeEvent(QCloseEvent *event)
{
    int i = 0;
    std::map<std::string, int> mapLabelProperty;
    while (i<ui.listWidget->count())
    {
        QListWidgetItem *pListWidgetItem = ui.listWidget->item(i);
        SettingColorItem *pSettingColorItem = (SettingColorItem*)ui.listWidget->itemWidget(pListWidgetItem);

        QColor clr;
        QString str;
        pSettingColorItem->GetColorAndLabel(clr, str);
        if (str.isEmpty()||str.simplified().isEmpty() || mapLabelProperty.find(str.toLocal8Bit().data()) != mapLabelProperty.end())
        {
            QMessageBox::information(this, "Warning", QString::fromLocal8Bit("有标签值为空，或重复！\nLabel Empty Or Repeat!"));
            event->ignore();
            return;
        }
        mapLabelProperty[str.toLocal8Bit().data()] = clr.rgb();
        i++;
    }

    ImageDataManager::getInstance()->updateLabelProperty(mapLabelProperty);

    emit sigColorMapChanged();
    return;
}

/******************************************************** 
*  @function : OnAddColorLabel
*  @brief    : 增加标签
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void SettingUI::OnAddColorLabel()
{
    QString strLabel = "Label";
    int i = 0;
    int nSuffix = 0;
    QString strLabelName = strLabel + QString::number(nSuffix);
    while (i < ui.listWidget->count())
    {
        QListWidgetItem *pListWidgetItem = ui.listWidget->item(i);
        SettingColorItem *pSettingColorItem = (SettingColorItem*)ui.listWidget->itemWidget(pListWidgetItem);

        QColor clr;
        QString str;
        pSettingColorItem->GetColorAndLabel(clr, str);
        if (str== strLabelName)
        {
            i = 0;
            ++nSuffix;
            strLabelName = strLabel + QString::number(nSuffix);
            continue;
        }
        i++;
    }
    AddItem(QColor(), strLabelName);
}

/******************************************************** 
*  @function : OnDelColorLabel
*  @brief    : 删除标签
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void SettingUI::OnDelColorLabel()
{
    int nIndex = ui.listWidget->currentRow();
    if (ui.listWidget->count()<nIndex)
    {
        return;
    }
    QListWidgetItem *pListWidgetItem = ui.listWidget->item(nIndex);
    SettingColorItem *pSettingColorItem = (SettingColorItem*)ui.listWidget->itemWidget(pListWidgetItem);
    if (pSettingColorItem != nullptr)
    {
        delete pSettingColorItem;
        pSettingColorItem = nullptr;
    }
    if (pListWidgetItem != nullptr)
    {
        delete pListWidgetItem;
        pListWidgetItem = nullptr;
    }

}
