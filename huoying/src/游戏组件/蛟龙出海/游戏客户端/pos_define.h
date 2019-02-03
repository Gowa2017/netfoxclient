
#ifndef POS_DEFINE_H_
#define POS_DEFINE_H_

// 自己信息高度
const int kBottomHeight = 40;

// 炮背底座
const FPoint kPosBoard[GAME_PLAYER] = { {233,47}, {700,47}, {1155,47}, {1319,397}, {1137,721-kBottomHeight}, {670,721-kBottomHeight}, {215,721-kBottomHeight}, {49,371} };
const FPoint kPosBoard1[GAME_PLAYER] = { { 233, 215 }, { 700, 215 }, { 1155, 215 }, { 1165, 397 }, { 1137, 580 - kBottomHeight-10 }, {670,580 - kBottomHeight-10 }, { 215, 580 - kBottomHeight-10 }, { 200, 371 } };

// 炮辅助圈
const FPoint kPosDeco[GAME_PLAYER] = { {233,41}, {700,41}, {1155,41}, {1325,397}, {1137,727-kBottomHeight}, {670,727-kBottomHeight}, {215,727-kBottomHeight}, {42,371} };

// 炮后底座
const FPoint kPosSeat[GAME_PLAYER] = { {233,45}, {700,45}, {1155,45}, {1322,397}, {1137,722-kBottomHeight}, {670,722-kBottomHeight}, {215,722-kBottomHeight}, {45,371} };

// 炮筒子
const FPoint kPosGun[GAME_PLAYER] = { {233,77}, {700,77}, {1155,77}, {1299-10,397}, {1137,701-kBottomHeight-10}, {670,701-kBottomHeight-10}, {215,701-kBottomHeight-10}, {67+20,371} };

//不收回的炮筒子
const FPoint kPosGun1[GAME_PLAYER] = { { 233,85}, { 700, 85 }, { 1155, 85 }, { 1299 -18, 397 }, { 1137, 701 - kBottomHeight -18 }, { 670, 701 - kBottomHeight-18  }, { 215, 701 - kBottomHeight -18 }, { 67+15 , 371 } };
// 炮倍数
const FPoint kPosCannonMul[GAME_PLAYER] = { {233,25}, {700,25}, {1155,25}, {1340,397}, {1137,742-kBottomHeight}, {670,742-kBottomHeight}, {215,742-kBottomHeight}, {25,371} };

// 分值框
const FPoint kPosScoreBox[GAME_PLAYER] = { {80,45}, {528,45}, {995,45}, {1321,236}, {1295,723-kBottomHeight}, {828,723-kBottomHeight}, {375,723-kBottomHeight},{46,530} };

// 分数
const FPoint kPosScore[GAME_PLAYER] = { {22,45}, {470,45}, {935,45}, {1321,176}, {1353,723-kBottomHeight}, {888,723-kBottomHeight}, {434,723-kBottomHeight}, {46,590} };

// 金币堆
const FPoint kPosStack[GAME_PLAYER] = { {358,0}, {825,0}, {1280,0}, {1366,522}, {1012,768-kBottomHeight}, {545,768-kBottomHeight}, {90,768-kBottomHeight}, {0,246} };

// 能量炮卡片
const FPoint kPosCardIon[GAME_PLAYER] = { {180,162}, {650,162}, {1100,162}, {1204,345}, {1190,606-kBottomHeight}, {720,606-kBottomHeight}, {270,606-kBottomHeight}, {162,422} };

// 锁定卡片
const FPoint kPosLockFlag[GAME_PLAYER] = { {286,162}, {750,162}, {1210,162}, {1204,450}, {1085,606-kBottomHeight}, {620,606-kBottomHeight}, {160,606-kBottomHeight}, {162,320} };
//提示位置
const FPoint kPosHere[GAME_PLAYER] = { {233,67+73}, {700,67+73}, {1155,67+73}, {1299-73,397}, {1137,701-kBottomHeight-73}, {670,701-kBottomHeight-73}, {215,701-kBottomHeight-73}, {67+73,371} };
#endif	// POS_DEFINE_H_