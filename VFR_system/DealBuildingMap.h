#pragma once

#include <QLabel>
#include <QDebug>
#include <QMouseEvent>
#include <QVector>
#include <QPoint>
#include <QPainter>
#include <QPen>

#include "json.h"

//当前label模式枚举值
enum MAP_MODE{ BUILDING_CREATE_MAP, BUILDING_SHOW_MAP, BUILDING_SHOW_TRACK };

#define  CAMERA_POSITION_SIZE   20
#define  CAMERA_POSITION_LSIZE  4
#define  CAMERA_SIZE_SCALED    0.02

#define  BODY_CAMERA_INFO      "camera_info"
#define  BODY_CAMERA_ID        "camera_id"
#define  BODY_CAMERA_IP        "camera_ip"
#define  BODY_CAMERA_PX        "camera_position_x"
#define  BODY_CAMERA_PY        "camera_position_y"
#define  BODY_CAMERA_NAME      "camera_name"
#define  BODY_CAMERA_NUM       "camera_num"
#define  BODY_POSITION_NAME    "position_name"

 
enum  ERROR_CODE {
	error_camera_position_info = 1001,
	error_camera_position_json = 1002
};

typedef struct CameraPositionInfo_ {
	int     camera_id;
	QString camera_ip;
	QString position_name;
	QPoint  position;
}CameraPositionInfo;

class DealBuildingMap : public QLabel
{
	Q_OBJECT

public:
	DealBuildingMap(QWidget *parent);
	~DealBuildingMap();

	void SetScaledParam(float x=1, float y=1);
	void SetOperatorMode(MAP_MODE mode);
	void RecodeCurrentCameraPoint();
	void CalcelOneRecodePoint();

	void DealSelectedBuildingMap(QString &info);
	void CleanPlaceingNewBuinldingMap();
	void CleanSelectedBuildingMap();

protected:
	virtual void mousePressEvent(QMouseEvent *ev);
	virtual void paintEvent(QPaintEvent *e);

signals:
	void resizeLabel();
	void CurrentPlaceCameraPoint(QPoint pt);
	void CalcelOnePlacedCamera();
	void SendShowErrorMsg(int error);

private:
	QVector<QPoint> record_camera_position;
	QPoint          cur_camera_position;
	int             building_map_mode;

	float   building_map_scaled_x;
	float   building_map_scaled_y;

	QVector<CameraPositionInfo>  existed_camera_position;

};
