#pragma once

#include <string>
#include <vector>

class TSerialMessenger
{
private:
	std::string messageOut, messageIn, lastMessageOut, lastMessageIn;
	int cv1, cv2;
	int valCounter;
	std::string messageBuffer;

public:

	std::vector<std::string> log;

	void setMessageBuffer(std::string val) { messageBuffer = val; }
	void addMessageBuffer(std::string val) { messageBuffer += val; }
	

	std::string getMessageBuffer() { return messageBuffer; }

	TSerialMessenger() {
		messageIn = "";
		messageOut = "";
		valCounter = 0;
	};
	std::string getMessageIn()
	{
		std::string msg = messageIn;
		messageIn = "";
		return msg;
	}
	std::string getMessageOut()
	{
		std::string msg = messageOut;
		messageOut = "";
		return msg;
	}
	void setMessageIn(std::string val)
	{
		messageIn = val;
		lastMessageIn = messageIn;
		getLastMessages();
	}
	void setMessageOut(std::string val)
	{
		messageOut = val;
		lastMessageOut = messageOut;
		getLastMessages();
	}
	void setValues(int v1, int v2) { cv1 = v1; cv2 = v2; valCounter++; }
	void getValues(int &v1, int &v2) { v1=cv1; v2=cv2; }
	int getValCont() { return valCounter; }

	void writeToLog(std::string msg)
	{
		log.push_back(msg);
		if (log.size() > 20) log.erase(log.begin());
	}

	std::string getLastMessages()
	{
		std::string lml = "In: " + lastMessageIn + "; Out: " + lastMessageOut + ";\n";
		if (lastMessageIn != "" || lastMessageOut != "") writeToLog(lml);
		return lml;
	}

	void bufferMessage(std::string val)
	{
		std::string currentBuffer = getMessageBuffer();
		std::string newBuffer = val;

		std::string mergedBuffer = currentBuffer + newBuffer;

		//seek symbols S W D E
		int bl = mergedBuffer.length();
		bool hasS = false, hasW = false, hasD = false, hasE = false;
		int si = 0, wi = 0, di = 0, ei = 0;
		for (int i = 0; i < bl; i++)
		{
			if (mergedBuffer[i] == 'S') { hasS = true; si = i; }
			if (mergedBuffer[i] == 'W') { hasW = true; wi = i; }
			if (mergedBuffer[i] == 'D') { hasD = true; di = i; }
			if (mergedBuffer[i] == 'E') { hasE = true; ei = i; }
		}

		//buffer has full data
		std::string outBuffer = "";
		if (hasS && hasW && hasD && hasE)
		if (si<wi && wi<di & di<ei)
		{
			//we can form a real dataset
			for (int i = si; i <= ei; i++)
			{
				char cs = mergedBuffer[i];
				outBuffer += cs;
			}
		}
		if (outBuffer != "")
		{
			setMessageOut(outBuffer);
			setMessageBuffer("");
		}
		else
		{
			if (mergedBuffer.length()<20)
				setMessageBuffer(mergedBuffer);
			else
				setMessageBuffer("");
		}
	}
};

