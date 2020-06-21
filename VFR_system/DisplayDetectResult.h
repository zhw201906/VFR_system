#pragma once

#include <QWidget>
#include <QString>
#include "ui_DisplayDetectResult.h"
#include "FreeAdjustImageLabel.h"


class DisplayDetectResult : public QWidget
{
    Q_OBJECT

public:
    DisplayDetectResult(QString &path, QWidget *parent = Q_NULLPTR);
    ~DisplayDetectResult();

    void Exit();
    void AdaptScreenDisplay();

protected:
    virtual void resizeEvent(QResizeEvent *event);

private:
    Ui::DisplayDetectResult ui;
    QString  image_path;
};
