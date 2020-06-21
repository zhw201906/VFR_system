#include "DisplayRecgFaceImage.h"

DisplayRecgFaceImage::DisplayRecgFaceImage(QWidget *parent)
    : QLabel(parent)
{

}

DisplayRecgFaceImage::~DisplayRecgFaceImage()
{
}

void DisplayRecgFaceImage::SetLabelStyle(QString style)
{
    this->setStyleSheet(style);
}

void DisplayRecgFaceImage::mousePressEvent(QMouseEvent * ev)
{
    SetLabelStyle(SELECTED_LABEL_STYLE);
    emit SelectedCurrentLabel();
}
