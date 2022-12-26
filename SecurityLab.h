#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SecurityLab.h"
#include "SecurityLabModel.h"

class SecurityLab : public QMainWindow
{
	Q_OBJECT

public:
	SecurityLab(SecurityLabModel* ip_model, QWidget* parent = nullptr);
	~SecurityLab();
	void resetPage();

private: 
	void _InitConnections();
private:
	Ui::SecurityLabClass ui;
	SecurityLabModel* mp_model = nullptr;
};
