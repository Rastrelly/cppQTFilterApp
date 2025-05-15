#pragma once
#include <ceserial.h>
#include <string>

class TSerialHandler
{
private:
	bool portState;
	ceSerial * com;
public:
	TSerialHandler(int portNo, int baud, bool &success)
	{
		std::string port = "\\\\.\\COM" + std::to_string(portNo);
		com = new ceSerial(port, baud, 8, 'N', 1);
		if (com->Open() == 0) {			portState = true;		}		else {			portState = false;		}
		success = portState;
	}

	void writeToPort(std::string sym)
	{
		if (sym=="1")
			com->WriteChar('1');
		if (sym == "2")
			com->WriteChar('2');
		if (sym == "3")
			com->WriteChar('3');
	}

	std::string readFromPort()
	{
		int dataSize = com->GetDataSize();
		std::string portData="";
		if (dataSize > 0)
		for (int i=0;i<dataSize;i++)
		{
			bool charRead = true;
			char cc = com->ReadChar(charRead);
			if (charRead) portData += cc;
		}
		return portData;
	}
};

