--导入全局模块
import (".app.models.AppDF")
import (".model.base.Command")
import (".view.component.Toast")


--cc基础视图类
if (nil ~= cc) then
	cc.BaseNode = import(".view.base.BaseNode")
	cc.BaseLayer = import(".view.base.BaseLayer")
	cc.BaseScene = import(".view.base.BaseScene")
	cc.BaseLayout = import(".view.base.BaseLayout")
end