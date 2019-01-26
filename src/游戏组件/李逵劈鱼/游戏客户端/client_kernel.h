
#ifndef CLIENT_KERNEL_H_
#define CLIENT_KERNEL_H_
#pragma once

#include "process_message.h"
#include "game_user_manager.h"

struct UserAttribute {
  DWORD user_id;
  WORD table_id;
  WORD chair_id;

  DWORD user_right;
  DWORD master_right;
};

struct GameAttribute {
  WORD kind_id;
  WORD chair_count;
  DWORD client_version;
  TCHAR game_name[LEN_KIND];
};

struct ServerAttribute {
  WORD kind_id;
  WORD server_id;
  WORD server_type;
  DWORD server_rule;
  TCHAR server_name[LEN_KIND];
};
                    
class __declspec(novtable) IClientKernelSink {
 public:
  virtual void ResetGameClient() = 0;
  virtual void CloseGameClient() = 0;
  virtual bool OnEventSocket(WORD main_cmdid, WORD sub_cmdid, void* data, WORD data_size) = 0;
  virtual bool OnEventSceneMessage(BYTE game_status, bool lookon_user, void* data, WORD data_size) = 0;
  virtual void OnEventUserEnter(IGameUserItem* game_user_item, bool lookon_user) = 0;
  virtual void OnEventUserLeave(IGameUserItem* game_user_item, bool lookon_user) = 0;
  virtual void OnEventUserScore(IGameUserItem* game_user_item, bool lookon_user) = 0;
  virtual void OnEventUserStatus(IGameUserItem* game_user_item, bool lookon_user) = 0;
  virtual void OnSystemMessage(LPCTSTR sys_msg, WORD msg_len, WORD type) = 0;
};

class ClientKernel : public IProcessMessageSink, public IGameUserManagerSink {
 public:
  ClientKernel();
  ~ClientKernel();

  bool InitClientKernel(LPCTSTR cmd_line);
  bool SetClientKernelSink(IClientKernelSink* client_kernel_sink);
  bool SetGameAttribute(WORD kind_id, WORD player_count, DWORD client_version, LPCTSTR game_name);
  bool SendGameOption();

  bool CreateConnect();
  bool IntermitConnect();

  const UserAttribute& GetUserAttribute() const { return user_attribute_; }
  const GameAttribute& GetGameAttribute() const { return game_attribute_; }
  const ServerAttribute& GetServerAttribute() const { return server_attribute_; }

  bool IsSingleMode();
  bool IsLookonMode();
  bool IsAllowLookon() const;
  bool IsServiceStatus() const;

  BYTE GetGameStatus() const { return game_status_; }
  void SetGameStatus(BYTE game_status) { game_status_ = game_status; }

  WORD GetMeChairID() const;
  IGameUserItem* GetMeUserItem() const;
  IGameUserItem* GetTableUserItem(WORD chair_id);
  const GameUserItemVector& GetLookonUserVector();
  IGameUserItem* SearchUserItemByUserID(DWORD user_id);
  IGameUserItem* SearchUserItemByGameID(DWORD game_id);
  IGameUserItem* SearchUserItemByNickName(LPCTSTR nick_name);

  bool SendSocketData(WORD main_cmdid, WORD sub_cmdid);
  bool SendSocketData(WORD main_cmdid, WORD sub_cmdid, void* data, WORD data_size);
  bool SendSocketDataTren( void* data, WORD data_size);
  bool SendProcessData(WORD main_cmdid, WORD sub_cmdid);
  bool SendProcessData(WORD main_cmdid, WORD sub_cmdid, void* data, WORD data_size);
  bool SendProcessCommand(WORD main_cmdid, WORD sub_cmdid, void* data, WORD data_size);

  bool SendUserReady();
  bool SendUserLookon(DWORD user_id, bool allow_lookon);
  bool SendUserChatMessage(DWORD target_user_id, LPCTSTR message, COLORREF text_color);
  
  virtual void OnUserItemAcitve(IGameUserItem* game_user_item);
  virtual void OnUserItemDelete(IGameUserItem* game_user_item);
  virtual void OnUserItemUpdate(IGameUserItem* game_user_item, tagUserScore& last_score);
  virtual void OnUserItemUpdate(IGameUserItem* game_user_item, tagUserStatus& last_status);

  virtual bool OnProcessMessageData(WORD main_cmdid, WORD sub_cmdid, void* data, WORD data_size, HWND send_wnd);

 private:
  bool OnProcessMainSocket(WORD sub_cmdid, void* data, WORD data_size);
  bool OnProcessMainControl(WORD sub_cmdid, void* data, WORD data_size);
  bool OnProcessMainConfig(WORD sub_cmdid, void* data, WORD data_size);
  bool OnProcessMainUserInfo(WORD sub_cmdid, void* data, WORD data_size);

 private:
  bool SearchCommandItem(LPCTSTR command_line, LPCTSTR command, TCHAR param[], WORD param_len);

 private:
  bool in_service_;

  bool allow_lookon_;
  BYTE game_status_;

  IGameUserItem* myself_user_item_;
  GameUserManager* game_user_manager_;

  UserAttribute user_attribute_;
  GameAttribute game_attribute_;
  ServerAttribute server_attribute_;

  HWND wnd_server_;
  ProcessMessage* process_message_;

  IClientKernelSink* client_kernel_sink_;
};

#endif // CLIENT_KERNEL_H_
