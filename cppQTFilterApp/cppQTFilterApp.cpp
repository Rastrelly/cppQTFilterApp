#include "cppQTFilterApp.h"

//setup UI, init stuff
cppQTFilterApp::cppQTFilterApp(QWidget *parent)
    : QDialog(parent)
{
	iterDone = true;
	
	//default ui setup
	ui.setupUi(this);
	firstLaunch = true;
	signalGen = NULL;

	//setup button clicks
	connect(ui.btnApplyGeneratorSettings, SIGNAL(clicked()), SLOT(btnSetupGeneratorClick()));

	//add external timer
	extTimer = new in_timer();

	//run QT static timer
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()),SLOT(tmrTimerTime()));
	timer->start(0);

	//setup chart
	//create chart elements
	seriesSrc = new QtCharts::QLineSeries();
	seriesNoised = new QtCharts::QLineSeries();
	seriesFiltered = new QtCharts::QLineSeries();
	seriesAvgErr = new QtCharts::QLineSeries();
	seriesAvgNz = new QtCharts::QLineSeries();
	chart = new QChart();
	axisX = new QValueAxis();
	axisY = new QValueAxis();
	axisPercY = new QValueAxis();
	axisPercY->setRange(-100.0f, 100.0f);

	//SERIES	
	seriesSrc->setName("Source Signal");
	seriesSrc->setColor(QColor(255,0,0));

	seriesNoised->setName("Noised Signal");
	seriesNoised->setColor(QColor(0, 255, 0));

	seriesFiltered->setName("Filtered Signal");
	seriesFiltered->setColor(QColor(0, 0, 255));

	seriesAvgErr->setName("Filtered / Noise");
	seriesAvgErr->setColor(QColor(255, 0, 128));

	seriesAvgNz->setName("Rel Err Noised %");
	seriesAvgNz->setColor(QColor(128, 0, 255));


	//CHART OBJECT	
	chart->addSeries(seriesSrc);
	chart->addSeries(seriesNoised);
	chart->addSeries(seriesFiltered);
	chart->addSeries(seriesAvgErr);
	chart->addSeries(seriesAvgNz);

	chart->setTitle("Signal Output");
	chart->legend()->setVisible(true);
	chart->legend()->setAlignment(Qt::AlignRight);

	//AXII
	//X
	chart->addAxis(axisX, Qt::AlignBottom);
	//Y	
	chart->addAxis(axisY, Qt::AlignLeft);
	//%
	chart->addAxis(axisPercY, Qt::AlignLeft);
	axisPercY->setRange(0, 4);

	//Attach axii to series
	seriesSrc->attachAxis(axisX);
	seriesSrc->attachAxis(axisY);
	seriesNoised->attachAxis(axisX);
	seriesNoised->attachAxis(axisY);
	seriesFiltered->attachAxis(axisX);
	seriesFiltered->attachAxis(axisY);
	seriesAvgErr->attachAxis(axisX);
	seriesAvgErr->attachAxis(axisPercY);
	seriesAvgNz->attachAxis(axisX);
	seriesAvgNz->attachAxis(axisPercY);

	//assign chart to chart shower
	ui.gvChart->setChart(chart);

}

//run when closing
cppQTFilterApp::~cppQTFilterApp()
{}


//even when changing generator settings
void cppQTFilterApp::btnSetupGeneratorClick()
{
	timer->stop();

	if (!firstLaunch)
	if (signalGen)
	{
		delete[] signalGen;
		delete[] NG;
		delete[] kalmFilt;
		delete[] errCalc;
		delete[] errCalcNoise;
	}

	int sgt = ui.cbSigGenType->currentIndex();
	signalType sigType = ST_Sin;

	switch (sgt)
	{
		case 0: sigType = ST_Sin; break;
		case 1: sigType = ST_Bin; break;
		case 2: sigType = ST_Saw; break;
	}

	QString buffWidthStr = ui.leBufferVol->text();
	int buffWidth = buffWidthStr.toInt();

	float ampl = 0;
	float freq = 0;

	QString amplStr = ui.leAmplitude->text();
	QString freqStr = ui.leFrequency->text();

	ampl = amplStr.toFloat();
	freq = freqStr.toFloat();

	bool useFixed = ui.chFixedStep->checkState();

	QString fixedStepValStr = ui.leGenStepVal->text();
	float fixedStep = fixedStepValStr.toFloat();

	signalGen = new TSGClient(sigType, buffWidth, useFixed, fixedStep);
	signalGen->getGenerator()->setAmplitude(ampl);
	signalGen->getGenerator()->setFrequency(freq);

	//init nose gen
	float noiseAmpl = 0;
	QString noiseAmplStr = ui.leNoiseAmplitude->text();
	noiseAmpl = noiseAmplStr.toFloat();
	NG = new TNoiseGen(buffWidth, noiseAmpl);

	//init filter
	kalmFilt = new TKalmanFilter(buffWidth);

	//init error calculator
	errCalc = new TErrorCalc(buffWidth);
	errCalcNoise = new TErrorCalc(buffWidth);

	//prep series
	seriesSrc->clear();
	seriesNoised->clear();
	seriesFiltered->clear();
	seriesAvgErr->clear();
	seriesAvgNz->clear();

	//run timer
	timer->start(0);
}

//event for QT timer to force generator and filter to move
void cppQTFilterApp::tmrTimerTime()
{
	//get dt
	float dt = extTimer->getdeltatime();

	//work only if signal generator actually exists
	if (signalGen != NULL)
	{
		//execute signal gen op
		signalGen->getGenerator()->progressTime(dt);
		// update data axii
		float xmin = 0, xmax = 0, ymin = 0, ymax = 0;
		signalGen->getGenerator()->getDataExtremes(xmin, xmax, ymin, ymax);
		
		//update axii ranges
		float NoiseAmpl = NG->getNoiseAmpl();
		float yBot = ymin - .50f*NoiseAmpl;
		float yTop = ymax + .50f*NoiseAmpl;
			
		if (yBot == INFINITE) yBot = -100; 
		if (yTop == INFINITE) yTop = 100;

		axisX->setRange(xmin, xmax);
		axisY->setRange(yBot, yTop);
		

		//update chart contents
		//original signal
		int dataL = signalGen->getGenerator()->getDataSize();
		int maxDataL = signalGen->getGenerator()->getBufferVolume();
		float lastPointX = 0;
		float lastPointY = 0;
		signalGen->getGenerator()->getDataPoint(dataL - 1, lastPointX, lastPointY);
		seriesSrc->append(QPointF(lastPointX, lastPointY));
		//noise data
		NG->appendPoint(lastPointX, lastPointY);
		float lastNPointX = 0;
		float lastNPointY = 0;
		if (ui.chMakeNoise->checkState())
			NG->getLastPoint(lastNPointX, lastNPointY);
		else
		{
			lastNPointX = lastPointX;
			lastNPointY = lastPointY;
		}
		seriesNoised->append(QPointF(lastNPointX, lastNPointY));

		//filter point
		float lastPointFX = 0, lastPointFY = 0;
		kalmFilt->appendPoint(lastNPointX, lastNPointY);
		kalmFilt->runFilter(lastPointFX, lastPointFY);
		seriesFiltered->append(QPointF(lastPointFX, lastPointFY));

		//calc errors
		errCalc->calcErrors(lastPointFY, lastPointY);
		errCalcNoise->calcErrors(lastNPointY, lastPointY);

		//output errors
		ui.leAbsErrLast->setText(QString::number(errCalc->getLastAbsErr()));
		ui.leRelErrLast->setText(QString::number(errCalc->getLastRelErr() * 100));
		ui.leAbsErrAvg->setText(QString::number(errCalc->getAvgAbsErr()));
		ui.leRelErrAvg->setText(QString::number(errCalc->getAvgRelErr() * 100));
		ui.leAvgSqDev->setText(QString::number(errCalc->getSqDev()));
		
		seriesAvgErr->append(QPointF(lastPointX, errCalc->getAvgAbsErr() / errCalcNoise->getAvgAbsErr()));
		//seriesAvgNz->append(QPointF(lastPointX, errCalcNoise->getLastAbsErr()));

		//clean up charts
		int sL = seriesSrc->points().size();
		if (sL > maxDataL)
		{
			seriesSrc->removePoints(0, 1);
		}
		sL = seriesNoised->points().size();
		if (sL > maxDataL)
		{
			seriesNoised->removePoints(0, 1);
		}
		sL = seriesFiltered->points().size();
		if (sL > maxDataL)
		{
			seriesFiltered->removePoints(0, 1);
		}
		sL = seriesAvgErr->points().size();
		if (sL > maxDataL)
		{
			seriesAvgErr->removePoints(0, 1);
		}
		sL = seriesAvgNz->points().size();
		if (sL > maxDataL)
		{
			seriesAvgNz->removePoints(0, 1);
		}

	}
}