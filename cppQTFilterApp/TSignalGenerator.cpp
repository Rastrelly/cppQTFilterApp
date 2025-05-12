#include "TSignalGenerator.h"

void TSignalGenerator::progressTime(float deltaTime)
{
	float stepToUse = deltaTime;
	if (useFixedStep) stepToUse = stepValue;

    cTime += stepToUse;

	generateSignalPoint(stepToUse);
}

void TSignalGenerator::getDataPoint(int pos, float &x, float &y)
{
	if ((pos >= 0) && (pos < signalBufferVolume))
	{
		x = signal[pos].x;
		y = signal[pos].y;
	}
	else
	{
		x = 0;
		y = 0;
	}
}

void TSignalGenerator::appendDataPoint(dataPoint dp)
{
	if (signalPos < signalBufferVolume - 1)
	{
		signal[signalPos] = dp;
		signalPos++;
	}
	else
	{
		for (int i = 0; i < signalBufferVolume - 1; i++)
		{
			signal[i] = signal[i + 1];
		}
		signal[signalPos] = dp;
	}
}

void TSignalGenerator::getDataExtremes(float &xMin, float &xMax, float &yMin, float &yMax)
{
	float min, max;
	if (signal.size() > 0)
	{
		int l = signal.size();
		xMin = signal[0].x;
		xMax = signal[l - 1].x;
		min = signal[0].y;
		max = signal[0].y;
		for (int i = 1; i < l; i++)
		{
			if (signal[i].y < min) min = signal[i].y;
			if (signal[i].y > max) max = signal[i].y;
		}
		yMin = min;
		yMax = max;
	}
}

//generate point implementations
void TSigGenSinusoid::generateSignalPoint(float deltaTime)
{
	float cx = cTime;
	float cy = amplitude*sin(frequency * (cTime)*3.14f / 180.0f);

	dataPoint cSignal = { cx, cy };

	appendDataPoint(cSignal);
}

void TSigGenBinary::generateSignalPoint(float deltaTime)
{
	float cx = cTime;
	float cy = amplitude;

	float dt = deltaTime;
	if (deltaTime == 0) deltaTime = 0.000001;

	timeAccum += deltaTime;

	float timeCheck = 1.0f / frequency;

	if (timeAccum > timeCheck)
	{
		timeAccum = timeAccum - timeCheck;
		if (cSignalMultiplier > 0.5)
			cSignalMultiplier = 0;
		else
			cSignalMultiplier = 1;
	}

	dataPoint cSignal = { cx, cy*cSignalMultiplier };

	appendDataPoint(cSignal);

}


void TSigGenSaw::generateSignalPoint(float deltaTime)
{
	float cx = cTime;
	float cy = amplitude;

	timeAccum += deltaTime;

	float timeCheck = 1 / frequency;

	if (timeAccum > timeCheck)
	{
		timeAccum = timeAccum - timeCheck;
		if (cSignalMultiplier >= 0)
			cSignalMultiplier = -1;
		else
			cSignalMultiplier = 1;
	}

	float alpha = 1;
	if (cSignalMultiplier>0)
		alpha = timeAccum / timeCheck;
	else
		alpha = (timeCheck-timeAccum) / timeCheck;

	dataPoint cSignal = { cx, cy * alpha };

	appendDataPoint(cSignal);

}

TSignalGenerator * TSignalGenerator::Create(signalType genType, int bufferVol, bool fixStep, float genVal)
{
	switch (genType)
	{
	default:
	case ST_Sin:
	{
		TSigGenSinusoid * ns = new TSigGenSinusoid();
		ns->signalBufferVolume = bufferVol;
		ns->signalPos = 0;
		ns->signal.resize(bufferVol);
		ns->setUseFixedStep(fixStep);
		ns->setFixedStep(genVal);
		return ns;
	}
	break;
	case ST_Bin:
	{
		TSigGenBinary * ns = new TSigGenBinary();
		ns->signalBufferVolume = bufferVol;
		ns->signalPos = 0;
		ns->signal.resize(bufferVol);
		ns->setUseFixedStep(fixStep);
		ns->setFixedStep(genVal);
		ns->setTimeAccum(0.0f);
		ns->setCSignalMultiplier(0.0f);
		ns->setSigMode(false);
		return ns;
	}
	break;
	case ST_Saw:
	{
		TSigGenSaw * ns = new TSigGenSaw();
		ns->signalBufferVolume = bufferVol;
		ns->signalPos = 0;
		ns->signal.resize(bufferVol);
		ns->setUseFixedStep(fixStep);
		ns->setFixedStep(genVal);
		ns->setTimeAccum(0.0f);
		ns->setCSignalMultiplier(0.0f);
		ns->setSigMode(false);
		return ns;
	}
	break;
	}
}
