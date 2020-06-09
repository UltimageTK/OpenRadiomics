/******************************************************** 
* @file    : main.cpp
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-4-24
*********************************************************/
#include "UltimageTK.h"
#include <QtWidgets/QApplication>
#include <QDir>
#include <QStyleFactory>
#include <QMessageBox>
#include <QPixmap>

#define WINDOW_COLOR QColor(37, 37, 38)
#define WINDOW_TEXT_COLOR QColor(241, 241, 241)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //加载plugins中的dll
    QString strLibPath = a.applicationDirPath();
    strLibPath += QDir::separator() + QString("plugins");  //plugins就是插件目录
    a.addLibraryPath(strLibPath);

    QPalette palette;
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    palette.setBrush(QPalette::Window, QBrush(WINDOW_COLOR));//vs灰
    palette.setBrush(QPalette::Text, QBrush(WINDOW_TEXT_COLOR));//vs文字色
    palette.setBrush(QPalette::ToolTipText, QBrush(WINDOW_TEXT_COLOR));//vs文字色
    palette.setBrush(QPalette::HighlightedText, QBrush(WINDOW_TEXT_COLOR));//vs文字色
    palette.setBrush(QPalette::WindowText, QBrush(WINDOW_TEXT_COLOR));//vs文字色
    palette.setBrush(QPalette::ButtonText, QBrush(WINDOW_TEXT_COLOR));//vs文字色
    palette.setBrush(QPalette::Button, QBrush(WINDOW_COLOR));//vs灰
    palette.setBrush(QPalette::Base, QBrush(QColor(80, 80, 80)));
    palette.setBrush(QPalette::AlternateBase, QBrush(QColor(40, 40, 40)));
    palette.setBrush(QPalette::Disabled, QPalette::Button, QBrush(QColor(80, 40, 40)));
    palette.setBrush(QPalette::Active, QPalette::Button, QBrush(QColor(40, 40, 40)));
    QApplication::setPalette(palette);

	QIcon icon(":/UltimageTK/Resources/icon.ico");
    UltimageTK w;
    w.setWindowIcon(icon);
    w.showMaximized();
	
    if (!w.init())
    {
        return -1;
    }

	QString strTitle = "UltimageTK v1.1.1";

	w.setWindowTitle(strTitle);
    w.show();
    return a.exec();
}
