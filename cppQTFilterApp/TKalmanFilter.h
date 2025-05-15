#pragma once

#include <vector>


class TKalmanFilter
{
private:
	std::vector<float> x;
	std::vector<float> y;

	float errEst;
	float lastEst;

	float aDX;
	int bufferVol;

	float calcDAvg(std::vector<float> sv)
	{
		float sm = 0;
		if (sv.size() > 1)
		{
			for (float v : sv)
			{
				sm += v;
			}
			return sm / sv.size();
		}
		else return 0.0f;
	}

	float simpleKalman(float newVal, float errMeas, float valChSpeed) {
		float errMeasure = errMeas;
		float Q = valChSpeed;
		float Kk, currEst;

		errEst = errMeasure;

		Kk = (float)errEst / (errEst + errMeasure);
		currEst = lastEst + (float)Kk * (newVal - lastEst);
		errEst = (1.0 - Kk) * errEst + fabs(lastEst - currEst) * Q;
		lastEst = currEst;
		return currEst;
	}

	float findYErrApprox(std::vector<float> dta)
	{
		float avgDy = 0;
		int nOps = 0;
		int l = dta.size();
		if (l > 2)
		{
			for (int i = 0; i < l - 1; i++)
			{
				float cdy = abs(dta[i] - dta[i + 1]);
				avgDy += cdy;
				nOps++;
			}
		}
		else
		{
			return 1;
		}
		return avgDy / nOps;
	}

public:

	std::vector<float> filteredX;
	std::vector<float> filteredY;		

	TKalmanFilter(int buffV)
	{
		bufferVol = buffV;
	}

	void runFilterOnDataVector()
	{
		float err = findYErrApprox(y);
		filteredX.resize(x.size());
		filteredY.resize(y.size());

		for (int i = 0; i < x.size(); i++)
		{
			filteredX[i] = x[i];
			filteredY[i] = simpleKalman(y[i], err, aDX);
		}

	}

	void appendPoint(float px, float py)
	{
		x.push_back(px);
		y.push_back(py);
		if (x.size() > bufferVol)
		{
			x.erase(x.begin());
			y.erase(y.begin());
		}
		aDX += px; aDX /= 2.0f;
	}

	//simply doubles the last recorded point
	void tweenPoint()
	{
		float px = 0;
		float py = 0;
		if (x.size() > 0 && y.size() > 0)
		{
			px = x[x.size() - 1];
			py = y[y.size() - 1];
		}
		appendPoint(px, py);
	}

	void runFilter(float &fx, float &fy)
	{
		float yErr = findYErrApprox(y);
		float xFil = x[x.size() - 1];
		float yFil = simpleKalman(y[y.size()-1],yErr,aDX);
		fx = xFil; fy = yFil;
	}


};

