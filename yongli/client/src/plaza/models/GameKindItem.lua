local GameKindItem = class("GameKindItem")

function GameKindItem:ctor(pData)
	self.wTypeID			=0	
	self.wJoinID			=0		
	self.wSortID			=0	
	self.wKindID			=0	
	self.wGameID			=0			
	self.wRecommend 			=0									
	self.wGameFlag 			=0		

	self.dwOnLineCount		=0
	self.dwAndroidCount		=0
	self.dwDummyCount		=0			
	self.dwFullCount		=0									

	self.dwSuportType		=0	
	self.szKindName 		= ""								
	self.szProcessName 		= ""
end


function GameKindItem:onInit(pData)
	if pData == nil then
		print("GameKindItem-onInit-null")
		return
	end
	
	self.wTypeID			= pData:readword()	
	self.wJoinID			= pData:readword()	
	self.wSortID			= pData:readword()	
	self.wKindID			= pData:readword()	
	self.wGameID			= pData:readword()
	self.wRecommend 		= pData:readword()	
	self.wGameFlag 			= pData:readword()	

	self.dwOnLineCount		= pData:readdword()
	self.dwAndroidCount		= pData:readdword()
	self.dwDummyCount		= pData:readdword()	
	self.dwFullCount		= pData:readdword()

	self.dwSuportType		= pData:readdword()
	self.szKindName 		= pData:readstring(32)
	self.szProcessName 		= pData:readstring(32)

	return self
end

return GameKindItem