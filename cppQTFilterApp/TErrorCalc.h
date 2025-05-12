#pragma once

#include <cmath>
#include <vector>

class TErrorCalc
{
private:
	float valAvg;
	float errAvgAbs, errAvgRel;
	float errLastAbs, errLastRel;
	float sqDev, sqDevMax;
	int buffVol;
	std::vector<float> vals;
public:
	TErrorCalc(int buffV) {
		errLastAbs = 0.0f;
		errLastRel = 0.0f;
		errAvgAbs = 0.0f;
		errAvgRel = 0.0f;
		buffVol = buffV;
		vals.resize(0);
		sqDev = 0;
	};
	void calcErrors(float val, float trueVal)
	{

		float tv = trueVal;
		if (abs(trueVal) == 0) tv = 0.0001;

		vals.push_back(val);
		if (vals.size() > buffVol) vals.erase(vals.begin());
		
		errLastAbs = val - trueVal;
		
		errLastRel = errLastAbs / trueVal;

		errAvgAbs += abs(errLastAbs); errAvgAbs *= 0.50f;
		errAvgRel += abs(errLastRel); errAvgRel *= 0.50f;

		calcSqDev();
	}

	void calcSqDev()
	{
		if (vals.size() > 1)
		{
			
			float sVals = 0.0f;
			float sSq = 0.0f;

			for (float vv : vals)
			{
				sVals += vv;
			}
			valAvg = sVals / (float)vals.size();

			for (int i = 0; i < vals.size(); i++)
			{
				sSq += pow(vals[i] - valAvg, 2);
			}
			sqDev = sqrt(sSq / ((float)vals.size()-1.0f));

			if (sqDev > sqDevMax) sqDevMax = sqDev;
		}
	}

	float getLastAbsErr() {
		return errLastAbs;
	}

	float getLastRelErr() {
		return errLastRel;
	}

	float getAvgAbsErr() {
		return errAvgAbs;
	}

	float getAvgRelErr() {
		return errAvgRel;
	}

	float getSqDev() {
		return sqDev;
	}

	float getSqDevNorm() {
		return (sqDev/sqDevMax)*100;
	}
};

