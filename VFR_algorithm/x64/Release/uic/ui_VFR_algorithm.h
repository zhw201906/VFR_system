/********************************************************************************
** Form generated from reading UI file 'VFR_algorithm.ui'
**
** Created by: Qt User Interface Compiler version 5.9.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VFR_ALGORITHM_H
#define UI_VFR_ALGORITHM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VFR_algorithmClass
{
public:
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLabel *label_video;
    QTextBrowser *textBrowser_log;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_selectVideo;
    QPushButton *pushButton_openCamera;
    QPushButton *pushButton_cleanLog;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *checkBox_saveSnap;
    QPushButton *pushButton_querySnap;
    QListWidget *listWidget_snap;

    void setupUi(QWidget *VFR_algorithmClass)
    {
        if (VFR_algorithmClass->objectName().isEmpty())
            VFR_algorithmClass->setObjectName(QStringLiteral("VFR_algorithmClass"));
        VFR_algorithmClass->resize(780, 580);
        VFR_algorithmClass->setMinimumSize(QSize(780, 580));
        VFR_algorithmClass->setMaximumSize(QSize(780, 580));
        gridLayout = new QGridLayout(VFR_algorithmClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_2 = new QLabel(VFR_algorithmClass);
        label_2->setObjectName(QStringLiteral("label_2"));
        QFont font;
        font.setFamily(QStringLiteral("04b_21"));
        font.setPointSize(14);
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_2, 0, 0, 1, 2);

        label_video = new QLabel(VFR_algorithmClass);
        label_video->setObjectName(QStringLiteral("label_video"));
        label_video->setMinimumSize(QSize(500, 375));
        label_video->setFrameShape(QFrame::Box);
        label_video->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_video, 1, 0, 1, 1);

        textBrowser_log = new QTextBrowser(VFR_algorithmClass);
        textBrowser_log->setObjectName(QStringLiteral("textBrowser_log"));

        gridLayout->addWidget(textBrowser_log, 1, 1, 1, 1);

        widget = new QWidget(VFR_algorithmClass);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 0, -1, 0);
        pushButton_selectVideo = new QPushButton(widget);
        pushButton_selectVideo->setObjectName(QStringLiteral("pushButton_selectVideo"));

        horizontalLayout->addWidget(pushButton_selectVideo);

        pushButton_openCamera = new QPushButton(widget);
        pushButton_openCamera->setObjectName(QStringLiteral("pushButton_openCamera"));

        horizontalLayout->addWidget(pushButton_openCamera);

        pushButton_cleanLog = new QPushButton(widget);
        pushButton_cleanLog->setObjectName(QStringLiteral("pushButton_cleanLog"));

        horizontalLayout->addWidget(pushButton_cleanLog);


        gridLayout->addWidget(widget, 2, 0, 1, 1);

        widget_2 = new QWidget(VFR_algorithmClass);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 0, -1, 0);
        checkBox_saveSnap = new QCheckBox(widget_2);
        checkBox_saveSnap->setObjectName(QStringLiteral("checkBox_saveSnap"));

        horizontalLayout_2->addWidget(checkBox_saveSnap);

        pushButton_querySnap = new QPushButton(widget_2);
        pushButton_querySnap->setObjectName(QStringLiteral("pushButton_querySnap"));

        horizontalLayout_2->addWidget(pushButton_querySnap);


        gridLayout->addWidget(widget_2, 2, 1, 1, 1);

        listWidget_snap = new QListWidget(VFR_algorithmClass);
        listWidget_snap->setObjectName(QStringLiteral("listWidget_snap"));

        gridLayout->addWidget(listWidget_snap, 3, 0, 1, 2);

        gridLayout->setRowStretch(0, 1);
        gridLayout->setRowStretch(1, 5);
        gridLayout->setRowStretch(2, 1);
        gridLayout->setRowStretch(3, 4);

        retranslateUi(VFR_algorithmClass);

        QMetaObject::connectSlotsByName(VFR_algorithmClass);
    } // setupUi

    void retranslateUi(QWidget *VFR_algorithmClass)
    {
        VFR_algorithmClass->setWindowTitle(QApplication::translate("VFR_algorithmClass", "\351\252\214\350\257\201Demo", Q_NULLPTR));
        label_2->setText(QApplication::translate("VFR_algorithmClass", "\347\233\221\346\216\247\344\272\272\350\204\270\350\257\206\345\210\253\347\256\227\346\263\225\351\252\214\350\257\201\345\271\263\345\217\260", Q_NULLPTR));
        label_video->setText(QString());
        pushButton_selectVideo->setText(QApplication::translate("VFR_algorithmClass", "\351\200\211\346\213\251\350\247\206\351\242\221", Q_NULLPTR));
        pushButton_openCamera->setText(QApplication::translate("VFR_algorithmClass", "\345\274\200\345\220\257\346\221\204\345\203\217\345\244\264", Q_NULLPTR));
        pushButton_cleanLog->setText(QApplication::translate("VFR_algorithmClass", "\346\270\205\347\251\272\346\227\245\345\277\227", Q_NULLPTR));
        checkBox_saveSnap->setText(QApplication::translate("VFR_algorithmClass", "\344\277\235\345\255\230\346\212\223\346\213\215", Q_NULLPTR));
        pushButton_querySnap->setText(QApplication::translate("VFR_algorithmClass", "\346\237\245\347\234\213\346\212\223\346\213\215", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class VFR_algorithmClass: public Ui_VFR_algorithmClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VFR_ALGORITHM_H
