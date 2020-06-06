#pragma once

#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QPixmap>
#include <QImage>
#include <QPoint>
#include <QPainter>
#include <QPen>

class BuildingMapLabel : public QLabel
{
    Q_OBJECT

public:
    BuildingMapLabel(QWidget *parent);
    ~BuildingMapLabel();

    void SetShowImage(QString &path);
    void SetImageScaled(float &scaled_x, float &scaled_y);

    void RefreshDisplayImage();

    virtual void mousePressEvent(QMouseEvent *ev);
    virtual void paintEvent(QPaintEvent *);
    //virtual void resizeEvent(QResizeEvent *event);

signals:
    void selectPosition(QPoint pt);

    


private:
    float   imageScaled_x, imageScaled_y;
    QString imgPath;
    int     cur_x, cur_y;

};
