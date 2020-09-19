/********************************************************************************
** Form generated from reading UI file 'VFR_algorithm.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VFR_ALGORITHM_H
#define UI_VFR_ALGORITHM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
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
    QCheckBox *checkBox_rect;
    QCheckBox *checkBox_snap;
    QCheckBox *checkBox_saveSnap;
    QListWidget *listWidget_snap;

    void setupUi(QWidget *VFR_algorithmClass)
    {
        if (VFR_algorithmClass->objectName().isEmpty())
            VFR_algorithmClass->setObjectName(QString::fromUtf8("VFR_algorithmClass"));
        VFR_algorithmClass->resize(780, 580);
        VFR_algorithmClass->setMinimumSize(QSize(780, 580));
        VFR_algorithmClass->setMaximumSize(QSize(780, 580));
        gridLayout = new QGridLayout(VFR_algorithmClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_2 = new QLabel(VFR_algorithmClass);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QFont font;
        font.setFamily(QString::fromUtf8("04b_21"));
        font.setPointSize(14);
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_2, 0, 0, 1, 2);

        label_video = new QLabel(VFR_algorithmClass);
        label_video->setObjectName(QString::fromUtf8("label_video"));
        label_video->setMinimumSize(QSize(500, 375));
        label_video->setFrameShape(QFrame::Box);
        label_video->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_video, 1, 0, 1, 1);

        textBrowser_log = new QTextBrowser(VFR_algorithmClass);
        textBrowser_log->setObjectName(QString::fromUtf8("textBrowser_log"));

        gridLayout->addWidget(textBrowser_log, 1, 1, 1, 1);

        widget = new QWidget(VFR_algorithmClass);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 0, -1, 0);
        pushButton_selectVideo = new QPushButton(widget);
        pushButton_selectVideo->setObjectName(QString::fromUtf8("pushButton_selectVideo"));

        horizontalLayout->addWidget(pushButton_selectVideo);

        pushButton_openCamera = new QPushButton(widget);
        pushButton_openCamera->setObjectName(QString::fromUtf8("pushButton_openCamera"));

        horizontalLayout->addWidget(pushButton_openCamera);

        pushButton_cleanLog = new QPushButton(widget);
        pushButton_cleanLog->setObjectName(QString::fromUtf8("pushButton_cleanLog"));

        horizontalLayout->addWidget(pushButton_cleanLog);


        gridLayout->addWidget(widget, 2, 0, 1, 1);

        widget_2 = new QWidget(VFR_algorithmClass);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 0, 0, 0);
        checkBox_rect = new QCheckBox(widget_2);
        checkBox_rect->setObjectName(QString::fromUtf8("checkBox_rect"));

        horizontalLayout_2->addWidget(checkBox_rect);

        checkBox_snap = new QCheckBox(widget_2);
        checkBox_snap->setObjectName(QString::fromUtf8("checkBox_snap"));

        horizontalLayout_2->addWidget(checkBox_snap);

        checkBox_saveSnap = new QCheckBox(widget_2);
        checkBox_saveSnap->setObjectName(QString::fromUtf8("checkBox_saveSnap"));

        horizontalLayout_2->addWidget(checkBox_saveSnap);


        gridLayout->addWidget(widget_2, 2, 1, 1, 1);

        listWidget_snap = new QListWidget(VFR_algorithmClass);
        listWidget_snap->setObjectName(QString::fromUtf8("listWidget_snap"));

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
        VFR_algorithmClass->setWindowTitle(QCoreApplication::translate("VFR_algorithmClass", "\351\252\214\350\257\201Demo", nullptr));
        label_2->setText(QCoreApplication::translate("VFR_algorithmClass", "\347\233\221\346\216\247\344\272\272\350\204\270\350\257\206\345\210\253\347\256\227\346\263\225\351\252\214\350\257\201\345\271\263\345\217\260", nullptr));
        label_video->setText(QString());
        pushButton_selectVideo->setText(QCoreApplication::translate("VFR_algorithmClass", "\351\200\211\346\213\251\350\247\206\351\242\221", nullptr));
        pushButton_openCamera->setText(QCoreApplication::translate("VFR_algorithmClass", "\345\274\200\345\220\257\346\221\204\345\203\217\345\244\264", nullptr));
        pushButton_cleanLog->setText(QCoreApplication::translate("VFR_algorithmClass", "\346\270\205\347\251\272\346\227\245\345\277\227", nullptr));
        checkBox_rect->setText(QCoreApplication::translate("VFR_algorithmClass", "\344\272\272\350\204\270\346\241\206", nullptr));
        checkBox_snap->setText(QCoreApplication::translate("VFR_algorithmClass", "\346\212\223\346\213\215", nullptr));
        checkBox_saveSnap->setText(QCoreApplication::translate("VFR_algorithmClass", "\344\277\235\345\255\230", nullptr));
    } // retranslateUi

};

namespace Ui {
    class VFR_algorithmClass: public Ui_VFR_algorithmClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VFR_ALGORITHM_H
