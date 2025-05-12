#pragma once

#include <vector>
#include <ctime>

class TNoiseGen
{
private:
	std::vector<float> x, y;
	float noiseAmp;
	int cp, bv;
public:
	TNoiseGen(int bufferVol, float vNoiseAmp)
	{
		bv = bufferVol;
		srand(time(NULL));
		x.resize(bv);
		y.resize(bv);
		cp = -1;
		noiseAmp = vNoiseAmp;
	}

	void appendPoint(float vx, float vy)
	{
		cp++;
		if (cp > bv - 1)
		{
			x.erase(x.begin());
			x.push_back(vx);
			y.erase(y.begin());
			y.push_back(vy);
			cp = bv - 1;
		}
		else
		{
			x[cp] = vx;
			y[cp] = vy;
		}

		float LO = -1 * noiseAmp / 2.0f;
		float HI =      noiseAmp / 2.0f;

		float noiseVal = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
		y[cp] += noiseVal;
	}

	float getNoiseAmpl() { return noiseAmp; }
	void getLastPoint(float &px, float &py)
	{
		px = x[cp];
		py = y[cp];
	}

	void getVectors(std::vector<float> &ox, std::vector<float> &oy)
	{
		ox = x;
		oy = y;
	}
};

