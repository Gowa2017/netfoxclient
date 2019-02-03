BaseConfig = BaseConfig or {}
--官网（资源服务器 + 网站）
BaseConfig.WEB_HTTP_URL ="http://222.85.178.240:9999" --"http://web.daoen1kj.cn"--"http://web.daoen1kj.cn""http://web.yoohg.xyz:35666""http://wwww.iwcx.cx:37951"--------"https://web.iwcx.cx/"
--是否为AppStore版本
BaseConfig.APPSTORE_VERSION = false 
--代理号标识（每一个包一个代理号 0无代理）

BaseConfig.AGENT_IDENTIFICATION = ""
--审核模式显示的游戏列表(非审核模式和审核模式都会显示的游戏，在审核模式下 只显示以下游戏,其他游戏不会显示)
BaseConfig.APPSTORE_GAME_LIST = {}
--只允许在审核模式显示的游戏列表 (非审核模式下,以下KINID的游戏将不会显示, 以下游戏"有且仅有"在审核模式下显示)
--谨慎添加,以下添加的游戏 在非审核模式下永远无法显示
BaseConfig.ONLY_APPSTORE_GAME = {389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 400}
--进入前后台通知（应用进入前台后台通知）
BaseConfig.RY_APPLICATION_NOTIFY	= "RY_APPLICATION_NOTIFY"
--二进制版本
BaseConfig.BASE_C_BINVERSION = 2
--程序版本
BaseConfig.BASE_C_VERSION = 6 --@app_version
--资源版本
BaseConfig.BASE_C_RESVERSION = 1 --@client_version211