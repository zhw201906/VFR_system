#include "FreeAdjustImageLabel.h"

//等比缩放图像
//参数1：图像路径（IN）
//参数2：空间宽度（IN）
//参数3：控件高度（IN）
//参数4：缩放后结果图（OUT）
static void Geometric_Scaling_Image(const QString path, const int control_width, const int control_height, QPixmap &dst_pix)
{
    QPixmap pix;
    pix.load(path);

    double ratio_w = pix.width() * 1.0 / control_width;
    double ratio_h = pix.height() * 1.0 / control_height;

    if (ratio_w >= ratio_h)
        pix = pix.scaled(pix.width() / ratio_w, pix.height() / ratio_w);
    else
        pix = pix.scaled(pix.width() / ratio_h, pix.height() / ratio_h);

    dst_pix = pix;
}

FreeAdjustImageLabel::FreeAdjustImageLabel(QWidget *parent)
    : QLabel(parent)
{
    this->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    mouse_status = false;
}

FreeAdjustImageLabel::~FreeAdjustImageLabel()
{

}

void FreeAdjustImageLabel::ShowImage(QString &path)
{
    if (path.isEmpty())
    {
        return;
    }

    image_path = path;
    QPixmap pix(image_path);
    init_size_w = pix.width();
    init_size_h = pix.height();
    Geometric_Scaling_Image(image_path, this->width(), this->height(), pix);
    this->setPixmap(pix);
    scaled_param = pix.width() * 100.0 / init_size_w;
}

void FreeAdjustImageLabel::GetInitScaled()
{
    QPixmap pix;
    Geometric_Scaling_Image(image_path, this->width(), this->height(), pix);
    scaled_param = pix.width() * 100.0 / init_size_w;
}

void FreeAdjustImageLabel::RefreshShowImage()
{
    if (!image_path.isEmpty())
    {
        QPixmap pix;
        Geometric_Scaling_Image(image_path, this->width(), this->height(), pix);
        this->setPixmap(pix);
    }
}

void FreeAdjustImageLabel::LimitScaledParam()
{
    scaled_param = scaled_param >= SCALED_MAX ? SCALED_MAX : scaled_param;
    scaled_param = scaled_param <= SCALED_MIN ? SCALED_MIN : scaled_param;
}

void FreeAdjustImageLabel::ScaledDisplayImage(QRect rect)
{
    int scaled_w = init_size_w * scaled_param / 100.0;
    int scaled_h = init_size_h * scaled_param / 100.0;

    QImage  img(image_path);
    QPixmap pix = QPixmap::fromImage(img.scaled(scaled_w, scaled_h).copy(rect));
    this->setPixmap(pix);    
}

void FreeAdjustImageLabel::mousePressEvent(QMouseEvent * ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        mouse_status = true;
        this->setCursor(Qt::ClosedHandCursor);
        begin_pt = QPoint(ev->x(), ev->y());
    }
}

void FreeAdjustImageLabel::mouseMoveEvent(QMouseEvent * ev)
{
    //cur_position = QPoint(ev->x(), ev->y());
    //qDebug() << "current x:" << cur_position.x() << "  y:" << cur_position.y();

    //QPoint diff_pt = cur_position - begin_pt;
    //int dis_x = display_rect.x() - diff_pt.x() >= 0 ? display_rect.x() - diff_pt.x() : 0;
    //int dis_y = display_rect.y() - diff_pt.y() >= 0 ? display_rect.y() - diff_pt.y() : 0;

    //display_rect.setX(dis_x);
    //display_rect.setY(dis_y);
    //ScaledDisplayImage(display_rect);
}

void FreeAdjustImageLabel::mouseReleaseEvent(QMouseEvent * ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        mouse_status = false;
        this->setCursor(Qt::ArrowCursor);
    }
}

void FreeAdjustImageLabel::wheelEvent(QWheelEvent * event)
{
 //   if (event->delta() > 0)
 //   {
 //       qDebug() << QString::fromLocal8Bit("滚轮向上滚动");
 //       scaled_param += 5;
 //       LimitScaledParam();
 //       qDebug() << "scaled_param=" << scaled_param;

 //       //QRect rect(0, 0, this->width(), this->height());
 //       //ScaledDisplayImage(rect);

 //       int cur_x = cur_position.x()*scaled_param / 100.0;
 //       int cur_y = cur_position.y()*scaled_param / 100.0;

 ///*       int left_x = cur_x - cur_position.x();
 //       int left_y = cur_y - cur_position.y();
 //       int right_x=this->width()*scaled_param/100 -*/

 //       int begin_x = (cur_x - cur_position.x()) > 0 ? cur_x - cur_position.x() : 0;
 //       int begin_y = (cur_y - cur_position.y()) > 0 ? cur_x - cur_position.y() : 0;

 //       display_rect.setX(begin_x);
 //       display_rect.setY(begin_y);
 //       display_rect.setWidth(this->width());
 //       display_rect.setHeight(this->height());
 //       ScaledDisplayImage(display_rect);

 //       //int end_x = (cur_x - cur_position.x()) > 0 ? cur_x - cur_position.x() : 0;
 //       //QPoint  begin_pt;
 //       //begin_pt.setX(cur_x - this->width());
 //       //QRect rect;
 //       //int left_diff_w = cur_position.x();
 //       //int top_diff_h  = cur_position.y();

 //   }
 //   else if (event->delta() < 0)
 //   {
 //       qDebug() << QString::fromLocal8Bit("滚轮向下滚动");
 //       scaled_param -= 5;
 //       LimitScaledParam();
 //       qDebug() << "scaled_param=" << scaled_param;


 //       QRect rect(0, 0, this->width(), this->height());
 //       ScaledDisplayImage(rect);
 ///*       int scaled_w = init_size_w * scaled_param / 100;
 //       int scaled_h = init_size_h * scaled_param / 100;*/

 //   }
}
