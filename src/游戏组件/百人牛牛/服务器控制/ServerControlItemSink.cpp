#include "StdAfx.h"
#include "servercontrolitemsink.h"
#include <locale>

CServerControlItemSink::CServerControlItemSink(void)
{
  m_cbExcuteTimes = 0;
  m_cbControlStyle = 0;
  m_cbWinAreaCount = 0;
  ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
  ZeroMemory(m_nCompareCard, sizeof(m_nCompareCard));
  ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));
  ZeroMemory(m_cbTableCard, sizeof(m_cbTableCard));
  ZeroMemory(m_lAllJettonScore, sizeof(m_lAllJettonScore));
  ZeroMemory(m_bBakWinArea, sizeof(m_bBakWinArea));
}

CServerControlItemSink::~CServerControlItemSink(void)
{

}

//服务器控制
bool __cdecl CServerControlItemSink::ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption)
{
  //如果不具有管理员权限 则返回错误
  if(!CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
  {
    return false;
  }

  const CMD_C_AdminReq* AdminReq=static_cast<const CMD_C_AdminReq*>(pDataBuffer);
  switch(AdminReq->cbReqType)
  {
    case RQ_RESET_CONTROL:
    {
      m_cbControlStyle=0;
      m_cbWinAreaCount=0;
      m_cbExcuteTimes=0;
      ZeroMemory(m_bWinArea,sizeof(m_bWinArea));


      CMD_S_CommandResult cResult;
      cResult.cbResult=CR_ACCEPT;
      cResult.cbAckType=ACK_RESET_CONTROL;
      pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

      //控制信息
      ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

      break;
    }
    case RQ_SET_WIN_AREA:
    {

      const tagAdminReq*pAdminReq=reinterpret_cast<const tagAdminReq*>(AdminReq->cbExtendData);
      switch(pAdminReq->m_cbControlStyle)
      {
        case CS_BET_AREA: //区域设置
        {
          m_cbControlStyle=pAdminReq->m_cbControlStyle;
          m_cbExcuteTimes=pAdminReq->m_cbExcuteTimes;
          m_cbWinAreaCount=0;
          BYTE cbIndex=0;
          for(cbIndex=0; cbIndex<AREA_COUNT; cbIndex++)
          {
            m_bWinArea[cbIndex]=pAdminReq->m_bWinArea[cbIndex];
            if(m_bWinArea[cbIndex])
            {
              m_cbWinAreaCount++;
            }
          }
          CMD_S_CommandResult cResult;
          cResult.cbResult=CR_ACCEPT;
          cResult.cbAckType=ACK_SET_WIN_AREA;
          pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

          //控制信息
          ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

          break;
        }
        case CS_BANKER_LOSE:  //庄家输牌
        {
          m_cbControlStyle=pAdminReq->m_cbControlStyle;
          m_cbExcuteTimes=pAdminReq->m_cbExcuteTimes;
          CMD_S_CommandResult cResult;
          cResult.cbResult=CR_ACCEPT;
          cResult.cbAckType=ACK_SET_WIN_AREA;
          pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

          //控制信息
          ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

          break;
        }
        case CS_BANKER_WIN:   //庄家赢牌
        {
          m_cbControlStyle=pAdminReq->m_cbControlStyle;
          m_cbExcuteTimes=pAdminReq->m_cbExcuteTimes;
          CMD_S_CommandResult cResult;
          cResult.cbResult=CR_ACCEPT;
          cResult.cbAckType=ACK_SET_WIN_AREA;
          pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

          //控制信息
          ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

          break;
        }
        default:  //拒绝请求
        {
          CMD_S_CommandResult cResult;
          cResult.cbResult=CR_REFUSAL;
          cResult.cbAckType=ACK_SET_WIN_AREA;
          pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

          //控制信息
          ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

          break;
        }
      }

      break;
    }
    case RQ_PRINT_SYN:
    {
      CMD_S_CommandResult cResult;
      cResult.cbResult=CR_ACCEPT;
      cResult.cbAckType=ACK_PRINT_SYN;
      tagAdminReq*pAdminReq=reinterpret_cast<tagAdminReq*>(cResult.cbExtendData);
      pAdminReq->m_cbControlStyle=m_cbControlStyle;
      pAdminReq->m_cbExcuteTimes=m_cbExcuteTimes;
      memcpy(pAdminReq->m_bWinArea,m_bWinArea,sizeof(m_bWinArea));
      pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

      //控制信息
      ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

      break;
    }
    default:
    {
      break;
    }
  }
  return true;
}

//需要控制
bool  CServerControlItemSink::NeedControl()
{
  if(m_cbControlStyle > 0 && m_cbExcuteTimes > 0)
  {
    return true;
  }

  return false;
}

//满足控制
bool  CServerControlItemSink::MeetControl(tagControlInfo ControlInfo)
{
  return true;
}

//完成控制
bool  CServerControlItemSink::CompleteControl()
{
  if(m_cbExcuteTimes > 0)
  {
    m_cbExcuteTimes--;
  }

  if(m_cbExcuteTimes == 0)
  {
    m_cbExcuteTimes = 0;
    m_cbControlStyle = 0;
    m_cbWinAreaCount = 0;
    ZeroMemory(m_bWinArea, sizeof(m_bWinArea));
    ZeroMemory(m_nCompareCard, sizeof(m_nCompareCard));
    ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));
    ZeroMemory(m_cbTableCard, sizeof(m_cbTableCard));
    ZeroMemory(m_lAllJettonScore, sizeof(m_lAllJettonScore));
  }

  return true;
}

//设置是否能成功
bool CServerControlItemSink::IsSettingSuccess(BYTE cbControlArea[MAX_INDEX])
{
  return true;
}

//返回控制区域
bool  CServerControlItemSink::ReturnControlArea(tagControlInfo& ControlInfo)
{
  return true;
}

//开始控制
void  CServerControlItemSink::GetControlArea(BYTE Area[], BYTE &cbControlStyle)
{
	for(BYTE cbIndex =0; cbIndex<AREA_COUNT; cbIndex++)
	{
		Area[cbIndex] = m_bWinArea[cbIndex];
	}
	cbControlStyle = m_cbControlStyle;
}
//控制牌型
void  CServerControlItemSink::GetSuitResult(BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], SCORE lAllJettonScore[])
{
  //填充变量
  memcpy(m_cbTableCardArray, cbTableCardArray, sizeof(m_cbTableCardArray));
  memcpy(m_cbTableCard, cbTableCard, sizeof(m_cbTableCard));
  memcpy(m_lAllJettonScore, lAllJettonScore, sizeof(m_lAllJettonScore));

  //排序扑克
  BYTE cbSuitStack[MAX_CARDGROUP] = {};

  //自动组合
  GetSuitCardCombine(cbSuitStack);

  //重新设置纸牌排列
  BYTE UserCard[MAX_CARDGROUP][MAX_CARD] = {};
  memcpy(UserCard,m_cbTableCardArray,sizeof(UserCard));
  BYTE cbIndex=0;
  for(cbIndex=0; cbIndex<MAX_CARDGROUP; cbIndex++)
  {
    memcpy(cbTableCardArray[cbIndex],UserCard[cbSuitStack[cbIndex]],sizeof(BYTE)*MAX_CARD);
  }
  memcpy(cbTableCard, m_cbTableCard, sizeof(m_cbTableCard));
}
//开始控制
BYTE  CServerControlItemSink::GetControlTybe()
{
	return m_cbControlStyle;
}
//开始控制
void  CServerControlItemSink::SetControlArea(BYTE Area[])
{
	memcpy(m_bBakWinArea,Area,sizeof(m_bBakWinArea));
}
//是否是有效组合
bool CServerControlItemSink::GetSuitCardCombine(BYTE cbStack[])
{
  switch(m_cbControlStyle)
  {
    case CS_BET_AREA:
    {
      AreaWinCard(cbStack);
      break;
    }
    case CS_BANKER_WIN:
    {
      BankerWinCard(true,cbStack);
      break;
    }
    case CS_BANKER_LOSE:
    {
      BankerWinCard(false,cbStack);
      break;
    }
  }

  return true;
}

bool CServerControlItemSink::AreaWinCard(BYTE cbStack[])
{
  bool bIsUser[MAX_CARDGROUP]= {0}; //是否已经使用
  BYTE cbStackCount=0;  //栈中元素数量
  BYTE cbIndex=0;
  BYTE cbMultiple=0;
  //比较牌之间关系
  for(cbIndex=0; cbIndex<MAX_CARDGROUP; cbIndex++)
  {
    for(BYTE j=cbIndex; j<MAX_CARDGROUP; j++)
    {

      m_nCompareCard[j][cbIndex]=m_GameLogic.CompareCard(m_cbTableCardArray[cbIndex],MAX_CARD,m_cbTableCardArray[j],MAX_CARD,cbMultiple);
      m_nCompareCard[j][cbIndex]*=cbMultiple;
      m_nCompareCard[cbIndex][j]=-m_nCompareCard[j][cbIndex];
    }
  }

  //查找派牌组合
  for(cbIndex=0; cbIndex<=MAX_CARDGROUP;)
  {
    if(cbIndex<MAX_CARDGROUP)
    {
      if(bIsUser[cbIndex])
      {
        cbIndex++;
      }
      else
      {
        cbStack[cbStackCount]=cbIndex;
        bIsUser[cbIndex]=true;
        cbStackCount++;
        if(cbStackCount==MAX_CARDGROUP) //如果已经找到一种组合
        {

          if(m_bWinArea[0]==(m_nCompareCard[cbStack[1]][cbStack[0]]>0?true:false)   \
             &&m_bWinArea[1]==(m_nCompareCard[cbStack[2]][cbStack[0]]>0?true:false)  \
             &&m_bWinArea[2]==(m_nCompareCard[cbStack[3]][cbStack[0]]>0?true:false)  \
             &&m_bWinArea[3]==(m_nCompareCard[cbStack[4]][cbStack[0]]>0?true:false))
          {
            break;
          }
          cbIndex=MAX_CARDGROUP;
        }
        else
        {
          cbIndex=0;
          continue;
        }
      }
    }
    else
    {
      if(cbStackCount>0)
      {
        cbIndex=cbStack[--cbStackCount];
        bIsUser[cbIndex]=false;
        cbIndex++;
        continue;
      }
      else
      {
        break;
      }
    }
  }
  return true;
}

SCORE CServerControlItemSink::GetBankerWinScore(int nWinMultiple[])
{
  //庄家总量
  SCORE lBankerWinScore = 0;

  for(BYTE cbIndex=0; cbIndex<AREA_COUNT; cbIndex++)
  {
    lBankerWinScore+=m_lAllJettonScore[cbIndex+1]*nWinMultiple[cbIndex];
  }
  return lBankerWinScore;
}

void CServerControlItemSink::BankerWinCard(bool bIsWin,BYTE cbStack[])
{
	BYTE bWinArea[CONTROL_AREA]= {false};
	bool bIsUser[MAX_CARDGROUP]= {0}; //是否已经使用
	BYTE cbStackCount=0;  //栈中元素数量
	BYTE cbIndex=0;
	BYTE cbMultiple=0;
	if (bIsWin == false)
	{
		for(BYTE Index=0; Index<AREA_COUNT; Index++)
		{
			if(m_lAllJettonScore[Index+1] > 0.001)
			{
				bWinArea[Index] = true;
			}
			else
			{
				if (rand()%100 < 50 )
				{
					bWinArea[Index] = true;
				}
				else
				{
					bWinArea[Index] = false;
				}
			}
		}
	}
	else
	{
		for(BYTE Index=0; Index<AREA_COUNT; Index++)
		{
			if(m_lAllJettonScore[Index+1] > 0.001)
			{
				bWinArea[Index] = false;
			}
			else
			{
				if (rand()%100 < 50 )
				{
					bWinArea[Index] = true;
				}
				else
				{
					bWinArea[Index] = false;
				}
			}
		}
	}

	//比较牌之间关系
	for(cbIndex=0; cbIndex<MAX_CARDGROUP; cbIndex++)
	{
		for(BYTE j=cbIndex; j<MAX_CARDGROUP; j++)
		{

			m_nCompareCard[j][cbIndex]=m_GameLogic.CompareCard(m_cbTableCardArray[cbIndex],MAX_CARD,m_cbTableCardArray[j],MAX_CARD,cbMultiple);
			m_nCompareCard[j][cbIndex]*=cbMultiple;
			m_nCompareCard[cbIndex][j]=-m_nCompareCard[j][cbIndex];
		}
	}

	//查找派牌组合
	for(cbIndex=0; cbIndex<=MAX_CARDGROUP;)
	{
		if(cbIndex<MAX_CARDGROUP)
		{
			if(bIsUser[cbIndex])
			{
				cbIndex++;
			}
			else
			{
				cbStack[cbStackCount]=cbIndex;
				bIsUser[cbIndex]=true;
				cbStackCount++;
				if(cbStackCount==MAX_CARDGROUP) //如果已经找到一种组合
				{


					if (bIsWin == true)
					{
						if(m_bBakWinArea[0]==(m_nCompareCard[cbStack[1]][cbStack[0]]>0?true:false)   \
							&&m_bBakWinArea[1]==(m_nCompareCard[cbStack[2]][cbStack[0]]>0?true:false)   \
							&&m_bBakWinArea[2]==(m_nCompareCard[cbStack[3]][cbStack[0]]>0?true:false)  \
							&&m_bBakWinArea[3]==(m_nCompareCard[cbStack[4]][cbStack[0]]>0?true:false)  )
						{
							break;
						}
					}
					else
					{	
						bool bOK = false;
						for ( int  i = 1 ;i<= 4;i++)
						{
							bool cbFirstCardType = m_GameLogic.GetCardType(m_cbTableCardArray[cbStack[i]], 5) > 11?true:false;
							if(m_lAllJettonScore[i] > 0.001)
							{
								if(m_bBakWinArea[i-1]==(m_nCompareCard[cbStack[i]][cbStack[0]]>0?true:false && cbFirstCardType == true))
								{
									bOK = true;
								}
								else
								{
									bOK = false;
								}
							}
							else
							{
								if(m_bBakWinArea[i-1]==(m_nCompareCard[cbStack[i]][cbStack[0]]>0?true:false))
								{
									bOK = true;
								}
								else
								{
									bOK = false;
								}
							}
							if (bOK == false)
							{
								break;
							}
						}
						if (bOK == true)
						{
							break;
						}

						//if(m_lAllJettonScore[2] > 0.001)
						//{
						//	if(m_bBakWinArea[0]==(m_nCompareCard[cbStack[1]][cbStack[0]]>0?true:false && cbFirstCardType == true))
						//	{
						//		bOK = true;
						//	}
						//	else
						//	{
						//		bOK = false;
						//	}
						//}
						//else
						//{
						//	if(m_bBakWinArea[0]==(m_nCompareCard[cbStack[1]][cbStack[0]]>0?true:false))
						//	{
						//		bOK = true;
						//	}
						//	else
						//	{
						//		bOK = false;
						//	}
						//}

						//bool cbFirstCardType = m_GameLogic.GetCardType(m_cbTableCardArray[cbStack[1]], 5) <= 10?true:false;
						//if(m_bBakWinArea[0]==(m_nCompareCard[cbStack[1]][cbStack[0]]>0?true:false && cbFirstCardType == true)   \
						//	&&m_bBakWinArea[1]==(m_nCompareCard[cbStack[2]][cbStack[0]]>0?true:false)   \
						//	&&m_bBakWinArea[2]==(m_nCompareCard[cbStack[3]][cbStack[0]]>0?true:false)  \
						//	&&m_bBakWinArea[3]==(m_nCompareCard[cbStack[4]][cbStack[0]]>0?true:false)  )
						//{
						//	break;
						//}
					}


					cbIndex=MAX_CARDGROUP;
				}
				else
				{
					cbIndex=0;
					continue;
				}
			}
		}
		else
		{
			if(cbStackCount>0)
			{
				cbIndex=cbStack[--cbStackCount];
				bIsUser[cbIndex]=false;
				cbIndex++;
				continue;
			}
			else
			{
				break;
			}
		}
	}
	//return true;


  //bool bIsUser[MAX_CARDGROUP]= {0}; //是否已经使用
  //BYTE cbStackCount=0;  //栈中元素数量
  //BYTE cbIndex=0;
  //BYTE cbMultiple;
  ////比较牌之间关系
  //for(cbIndex=0; cbIndex<MAX_CARDGROUP; cbIndex++)
  //{
  //  for(BYTE j=cbIndex; j<MAX_CARDGROUP; j++)
  //  {
  //    m_nCompareCard[j][cbIndex]=m_GameLogic.CompareCard(m_cbTableCardArray[cbIndex],MAX_CARD,m_cbTableCardArray[j],MAX_CARD,cbMultiple);
  //    m_nCompareCard[j][cbIndex]*=cbMultiple;
  //    m_nCompareCard[cbIndex][j]=-m_nCompareCard[j][cbIndex];
  //  }
  //}

  ////查找派牌组合
  //for(cbIndex=0; cbIndex<=MAX_CARDGROUP;)
  //{
  //  if(cbIndex<MAX_CARDGROUP)
  //  {
  //    if(bIsUser[cbIndex])
  //    {
  //      cbIndex++;
  //    }
  //    else
  //    {
  //      cbStack[cbStackCount]=cbIndex;
  //      bIsUser[cbIndex]=true;
  //      cbStackCount++;
  //      if(cbStackCount==MAX_CARDGROUP) //如果已经找到一种组合
  //      {
  //        int nWinMultiple[4];
  //        for(int x=0; x<4; x++)
  //        {
  //          nWinMultiple[x]=m_nCompareCard[cbStack[x+1]][cbStack[0]];
  //        }

  //        if(bIsWin&&GetBankerWinScore(nWinMultiple)<0)
  //        {
  //          break;
  //        }
  //        if(!bIsWin&&GetBankerWinScore(nWinMultiple)>=0)
  //        {
  //          break;
  //        }
  //        cbIndex=MAX_CARDGROUP;
  //      }
  //      else
  //      {
  //        cbIndex=0;
  //        continue;
  //      }
  //    }
  //  }
  //  else
  //  {
  //    if(cbStackCount>0)
  //    {
  //      cbIndex=cbStack[--cbStackCount];
  //      bIsUser[cbIndex]=false;
  //      cbIndex++;
  //      continue;
  //    }
  //    else
  //    {
  //      break;
  //    }
  //  }
  //}
}

//申请结果
VOID CServerControlItemSink::ControlInfo(const void * pBuffer, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption)
{
  const CMD_S_CommandResult*pResult=(CMD_S_CommandResult*)pBuffer;
  CString str;
  switch(pResult->cbAckType)
  {
    case ACK_SET_WIN_AREA:
    {
      if(pResult->cbResult==CR_ACCEPT)
      {
        str=TEXT("输赢控制命令已经接受！");
        switch(m_cbControlStyle)
        {
          case CS_BET_AREA:
          {
            str=TEXT("胜利区域:");
            BYTE cbIndex=0;
            bool bFlags=false;
            for(cbIndex=0; cbIndex<AREA_COUNT; cbIndex++)
            {
              if(m_bWinArea[cbIndex])
              {
                bFlags=true;
                switch(cbIndex)
                {
                  case 0:
                    str+=TEXT("东 ");
                    break;
                  case 1:
                    str+=TEXT("南 ");
                    break;
                  case 2:
                    str+=TEXT("西 ");
                    break;
                  case 3:
                    str+=TEXT("北 ");
                    break;
                  default:
                    break;
                }
              }
            }
            if(!bFlags)
            {
              str+=TEXT("庄家通杀");
            }
            str.AppendFormat(TEXT(",执行次数:%d"),m_cbExcuteTimes);
            break;
          }
          case CS_BANKER_WIN:
          {
            str.Format(TEXT("庄家受控,受控方式:赢,执行次数:%d"),m_cbExcuteTimes);
            break;
          }
          case CS_BANKER_LOSE:
          {
            str.Format(TEXT("庄家受控,受控方式:输,执行次数:%d"),m_cbExcuteTimes);
            break;
          }
          default:
            str=TEXT("非控制状态");
            break;
        }
      }
      else
      {
        str.Format(TEXT("执行控制失败!"));
      }
      break;
    }
    case ACK_RESET_CONTROL:
    {
      if(pResult->cbResult==CR_ACCEPT)
      {
        str=TEXT("取消控制!");
      }
      break;
    }
    case ACK_PRINT_SYN:
    {
      if(pResult->cbResult==CR_ACCEPT)
      {
        str=TEXT("服务器同步成功!");
      }
      else
      {
        str.Format(TEXT("服务器同步失败!"));
      }
      break;
    }

    default:
      break;
  }

  //记录信息
  CString strControlInfo;
  CTime Time(CTime::GetCurrentTime());
  strControlInfo.Format(TEXT("房间: %s | 桌号: %u | 时间: %d-%d-%d %d:%d:%d\n控制人账号: %s | 控制人ID: %u\n%s\r\n"),
                        pGameServiceOption->szServerName, pITableFrame->GetTableID()+1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
                        Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

  WriteInfo(TEXT("百人牛牛控制信息.log"),strControlInfo);

  return;
}

//记录信息
VOID CServerControlItemSink::WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString)
{
  //设置语言区域
  char* old_locale = _strdup(setlocale(LC_CTYPE,NULL));
  setlocale(LC_CTYPE, "chs");

  CStdioFile myFile;
  CString strFileName;
  strFileName.Format(TEXT("%s"), pszFileName);
  BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
  if(bOpen)
  {
    myFile.SeekToEnd();
    myFile.WriteString(pszString);
    myFile.Flush();
    myFile.Close();
  }

  //还原区域设定
  setlocale(LC_CTYPE, old_locale);
  free(old_locale);

  return;
}