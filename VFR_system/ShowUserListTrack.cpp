#include "ShowUserListTrack.h"

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

ShowUserListTrack::ShowUserListTrack(QMap<QString, CameraPositionInfo>& track_map, QString& dir, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	p_show_snap_image = NULL;

	ui.tableWidget_userTrack->setColumnCount(TABLE_COLS); //设置列数
	ui.tableWidget_userTrack->setColumnWidth(0, 80); 
	ui.tableWidget_userTrack->setColumnWidth(1, 80); 
	ui.tableWidget_userTrack->setColumnWidth(2, 80); 
	//ui.tableWidget_userTrack->setColumnWidth(3, 120); 
	//ui.tableWidget_userTrack->setColumnWidth(4, 80);  
	//ui.tableWidget_userTrack->setColumnWidth(5, 80);  
	//ui.tableWidget_userTrack->setColumnWidth(6, 80);  
	ui.tableWidget_userTrack->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget_userTrack->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QStringList sListHeader;
	sListHeader << QString::fromLocal8Bit("开始时间")
		//<< QString::fromLocal8Bit("到达时间")
		<< QString::fromLocal8Bit("开始位置")
		//<< QString::fromLocal8Bit("到达位置")
		<< QString::fromLocal8Bit("耗时(s)");
		//<< QString::fromLocal8Bit("抓拍图");
	ui.tableWidget_userTrack->setHorizontalHeaderLabels(sListHeader);

	ui.tableWidget_userTrack->setRowCount(track_map.size());

	QString link_info;
	int row_idx = 0;
	auto it = track_map.begin();
	CameraPositionInfo last_info = it.value();
	QString last_time = it.key();
	link_info.append(it.value().position_name);

	ui.tableWidget_userTrack->setItem(row_idx, 0, new QTableWidgetItem(it.key().mid(11, 8)));
	ui.tableWidget_userTrack->setItem(row_idx, 1, new QTableWidgetItem(it.value().position_name));
	ui.tableWidget_userTrack->setItem(row_idx, 2, new QTableWidgetItem(QString("%1").arg(0)));	
	AdjoinPosition adj_position;
	qint64  tmp_val = QDateTime::fromString(it.key(), "yyyy-MM-dd hh:mm:ss.zzz").toMSecsSinceEpoch();
	adj_position.url = dir + QString('/') + QString("%1").arg(tmp_val);
	adj_position.start = QPoint(0, 0);
	adj_position.end = QPoint(0, 0);
	link_track_buff.push_back(adj_position);

	it++;
	row_idx++;
	while (it != track_map.end())
	{
		if (it.value().position == last_info.position)
		{
			last_info = it.value();
			last_time = it.key();
			++it;
			//++row_idx;
			continue;
		}
		qint64  start_val = QDateTime::fromString(last_time, "yyyy-MM-dd hh:mm:ss.zzz").toMSecsSinceEpoch();
		qint64  end_val   = QDateTime::fromString(it.key(),  "yyyy-MM-dd hh:mm:ss.zzz").toMSecsSinceEpoch();
		int diff = (end_val - start_val) / 1000;

		//ui.tableWidget_userTrack->setItem(row_idx, 0, new QTableWidgetItem(last_time.mid(11, 8)));
		////ui.tableWidget_userTrack->setItem(row_idx, 1, new QTableWidgetItem(it.key().mid(11)));
		//ui.tableWidget_userTrack->setItem(row_idx, 1, new QTableWidgetItem(last_info.position_name));
		////ui.tableWidget_userTrack->setItem(row_idx, 3, new QTableWidgetItem(it.value().position_name));
		//ui.tableWidget_userTrack->setItem(row_idx, 2, new QTableWidgetItem(QString("%1").arg(diff)));

		ui.tableWidget_userTrack->setItem(row_idx, 0, new QTableWidgetItem(it.key().mid(11, 8)));
		//ui.tableWidget_userTrack->setItem(row_idx, 1, new QTableWidgetItem(it.key().mid(11)));
		ui.tableWidget_userTrack->setItem(row_idx, 1, new QTableWidgetItem(it.value().position_name));
		//ui.tableWidget_userTrack->setItem(row_idx, 3, new QTableWidgetItem(it.value().position_name));
		ui.tableWidget_userTrack->setItem(row_idx, 2, new QTableWidgetItem(QString("%1").arg(diff)));

		QPushButton *push_button = new QPushButton();
		push_button->setText(QString::fromLocal8Bit("图片"));
		ui.tableWidget_userTrack->setCellWidget(row_idx, 5, push_button);

		AdjoinPosition adj_position;
		adj_position.start = last_info.position;
		adj_position.end = it.value().position;

		adj_position.url = dir + QString('/') + QString("%1").arg(end_val);
		link_track_buff.push_back(adj_position);

		last_info = it.value();
		last_time = it.key();

		link_info.append(QString("  >>>>>  "));
		link_info.append(it.value().position_name);

		++it;
		++row_idx;
	}	

	ui.tableWidget_userTrack->setRowCount(row_idx);

	ui.textBrowser_linkTrack->append(link_info);

	connect(ui.tableWidget_userTrack, &QTableWidget::itemClicked, this, &ShowUserListTrack::DealClickedItem);
	connect(ui.tableWidget_userTrack, &QTableWidget::cellClicked, this, &ShowUserListTrack::DealClickedItemCell);

}

ShowUserListTrack::~ShowUserListTrack()
{
}

//点击某一行，返回信号给主界面，对应轨迹高亮显示
void ShowUserListTrack::DealClickedItem(QTableWidgetItem * item)
{
	qDebug() << "select rows:" << item->row();
	ui.label_smallImage->clear();
	ui.label_bigImage->clear();

	QString small_path = link_track_buff[item->row()].url + QString(".jpg");
	QString big_path = link_track_buff[item->row()].url + QString("_big.jpg");

	QPixmap small_pix, big_pix;
	Geometric_Scaling_Image(small_path, ui.label_smallImage->width(), ui.label_smallImage->height(), small_pix);
	Geometric_Scaling_Image(big_path, ui.label_bigImage->width(), ui.label_bigImage->height(), big_pix);

	ui.label_smallImage->setPixmap(small_pix);
	ui.label_bigImage->setPixmap(big_pix);

	emit CurrentClickedLinkTrack(link_track_buff[item->row()].start, link_track_buff[item->row()].end);

}

//点击某一行的控件，就是显示抓拍图
void ShowUserListTrack::DealClickedItemCell(int row, int col)
{
	//if (p_show_snap_image == NULL)
	//{
	//	if (!link_track_buff[row].url.isEmpty())
	//	{
	//		p_show_snap_image = new ShowSnapRecgImage(link_track_buff[row].url);
	//		p_show_snap_image->move(QPoint(this->x() + this->width(), this->y()));
	//		p_show_snap_image->show();
	//	}
	//}
	//else
	//{
	//	if (!link_track_buff[row].url.isEmpty())
	//	{
	//		p_show_snap_image->SetShowImagePath(link_track_buff[row].url);
	//		p_show_snap_image->move(QPoint(this->width(), this->height()));
	//		p_show_snap_image->show();
	//	}
	//}
}


void ShowUserListTrack::closeEvent(QCloseEvent * event)
{
	emit CloseSignals();
}
