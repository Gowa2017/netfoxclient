#include "StdAfx.h"
#include "servercontrolitemsink.h"
#include <locale>

CServerControlItemSink::CServerControlItemSink(void)
{
  m_cbControlArea = 0xff;
  m_cbControlTimes = 0;
}

CServerControlItemSink::~CServerControlItemSink(void)
{

}

//服务器控制
bool  CServerControlItemSink::ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame)
{
  ASSERT(wDataSize==sizeof(CMD_C_ControlApplication));
  if(wDataSize!=sizeof(CMD_C_ControlApplication))
  {
    return false;
  }
  //如果不具有管理员权限 则返回错误
  if(!CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
  {
    return false;
  }

  CMD_C_ControlApplication* pControlApplication = (CMD_C_ControlApplication*)pDataBuffer;

  switch(pControlApplication->cbControlAppType)
  {
    case C_CA_UPDATE: //更新
    {
      CMD_S_ControlReturns ControlReturns;
      ZeroMemory(&ControlReturns,sizeof(ControlReturns));
      ControlReturns.cbReturnsType = S_CR_UPDATE_SUCCES;
      ControlReturns.cbControlArea = m_cbControlArea;
      ControlReturns.cbControlTimes = m_cbControlTimes;
      UpdateControl(&ControlReturns,pIServerUserItem);
      pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
    }
    return true;
    case C_CA_SET:    //设置
    {
      if(pControlApplication->cbControlArea  > 7)
      {
        //设置失败
        CMD_S_ControlReturns ControlReturns;
        ZeroMemory(&ControlReturns,sizeof(ControlReturns));
        ControlReturns.cbReturnsType = S_CR_FAILURE;
        ControlReturns.cbControlArea = 0xff;
        UpdateControl(&ControlReturns,pIServerUserItem);
        pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
        return true;
      }

      m_cbControlArea = pControlApplication->cbControlArea;
      m_cbControlTimes = pControlApplication->cbControlTimes;
      CMD_S_ControlReturns ControlReturns;
      ZeroMemory(&ControlReturns,sizeof(ControlReturns));
      ControlReturns.cbReturnsType = S_CR_SET_SUCCESS;
      ControlReturns.cbControlArea = m_cbControlArea;
      ControlReturns.cbControlTimes = m_cbControlTimes;
      UpdateControl(&ControlReturns,pIServerUserItem);
      pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
    }
    return true;
    case C_CA_CANCELS:  //取消
    {

      m_cbControlArea = 0xff;
      m_cbControlTimes = 0;
      CMD_S_ControlReturns ControlReturns;
      ZeroMemory(&ControlReturns,sizeof(ControlReturns));
      ControlReturns.cbReturnsType = S_CR_CANCEL_SUCCESS;
      ControlReturns.cbControlArea = 0xff;
      pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
      UpdateControl(&ControlReturns,pIServerUserItem);
    }
    return true;
  }
  //设置失败
  CMD_S_ControlReturns ControlReturns;
  ZeroMemory(&ControlReturns,sizeof(ControlReturns));
  ControlReturns.cbReturnsType = S_CR_FAILURE;
  ControlReturns.cbControlArea = 0xff;
  pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
  UpdateControl(&ControlReturns,pIServerUserItem);
  return true;
}

//需要控制
bool  CServerControlItemSink::NeedControl()
{
  if(m_cbControlArea != 0xff && m_cbControlArea <= 7 && m_cbControlTimes > 0)
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
  m_cbControlTimes--;
  if(m_cbControlTimes == 0)
  {
    m_cbControlArea = 0xff;
  }
  return true;
}

//返回控制区域
bool  CServerControlItemSink::ReturnControlArea(tagControlInfo& ControlInfo)
{
  BYTE cbControlArea[8][4] = { 1, 9, 1, 9,   3, 11, 3, 11,   5, 13, 5, 13,   7, 15, 7, 15,   2, 10, 2, 10,   4, 12, 4, 12,   6, 14, 6, 14,   8, 16, 8, 16 };
  ControlInfo.cbControlArea = cbControlArea[m_cbControlArea][rand()%4];
  return true;
}

//更新控制
void  CServerControlItemSink::UpdateControl(CMD_S_ControlReturns* pControlReturns,IServerUserItem* pIServerUserItem)
{
  CString str;
  switch(pControlReturns->cbReturnsType)
  {
    case S_CR_FAILURE:
    {
      //SetDlgItemText(IDC_STATIC_INFO,TEXT("操作失败！"));
      TCHAR zsText[256] = TEXT("操作失败");
      str+=zsText;
      break;
    }
    case S_CR_UPDATE_SUCCES:
    {
      TCHAR zsText[256] = TEXT("");
      TCHAR zsTextTemp[256] = TEXT("");
      PrintingInfo(zsTextTemp,256,pControlReturns->cbControlArea,pControlReturns->cbControlTimes);
      myprintf(zsText,CountArray(zsText),TEXT("更新数据成功！\r\n %s"),zsTextTemp);
      str+=zsText;
      //SetDlgItemText(IDC_STATIC_INFO,zsText);
      break;
    }
    case S_CR_SET_SUCCESS:
    {
      TCHAR zsText[256] = TEXT("");
      TCHAR zsTextTemp[256] = TEXT("");
      PrintingInfo(zsTextTemp,256,pControlReturns->cbControlArea,pControlReturns->cbControlTimes);
      myprintf(zsText,CountArray(zsText),TEXT("设置数据成功！\r\n %s"),zsTextTemp);
      str+=zsText;
      //SetDlgItemText(IDC_STATIC_INFO,zsText);
      break;
    }
    case S_CR_CANCEL_SUCCESS:
    {
      TCHAR zsText[256] = TEXT("取消设置成功!");
      //SetDlgItemText(IDC_STATIC_INFO,TEXT("取消设置成功！"));
      str+=zsText;
      break;
    }
  }
  CString strRecord;
  CTime Time(CTime::GetCurrentTime());
  strRecord.Format(TEXT("\n %s :| 时间: %d-%d-%d %d:%d:%d "),pIServerUserItem->GetNickName(), Time.GetYear(), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond());
  strRecord=strRecord+str;
  WriteInfo(strRecord);
}

//信息
void CServerControlItemSink::PrintingInfo(TCHAR* pText, WORD cbCount, BYTE cbArea, BYTE cbTimes)
{
  if(cbArea == 0xff)
  {
    myprintf(pText,cbCount,TEXT("暂时无控制。"));
    return;
  }
  TCHAR szDesc[32] = TEXT("");
  myprintf(pText,cbCount,TEXT("胜利区域："));
  if(cbArea == 0)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("保时捷×40，"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 1)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("宝马×30，"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 2)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("奔驰×20，"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 3)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("大众×5，"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 4)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("保时捷×5，"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 5)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("宝马×5，"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 6)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("奔驰×5，"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 7)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("大众×5，"));
    lstrcat(pText,szDesc);
  }
  myprintf(szDesc,CountArray(szDesc),TEXT("执行次数：%d。"), cbTimes);
  lstrcat(pText,szDesc);
}

//测试写信息
void CServerControlItemSink::WriteInfo(LPCTSTR pszString)
{
  //设置语言区域
  char* old_locale = _strdup(setlocale(LC_CTYPE,NULL));
  setlocale(LC_CTYPE, "chs");

  CStdioFile myFile;
  CString strFileName;
  strFileName.Format(TEXT("碰碰车控制记录.txt"));
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
}

