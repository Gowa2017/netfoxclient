
#include "stdafx.h"
#include <process.h>
#include "resource.h"
#include "hge.h"
#include "hgesprite.h"
#include "game_manager.h"
#include "gui_manager.h"
#include "MiniDumper.h"
#include "magic/platform_win.h"
#include "magic/mp_wrap.h"

#define DEF_SCREEN_WIDTH	  1920		// 默认屏宽
#define DEF_SCREEN_HEIGHT	  1080			// 默认屏高

int screen_width_ = DEF_SCREEN_WIDTH;
int screen_height_ = DEF_SCREEN_HEIGHT;

HGE* hge_ = NULL;
MiniDumper* g_mini_dumper = NULL;

// 资源进度条相关
volatile int loading_result = 0; // 0 正在加载 1 加载成功 -1 加载失败
int loading_progress_ = 0;
hgeSprite* spr_logo_ = NULL;
HTEXTURE tex_logo_ = NULL;
hgeSprite* spr_loading_bar_ = NULL;
HTEXTURE tex_loading_bar_ = NULL;
HANDLE loading_handle_ = NULL;

// 资源加载线程 暂时性处理
// TODO:把content.res里的resgroup设置为不同(由小到大),然后调用Precache来实现进度
unsigned __stdcall LoadingThread(void* param) {
  LPTSTR cmd_line = (LPTSTR)param;
  if (GameManager::GetInstance().LoadGameResource(loading_progress_)) {
    loading_progress_ = 144;
    loading_result = 1;
  } else {
    loading_result = -1;
  }
  return 0;
}
//定义帧回调函数
bool FrameFunc() {
  float delta_time = hge_->Timer_GetDelta();//获取自上次调用此函数到此次调用此函数的时间间隔
  
  GuiManager::GetInstance().GetGui()->logic();
 // 什么是帧回调函数？有什么用处？
	//  A：这个帧回调函数是用来表示游戏逻辑的地方，会在后面的代码中设置这个函数被调用的频率，
  //一般以每秒多少次表示。在这个函数中，我们就可以进行游戏逻辑的编写，注意此函数在返回true的情况下，
  //游戏的逻辑将结束，而在返回false的情况下只是代表此次的函数调用结束了，并不是游戏的逻辑结束了(以后不会再被调用，称为逻辑结束)。
  if (loading_result == 1) {
    if (GameManager::GetInstance().OnFrame(delta_time)) return true;
  } else if (loading_result == -1) {
    hge_->System_Log("资源加载失败");
	
    return true;
  }

  MP_Manager::GetInstance().UpdateByTimer();

  return false;
}

bool RenderFunc() {
  hge_->Gfx_BeginScene();
  hge_->Gfx_Clear(0);

  if (loading_result == 1) {
    GameManager::GetInstance().OnRender(static_cast<float>(screen_width_) / kResolutionWidth, static_cast<float>(screen_height_) / kResolutionHeight);
  } else {
    float hscale = static_cast<float>(screen_width_) / kResolutionWidth;
    float vscale = static_cast<float>(screen_height_) / kResolutionHeight;

    spr_logo_->RenderEx(0.0f, 0.0f, 0.0f, hscale, vscale);
    float bar_ypos = static_cast<float>(screen_height_) - hge_->Texture_GetHeight(tex_loading_bar_) * vscale;
    float bar_width = static_cast<float>(hge_->Texture_GetWidth(tex_loading_bar_));
    for (int i = 0; i < loading_progress_; ++i) {
      spr_loading_bar_->RenderEx(i * bar_width * hscale, bar_ypos, 0.0f, hscale, vscale);
    }
  }

  GuiManager::GetInstance().GetGui()->draw();

  // 只有几个效果 直接写这了.
  MP_Manager& MP=MP_Manager::GetInstance();
  HM_EMITTER hmEmitter=MP.GetFirstEmitter();
  while (hmEmitter) {
    MP_Emitter* emitter=MP.GetEmitter(hmEmitter);
    emitter->Render();
    hmEmitter=MP.GetNextEmitter(hmEmitter);
  }

  hge_->Gfx_EndScene();

  return false;
}

bool FocusGainFunc() {
  GameManager::GetInstance().set_game_active(true);
  return false;
}

bool FocusLostFunc() {
  GameManager::GetInstance().set_game_active(false);
  return false;
}

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR cmd_line, int) {

  g_mini_dumper = new MiniDumper(true);
  hge_ = hgeCreate(HGE_VERSION);

  //assert(0);  // for debug

  RECT work_area;
  SystemParametersInfo(SPI_GETWORKAREA, 0, &work_area, 0);
  int screen_real_height = work_area.bottom - work_area.top - GetSystemMetrics(SM_CYFIXEDFRAME) * 2 - GetSystemMetrics(SM_CYCAPTION);
  screen_width_ = GetSystemMetrics(SM_CXSCREEN);
  screen_height_ = GetSystemMetrics(SM_CXSCREEN) * kResolutionHeight / kResolutionWidth;
  if (screen_height_ > screen_real_height) {
    screen_width_ = screen_real_height * kResolutionWidth / kResolutionHeight;
    screen_height_ = screen_real_height;
  }

  //screen_width_ = 1920;
  //screen_height_ = 1080;
  hge_->System_SetState(HGE_LOGFILE, "lkpy.log");
  hge_->System_SetState(HGE_SHOWSPLASH, false);
  hge_->System_SetState(HGE_FRAMEFUNC, FrameFunc);
  hge_->System_SetState(HGE_RENDERFUNC, RenderFunc);
  hge_->System_SetState(HGE_FOCUSGAINFUNC, FocusGainFunc);
  hge_->System_SetState(HGE_FOCUSLOSTFUNC, FocusLostFunc);
  hge_->System_SetState(HGE_TITLE, "李逵捕鱼");
  hge_->System_SetState(HGE_ICON, MAKEINTRESOURCEA(IDI_FISH));
  hge_->System_SetState(HGE_USESOUND, true);
  hge_->System_SetState(HGE_WINDOWED, true);
  hge_->System_SetState(HGE_SCREENWIDTH, screen_width_);
  hge_->System_SetState(HGE_SCREENHEIGHT, screen_height_);
  hge_->System_SetState(HGE_SCREENBPP, 32);
  hge_->System_SetState(HGE_HIDEMOUSE, true);
  hge_->System_SetState(HGE_FPS, 30);
  hge_->System_SetState(HGE_DONTSUSPEND, true);
  hge_->System_SetState(HGE_RESENCRYPT, true);

  // 资源包
  //hge_->Resource_AttachPack("fish\\resource.pak", "123456");

  if (hge_->System_Initiate()) {
	
    HWND hwnd = hge_->System_GetState(HGE_HWND);
    RECT rect;
    GetWindowRect(hwnd, &rect);
    SetWindowPos(hwnd, NULL, rect.left, 0, 0, 0, SWP_NOSIZE);

    DWORD size;
    void* data;
    data = hge_->Resource_Load("lkpy\\images\\logo.fish", &size);
    tex_logo_ = hge_->Texture_Load(static_cast<const char*>(data), size);
    hge_->Resource_Free(data);
    spr_logo_ = new hgeSprite(tex_logo_, 0.f, 0.f, static_cast<float>(hge_->Texture_GetWidth(tex_logo_)),
      static_cast<float>(hge_->Texture_GetHeight(tex_logo_)));
    data = hge_->Resource_Load("lkpy\\images\\loading_bar.fish", &size);
    tex_loading_bar_ = hge_->Texture_Load(static_cast<const char*>(data), size);
    hge_->Resource_Free(data);
    spr_loading_bar_ = new hgeSprite(tex_loading_bar_, 0.f, 0.f, static_cast<float>(hge_->Texture_GetWidth(tex_loading_bar_)),
      static_cast<float>(hge_->Texture_GetHeight(tex_loading_bar_)));

    GuiManager::GetInstance().Initialize(screen_width_, screen_real_height);
    GameManager::GetInstance().InitClientKernel(cmd_line);
    loading_handle_ = (HANDLE)::_beginthreadex(NULL, 0, LoadingThread, NULL, 0, NULL);

    MP_Device_WRAP device(screen_width_,screen_height_, hge_);
    device.Create();

    MP_Manager& MP=MP_Manager::GetInstance();

    MP_Platform* platform=new MP_Platform_WINDOWS;
    MP.Initialization(platform, MAGIC_INTERPOLATION_ENABLE, MAGIC_NOLOOP, MAGIC_CHANGE_EMITTER_DEFAULT, 1366, 768, 1,
      (float)screen_width_ / (float)kResolutionWidth, 0.1f, true);

    MP.LoadAllEmitters();
	
    MP.RefreshAtlas();

    MP.CloseFiles();

    MP.Stop();

    hge_->System_Start();

    MP.Destroy();
    device.Destroy();

    delete spr_loading_bar_;
    hge_->Texture_Free(tex_loading_bar_);
    delete spr_logo_;
    hge_->Texture_Free(tex_logo_);
    if (loading_result == 0) TerminateThread(loading_handle_, 0); // 暂时性处理
    if (loading_handle_ != NULL) ::CloseHandle(loading_handle_);

    GuiManager::GetInstance().DelInstance();
    GameManager::GetInstance().GetClientKernel()->IntermitConnect();
    GameManager::GetInstance().DelInstance();
  }

  //hge_->Resource_RemoveAllPacks();
  //AfxMessageBox(TEXT("233"), MB_YESNO);
  hge_->System_Shutdown();
  hge_->Release();
  delete g_mini_dumper;

  return 0;
}