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

//����������
bool __cdecl CServerControlItemSink::ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption)
{
  //��������й���ԱȨ�� �򷵻ش���
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

      //������Ϣ
      ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

      break;
    }
    case RQ_SET_WIN_AREA:
    {

      const tagAdminReq*pAdminReq=reinterpret_cast<const tagAdminReq*>(AdminReq->cbExtendData);
      switch(pAdminReq->m_cbControlStyle)
      {
        case CS_BET_AREA: //��������
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

          //������Ϣ
          ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

          break;
        }
        case CS_BANKER_LOSE:  //ׯ������
        {
          m_cbControlStyle=pAdminReq->m_cbControlStyle;
          m_cbExcuteTimes=pAdminReq->m_cbExcuteTimes;
          CMD_S_CommandResult cResult;
          cResult.cbResult=CR_ACCEPT;
          cResult.cbAckType=ACK_SET_WIN_AREA;
          pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

          //������Ϣ
          ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

          break;
        }
        case CS_BANKER_WIN:   //ׯ��Ӯ��
        {
          m_cbControlStyle=pAdminReq->m_cbControlStyle;
          m_cbExcuteTimes=pAdminReq->m_cbExcuteTimes;
          CMD_S_CommandResult cResult;
          cResult.cbResult=CR_ACCEPT;
          cResult.cbAckType=ACK_SET_WIN_AREA;
          pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

          //������Ϣ
          ControlInfo(&cResult, pIServerUserItem, pITableFrame, pGameServiceOption);

          break;
        }
        default:  //�ܾ�����
        {
          CMD_S_CommandResult cResult;
          cResult.cbResult=CR_REFUSAL;
          cResult.cbAckType=ACK_SET_WIN_AREA;
          pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

          //������Ϣ
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

      //������Ϣ
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

//��Ҫ����
bool  CServerControlItemSink::NeedControl()
{
  if(m_cbControlStyle > 0 && m_cbExcuteTimes > 0)
  {
    return true;
  }

  return false;
}

//�������
bool  CServerControlItemSink::MeetControl(tagControlInfo ControlInfo)
{
  return true;
}

//��ɿ���
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

//�����Ƿ��ܳɹ�
bool CServerControlItemSink::IsSettingSuccess(BYTE cbControlArea[MAX_INDEX])
{
  return true;
}

//���ؿ�������
bool  CServerControlItemSink::ReturnControlArea(tagControlInfo& ControlInfo)
{
  return true;
}

//��ʼ����
void  CServerControlItemSink::GetControlArea(BYTE Area[], BYTE &cbControlStyle)
{
	for(BYTE cbIndex =0; cbIndex<AREA_COUNT; cbIndex++)
	{
		Area[cbIndex] = m_bWinArea[cbIndex];
	}
	cbControlStyle = m_cbControlStyle;
}
//��������
void  CServerControlItemSink::GetSuitResult(BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], SCORE lAllJettonScore[])
{
  //������
  memcpy(m_cbTableCardArray, cbTableCardArray, sizeof(m_cbTableCardArray));
  memcpy(m_cbTableCard, cbTableCard, sizeof(m_cbTableCard));
  memcpy(m_lAllJettonScore, lAllJettonScore, sizeof(m_lAllJettonScore));

  //�����˿�
  BYTE cbSuitStack[MAX_CARDGROUP] = {};

  //�Զ����
  GetSuitCardCombine(cbSuitStack);

  //��������ֽ������
  BYTE UserCard[MAX_CARDGROUP][MAX_CARD] = {};
  memcpy(UserCard,m_cbTableCardArray,sizeof(UserCard));
  BYTE cbIndex=0;
  for(cbIndex=0; cbIndex<MAX_CARDGROUP; cbIndex++)
  {
    memcpy(cbTableCardArray[cbIndex],UserCard[cbSuitStack[cbIndex]],sizeof(BYTE)*MAX_CARD);
  }
  memcpy(cbTableCard, m_cbTableCard, sizeof(m_cbTableCard));
}
//��ʼ����
BYTE  CServerControlItemSink::GetControlTybe()
{
	return m_cbControlStyle;
}
//��ʼ����
void  CServerControlItemSink::SetControlArea(BYTE Area[])
{
	memcpy(m_bBakWinArea,Area,sizeof(m_bBakWinArea));
}
//�Ƿ�����Ч���
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
  bool bIsUser[MAX_CARDGROUP]= {0}; //�Ƿ��Ѿ�ʹ��
  BYTE cbStackCount=0;  //ջ��Ԫ������
  BYTE cbIndex=0;
  BYTE cbMultiple=0;
  //�Ƚ���֮���ϵ
  for(cbIndex=0; cbIndex<MAX_CARDGROUP; cbIndex++)
  {
    for(BYTE j=cbIndex; j<MAX_CARDGROUP; j++)
    {

      m_nCompareCard[j][cbIndex]=m_GameLogic.CompareCard(m_cbTableCardArray[cbIndex],MAX_CARD,m_cbTableCardArray[j],MAX_CARD,cbMultiple);
      m_nCompareCard[j][cbIndex]*=cbMultiple;
      m_nCompareCard[cbIndex][j]=-m_nCompareCard[j][cbIndex];
    }
  }

  //�����������
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
        if(cbStackCount==MAX_CARDGROUP) //����Ѿ��ҵ�һ�����
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
  //ׯ������
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
	bool bIsUser[MAX_CARDGROUP]= {0}; //�Ƿ��Ѿ�ʹ��
	BYTE cbStackCount=0;  //ջ��Ԫ������
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

	//�Ƚ���֮���ϵ
	for(cbIndex=0; cbIndex<MAX_CARDGROUP; cbIndex++)
	{
		for(BYTE j=cbIndex; j<MAX_CARDGROUP; j++)
		{

			m_nCompareCard[j][cbIndex]=m_GameLogic.CompareCard(m_cbTableCardArray[cbIndex],MAX_CARD,m_cbTableCardArray[j],MAX_CARD,cbMultiple);
			m_nCompareCard[j][cbIndex]*=cbMultiple;
			m_nCompareCard[cbIndex][j]=-m_nCompareCard[j][cbIndex];
		}
	}

	//�����������
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
				if(cbStackCount==MAX_CARDGROUP) //����Ѿ��ҵ�һ�����
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


  //bool bIsUser[MAX_CARDGROUP]= {0}; //�Ƿ��Ѿ�ʹ��
  //BYTE cbStackCount=0;  //ջ��Ԫ������
  //BYTE cbIndex=0;
  //BYTE cbMultiple;
  ////�Ƚ���֮���ϵ
  //for(cbIndex=0; cbIndex<MAX_CARDGROUP; cbIndex++)
  //{
  //  for(BYTE j=cbIndex; j<MAX_CARDGROUP; j++)
  //  {
  //    m_nCompareCard[j][cbIndex]=m_GameLogic.CompareCard(m_cbTableCardArray[cbIndex],MAX_CARD,m_cbTableCardArray[j],MAX_CARD,cbMultiple);
  //    m_nCompareCard[j][cbIndex]*=cbMultiple;
  //    m_nCompareCard[cbIndex][j]=-m_nCompareCard[j][cbIndex];
  //  }
  //}

  ////�����������
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
  //      if(cbStackCount==MAX_CARDGROUP) //����Ѿ��ҵ�һ�����
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

//������
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
        str=TEXT("��Ӯ���������Ѿ����ܣ�");
        switch(m_cbControlStyle)
        {
          case CS_BET_AREA:
          {
            str=TEXT("ʤ������:");
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
                    str+=TEXT("�� ");
                    break;
                  case 1:
                    str+=TEXT("�� ");
                    break;
                  case 2:
                    str+=TEXT("�� ");
                    break;
                  case 3:
                    str+=TEXT("�� ");
                    break;
                  default:
                    break;
                }
              }
            }
            if(!bFlags)
            {
              str+=TEXT("ׯ��ͨɱ");
            }
            str.AppendFormat(TEXT(",ִ�д���:%d"),m_cbExcuteTimes);
            break;
          }
          case CS_BANKER_WIN:
          {
            str.Format(TEXT("ׯ���ܿ�,�ܿط�ʽ:Ӯ,ִ�д���:%d"),m_cbExcuteTimes);
            break;
          }
          case CS_BANKER_LOSE:
          {
            str.Format(TEXT("ׯ���ܿ�,�ܿط�ʽ:��,ִ�д���:%d"),m_cbExcuteTimes);
            break;
          }
          default:
            str=TEXT("�ǿ���״̬");
            break;
        }
      }
      else
      {
        str.Format(TEXT("ִ�п���ʧ��!"));
      }
      break;
    }
    case ACK_RESET_CONTROL:
    {
      if(pResult->cbResult==CR_ACCEPT)
      {
        str=TEXT("ȡ������!");
      }
      break;
    }
    case ACK_PRINT_SYN:
    {
      if(pResult->cbResult==CR_ACCEPT)
      {
        str=TEXT("������ͬ���ɹ�!");
      }
      else
      {
        str.Format(TEXT("������ͬ��ʧ��!"));
      }
      break;
    }

    default:
      break;
  }

  //��¼��Ϣ
  CString strControlInfo;
  CTime Time(CTime::GetCurrentTime());
  strControlInfo.Format(TEXT("����: %s | ����: %u | ʱ��: %d-%d-%d %d:%d:%d\n�������˺�: %s | ������ID: %u\n%s\r\n"),
                        pGameServiceOption->szServerName, pITableFrame->GetTableID()+1, Time.GetYear(), Time.GetMonth(), Time.GetDay(),
                        Time.GetHour(), Time.GetMinute(), Time.GetSecond(), pIServerUserItem->GetNickName(), pIServerUserItem->GetGameID(), str);

  WriteInfo(TEXT("����ţţ������Ϣ.log"),strControlInfo);

  return;
}

//��¼��Ϣ
VOID CServerControlItemSink::WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString)
{
  //������������
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

  //��ԭ�����趨
  setlocale(LC_CTYPE, old_locale);
  free(old_locale);

  return;
}