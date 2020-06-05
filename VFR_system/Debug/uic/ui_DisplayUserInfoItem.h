/********************************************************************************
** Form generated from reading UI file 'DisplayUserInfoItem.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISPLAYUSERINFOITEM_H
#define UI_DISPLAYUSERINFOITEM_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DisplayUserInfoItem
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_image;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QCheckBox *checkBox_userName;
    QToolButton *toolButton_edit;
    QToolButton *toolButton_del;

    void setupUi(QWidget *DisplayUserInfoItem)
    {
        if (DisplayUserInfoItem->objectName().isEmpty())
            DisplayUserInfoItem->setObjectName(QString::fromUtf8("DisplayUserInfoItem"));
        DisplayUserInfoItem->resize(198, 246);
        DisplayUserInfoItem->setMinimumSize(QSize(0, 0));
        DisplayUserInfoItem->setMaximumSize(QSize(198, 261));
        verticalLayout = new QVBoxLayout(DisplayUserInfoItem);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_image = new QLabel(DisplayUserInfoItem);
        label_image->setObjectName(QString::fromUtf8("label_image"));
        label_image->setMinimumSize(QSize(180, 200));
        label_image->setMaximumSize(QSize(180, 200));
        label_image->setFrameShape(QFrame::Box);
        label_image->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_image);

        frame = new QFrame(DisplayUserInfoItem);
        frame->setObjectName(QString::fromUtf8("frame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        checkBox_userName = new QCheckBox(frame);
        checkBox_userName->setObjectName(QString::fromUtf8("checkBox_userName"));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font.setPointSize(10);
        checkBox_userName->setFont(font);

        horizontalLayout->addWidget(checkBox_userName);

        toolButton_edit = new QToolButton(frame);
        toolButton_edit->setObjectName(QString::fromUtf8("toolButton_edit"));
        toolButton_edit->setMinimumSize(QSize(20, 20));
        toolButton_edit->setMaximumSize(QSize(20, 20));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icon/icon/edit.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_edit->setIcon(icon);
        toolButton_edit->setIconSize(QSize(16, 16));

        horizontalLayout->addWidget(toolButton_edit);

        toolButton_del = new QToolButton(frame);
        toolButton_del->setObjectName(QString::fromUtf8("toolButton_del"));
        toolButton_del->setMinimumSize(QSize(20, 20));
        toolButton_del->setMaximumSize(QSize(20, 20));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icon/icon/del.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_del->setIcon(icon1);
        toolButton_del->setIconSize(QSize(16, 16));

        horizontalLayout->addWidget(toolButton_del);

        horizontalLayout->setStretch(0, 10);

        verticalLayout->addWidget(frame);


        retranslateUi(DisplayUserInfoItem);

        QMetaObject::connectSlotsByName(DisplayUserInfoItem);
    } // setupUi

    void retranslateUi(QWidget *DisplayUserInfoItem)
    {
        DisplayUserInfoItem->setWindowTitle(QCoreApplication::translate("DisplayUserInfoItem", "DisplayUserInfoItem", nullptr));
        label_image->setText(QString());
        checkBox_userName->setText(QString());
        toolButton_edit->setText(QCoreApplication::translate("DisplayUserInfoItem", "E", nullptr));
        toolButton_del->setText(QCoreApplication::translate("DisplayUserInfoItem", "D", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DisplayUserInfoItem: public Ui_DisplayUserInfoItem {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISPLAYUSERINFOITEM_H
