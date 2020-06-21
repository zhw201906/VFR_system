#include "DisplayDetectResult.h"

DisplayDetectResult::DisplayDetectResult(QString &path, QWidget *parent)
    : QWidget(parent), image_path(path)
{
    ui.setupUi(this);

    //setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::WindowModal);

    ui.label_detectImage->ShowImage(image_path);

    connect(ui.pushButton_exit, &QPushButton::clicked, this, &DisplayDetectResult::Exit);
    connect(ui.pushButton_adaptScreen, &QPushButton::clicked, this, &DisplayDetectResult::AdaptScreenDisplay);

}

DisplayDetectResult::~DisplayDetectResult()
{
}

void DisplayDetectResult::Exit()
{
    this->close();
}

void DisplayDetectResult::AdaptScreenDisplay()
{
    ui.label_detectImage->RefreshShowImage();
    ui.label_detectImage->GetInitScaled();
}

void DisplayDetectResult::resizeEvent(QResizeEvent *event)
{
    ui.label_detectImage->RefreshShowImage();
    ui.label_detectImage->GetInitScaled();
}
