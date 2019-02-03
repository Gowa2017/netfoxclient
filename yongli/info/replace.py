#!/usr/bin/env python
#coding=utf-8

#
# 项目派生相关配置替换脚本
# 2016-9-20 by zhong
#

import sys
import traceback
import os
import re
from optparse import OptionParser
from sys import stdout
import json
import codecs

def working(options):
	#读取配置文件
	print "******"
	we_file = "../client/command/extra_command.lua"
	if True == os.path.exists(we_file):
		yf = codecs.open(we_file, "r", "utf-8")
		res = yf.read()
		yf.close()
		
		r_u = str(options.newver)
		reg = r'local EXTRA_(.+?) --@version_mark'
		cpListS = re.findall(re.compile(reg),res)
		for cps in cpListS:
			print "[EXTRA_COMMAND_VER] " + cps + " ==> " + r_u
			res = re.sub(cps, "COMMAND_VER = " + r_u, res, 1)
		
		yf = codecs.open(we_file, "wb", 'utf-8')
		yf.write(res)
		yf.close()
	
def main():
	if(sys.version_info[0] > 2):
		print("Found Python interpreter : %s\n"%sys.version)
		print("This script works only with Python version up to 2.x *but* not with an above version")
		sys.exit(1)
	else:
		parser = OptionParser(usage="%prog -i <IFNAME> -o <opname>", version="%prog 0.3")
		parser.add_option("--newver", "--newver", dest="newver", help="new version")
		(options, args) = parser.parse_args()
		options.newver = args[0]
		
		#开始替换
		working(options)


# -------------- main --------------
if __name__ == '__main__':
	try:
		main()
	except Exception as e:
		traceback.print_exc()
		sys.exit(1)