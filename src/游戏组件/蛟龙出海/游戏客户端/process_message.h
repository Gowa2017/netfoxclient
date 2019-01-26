
#ifndef PROCESS_MESSAGE_H_
#define PROCESS_MESSAGE_H_
#pragma once

#include <deque>

struct AsynchronismData;

class __declspec(novtable) IProcessMessageSink {
 public:
  virtual bool OnProcessMessageData(WORD main_cmdid, WORD sub_cmdid, void* data, WORD data_size, HWND send_wnd) = 0;
};

class ProcessMessage
{
 public:
  ProcessMessage();
  ~ProcessMessage();

  bool SetProcessMessageSink(IProcessMessageSink* process_message_sink);

  bool InitializeLink();
  bool UnInitializeLink();

  bool SendData(HWND server_wnd, WORD main_cmdid, WORD sub_cmdid);
  bool SendData(HWND server_wnd, WORD main_cmdid, WORD sub_cmdid, void* data, WORD data_size);
  bool SendCommand(HWND server_wnd, WORD main_cmdid, WORD sub_cmdid, void* data, WORD data_size);

 private:
  BOOL OnCopyData(HWND wnd, COPYDATASTRUCT* copy_data);
  LRESULT	OnMessageAsynchronismData(WPARAM wparam, LPARAM lparam);
  static LRESULT CALLBACK MessageWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

 private:
  std::deque<AsynchronismData*> buffer_queue_;
  IProcessMessageSink* process_message_sink_;
  HWND message_wnd_;
};

#endif // PROCESS_MESSAGE_H_
