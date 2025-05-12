#pragma once
#include <vector>

enum signalType
{
	ST_Sin, ST_Bin, ST_Saw
};

struct dataPoint { float x, y; };

class TSignalGenerator
{
protected:
	//fields
	int signalBufferVolume;
	std::vector<dataPoint> signal;
	float cTime;
	int signalPos;
	float frequency, amplitude;
	bool useFixedStep;
	float stepValue;	
public:
	//add a point
	virtual void generateSignalPoint(float delatTime) = 0;

	static TSignalGenerator * Create(signalType genType, int bufferVol, bool fixStep, float genVal);

	//apends a point to list
	void appendDataPoint(dataPoint dp);

	//progress time and add a point
	void progressTime(float deltaTime);	

	//return signal point
	void getDataPoint(int pos, float &x, float &y);

	//return buffer volume
	int getBufferVolume() { return signalBufferVolume; };

	//return data size
	int getDataSize() { return signal.size(); }

	//get vector ranges
	void getDataExtremes(float &xMin, float &xMax, float &yMin, float &yMax);
	
	//setup generator
	void setFrequency(float val) { frequency = val; };
	void setAmplitude(float val) { amplitude = val; };
	void setUseFixedStep(bool val) { useFixedStep = val; }
	void setFixedStep(float val) { stepValue = val; }

};

class TSigGenSinusoid : public TSignalGenerator
{
public:
	void generateSignalPoint(float deltaTime);
};


class TSigGenBinary : public TSignalGenerator
{
private:
	float timeAccum;
	bool SigMode;
	float cSignalMultiplier;
public:
	void setSigMode(bool val) { SigMode = val; }
	void setTimeAccum(float val) { timeAccum = val; }
	void setCSignalMultiplier(float val) { cSignalMultiplier = val; }
	void generateSignalPoint(float deltaTime);
};


class TSigGenSaw : public TSignalGenerator
{
private:
	float timeAccum;
	bool SigMode;
	float cSignalMultiplier;
public:
	void setSigMode(bool val) { SigMode = val; }
	void setTimeAccum(float val) { timeAccum = val; }
	void setCSignalMultiplier(float val) { cSignalMultiplier = val; }
	void generateSignalPoint(float deltaTime);
};

class TSGClient
{
private:
	TSignalGenerator * SG;
public:
	TSGClient(signalType ST, int BV, bool fixedStep, float fixedStepVal)
	{
		SG = TSignalGenerator::Create(ST, BV, fixedStep, fixedStepVal);
	}

	~TSGClient()
	{
		if (SG)
		{
			delete[] SG;
			SG = NULL;
		}
	}

	TSignalGenerator * getGenerator() { return SG; }
};