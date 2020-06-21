#pragma once

#include <QObject>
#include <QLabel>
#include <QPoint>
#include <QString>
#include <QPixmap>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QRect>
#include <QDebug>
#include <QImage>

#define  SCALED_MAX   1000   //(最大放大10倍)
#define  SCALED_MIN   20     //(最小缩小为20%)

class FreeAdjustImageLabel : public QLabel
{
    Q_OBJECT

public:
    FreeAdjustImageLabel(QWidget *parent);
    ~FreeAdjustImageLabel();
    void ShowImage(QString &path);
    void GetInitScaled();
    void RefreshShowImage();

private:
    void LimitScaledParam();
    void ScaledDisplayImage(QRect rect);

protected:
    virtual void mousePressEvent(QMouseEvent *ev);
    virtual void mouseMoveEvent(QMouseEvent *ev);
    virtual void mouseReleaseEvent(QMouseEvent *ev);
    virtual void wheelEvent(QWheelEvent*event);

private:
    QPoint  cur_position;
    QPoint  begin_pt;
    QPoint  end_pt;
    bool    mouse_status;
    QString image_path;

    int     init_size_w;
    int     init_size_h;

    QRect   display_rect;

    int     scaled_param;
};
