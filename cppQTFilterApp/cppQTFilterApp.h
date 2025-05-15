#pragma once

#include <QtWidgets/QDialog>
#include "ui_cppQTFilterApp.h"
#include "TSignalGenerator.h"
#include "TNoiseGen.h"
#include <ourTimer.h>
#include "TKalmanFilter.h"
#include "TErrorCalc.h"
#include "TSerialHandler.h"
#include "TSerialMessenger.h"

class cppQTFilterApp : public QDialog
{
    Q_OBJECT

public:
	QLineSeries *seriesSrc, *seriesNoised, *seriesFiltered, *seriesAvgErr, *seriesAvgNz;
	QChart *chart;
	QValueAxis *axisX, *axisY, *axisPercY;
	QTimer *timer;

	in_timer *extTimer;

    cppQTFilterApp(QWidget *parent = nullptr);
    ~cppQTFilterApp();
	TSGClient * signalGen;
	TNoiseGen * NG;
	TKalmanFilter * kalmFilt;
	TErrorCalc * errCalc;
	TErrorCalc * errCalcNoise;
	TSerialHandler * serialHandler;
	TSerialMessenger * serialMessenger;

	bool iterDone;
	bool useCOM;
	int comPortNo;


private:
    Ui::cppQTFilterAppClass ui;
	bool firstLaunch;

private slots:
	void btnSetupGeneratorClick();
	void tmrTimerTime();
};
