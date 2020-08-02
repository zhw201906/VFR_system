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

	connect(p_ctrl_draw_track_timer, &QTimer::timeout, this, &DealBuildingMap::DealRunTrackProcess);
}

DealBuildingMap::~DealBuildingMap()
{

}

//������ʾ����ϵ��
void DealBuildingMap::SetScaledParam(float x, float y)
{
	building_map_scaled_x = x;
	building_map_scaled_y = y;
}

//���ò���ģʽ���½���ͼ�������ͼ�����ƹ켣
void DealBuildingMap::SetOperatorMode(MAP_MODE mode)
{
	building_map_mode = mode;
}

//�洢��ǰ����ֵ
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
	record_camera_position.push_back(cur_camera_position);   //��¼ͼƬ����λ��
	cur_camera_position = QPoint(0, 0);
	update();

	emit CurrentPlaceCameraPoint(pt_temp);
}

//���������õ����λ��
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

//��ȡץ��ʶ����û����ݣ�������ʼ��ͼ
void DealBuildingMap::SetDrawTrackData(MAP_MODE mode, const QString & data)
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
	int snap_size = snap_json[BODY_RECG_INFO].size();
	for (int i = 0; i < snap_size; i++)
	{
		CameraPositionInfo cam_info;
		cam_info.camera_id = snap_json[BODY_RECG_INFO][i][BODY_RECG_CAMID].asInt();		
		std::string stime_stamp = snap_json[BODY_RECG_INFO][i][BODY_RECG_TIME].asString();
		long long time_ = StringToInt64(stime_stamp);
		QString  time_stamp = QDateTime::fromMSecsSinceEpoch(time_).toString("yyyy-MM-dd hh:mm:ss.zzz");
		user_run_time_campos_map[time_stamp] = cam_info;
	}
	DrawTrackDataToCache();
	CtrlDrawRunTrackProcess(1000);
}

//����ѡ�еĽ�����ͼ
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

//�������ڴ����ĵ�ͼ
void DealBuildingMap::CleanPlaceingNewBuinldingMap()
{
	record_camera_position.clear();
	cur_camera_position = QPoint(0, 0);
	update();
}

//������ѡ�еĵ�ͼ
void DealBuildingMap::CleanSelectedBuildingMap()
{
	existed_camera_position.clear();
	update();
}

//���ƻ������й켣����
void DealBuildingMap::DealRunTrackProcess()
{
	n_ctrl_draw_track++;
	update();
}

//���û��ƹ켣���������ݵ�Cache�У��ȴ�����
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
			cache_draw_track_endpoint.push_back(pbegin);
		}
		pbegin = pend;
		++iter;
	}
}

//ͨ�����IP����ID�����Ѵ����������Ϣ
int DealBuildingMap::FindExistCameraInfo(const QString & cam_ip, int cam_id)
{
	int size = existed_camera_position.size();
	for (int i = 0; i < size; i++)
	{
		if (cam_id == -1)  //ͨ��ip��ѯ���
		{
			if (existed_camera_position[i].camera_ip.compare(cam_ip) == 0)
			{
				return i;
			}
		}
		else               //ͨ��id��ѯ���
		{
			if (existed_camera_position[i].camera_id == cam_id)
			{
				return i;
			}
		}
	}
	return OPER_ERR;
}

void DealBuildingMap::CtrlDrawRunTrackProcess(int time)
{
	if (p_ctrl_draw_track_timer->isActive() == false)
	{
		p_ctrl_draw_track_timer->start(time / POINT_DIV_NUMS);
	}
}

//����¼��������������λ��
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

//��ͼ�¼�
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
	else if (building_map_mode == BUILDING_SHOW_MAP || building_map_mode == BUILDING_SHOW_TRACK)
	{
		pen.setColor(QColor(0, 255, 0));
		painter.setPen(pen);
		auto it = existed_camera_position.begin();
		while (it != existed_camera_position.end())
		{
			int cenx = (it->position.x() / building_map_scaled_x - 0.5*CAMERA_SIZE_SCALED * width()) > 0 ? (it->position.x() / building_map_scaled_x - 0.5*CAMERA_SIZE_SCALED * width()) : 0;
			int ceny = (it->position.y() / building_map_scaled_y - 0.5*CAMERA_SIZE_SCALED * width()) > 0 ? (it->position.y() / building_map_scaled_y - 0.5*CAMERA_SIZE_SCALED * width()) : 0;
			painter.drawEllipse(cenx, ceny, CAMERA_SIZE_SCALED*width(), CAMERA_SIZE_SCALED*width());
			painter.drawPoint(it->position.x() / building_map_scaled_x, it->position.y() / building_map_scaled_y);		
			++it;
		}

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
				//n_ctrl_draw_track = 0;
				if (p_ctrl_draw_track_timer->isActive() == true)
				{
					p_ctrl_draw_track_timer->stop();
				}

				for (int j = 1; j < cache_draw_track_endpoint.size(); j++)
				{
					path.lineTo(cache_draw_track_endpoint[j].x() / building_map_scaled_x, cache_draw_track_endpoint[j].y() / building_map_scaled_y);
				}
			}
			//auto it = user_run_time_campos_map.begin();
			//path.moveTo(it.value().position.x() / building_map_scaled_x, it.value().position.y() / building_map_scaled_y);
			//it++;
			//while (it != user_run_time_campos_map.end())
			//{
			//	path.lineTo(it.value().position.x() / building_map_scaled_x, it.value().position.y() / building_map_scaled_y);
			//	++it;
			//}

			painter.drawPath(path);   // ����ǰ�洴����path

		}

	}
}


