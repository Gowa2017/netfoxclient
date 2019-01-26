#ifndef MACRO_HEAD_FILE
#define MACRO_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//常用常量

//无效数值
#define INVALID_BYTE				((BYTE)(0xFF))						//无效数值
#define INVALID_WORD				((WORD)(0xFFFF))					//无效数值
#define INVALID_DWORD				((DWORD)(0xFFFFFFFF))				//无效数值

//////////////////////////////////////////////////////////////////////////////////

//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//无效地址
#define INVALID_IP_ADDRESS(IPAddress) (((IPAddress==0L)||(IPAddress==INADDR_NONE)))

//////////////////////////////////////////////////////////////////////////////////

//存储长度
#ifdef _UNICODE
	#define CountStringBuffer CountStringBufferW
#else
	#define CountStringBuffer CountStringBufferA
#endif

//存储长度
#define CountStringBufferA(String) ((UINT)((lstrlenA(String)+1)*sizeof(CHAR)))
#define CountStringBufferW(String) ((UINT)((lstrlenW(String)+1)*sizeof(WCHAR)))

//////////////////////////////////////////////////////////////////////////////////

//接口释放
#define SafeRelease(pObject) { if (pObject!=NULL) { pObject->Release(); pObject=NULL; } }

//删除指针
#define SafeDelete(pData) { try { delete pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 

//关闭句柄
#define SafeCloseHandle(hHandle) { if (hHandle!=NULL) { CloseHandle(hHandle); hHandle=NULL; } }

//删除数组
#define SafeDeleteArray(pData) { try { delete [] pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 

//////////////////////////////////////////////////////////////////////////////////

//转换字符
inline VOID SwitchScoreFormat(LONGLONG lScore, UINT uSpace,LPCTSTR pszFormat,LPTSTR pszBuffer, WORD wBufferSize)
{
	//转换数值
	TCHAR szSwitchScore[32]=TEXT("");
	_sntprintf(szSwitchScore,CountArray(szSwitchScore),pszFormat,lScore);

	if(uSpace>0)
	{
		//变量定义
		WORD wTargetIndex=0;
		WORD wSourceIndex=0;
		WORD wSourceStringLen=0;

		//计算长度
		while (szSwitchScore[wSourceStringLen]!=0 && szSwitchScore[wSourceStringLen]!='.') 
		{
			++wSourceStringLen;
		}

		//转换字符
		for (INT i=0;i<wSourceStringLen;i++)
		{
			//拷贝字符
			pszBuffer[wTargetIndex++]=szSwitchScore[wSourceIndex++];

			//插入逗号
			if ((wSourceStringLen-wSourceIndex>0) && ((wSourceStringLen-wSourceIndex)%uSpace==0)) 
			{
				pszBuffer[wTargetIndex++]=TEXT(',');
			}			
		}

		//补充小数	
		while (szSwitchScore[wSourceIndex]!=0)
		{
			pszBuffer[wTargetIndex++] = szSwitchScore[wSourceIndex++];
		}

		//结束字符
		pszBuffer[wTargetIndex++]=0;
	}
	else
	{
		CopyMemory(pszBuffer,szSwitchScore,wBufferSize);
	}

	return;
}


//转换字符
inline VOID SwitchScoreFormat(double lScore, UINT uSpace,LPCTSTR pszFormat,LPTSTR pszBuffer, WORD wBufferSize)
{
	//转换数值
	TCHAR szSwitchScore[32]=TEXT("");
	_sntprintf(szSwitchScore,CountArray(szSwitchScore),pszFormat,lScore);

	if(uSpace>0)
	{
		//变量定义
		WORD wTargetIndex=0;
		WORD wSourceIndex=0;
		WORD wSourceStringLen=0;

		//计算长度
		while (szSwitchScore[wSourceStringLen]!=0 && szSwitchScore[wSourceStringLen]!='.') 
		{
			++wSourceStringLen;
		}

		//转换字符
		for (INT i=0;i<wSourceStringLen;i++)
		{
			//拷贝字符
			pszBuffer[wTargetIndex++]=szSwitchScore[wSourceIndex++];

			//插入逗号
			if ((wSourceStringLen-wSourceIndex>0) && ((wSourceStringLen-wSourceIndex)%uSpace==0)) 
			{
				pszBuffer[wTargetIndex++]=TEXT(',');
			}			
		}

		//补充小数	
		while (szSwitchScore[wSourceIndex]!=0)
		{
			pszBuffer[wTargetIndex++] = szSwitchScore[wSourceIndex++];
		}

		//结束字符
		pszBuffer[wTargetIndex++]=0;
	}
	else
	{
		CopyMemory(pszBuffer,szSwitchScore,wBufferSize);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

#endif