#ifndef CMD_HTTPSERVER_HEAD_FILE
#define CMD_HTTPSERVER_HEAD_FILE

#pragma pack(1)

//平台定义
#include "..\..\全局定义\Platform.h"

//////////////////////////////////////////////////////////////////////////////////

//后台解散桌子
#define  SUB_CS_C_CLEARUSER				1									//清理玩家
//查询在线信息
#define SUB_CS_C_HTTP_QUERY_ONLINE		2									//查询用户在线
//网站获取虚拟在线人数
#define SUB_CS_C_HTTP_QUERY_DUMMY_ONLINE 3									//查询虚拟人数
//网站修改虚拟在线时间段有变更
#define SUB_CS_C_HTTP_CHANGE_DUMMY_ONLINE 5									//网站修改虚拟在线时间段有变更
//强制用户下线
#define SUB_CS_C_HTTP_FORCE_OFFLINE		6									//强制用户下线

#pragma pack()

#endif