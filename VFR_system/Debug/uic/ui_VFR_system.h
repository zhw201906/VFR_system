/********************************************************************************
** Form generated from reading UI file 'VFR_system.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VFR_SYSTEM_H
#define UI_VFR_SYSTEM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VFR_systemClass
{
public:
    QGridLayout *gridLayout_2;
    QSpacerItem *verticalSpacer;
    QLabel *label_3;
    QFrame *frame;
    QGridLayout *gridLayout;
    QLineEdit *lineEdit_password;
    QPushButton *pushButton_registerAccount;
    QLabel *label_2;
    QLineEdit *lineEdit_account;
    QPushButton *pushButton_forgetPwd;
    QLabel *label;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *horizontalSpacer_4;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QCheckBox *checkBox_savePwd;
    QCheckBox *checkBox_autoLogin;
    QSpacerItem *horizontalSpacer_2;
    QFrame *frame_3;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *pushButton_exit;
    QSpacerItem *horizontalSpacer_7;
    QPushButton *pushButton_login;
    QSpacerItem *horizontalSpacer_6;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *verticalSpacer_3;

    void setupUi(QWidget *VFR_systemClass)
    {
        if (VFR_systemClass->objectName().isEmpty())
            VFR_systemClass->setObjectName(QString::fromUtf8("VFR_systemClass"));
        VFR_systemClass->resize(600, 400);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(VFR_systemClass->sizePolicy().hasHeightForWidth());
        VFR_systemClass->setSizePolicy(sizePolicy);
        VFR_systemClass->setMinimumSize(QSize(600, 400));
        VFR_systemClass->setMaximumSize(QSize(600, 400));
        QFont font;
        font.setUnderline(true);
        VFR_systemClass->setFont(font);
        gridLayout_2 = new QGridLayout(VFR_systemClass);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 2, 0, 1, 1);

        label_3 = new QLabel(VFR_systemClass);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\271\274\345\234\206"));
        font1.setPointSize(24);
        font1.setBold(true);
        font1.setUnderline(false);
        font1.setWeight(75);
        label_3->setFont(font1);
        label_3->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_3, 1, 0, 1, 1);

        frame = new QFrame(VFR_systemClass);
        frame->setObjectName(QString::fromUtf8("frame"));
        sizePolicy1.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy1);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lineEdit_password = new QLineEdit(frame);
        lineEdit_password->setObjectName(QString::fromUtf8("lineEdit_password"));
        lineEdit_password->setMinimumSize(QSize(180, 30));
        lineEdit_password->setMaximumSize(QSize(180, 30));

        gridLayout->addWidget(lineEdit_password, 1, 2, 1, 1);

        pushButton_registerAccount = new QPushButton(frame);
        pushButton_registerAccount->setObjectName(QString::fromUtf8("pushButton_registerAccount"));
        pushButton_registerAccount->setMinimumSize(QSize(95, 35));
        pushButton_registerAccount->setMaximumSize(QSize(95, 35));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Agency FB"));
        font2.setPointSize(12);
        font2.setUnderline(true);
        pushButton_registerAccount->setFont(font2);
        pushButton_registerAccount->setCursor(QCursor(Qt::PointingHandCursor));
        pushButton_registerAccount->setFlat(true);

        gridLayout->addWidget(pushButton_registerAccount, 0, 3, 1, 1);

        label_2 = new QLabel(frame);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setMinimumSize(QSize(65, 35));
        label_2->setMaximumSize(QSize(65, 35));
        QFont font3;
        font3.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font3.setPointSize(14);
        font3.setBold(false);
        font3.setUnderline(false);
        font3.setWeight(50);
        label_2->setFont(font3);

        gridLayout->addWidget(label_2, 1, 1, 1, 1);

        lineEdit_account = new QLineEdit(frame);
        lineEdit_account->setObjectName(QString::fromUtf8("lineEdit_account"));
        lineEdit_account->setMinimumSize(QSize(180, 30));
        lineEdit_account->setMaximumSize(QSize(180, 30));

        gridLayout->addWidget(lineEdit_account, 0, 2, 1, 1);

        pushButton_forgetPwd = new QPushButton(frame);
        pushButton_forgetPwd->setObjectName(QString::fromUtf8("pushButton_forgetPwd"));
        pushButton_forgetPwd->setMinimumSize(QSize(95, 35));
        pushButton_forgetPwd->setMaximumSize(QSize(95, 35));
        pushButton_forgetPwd->setFont(font2);
        pushButton_forgetPwd->setCursor(QCursor(Qt::PointingHandCursor));
        pushButton_forgetPwd->setFlat(true);

        gridLayout->addWidget(pushButton_forgetPwd, 1, 3, 1, 1);

        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(65, 35));
        label->setMaximumSize(QSize(65, 35));
        label->setFont(font3);

        gridLayout->addWidget(label, 0, 1, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 0, 0, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_4, 0, 4, 1, 1);


        gridLayout_2->addWidget(frame, 3, 0, 1, 1);

        frame_2 = new QFrame(VFR_systemClass);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        sizePolicy1.setHeightForWidth(frame_2->sizePolicy().hasHeightForWidth());
        frame_2->setSizePolicy(sizePolicy1);
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame_2);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        checkBox_savePwd = new QCheckBox(frame_2);
        checkBox_savePwd->setObjectName(QString::fromUtf8("checkBox_savePwd"));
        checkBox_savePwd->setMinimumSize(QSize(90, 25));
        QFont font4;
        font4.setPointSize(10);
        font4.setUnderline(false);
        checkBox_savePwd->setFont(font4);

        horizontalLayout->addWidget(checkBox_savePwd);

        checkBox_autoLogin = new QCheckBox(frame_2);
        checkBox_autoLogin->setObjectName(QString::fromUtf8("checkBox_autoLogin"));
        checkBox_autoLogin->setMinimumSize(QSize(90, 25));
        checkBox_autoLogin->setFont(font4);

        horizontalLayout->addWidget(checkBox_autoLogin);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        gridLayout_2->addWidget(frame_2, 4, 0, 1, 1);

        frame_3 = new QFrame(VFR_systemClass);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        sizePolicy1.setHeightForWidth(frame_3->sizePolicy().hasHeightForWidth());
        frame_3->setSizePolicy(sizePolicy1);
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(frame_3);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_5);

        pushButton_exit = new QPushButton(frame_3);
        pushButton_exit->setObjectName(QString::fromUtf8("pushButton_exit"));
        pushButton_exit->setMinimumSize(QSize(120, 40));
        QFont font5;
        font5.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font5.setPointSize(12);
        font5.setBold(true);
        font5.setUnderline(false);
        font5.setWeight(75);
        pushButton_exit->setFont(font5);

        horizontalLayout_2->addWidget(pushButton_exit);

        horizontalSpacer_7 = new QSpacerItem(44, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_7);

        pushButton_login = new QPushButton(frame_3);
        pushButton_login->setObjectName(QString::fromUtf8("pushButton_login"));
        pushButton_login->setMinimumSize(QSize(120, 40));
        pushButton_login->setFont(font5);

        horizontalLayout_2->addWidget(pushButton_login);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_6);


        gridLayout_2->addWidget(frame_3, 6, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_2, 0, 0, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 28, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout_2->addItem(verticalSpacer_3, 5, 0, 1, 1);


        retranslateUi(VFR_systemClass);

        QMetaObject::connectSlotsByName(VFR_systemClass);
    } // setupUi

    void retranslateUi(QWidget *VFR_systemClass)
    {
        VFR_systemClass->setWindowTitle(QCoreApplication::translate("VFR_systemClass", "\347\263\273\347\273\237\347\231\273\345\275\225", nullptr));
        label_3->setText(QCoreApplication::translate("VFR_systemClass", "\346\245\274\345\256\207\347\233\221\346\216\247\344\272\272\350\204\270\350\257\206\345\210\253\347\263\273\347\273\237", nullptr));
        pushButton_registerAccount->setText(QCoreApplication::translate("VFR_systemClass", "\346\263\250\345\206\214\345\270\220\345\217\267", nullptr));
        label_2->setText(QCoreApplication::translate("VFR_systemClass", "\345\257\206\347\240\201\357\274\232", nullptr));
        pushButton_forgetPwd->setText(QCoreApplication::translate("VFR_systemClass", "\345\277\230\350\256\260\345\257\206\347\240\201", nullptr));
        label->setText(QCoreApplication::translate("VFR_systemClass", "\345\270\220\345\217\267\357\274\232", nullptr));
        checkBox_savePwd->setText(QCoreApplication::translate("VFR_systemClass", "\344\277\235\345\255\230\345\257\206\347\240\201", nullptr));
        checkBox_autoLogin->setText(QCoreApplication::translate("VFR_systemClass", "\350\207\252\345\212\250\347\231\273\345\275\225", nullptr));
        pushButton_exit->setText(QCoreApplication::translate("VFR_systemClass", "\351\200\200\345\207\272", nullptr));
        pushButton_login->setText(QCoreApplication::translate("VFR_systemClass", "\347\231\273\345\275\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class VFR_systemClass: public Ui_VFR_systemClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VFR_SYSTEM_H
