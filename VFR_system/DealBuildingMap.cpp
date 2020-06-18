#include "DealBuildingMap.h"

DealBuildingMap::DealBuildingMap(QWidget *parent)
	: QLabel(parent)
{
	record_camera_position.clear();
	cur_camera_position = QPoint(0, 0);
	building_map_scaled_x = 1;
	building_map_scaled_y = 1;
	building_map_mode = BUILDING_SHOW_MAP;
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
	update();
}

//清理已选中的地图
void DealBuildingMap::CleanSelectedBuildingMap()
{
	existed_camera_position.clear();
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
		//qDebug() << "-----------leftButton pressed" << "x:" << ev->x() << "y" << ev->y();
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
	else if (building_map_mode == BUILDING_SHOW_MAP)
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

	}
}


