#include "BuildingMapLabel.h"

BuildingMapLabel::BuildingMapLabel(QWidget *parent)
    : QLabel(parent)
{
    imageScaled_x = 1;
    imageScaled_y = 1;
    cur_x = 0;
    cur_y = 0;
}

BuildingMapLabel::~BuildingMapLabel()
{
}

void BuildingMapLabel::SetShowImage(QString & path)
{
    imgPath = path;
    if (!imgPath.isEmpty())
    {
        QImage img(imgPath);
        this->setPixmap(QPixmap::fromImage(img.scaled(this->width(), this->height())));
    }
    else
    {
        this->clear();
    }
}

void BuildingMapLabel::SetImageScaled(float & scaled_x, float &scaled_y)
{
    this->imageScaled_x = scaled_x;
    this->imageScaled_y = scaled_y;
}

void BuildingMapLabel::RefreshDisplayImage()
{
    //qDebug() << "RefreshDisplayImage...";
    if (!imgPath.isEmpty())
    {
        QImage img(imgPath);
        this->setPixmap(QPixmap::fromImage(img.scaled(this->width(), this->height())));
    }
}

void BuildingMapLabel::mousePressEvent(QMouseEvent * ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        qDebug() << "press button x:" << ev->x()*imageScaled_x << "  y:" << ev->y()*imageScaled_y;
        cur_x = ev->x();
        cur_y = ev->y();
        update();
        emit selectPosition(QPoint(ev->x()*imageScaled_x, ev->y()*imageScaled_y));
    }
}

void BuildingMapLabel::paintEvent(QPaintEvent *)
{
    qDebug() << "label paint event...";
    QPainter painter(this);
    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor(0, 255, 0));
    painter.setPen(pen);

    painter.drawEllipse(cur_x, cur_y, 50, 50);
}

//void BuildingMapLabel::resizeEvent(QResizeEvent * event)
//{
//    qDebug() << "building label resize event   w:" << width() << "  h:" << height();
//    //if (!imgPath.isEmpty())
//    //{
//    //    QImage img(imgPath);
//    //    clear();
//    //    setPixmap(QPixmap::fromImage(img.scaled(width(), height())));
//    //}
//    //else
//    //{
//    //    this->clear();
//    //}
//}
