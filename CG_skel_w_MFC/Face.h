#pragma once

struct Face
{
	int v[4];
	int vn[4];
	int vt[4];

	Face()
	{
		for (int i=0; i<4; i++)
			v[i] = vn[i] = vt[i] = -1;

	}

	Face(std::istream & aStream)
	{
		for (int i=0; i<4; i++)
			v[i] = vn[i] = vt[i] = -1;


		char c;
		for(int i = 0; i < 4; i++)
		{
			aStream >> std::ws >> v[i] >> std::ws;
			if (aStream.peek() != '/')
				continue;
			aStream >> c >> std::ws;
			if (aStream.peek() == '/')
			{
				aStream >> c >> std::ws >> vn[i];
				continue;
			}
			else
				aStream >> vt[i];
			if (aStream.peek() != '/')
				continue;
			aStream >> c >> vn[i];
		}
	}

};

