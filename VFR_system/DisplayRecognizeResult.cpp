#include "DisplayRecognizeResult.h"

DisplayRecognizeResult::DisplayRecognizeResult(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::WindowModal);

    cur_selected_image = 1;
    ui.label_userImage1->SetLabelStyle(SELECTED_LABEL_STYLE);
    connect(ui.label_userImage1, &DisplayRecgFaceImage::SelectedCurrentLabel, [=]() {
        cur_selected_image = 1;
        DisplaySelectedUserInfo(cur_selected_image);
        ui.label_userImage2->SetLabelStyle(FREE_LABEL_STYLE);
        ui.label_userImage3->SetLabelStyle(FREE_LABEL_STYLE);
    });

    connect(ui.label_userImage2, &DisplayRecgFaceImage::SelectedCurrentLabel, [=]() {
        cur_selected_image = 2;
        DisplaySelectedUserInfo(cur_selected_image);
        ui.label_userImage1->SetLabelStyle(FREE_LABEL_STYLE);
        ui.label_userImage3->SetLabelStyle(FREE_LABEL_STYLE);
    });

    connect(ui.label_userImage3, &DisplayRecgFaceImage::SelectedCurrentLabel, [=]() {
        cur_selected_image = 3;
        DisplaySelectedUserInfo(cur_selected_image);
        ui.label_userImage1->SetLabelStyle(FREE_LABEL_STYLE);
        ui.label_userImage2->SetLabelStyle(FREE_LABEL_STYLE);
    });

    connect(ui.pushButton_exit, &QPushButton::clicked, [=]() {
        this->close();
    });

}

DisplayRecognizeResult::~DisplayRecognizeResult()
{

}

void DisplayRecognizeResult::DisplaySelectedUserInfo(int imgn)
{
    switch (imgn)
    {
    case 1:; break;
    case 2:; break;
    case 3:; break;
    }
}
