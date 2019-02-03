
#include "StdAfx.h"
#include "client_kernel.h"

ClientKernel::ClientKernel() : in_service_(false), allow_lookon_(false), game_status_(GAME_STATUS_FREE), myself_user_item_(NULL),
  wnd_server_(NULL), client_kernel_sink_(NULL) {
  game_user_manager_ = new GameUserManager();

  memset(&user_attribute_, 0, sizeof(user_attribute_));
  memset(&game_attribute_, 0, sizeof(game_attribute_));
  memset(&server_attribute_, 0, sizeof(server_attribute_));

  process_message_ = new ProcessMessage();
}

ClientKernel::~ClientKernel() {
  SAFE_DELETE(game_user_manager_);
  SAFE_DELETE(process_message_);
}

bool ClientKernel::InitClientKernel(LPCTSTR cmd_line) {
  if (cmd_line == NULL) return false;

  TCHAR transmittal[32] = { 0 };
  if (SearchCommandItem(cmd_line, TEXT("/Transmittal:"), transmittal, arraysize(transmittal))) {
    wnd_server_ = reinterpret_cast<HWND>(_tstoi64(transmittal));
    if (!IsWindow(wnd_server_)) {
      MessageBox(NULL, cmd_line, NULL, MB_OK);
      wnd_server_ = NULL;
    }
  }

  game_user_manager_->SetGameUserManagerSink(this);
  process_message_->SetProcessMessageSink(this);

  if (wnd_server_ != NULL && !process_message_->InitializeLink()) return false;

  return true;
}

bool ClientKernel::SetClientKernelSink(IClientKernelSink* client_kernel_sink) {
  client_kernel_sink_ = client_kernel_sink;
  return true;
}

bool ClientKernel::SetGameAttribute(WORD kind_id, WORD player_count, DWORD client_version, LPCTSTR game_name) {
  game_attribute_.kind_id = kind_id;
  game_attribute_.chair_count = player_count;
  game_attribute_.client_version = client_version;
  lstrcpyn(game_attribute_.game_name, game_name, arraysize(game_attribute_.game_name));

  return true;
}

bool ClientKernel::SendGameOption() {
  if (IsSingleMode()) return false;

  CMD_GF_GameOption game_option;
  game_option.cbAllowLookon = allow_lookon_ ? 1 : 0;
  game_option.dwClientVersion = game_attribute_.client_version;
  game_option.dwFrameVersion = VERSION_FRAME;
  SendSocketData(MDM_GF_FRAME, SUB_GF_GAME_OPTION, &game_option,sizeof(game_option));

  return true;
}

bool ClientKernel::CreateConnect() {
  if (IsSingleMode()) return false;
  return SendProcessData(IPC_CMD_GF_CONTROL, IPC_SUB_GF_CLIENT_READY);
}

bool ClientKernel::IntermitConnect() {
  if (IsSingleMode()) return false;
  SendProcessData(IPC_CMD_GF_CONTROL, IPC_SUB_GF_CLIENT_CLOSE);
  if (process_message_ != NULL) process_message_->UnInitializeLink();
  
  return true;
}

bool ClientKernel::IsSingleMode() {
  return wnd_server_ == NULL;
}

bool ClientKernel::IsLookonMode() {
  if (myself_user_item_ == NULL) return true;
  return myself_user_item_->GetUserStatus() == US_LOOKON;
}

bool ClientKernel::IsAllowLookon() const {
  return allow_lookon_;
}

bool ClientKernel::IsServiceStatus() const {
  return in_service_;
}

WORD ClientKernel::GetMeChairID() const {
  if (myself_user_item_ == NULL) return INVALID_CHAIR;
  return myself_user_item_->GetChairID();
}

IGameUserItem* ClientKernel::GetMeUserItem() const {
  return myself_user_item_;
}

IGameUserItem* ClientKernel::GetTableUserItem(WORD chair_id) {
  return game_user_manager_->GetTableUserItem(chair_id);
}

const GameUserItemVector& ClientKernel::GetLookonUserVector() {
  return game_user_manager_->GetLookonUserVector();
}

IGameUserItem* ClientKernel::SearchUserItemByUserID(DWORD user_id) {
  return game_user_manager_->SearchUserItemByUserID(user_id);
}

IGameUserItem* ClientKernel::SearchUserItemByGameID(DWORD game_id) {
  return game_user_manager_->SearchUserItemByGameID(game_id);
}

IGameUserItem* ClientKernel::SearchUserItemByNickName(LPCTSTR nick_name) {
  return game_user_manager_->SearchUserItemByNickName(nick_name);
}

bool ClientKernel::SendSocketData(WORD main_cmdid, WORD sub_cmdid) {
  if (wnd_server_ == NULL) return false;

  IPC_GF_SocketSend socket_send;
  memset(&socket_send, 0, sizeof(socket_send));
  socket_send.CommandInfo.wMainCmdID = main_cmdid;
  socket_send.CommandInfo.wSubCmdID = sub_cmdid;
  WORD send_size = sizeof(socket_send) - sizeof(socket_send.cbBuffer);
  return process_message_->SendData(wnd_server_, IPC_CMD_GF_SOCKET, IPC_SUB_GF_SOCKET_SEND, &socket_send, send_size);
}

bool ClientKernel::SendSocketData(WORD main_cmdid, WORD sub_cmdid, void* data, WORD data_size) {
  if (data_size > SOCKET_TCP_PACKET) return false;
  if (wnd_server_ == NULL) return false;

  IPC_GF_SocketSend socket_send;
  memset(&socket_send, 0, sizeof(socket_send));
  socket_send.CommandInfo.wMainCmdID = main_cmdid;
  socket_send.CommandInfo.wSubCmdID = sub_cmdid;
  if (data_size > 0) {
    memcpy(socket_send.cbBuffer, data, data_size);
  }
  WORD send_size = sizeof(socket_send) - sizeof(socket_send.cbBuffer) + data_size;
  return process_message_->SendData(wnd_server_, IPC_CMD_GF_SOCKET, IPC_SUB_GF_SOCKET_SEND, &socket_send, send_size);
}
bool ClientKernel::SendSocketDataTren( void* data, WORD data_size)
{
	if (data_size > SOCKET_TCP_PACKET) return false;
	if (wnd_server_ == NULL) return false;

	IPC_GF_KickUser socket_send;
	memset(&socket_send, 0, sizeof(socket_send));
	
	if (data_size > 0) {
		memcpy(&socket_send.dwTargetUserID, data, data_size);
	}
	
	return process_message_->SendData(wnd_server_, IPC_CMD_GF_USER_INFO, IPC_SUB_GF_KICK_USER, &socket_send, sizeof(IPC_GF_KickUser));
}
bool ClientKernel::SendProcessData(WORD main_cmdid, WORD sub_cmdid) {
  if (wnd_server_ == NULL) return false;
  return process_message_->SendData(wnd_server_, main_cmdid, sub_cmdid);
}

bool ClientKernel::SendProcessData(WORD main_cmdid, WORD sub_cmdid, void* data, WORD data_size) {
  if (wnd_server_ == NULL) return false;
  return process_message_->SendData(wnd_server_, main_cmdid, sub_cmdid, data, data_size);
}

bool ClientKernel::SendProcessCommand(WORD main_cmdid, WORD sub_cmdid, void* data, WORD data_size) {
  if (wnd_server_ == NULL) return false;
  return process_message_->SendCommand(wnd_server_, main_cmdid, sub_cmdid, data, data_size);
}

bool ClientKernel::SendUserReady() {
  if (myself_user_item_ == NULL) return false;
  return SendSocketData(MDM_GF_FRAME, SUB_GF_USER_READY, NULL, 0);
}

bool ClientKernel::SendUserLookon(DWORD user_id, bool allow_lookon) {
  if (!IsLookonMode()) {
    CMD_GF_LookonConfig lookon_config;
    lookon_config.dwUserID = user_id;
    lookon_config.cbAllowLookon = allow_lookon ? TRUE : FALSE;
    return SendSocketData(MDM_GF_FRAME, SUB_GF_LOOKON_CONFIG, &lookon_config, sizeof(lookon_config));
  }
  return true;
}

bool ClientKernel::SendUserChatMessage(DWORD target_user_id, LPCTSTR message, COLORREF text_color) {
  if (myself_user_item_ == NULL) return false;

  CMD_GF_C_UserChat user_chat;
  lstrcpyn(user_chat.szChatString, message, arraysize(user_chat.szChatString));
  user_chat.dwChatColor = text_color;
  user_chat.dwTargetUserID = target_user_id;
  user_chat.wChatLength = CountStringBuffer(user_chat.szChatString);
  WORD head_size = sizeof(user_chat) - sizeof(user_chat.szChatString);
  return SendSocketData(MDM_GF_FRAME, SUB_GF_USER_CHAT, &user_chat, head_size + user_chat.wChatLength * sizeof(TCHAR));
}

void ClientKernel::OnUserItemAcitve(IGameUserItem* game_user_item) {
  if (game_user_item == NULL || client_kernel_sink_ == NULL) return;
  if (myself_user_item_ == NULL && user_attribute_.user_id == game_user_item->GetUserID()) myself_user_item_ = game_user_item;
  client_kernel_sink_->OnEventUserEnter(game_user_item, game_user_item->GetUserStatus() == US_LOOKON);
}

void ClientKernel::OnUserItemDelete(IGameUserItem* game_user_item) {
  if (game_user_item == NULL || client_kernel_sink_ == NULL) return;
  client_kernel_sink_->OnEventUserLeave(game_user_item, game_user_item->GetUserStatus() == US_LOOKON);
}

void ClientKernel::OnUserItemUpdate(IGameUserItem* game_user_item, tagUserScore& last_score) {
  if (game_user_item == NULL || client_kernel_sink_ == NULL) return;
  client_kernel_sink_->OnEventUserScore(game_user_item, game_user_item->GetUserStatus() == US_LOOKON);
}

void ClientKernel::OnUserItemUpdate(IGameUserItem* game_user_item, tagUserStatus& last_status) {
  if (game_user_item == NULL || client_kernel_sink_ == NULL) return;
  client_kernel_sink_->OnEventUserStatus(game_user_item, game_user_item->GetUserStatus() == US_LOOKON);
}

bool ClientKernel::OnProcessMessageData(WORD main_cmdid, WORD sub_cmdid, void* data, WORD data_size, HWND send_wnd) {
  switch (main_cmdid) {
    case IPC_CMD_GF_SOCKET:
      if (!OnProcessMainSocket(sub_cmdid, data, data_size)) {
        if (client_kernel_sink_ != NULL) client_kernel_sink_->CloseGameClient();
      }
      return true;
    case IPC_CMD_GF_CONTROL:
      return OnProcessMainControl(sub_cmdid, data, data_size);
    case IPC_CMD_GF_CONFIG:
      return OnProcessMainConfig(sub_cmdid, data, data_size);
    case IPC_CMD_GF_USER_INFO:
      return OnProcessMainUserInfo(sub_cmdid, data, data_size);
    case IPC_CMD_GF_PROPERTY_INFO:
      return true;
  }

  return false;
}

bool ClientKernel::OnProcessMainSocket(WORD sub_cmdid, void* data, WORD data_size) {
  if (sub_cmdid != IPC_SUB_GF_SOCKET_RECV) return false;
  if (data_size < sizeof(TCP_Command)) return false;

  WORD packet_size = data_size - sizeof(TCP_Command);
  IPC_GF_SocketRecv* socket_recv = (IPC_GF_SocketRecv*)data;
  CString str;
  str.Format(L"ÅÖÃüÁî:%d,Ð¡ÃüÁî:%d", socket_recv->CommandInfo.wMainCmdID, socket_recv->CommandInfo.wSubCmdID);
  OutputDebugString(str.GetString());
  bool success = client_kernel_sink_->OnEventSocket(socket_recv->CommandInfo.wMainCmdID, socket_recv->CommandInfo.wSubCmdID, socket_recv->cbBuffer, packet_size);
  if (!success && socket_recv->CommandInfo.wMainCmdID == MDM_GF_FRAME) {
    switch (socket_recv->CommandInfo.wSubCmdID) {
      case SUB_GF_USER_CHAT:
      case SUB_GF_USER_EXPRESSION:
        return true;
      case SUB_GF_GAME_STATUS: {
        assert(packet_size == sizeof(CMD_GF_GameStatus));
        if (packet_size != sizeof(CMD_GF_GameStatus)) return false;
        CMD_GF_GameStatus* game_status = (CMD_GF_GameStatus*)socket_recv->cbBuffer;
        game_status_ = game_status->cbGameStatus;
        allow_lookon_ = !game_status->cbAllowLookon ? false : true;
        return true;
      }
      case SUB_GF_GAME_SCENE: {
        return client_kernel_sink_->OnEventSceneMessage(game_status_, false, socket_recv->cbBuffer, packet_size);
      }
      case SUB_GF_LOOKON_STATUS:
        return true;
      case SUB_GF_SYSTEM_MESSAGE: {
        CMD_CM_SystemMessage* system_msg = (CMD_CM_SystemMessage*)socket_recv->cbBuffer;
        WORD head_size = sizeof(CMD_CM_SystemMessage) - sizeof(system_msg->szString);
        if (packet_size < head_size || packet_size != head_size + system_msg->wLength * sizeof(TCHAR)) return false;

        client_kernel_sink_->OnSystemMessage(system_msg->szString, system_msg->wLength, system_msg->wType);

        if ((system_msg->wType & SMT_CLOSE_GAME) != 0) {
          in_service_ = false;
          IntermitConnect();
        }

        if ((system_msg->wType & SMT_CLOSE_GAME) != 0) {
          client_kernel_sink_->CloseGameClient();
        }

        return true;
      }
      case SUB_GF_ACTION_MESSAGE:
      case SUB_GF_USER_READY:
	 // case SUB_GF_TASK_INFO:
	  //case SUB_GF_TASK_STATUS:
      //case SUB_GF_POT_SCORE:

        return true;
    }
  }

  return success;
}

bool ClientKernel::OnProcessMainControl(WORD sub_cmdid, void* data, WORD data_size) {
  switch (sub_cmdid) {
    case IPC_SUB_GF_CLOSE_PROCESS:
      wnd_server_ = NULL;
      client_kernel_sink_->CloseGameClient();
      return true;
    case IPC_SUB_GF_ACTIVE_PROCESS:
    case IPC_SUB_GF_BOSS_COME:
    case IPC_SUB_GF_BOSS_LEFT:
      return true;
  }

  return false;
}

bool ClientKernel::OnProcessMainConfig(WORD sub_cmdid, void* data, WORD data_size) {
  switch (sub_cmdid) {
    case IPC_SUB_GF_LEVEL_INFO:
      return true;
    case IPC_SUB_GF_COLUMN_INFO:
      return true;
    case IPC_SUB_GF_SERVER_INFO: {
      assert(data_size == sizeof(IPC_GF_ServerInfo));
      if (data_size != sizeof(IPC_GF_ServerInfo)) return false;
      IPC_GF_ServerInfo* server_info = (IPC_GF_ServerInfo*)data;
      user_attribute_.user_id = server_info->dwUserID;
      user_attribute_.table_id = server_info->wTableID;
      user_attribute_.chair_id = server_info->wChairID;
      user_attribute_.user_right = server_info->dwUserRight;
      user_attribute_.master_right = server_info->dwMasterRight;

      server_attribute_.kind_id = server_info->wKindID;
      server_attribute_.server_id = server_info->wServerID;
      server_attribute_.server_type = server_info->wServerType;
      server_attribute_.server_rule = server_info->dwServerRule;
      lstrcpyn(server_attribute_.server_name, server_info->szServerName, arraysize(server_attribute_.server_name));
      return true;
    }
   /* case IPC_SUB_GF_PROPERTY_INFO:
      return true;*/
    case IPC_SUB_GF_CONFIG_FINISH: {
      in_service_ = true;
      /*CMD_GF_GameOption game_option;
      game_option.cbAllowLookon = allow_lookon_ ? 1 : 0;
      game_option.dwClientVersion = game_attribute_.client_version;
      game_option.dwFrameVersion = VERSION_FRAME;
      SendSocketData(MDM_GF_FRAME, SUB_GF_GAME_OPTION, &game_option,sizeof(game_option));*/
      return true;
    }
    case IPC_SUB_GF_USER_RIGHT: {
      if (data_size < sizeof(IPC_GF_UserRight)) return false;
      IPC_GF_UserRight* user_right = (IPC_GF_UserRight*)data;
      user_attribute_.user_right = user_right->dwUserRight;
      return true;
    }
  }
  return false;
}

bool ClientKernel::OnProcessMainUserInfo(WORD sub_cmdid, void* data, WORD data_size) {
  switch (sub_cmdid) {
    case IPC_SUB_GF_USER_ENTER: {
      assert(data_size >= sizeof(IPC_GF_UserInfo));
      if (data_size < sizeof(IPC_GF_UserInfo)) return false;

      IPC_GF_UserInfo* ipc_user_info = (IPC_GF_UserInfo*)data;
      tagUserInfoHead* user_info_head = &ipc_user_info->UserInfoHead;

      TCHAR user_note[64] = { 0 };

      tagUserInfo user_info;
      tagCustomFaceInfo custon_face_info;
      ZeroMemory(&user_info, sizeof(user_info));
      ZeroMemory(&custon_face_info, sizeof(custon_face_info));

      user_info.wFaceID = user_info_head->wFaceID;
      user_info.dwGameID = user_info_head->dwGameID;
      user_info.dwUserID = user_info_head->dwUserID;
      user_info.dwGroupID = user_info_head->dwGroupID;
      user_info.dwCustomID = user_info_head->dwCustomID;

      user_info.cbGender = user_info_head->cbGender;
      user_info.cbMemberOrder = user_info_head->cbMemberOrder;
      user_info.cbMasterOrder = user_info_head->cbMasterOrder;

      user_info.wTableID = user_info_head->wTableID;
      user_info.wChairID = user_info_head->wChairID;
      user_info.cbUserStatus = user_info_head->cbUserStatus;

      user_info.lScore = user_info_head->lScore;
      user_info.lGrade = user_info_head->lGrade;
      user_info.lInsure = user_info_head->lInsure;
      user_info.dwWinCount = user_info_head->dwWinCount;
      user_info.dwLostCount = user_info_head->dwLostCount;
      user_info.dwDrawCount = user_info_head->dwDrawCount;
      user_info.dwFleeCount = user_info_head->dwFleeCount;
     // user_info.dwUserMedal = user_info_head->dwUserMedal;
      user_info.dwExperience = user_info_head->dwExperience;
      user_info.lLoveLiness = user_info_head->lLoveLiness;

      void* data_buffer = NULL;
      tagDataDescribe data_describe;
      CRecvPacketHelper RecvPacket(ipc_user_info + 1,data_size - sizeof(IPC_GF_UserInfo));

      while (true) {
        data_buffer = RecvPacket.GetData(data_describe);
        if (data_describe.wDataDescribe == DTP_NULL) break;

        switch (data_describe.wDataDescribe) {
          case DTP_GR_NICK_NAME:
            if (data_describe.wDataSize <= sizeof(user_info.szNickName)) {
              CopyMemory(&user_info.szNickName, data_buffer, data_describe.wDataSize);
              user_info.szNickName[arraysize(user_info.szNickName) - 1] = 0;
            }
            break;
          case DTP_GR_GROUP_NAME:
            if (data_describe.wDataSize <= sizeof(user_info.szGroupName)) {
              CopyMemory(&user_info.szGroupName, data_buffer, data_describe.wDataSize);
              user_info.szGroupName[arraysize(user_info.szGroupName) - 1] = 0;
            }
            break;
          case DTP_GR_UNDER_WRITE:
            if (data_describe.wDataSize <= sizeof(user_info.szUnderWrite)) {
              CopyMemory(user_info.szUnderWrite, data_buffer, data_describe.wDataSize);
              user_info.szUnderWrite[arraysize(user_info.szUnderWrite) - 1] = 0;
            }
            break;
          case DTP_GR_USER_NOTE:
            if (data_describe.wDataSize <= sizeof(user_note)) {
              CopyMemory(user_note, data_buffer, data_describe.wDataSize);
              user_note[arraysize(user_note) - 1] = 0;
            }
            break;
          case DTP_GR_CUSTOM_FACE:
            if (data_describe.wDataSize == sizeof(tagCustomFaceInfo)) {
              CopyMemory(&custon_face_info, data_buffer, data_describe.wDataSize);
            }
            break;
        }
      }

      game_user_manager_->ActiveUserItem(user_info);

      return true;
    }
    case IPC_SUB_GF_USER_SCORE: {
      assert(data_size == sizeof(IPC_GF_UserScore));
      if (data_size != sizeof(IPC_GF_UserScore)) return false;
      IPC_GF_UserScore* user_score = (IPC_GF_UserScore*)data;
      IGameUserItem* game_user_item = game_user_manager_->SearchUserItemByUserID(user_score->dwUserID);
      if (game_user_item == NULL) return true;
      game_user_manager_->UpdateUserItemScore(game_user_item, &user_score->UserScore);
      return true;
    }
    case IPC_SUB_GF_USER_STATUS: {
      assert(data_size == sizeof(IPC_GF_UserStatus));
      if (data_size != sizeof(IPC_GF_UserStatus)) return false;
      IPC_GF_UserStatus* user_status = (IPC_GF_UserStatus*)data;
      IGameUserItem* game_user_item = game_user_manager_->SearchUserItemByUserID(user_status->dwUserID);
      if (game_user_item == NULL) return true;

      if (user_status->UserStatus.cbUserStatus == US_NULL || user_status->UserStatus.cbUserStatus == US_FREE) {
        if (game_user_item == myself_user_item_) {
          in_service_ = false;
          allow_lookon_ = false;
          myself_user_item_ = NULL;
          game_status_ = GAME_STATUS_FREE;
          memset(&user_attribute_, 0, sizeof(user_attribute_));
          client_kernel_sink_->ResetGameClient();
          game_user_manager_->ResetUserManager();
        } else {
          game_user_manager_->DeleteUserItem(game_user_item);
        }
        return true;
      }
      game_user_manager_->UpdateUserItemStatus(game_user_item, &user_status->UserStatus);
      return true;
    }
    case IPC_SUB_GF_USER_ATTRIB:
      assert(data_size == sizeof(IPC_GF_UserAttrib));
      if (data_size != sizeof(IPC_GF_UserAttrib)) return false;
      return true;
    case IPC_SUB_GF_CUSTOM_FACE:
      return true;
  }

  return false;
}

bool ClientKernel::SearchCommandItem(LPCTSTR command_line, LPCTSTR command, TCHAR param[], WORD param_len) {
  if (command_line == NULL || command == NULL) return false;

  if (command_line[0] != 0) {
    UINT command_len = lstrlen(command);
    LPCTSTR begin_string = command_line;

    while (true) {
      LPCTSTR end_string = _tcschr(begin_string, TEXT(' '));
      UINT string_len = end_string == NULL ? lstrlen(begin_string) : UINT(end_string - begin_string);
      if (string_len >= command_len && !memcmp(begin_string, command, command_len * sizeof(TCHAR))) {
        if (param_len <= string_len - command_len) return false;
        param[string_len - command_len] = 0;
        memcpy(param, begin_string + command_len, (string_len - command_len) * sizeof(TCHAR));
        return true;
      }

      if (end_string == NULL) break;
      begin_string = end_string + 1;
    }
  }

  return false;
}
