#pragma	  once
#pragma pack(push)
#pragma pack(1)
//人工智能通用
class CAIUtil
{
public:

	//检查几率
	static bool DetectionProbability(BYTE r)
	{
		if((rand() % 100) < r)
			return true;
		else
			return false;
	}
};

#pragma pack(pop)