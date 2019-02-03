local ClientUserItem = class("ClientUserItem")

function ClientUserItem:ctor()
	self.dwGameID		= 0
	self.dwUserID		= 0

	self.wFaceID 		= 0
	self.dwCustomID		= 0

	self.cbGender		= yl.GENDER_MANKIND
	self.cbMemberOrder	= 0

	self.wTableID		= yl.INVALID_TABLE
	self.wChairID		= yl.INVALID_CHAIR
	self.cbUserStatus 	= 0

	self.lScore 			= 0
	self.lIngot 			= 0
	self.dBeans 			= 0
	self.lGrade				= 0
	self.lInsure			= 0
	self.bAndroid = false				--是否为机器人
	
	self.dwWinCount		= 0
	self.dwLostCount	= 0
	self.dwDrawCount	= 0
	self.dwFleeCount	= 0
	self.dwExperience	= 0
	self.dwIpAddress	= 0
	self.szAdressLocation = ""		--归属地
	self.szNickName = ""
	self.szChangeLogonIP = ""
	self.dwAgentID = 0
end

function ClientUserItem:testlog()
	print("ClientUserItem*******************************************")
	print("dwGameID="..self.dwGameID)
	print("dwUserID="..self.dwUserID)

	print("wFaceID="..self.wFaceID)
	print("dwCustomID="..self.dwCustomID)

	print("cbGender="..self.cbGender)
	print("cbMemberOrder="..self.cbMemberOrder)

	print("wTableID="..self.wTableID)
	print("wChairID="..self.wChairID)
	print("cbUserStatus="..self.cbUserStatus)

	print("lScore="..self.lScore)
	print("lIngot="..self.lIngot)
	print("dBeans="..self.dBeans)

	print("dwWinCount="..self.dwWinCount)
	print("dwLostCount="..self.dwLostCount)
	print("dwDrawCount="..self.dwDrawCount)
	print("dwFleeCount="..self.dwFleeCount)
	print("dwExperience="..self.dwExperience)

	print("szNickName="..self.szNickName)
	print("*********************************************************")
end

return ClientUserItem