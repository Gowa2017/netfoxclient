# /////////////////////////////////////////////////////////////
#	iOS_Build_SH 2016-04-12 11:00:25 by: Ravioyla
# /////////////////////////////////////////////////////////////

# 加密出错提示
function CipherSrcError() {
echo $1
read -n 1
exit
}

# 编译程序
function Compile() {
echo "*********************cocos编译iOS*********************"

Cur_Dir=$(pwd)
# Build MyLuaGame for ios with release mode. Use the code sign identity "iPhone Distribution:xxxxxxxx".
cocos compile -s $Cur_Dir -p ios -m release --sign-identity "iPhone Developer: HuiFeng Jiang (V5UVK59M4D)"

if [ $? -eq 1 ]
then
CipherSrcError "cocos编译失败"
fi

if [ $? -eq 0 ]
then
CipherSrcError "编译成功"
fi
}

# 压缩文件
function ZipFile() {
echo "*********************文件zip*********************"
Cur_Dir=$(pwd)

# 复制资源文件
cp -Rf $Cur_Dir/../client/game $Cur_Dir/../ciphercode
cp -Rf $Cur_Dir/../client/client $Cur_Dir/../ciphercode
cp -Rf $Cur_Dir/../client/base $Cur_Dir/../ciphercode

# 删除所有.lua文件
find $Cur_Dir/../ciphercode -type f -name '*.lua' -delete

#    # 删除旧的zip文件
#    rm $Cur_Dir/../res/game.zip
#    rm $Cur_Dir/../res/client.zip
#
# 压缩文件
pushd $Cur_Dir/../ciphercode
zip -r $Cur_Dir/../ciphercode/base/res/game.zip game
if [ $? -eq 1 ]
then
CipherSrcError "压缩game出错"
fi

zip -r $Cur_Dir/../ciphercode/base/res/client.zip client
if [ $? -eq 1 ]
then
CipherSrcError "压缩client出错"
fi

if [ $? -eq 0 ]
then
#    Compile
CipherSrcError "Complete"
fi
}

# 主体代码
echo "*********************lua文件加密*********************"
Cur_Dir=$(pwd)
rm -rf $Cur_Dir/../ciphercode/client
rm -rf $Cur_Dir/../ciphercode/game
rm -rf $Cur_Dir/../ciphercode/base

cocos luacompile -s $Cur_Dir/../client/client -d $Cur_Dir/../ciphercode/client -e -k RY_QP_MBCLIENT_!2016 -b RY_QP_2016 --disable-compile
if [ $? -eq 1 ]
then
CipherSrcError "lua文件加密出错"
fi

cocos luacompile -s $Cur_Dir/../client/game -d $Cur_Dir/../ciphercode/game -e -k RY_QP_MBCLIENT_!2016 -b RY_QP_2016 --disable-compile
if [ $? -eq 1 ]
then
CipherSrcError "lua文件加密出错"
fi

cocos luacompile -s $Cur_Dir/../client/base -d $Cur_Dir/../ciphercode/base -e -k RY_QP_MBCLIENT_!2016 -b RY_QP_2016 --disable-compile
if [ $? -eq 1 ]
then
CipherSrcError "lua文件加密出错"
fi

if [ $? -eq 0 ]
then
ZipFile
fi

read -n 1
exit
