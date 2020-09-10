#include "DisplayRecognizeResult.h"

//等比缩放图像
//参数1：图像路径（IN）
//参数2：空间宽度（IN）
//参数3：控件高度（IN）
//参数4：缩放后结果图（OUT）
static void Geometric_Scaling_Image(QImage &pix, const int control_width, const int control_height, QPixmap &dst_pix)
{
	//QPixmap pix;
	//pix.fromImage(img);

	double ratio_w = pix.width() * 1.0 / control_width;
	double ratio_h = pix.height() * 1.0 / control_height;

	if (ratio_w >= ratio_h)
		pix = pix.scaled(pix.width() / ratio_w, pix.height() / ratio_w);
	else
		pix = pix.scaled(pix.width() / ratio_h, pix.height() / ratio_h);
	
	dst_pix = QPixmap::fromImage(pix);
}

DisplayRecognizeResult::DisplayRecognizeResult(FaceRecognizeShowInfo *pinfo, QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::WindowModal);

	p_show_recg_info = pinfo;

    
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

	cur_selected_image = 1;
	ui.label_userImage1->SetLabelStyle(SELECTED_LABEL_STYLE);
	DisplaySelectedUserInfo(cur_selected_image);

	QPixmap pix;
	Geometric_Scaling_Image(p_show_recg_info[0].lib_image, ui.label_userImage1->width(), ui.label_userImage1->height(), pix);
	ui.label_userImage1->setPixmap(pix);

	Geometric_Scaling_Image(p_show_recg_info[1].lib_image, ui.label_userImage2->width(), ui.label_userImage2->height(), pix);
	ui.label_userImage2->setPixmap(pix);

	Geometric_Scaling_Image(p_show_recg_info[2].lib_image, ui.label_userImage3->width(), ui.label_userImage3->height(), pix);
	ui.label_userImage3->setPixmap(pix);
}

DisplayRecognizeResult::~DisplayRecognizeResult()
{

}

void DisplayRecognizeResult::DisplayUserInfo(FaceRecognizeShowInfo *pinfo)
{
	ui.label_libName->setText(QString::fromLocal8Bit(pinfo->db_name));
	ui.label_userName->setText(QString::fromLocal8Bit(pinfo->user_name));
	//ui.label_userSex->setText(QString::fromLocal8Bit(pinfo->sex));
	//ui.label_userBirthday->setText(QString::fromLocal8Bit(pinfo->birt));
	ui.label_userCardId->setText(QString::fromLocal8Bit(pinfo->card_number));
	ui.label_userProvices->setText(QString::fromLocal8Bit(pinfo->province));
	ui.label_userCity->setText(QString::fromLocal8Bit(pinfo->city));
	ui.label_userAddress->setText(QString::fromLocal8Bit(pinfo->address));
	ui.label_compareScore->setText(QString::fromLocal8Bit("%1%").arg(pinfo->score * 100));
}

void DisplayRecognizeResult::DisplaySelectedUserInfo(int imgn)
{
    switch (imgn)
    {
	case 1:DisplayUserInfo(&p_show_recg_info[0]); break;
    case 2:DisplayUserInfo(&p_show_recg_info[1]); break;
    case 3:DisplayUserInfo(&p_show_recg_info[2]); break;
    }

}
