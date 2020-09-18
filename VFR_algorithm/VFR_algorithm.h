#pragma once

#include <QtWidgets/QWidget>
#include "ui_VFR_algorithm.h"

class VFR_algorithm : public QWidget
{
    Q_OBJECT

public:
    VFR_algorithm(QWidget *parent = Q_NULLPTR);

private:
    Ui::VFR_algorithmClass ui;
};
