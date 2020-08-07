#include "DealBuildingMap.h"

long long StringToInt64(std::string &str)
{
	long long res = 0;
	for (int i = 0; i < str.size(); i++)
	{
		res = (str[i] - '0') + res * 10;
	}
	return res;
}

DealBuildingMap::DealBuildingMap(QWidget *parent)
	: QLabel(parent)
{
	record_camera_position.clear();
	cur_camera_position = QPoint(0, 0);
	building_map_scaled_x = 1;
	building_map_scaled_y = 1;
	building_map_mode = BUILDING_SHOW_MAP;
	n_ctrl_draw_track = 0;
	p_ctrl_draw_track_timer = NULL;
	p_ctrl_draw_track_timer = new QTimer(this);
	p_list_track = NULL;
	high_light_flag = false;

	connect(p_ctrl_draw_track_timer, &QTimer::timeout, this, &DealBuildingMap::DealRunTrackProcess);
}

DealBuildingMap::~DealBuildingMap()
{

}

//设置显示缩放系数
void DealBuildingMap::SetScaledParam(float x, float y)
{
	building_map_scaled_x = x;
	building_map_scaled_y = y;
}

//设置操作模式，新建地图、载入地图、绘制轨迹
void DealBuildingMap::SetOperatorMode(MAP_MODE mode)
{
	building_map_mode = mode;
}

//存储当前坐标值
void DealBuildingMap::RecodeCurrentCameraPoint()
{
	if (building_map_mode != BUILDING_CREATE_MAP)
	{
		return;
	}
	
	if (cur_camera_position == QPoint(0, 0))
	{
		return;
	}
	QPoint pt_temp = cur_camera_position;
	record_camera_position.push_back(cur_camera_position);   //记录图片绝对位置
	cur_camera_position = QPoint(0, 0);
	update();

	emit CurrentPlaceCameraPoint(pt_temp);
}

//撤销已设置的相机位置
void DealBuildingMap::CalcelOneRecodePoint()
{
	if (building_map_mode != BUILDING_CREATE_MAP)
	{
		return;
	}

	if (cur_camera_position != QPoint(0, 0))
	{
		cur_camera_position = QPoint(0, 0);
	}
	else
	{
		if (record_camera_position.size() > 0)
		{
			auto it = record_camera_position.end();
			it--;
			record_camera_position.erase(it);
			emit CalcelOnePlacedCamera();
		}
	}
	update();
}

//获取抓拍识别的用户数据，解析后开始绘图
void DealBuildingMap::SetDrawTrackData(MAP_MODE mode, const QString & data, qint64 start_time, qint64 end_time)
{
	if (data.isEmpty())
	{
		return;
	}
	building_map_mode = mode;
	Json::Value snap_json;
	if (String2Json(data.toStdString(), snap_json) == false)
	{
		emit SendShowErrorMsg(error_snap_recg_user_json);
		return;
	}

	user_run_time_campos_map.clear();
	draw_user_name = QString(snap_json[BODY_RECG_NAME].asString().c_str());
	int snap_size = snap_json[BODY_RECG_INFO].size();
	if (start_time <= 0 && end_time <= 0)
	{
		for (int i = 0; i < snap_size; i++)
		{
			CameraPositionInfo cam_info;
			cam_info.camera_id = snap_json[BODY_RECG_INFO][i][BODY_RECG_CAMID].asInt();
			std::string stime_stamp = snap_json[BODY_RECG_INFO][i][BODY_RECG_TIME].asString();
			long long time_ = StringToInt64(stime_stamp);
			QString  time_stamp = QDateTime::fromMSecsSinceEpoch(time_).toString("yyyy-MM-dd hh:mm:ss.zzz");
			user_run_time_campos_map[time_stamp] = cam_info;
		}
	}
	else
	{
		for (int i = 0; i < snap_size; i++)
		{
			CameraPositionInfo cam_info;
			cam_info.camera_id = snap_json[BODY_RECG_INFO][i][BODY_RECG_CAMID].asInt();
			std::string stime_stamp = snap_json[BODY_RECG_INFO][i][BODY_RECG_TIME].asString();
			long long time_ = StringToInt64(stime_stamp);
			if (time_ >= start_time && time_ <= end_time)
			{
				QString  time_stamp = QDateTime::fromMSecsSinceEpoch(time_).toString("yyyy-MM-dd hh:mm:ss.zzz");
				user_run_time_campos_map[time_stamp] = cam_info;
			}
		}
	}

	if (user_run_time_campos_map.isEmpty())  //用户轨迹为空
	{
		emit SendShowErrorMsg(error_snap_recg_user_null);
		return;
	}

	DrawTrackDataToCache();
	CtrlDrawRunTrackProcess(1000);
}

//处理选中的建筑地图
void DealBuildingMap::DealSelectedBuildingMap(QString & info)
{
	if (building_map_mode != BUILDING_SHOW_MAP)
	{
		return;
	}

	Json::Value j_camera_info;
	if (String2Json(info.toStdString(), j_camera_info) == false)
	{
		emit SendShowErrorMsg(error_camera_position_json);
		return;
	}

	if (j_camera_info[BODY_CAMERA_INFO].size() != j_camera_info[BODY_CAMERA_NUM].asInt())
	{
		emit SendShowErrorMsg(error_camera_position_info);
		return;
	}

	existed_camera_position.clear();
	int size = j_camera_info[BODY_CAMERA_INFO].size();
	for (int i = 0; i < size; i++)
	{
		CameraPositionInfo scamera_info;

		scamera_info.camera_id = j_camera_info[BODY_CAMERA_INFO][i][BODY_CAMERA_ID].asInt();
		scamera_info.camera_ip = QString(j_camera_info[BODY_CAMERA_INFO][i][BODY_CAMERA_IP].asString().c_str());
		scamera_info.position_name = QString(j_camera_info[BODY_CAMERA_INFO][i][BODY_POSITION_NAME].asString().c_str());
		scamera_info.position.setX(j_camera_info[BODY_CAMERA_INFO][i][BODY_CAMERA_PX].asInt());
		scamera_info.position.setY(j_camera_info[BODY_CAMERA_INFO][i][BODY_CAMERA_PY].asInt());

		existed_camera_position.push_back(scamera_info);
	}
	update();
}

//清理正在创建的地图
void DealBuildingMap::CleanPlaceingNewBuinldingMap()
{
	record_camera_position.clear();
	cur_camera_position = QPoint(0, 0);
	existed_camera_position.clear();
	update();
}

//清理已选中的地图
void DealBuildingMap::CleanSelectedBuildingMap()
{
	existed_camera_position.clear();
	update();
}

//控制绘制运行轨迹过程
void DealBuildingMap::DealRunTrackProcess()
{
	n_ctrl_draw_track++;
	update();
}

//显示链式轨迹窗口
void DealBuildingMap::ShowListTrackWindow()
{
	if (user_run_time_campos_map.isEmpty())
	{
		emit SendShowErrorMsg(error_snap_recg_track_null);
		return;
	}

	if (p_list_track == NULL)
	{
		QString dir = QString(CAMERA_RECG_IMAGE_PATH) + QString('/') + draw_user_name;
		p_list_track = new ShowUserListTrack(user_run_time_campos_map, dir);

		connect(p_list_track, &ShowUserListTrack::CurrentClickedLinkTrack, this, &DealBuildingMap::DealSelectedTrack);
		connect(p_list_track, &ShowUserListTrack::CloseSignals, [=]() {
			delete p_list_track;
			p_list_track = NULL;
			high_light_flag = false;
			update();
		});
	}

	emit AdjustLinkTrackPosition();
}

//调整链式轨迹图界面的位置
void DealBuildingMap::MoveListTrackWindow(int x, int y, int w, int h)
{
	p_list_track->setFixedSize(w, h);
	p_list_track->move(x, y);
	p_list_track->show();
}

//处理高亮显示的路径
void DealBuildingMap::DealSelectedTrack(QPoint start, QPoint end)
{
	high_light_line.setPoints(start, end);
	high_light_flag = true;
	//building_map_mode = BUILDING_SHOW_HIGHLIGHT_TRACK;
	update();
}

//设置绘制轨迹的坐标数据到Cache中，等待绘制
void DealBuildingMap::DrawTrackDataToCache()
{
	auto it = user_run_time_campos_map.begin();
	while (it != user_run_time_campos_map.end())
	{
		int idx = FindExistCameraInfo(it.value().camera_ip, it.value().camera_id);
		if (idx == OPER_ERR)
		{
			qDebug() << "find camera_ip = " << it.value().camera_ip << "failed...";
			++it;
			continue;
		}
		it.value().position      = existed_camera_position[idx].position;
		it.value().position_name = existed_camera_position[idx].position_name;
		++it;
	}

	cache_draw_track_point.clear();
	cache_draw_track_endpoint.clear();
	auto iter = user_run_time_campos_map.begin();
	QPoint pbegin, pend;
	pbegin = iter.value().position;
	cache_draw_track_endpoint.push_back(pbegin);
	++iter;
	while (iter != user_run_time_campos_map.end())
	{
		pend = iter.value().position;
		if (pbegin != pend)
		{
			QPoint diff = (pend - pbegin) / POINT_DIV_NUMS;
			for (int i = 0; i < POINT_DIV_NUMS; i++)
			{
				cache_draw_track_point.push_back(pbegin + diff * i);
			}
			cache_draw_track_endpoint.push_back(pend);
		}
		pbegin = pend;
		++iter;
	}
	//PrintEndpoint();
}

//通过相机IP或者ID查找已存在相机的信息
int DealBuildingMap::FindExistCameraInfo(const QString & cam_ip, int cam_id)
{
	int size = existed_camera_position.size();
	for (int i = 0; i < size; i++)
	{
		if (cam_id <= 0 )  //通过ip查询相机
		{
			if (existed_camera_position[i].camera_ip.compare(cam_ip) == 0)
			{
				return i;
			}
		}
		else               //通过id查询相机
		{
			if (existed_camera_position[i].camera_id == cam_id)
			{
				return i;
			}
		}
	}
	return OPER_ERR;
}

void DealBuildingMap::PrintEndpoint()
{
	qDebug() << "begin print";
	auto it = cache_draw_track_endpoint.begin();
	while (it != cache_draw_track_endpoint.end())
	{
		qDebug() << "x:" << it->x() << "  y:" << it->y();
		++it;
	}
	qDebug() << "end print" << endl;
}

//启动绘制轨迹定时器
void DealBuildingMap::CtrlDrawRunTrackProcess(int time)
{
	n_ctrl_draw_track = 0;  //从头开始绘制
	if (p_ctrl_draw_track_timer->isActive() == false)
	{
		p_ctrl_draw_track_timer->start(time / POINT_DIV_NUMS);
	}
}

//清除已绘制的轨迹图
void DealBuildingMap::CleanDrawedRunTrack()
{
	building_map_mode = BUILDING_SHOW_MAP;
	cache_draw_track_point.clear();
	cache_draw_track_endpoint.clear();
	user_run_time_campos_map.clear();
	update();
	if (p_list_track != NULL)
	{
		p_list_track->close();
		delete p_list_track;
		p_list_track = NULL;
		high_light_flag = false;
	}
	update();

}

//点击事件，设置相机放置位置
void DealBuildingMap::mousePressEvent(QMouseEvent * ev)
{
	if (building_map_mode != BUILDING_CREATE_MAP)
	{
		return;
	}

	if (ev->button() == Qt::LeftButton)
	{
		cur_camera_position.setX(ev->x()*building_map_scaled_x);
		cur_camera_position.setY(ev->y()*building_map_scaled_y);
		update();
	}
}

//绘图事件
void DealBuildingMap::paintEvent(QPaintEvent * e)
{
	QLabel::paintEvent(e);

	QPainter painter(this);
	QPen pen;
	pen.setWidth(4);
	pen.setColor(QColor(255, 0, 0));
	painter.setPen(pen);
	if (building_map_mode == BUILDING_CREATE_MAP)
	{
		if (cur_camera_position != QPoint(0, 0))
		{
			int cenx = (cur_camera_position.x() / building_map_scaled_x - 0.5*CAMERA_SIZE_SCALED * width()) > 0 ? (cur_camera_position.x() / building_map_scaled_x - 0.5*CAMERA_SIZE_SCALED * width()) : 0;
			int ceny = (cur_camera_position.y() / building_map_scaled_y - 0.5*CAMERA_SIZE_SCALED * width()) > 0 ? (cur_camera_position.y() / building_map_scaled_y - 0.5*CAMERA_SIZE_SCALED * width()) : 0;
			painter.drawEllipse(cenx, ceny, CAMERA_SIZE_SCALED*width(), CAMERA_SIZE_SCALED*width());
			painter.drawPoint(cur_camera_position.x() / building_map_scaled_x, cur_camera_position.y() / building_map_scaled_y);
		}

		pen.setColor(QColor(0, 255, 0));
		painter.setPen(pen);
		if (!record_camera_position.isEmpty())
		{
			auto it = record_camera_position.begin();
			while (it != record_camera_position.end())
			{
				int cenx = (it->x() / building_map_scaled_x - 0.5*CAMERA_SIZE_SCALED * width()) > 0 ? (it->x() / building_map_scaled_x - 0.5*CAMERA_SIZE_SCALED * width()) : 0;
				int ceny = (it->y() / building_map_scaled_y - 0.5*CAMERA_SIZE_SCALED * width()) > 0 ? (it->y() / building_map_scaled_y - 0.5*CAMERA_SIZE_SCALED * width()) : 0;
				painter.drawEllipse(cenx, ceny, CAMERA_SIZE_SCALED*width(), CAMERA_SIZE_SCALED*width());
				painter.drawPoint(it->x() / building_map_scaled_x, it->y() / building_map_scaled_y);
				it++;
			}
		}
	}
	else if (building_map_mode == BUILDING_SHOW_MAP || building_map_mode == BUILDING_SHOW_TRACK || building_map_mode == BUILDING_SHOW_HIGHLIGHT_TRACK)
	{
		pen.setColor(QColor(0, 255, 0));
		painter.setPen(pen);
		QFont font;
		font.setFamily("Microsoft YaHei");
		font.setPointSize(16);
		painter.setFont(font);
		auto it = existed_camera_position.begin();
		while (it != existed_camera_position.end())
		{
			int cenx = (it->position.x() / building_map_scaled_x - 0.5*CAMERA_SIZE_SCALED * width()) > 0 ? (it->position.x() / building_map_scaled_x - 0.5*CAMERA_SIZE_SCALED * width()) : 0;
			int ceny = (it->position.y() / building_map_scaled_y - 0.5*CAMERA_SIZE_SCALED * width()) > 0 ? (it->position.y() / building_map_scaled_y - 0.5*CAMERA_SIZE_SCALED * width()) : 0;
			painter.drawEllipse(cenx, ceny, CAMERA_SIZE_SCALED*width(), CAMERA_SIZE_SCALED*width());
			painter.drawPoint(it->position.x() / building_map_scaled_x, it->position.y() / building_map_scaled_y);
			painter.drawText(cenx + CAMERA_SIZE_SCALED * width() + 5, ceny + CAMERA_SIZE_SCALED * width(), it->position_name);
			++it;
		}

		//绘制轨迹全过程图
		if (building_map_mode == BUILDING_SHOW_TRACK)
		{
			if (cache_draw_track_point.isEmpty())
			{
				return;
			}

			int i = 0;
			QPainterPath path;
			path.moveTo(cache_draw_track_point[i].x() / building_map_scaled_x, cache_draw_track_point[i].y() / building_map_scaled_y);
			if (n_ctrl_draw_track < cache_draw_track_point.size())
			{
				for (; i < n_ctrl_draw_track; i++)
				{
					path.lineTo(cache_draw_track_point[i].x() / building_map_scaled_x, cache_draw_track_point[i].y() / building_map_scaled_y);
				}
			}
			if (n_ctrl_draw_track == cache_draw_track_point.size())
			{
				if (p_ctrl_draw_track_timer->isActive() == true)
				{
					p_ctrl_draw_track_timer->stop();
				}

				for (int j = 1; j < cache_draw_track_endpoint.size(); j++)
				{
					path.lineTo(cache_draw_track_endpoint[j].x() / building_map_scaled_x, cache_draw_track_endpoint[j].y() / building_map_scaled_y);
				}
			}

			painter.drawPath(path);   // 绘制前面创建的path

		}

		//高亮显示某一段路径
		if (high_light_flag)
		{
			pen.setColor(QColor(255, 0, 0));
			pen.setWidth(5);
			painter.setPen(pen);
			QLine line(high_light_line.x1() / building_map_scaled_x, high_light_line.y1() / building_map_scaled_y,
				       high_light_line.x2() / building_map_scaled_x, high_light_line.y2() / building_map_scaled_y);
			painter.drawLine(line);

			const int array_size = 30;
			QLineF fline(line);
			QLineF dline = fline.unitVector();
			dline.setLength(array_size);
			
			QPoint pb = line.p1();
			QPoint pe = line.p2();
			QPointF pm = (pe - pb) / 2.0;

			dline.translate(pm);
			QLineF vline = dline.normalVector();
			vline.setLength(array_size*0.4);

			QLineF vline2 = vline.normalVector().normalVector();

			QPointF pt[3];
			pt[0] = dline.p2();
			pt[1] = vline.p2();
			pt[2] = vline2.p2();

			QBrush brush;
			brush.setColor(QColor(255, 0, 0));
			brush.setStyle(Qt::SolidPattern);
			painter.setBrush(brush);
			painter.drawPolygon(pt, 3);
		}

	}
}


