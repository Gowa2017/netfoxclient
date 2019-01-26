
#include "StdAfx.h"
#include "process_message.h"

#define IDT_IMMEDIATE         0x0101
#define IDT_ASYNCHRONISM      0x0201
#define WM_ASYNCHRONISM_DATA  (WM_USER + 100)

struct AsynchronismData {
  HWND send_wnd;
  WORD data_size;
  WORD main_cmdid;
  WORD sub_cmdid;
  BYTE buffer[IPC_PACKET];
};

ProcessMessage::ProcessMessage() : process_message_sink_(NULL), message_wnd_(NULL) {
}

ProcessMessage::~ProcessMessage() {
}

bool ProcessMessage::SetProcessMessageSink(IProcessMessageSink* process_message_sink) {
  process_message_sink_ = process_message_sink;
  return true;
}

bool ProcessMessage::InitializeLink() {
  if (process_message_sink_ == NULL) return false;
  if (message_wnd_ == NULL) {
    WNDCLASS ws = { 0 };
    ws.style = CS_HREDRAW | CS_VREDRAW;
    ws.lpfnWndProc = ProcessMessage::MessageWndProc;
    ws.cbClsExtra = 0;
    ws.cbWndExtra = 0;
    ws.hInstance = ::GetModuleHandle(NULL);
    ws.hIcon = NULL;
    ws.hCursor = NULL;
    ws.hbrBackground = NULL;
    ws.lpszMenuName = NULL;
    ws.lpszClassName = TEXT("ProcMsgWnd");
    RegisterClass(&ws);
    message_wnd_ = CreateWindowEx(0, ws.lpszClassName, ws.lpszClassName, WS_CHILD, 0, 0, 0, 0, GetDesktopWindow(), NULL, ws.hInstance, this);
  }

  return true;
}

bool ProcessMessage::UnInitializeLink() {
	int a = 1;
  if (message_wnd_ != NULL) DestroyWindow(message_wnd_);
  std::deque<AsynchronismData*>::iterator iter;
  for (iter = buffer_queue_.begin(); iter != buffer_queue_.end(); ++iter) {
    delete (*iter);
  }
  buffer_queue_.clear();

  return true;
}

bool ProcessMessage::SendData(HWND server_wnd, WORD main_cmdid, WORD sub_cmdid) {
  if (!IsWindow(server_wnd)) return false;
  if (!IsWindow(message_wnd_)) return false;

  IPC_Head head;
  head.wVersion = IPC_VER;
  head.wMainCmdID = main_cmdid;
  head.wSubCmdID = sub_cmdid;
  head.wPacketSize = sizeof(IPC_Head);

  COPYDATASTRUCT copydata;
  copydata.lpData = &head;
  copydata.dwData = IDT_ASYNCHRONISM;
  copydata.cbData = head.wPacketSize;
  SendMessage(server_wnd, WM_COPYDATA, (LPARAM)message_wnd_, (WPARAM)&copydata);

  return true;
}

bool ProcessMessage::SendData(HWND server_wnd, WORD main_cmdid, WORD sub_cmdid, void* data, WORD data_size) {
  if (!IsWindow(server_wnd)) return false;
  if (!IsWindow(message_wnd_)) return false;

  IPC_Buffer buffer;
  buffer.Head.wVersion = IPC_VER;
  buffer.Head.wMainCmdID = main_cmdid;
  buffer.Head.wSubCmdID = sub_cmdid;
  buffer.Head.wPacketSize = sizeof(buffer.Head);

  if (data != NULL) {
    if (data_size > sizeof(buffer.cbBuffer)) return false;
    buffer.Head.wPacketSize += data_size;
    memcpy(buffer.cbBuffer, data, data_size);
  }

  COPYDATASTRUCT copydata;
  copydata.lpData = &buffer;
  copydata.dwData = IDT_ASYNCHRONISM;
  copydata.cbData = buffer.Head.wPacketSize;
  //·¢ËÍµ½´óÌü
  SendMessage(server_wnd, WM_COPYDATA, (LPARAM)message_wnd_, (WPARAM)&copydata);

  return true;
}

bool ProcessMessage::SendCommand(HWND server_wnd, WORD main_cmdid, WORD sub_cmdid, void* data, WORD data_size) {
  if (!IsWindow(server_wnd)) return false;
  if (!IsWindow(message_wnd_)) return false;

  IPC_Buffer buffer;
  buffer.Head.wVersion = IPC_VER;
  buffer.Head.wMainCmdID = main_cmdid;
  buffer.Head.wSubCmdID = sub_cmdid;
  buffer.Head.wPacketSize = sizeof(buffer.Head);

  if (data != NULL) {
    if (data_size > sizeof(buffer.cbBuffer)) return false;
    buffer.Head.wPacketSize += data_size;
    memcpy(buffer.cbBuffer, data, data_size);
  }

  COPYDATASTRUCT copydata;
  copydata.lpData = &buffer;
  copydata.dwData = IDT_IMMEDIATE;
  copydata.cbData = buffer.Head.wPacketSize;
  SendMessage(server_wnd, WM_COPYDATA, (LPARAM)message_wnd_, (WPARAM)&copydata);

  return true;
}

BOOL ProcessMessage::OnCopyData(HWND wnd, COPYDATASTRUCT* copydata) {
  if (process_message_sink_ == NULL) return FALSE;
  if (copydata->cbData < sizeof(IPC_Head)) return FALSE;
  IPC_Head* head = static_cast<IPC_Head*>(copydata->lpData);
  if (head->wVersion != IPC_VER || head->wPacketSize != copydata->cbData) return FALSE;
  if (copydata->dwData == IDT_IMMEDIATE) {
    WORD data_size = head->wPacketSize - sizeof(IPC_Head);
    process_message_sink_->OnProcessMessageData(head->wMainCmdID, head->wSubCmdID, head + 1, data_size, wnd);
  } else {
    if (message_wnd_ != NULL) {
      AsynchronismData* asyn_data = new AsynchronismData;
      asyn_data->main_cmdid = head->wMainCmdID;
      asyn_data->sub_cmdid = head->wSubCmdID;
      asyn_data->data_size = head->wPacketSize - sizeof(IPC_Head);
      asyn_data->send_wnd = wnd;
      memcpy(asyn_data->buffer, head + 1, asyn_data->data_size);
      buffer_queue_.push_front(asyn_data);
      PostMessage(message_wnd_, WM_ASYNCHRONISM_DATA, 0, 0);
    }
  }

  return TRUE;
}

LRESULT	ProcessMessage::OnMessageAsynchronismData(WPARAM wparam, LPARAM lparam) {
  AsynchronismData* asyn_data = buffer_queue_.back();
  if (process_message_sink_ != NULL) {
    process_message_sink_->OnProcessMessageData(asyn_data->main_cmdid, asyn_data->sub_cmdid, asyn_data->buffer, asyn_data->data_size, asyn_data->send_wnd);
  }
  delete asyn_data;
  buffer_queue_.pop_back();

  return 0;
}

LRESULT CALLBACK ProcessMessage::MessageWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  ProcessMessage* window = NULL;
  if (msg == WM_NCCREATE) {
    LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lparam);
    window = static_cast<ProcessMessage*>(lpcs->lpCreateParams);
    window->message_wnd_ = wnd;
    SetWindowLongPtr(wnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(window));
  } else {
    window = reinterpret_cast<ProcessMessage*>(::GetWindowLongPtr(wnd, GWLP_USERDATA));
    if (msg == WM_NCDESTROY && window != NULL) {
		LRESULT res = ::CallWindowProc(DefWindowProc, wnd, msg, wparam, lparam);
      ::SetWindowLongPtr(window->message_wnd_, GWLP_USERDATA, 0L);
      window->message_wnd_ = NULL;
      return res;
    }
	/*if (WM_DESTROY == msg && window != NULL)
	{
		MessageBox(0, 0, 0, 0);
	}*/
  }

  switch (msg) {
  case WM_COPYDATA:
    if (window != NULL) window->OnCopyData((HWND)wparam, reinterpret_cast<COPYDATASTRUCT*>(lparam));
    return 0;
  case WM_ASYNCHRONISM_DATA:
    if (window != NULL) window->OnMessageAsynchronismData(wparam, lparam);
    return 0;
  }

  return DefWindowProc(wnd, msg, wparam, lparam);
}
