#pragma	  once
#pragma pack(push)
#pragma pack(1)
//�˹�����ͨ��
class CAIUtil
{
public:

	//��鼸��
	static bool DetectionProbability(BYTE r)
	{
		if((rand() % 100) < r)
			return true;
		else
			return false;
	}
};

#pragma pack(pop)