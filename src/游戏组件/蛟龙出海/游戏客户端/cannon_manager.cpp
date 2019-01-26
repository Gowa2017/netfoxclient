
#include "StdAfx.h"
#include "cannon_manager.h"
#include "game_manager.h"
#include "pos_define.h"

const float kRotateRadius = 10.f;
const float kRotateAngle = 10.f * M_PI / 180.f;

CannonManager::CannonManager() : spr_cannon_base_(NULL), spr_cannon_plate_(NULL), rotate_angle_(0.f), show_seat_tip_(5.0f), spr_here0(NULL), spr_here1(NULL), mechairID(0), bhere(false) {
  hge_ = hgeCreate(HGE_VERSION);

  for (WORD i = 0; i < GAME_PLAYER; ++i) {
    current_angle_[i] = kChairDefaultAngle[i];
    current_bullet_kind_[i] = /*BULLET_KIND_1_NORMAL*/BULLET_KIND_3_NORMAL;
    current_mulriple_[i] = 1000;
    fish_score_[i] = 0;
  }
}

CannonManager::~CannonManager() {
  hge_->Release();
  for (int i = 0; i < 8; i++)
	  for (int j = 0; j < 8; j++)
	  {
	  delete ani_gun_fire_[i][j];
	  ani_gun_fire_[i][j] = NULL;
	  delete  ani_gun_fire_self_[i][j];
	  ani_gun_fire_self_[i][j] = NULL;
	  }
}

void CannonManager::SetFishScore(WORD chair_id, SCORE swap_fish_score) {
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return;
  fish_score_[chair_id] += swap_fish_score;
}

void CannonManager::ResetFishScore(WORD chair_id) {
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return;
  fish_score_[chair_id] = 0;
}

SCORE CannonManager::GetFishScore(WORD chair_id) {
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return 0;
  return fish_score_[chair_id];
}

bool CannonManager::LoadGameResource() {
  char file_name[MAX_PATH] = { 0 };

  hgeResourceManager* resource_manager = GameManager::GetInstance().GetResourceManager();
  spr_cannon_base_ = resource_manager->GetSprite("cannon_base");
  spr_cannon_plate_ = resource_manager->GetSprite("cannon_plate");
  spr_cannon_num_ = resource_manager->GetAnimation("cannon_num");
  spr_here0 = resource_manager->GetSprite("here0");
  spr_here1 = resource_manager->GetSprite("here1");

  for (WORD i = 0; i < GAME_PLAYER; ++i) {
    _snprintf(file_name, arraysize(file_name), "deco%d", i + 1);
    spr_cannon_deco_[i] = resource_manager->GetSprite(file_name);
  }

  for (int i = 0; i < BULLET_KIND_COUNT; ++i) {
    if (i >= BULLET_KIND_1_ION) {
	ZeroMemory(file_name,MAX_PATH);
	//1疾跑
      _snprintf(file_name, arraysize(file_name), "cannonIon%d", i - BULLET_KIND_1_ION + 1);
      spr_cannon_[i] = resource_manager->GetSprite(file_name);
	  ZeroMemory(file_name,MAX_PATH);
	  //2级炮
	  _snprintf(file_name, arraysize(file_name), "cannonSelfIon%d", i - BULLET_KIND_1_ION + 1);
	  spr_cannon_self_[i] = resource_manager->GetSprite(file_name);

    } else {
		ZeroMemory(file_name,MAX_PATH);
		//三级炮
      _snprintf(file_name, arraysize(file_name), "cannonNormal%d", i + 1);
      spr_cannon_[i] = resource_manager->GetSprite(file_name);

	  ZeroMemory(file_name,MAX_PATH);
	  //4级炮
	  _snprintf(file_name, arraysize(file_name), "cannonSelfNormal%d", i + 1);
	  spr_cannon_self_[i] = resource_manager->GetSprite(file_name);
    }
  }

  spr_score_box_ = resource_manager->GetSprite("score_box");
  spr_credit_num_ = resource_manager->GetAnimation("credit_num");
  spr_card_ion_ = resource_manager->GetSprite("card_ion");

  float x = 0.f, y = 0.f;

  for (WORD i=0; i<GAME_PLAYER; i++)
  {
	  for (WORD j=0; j<BULLET_KIND_COUNT; j++)
	  {
		  char szAni[64];
		  ZeroMemory(szAni, sizeof szAni);
		  _snprintf(szAni, sizeof szAni, "Ani_connon%d", j + 1);
		  hgeAnimation* ani = resource_manager->GetAnimation(szAni);
		  ani_gun_fire_[i][j] = new hgeAnimation(*ani);

		  ZeroMemory(szAni, sizeof szAni);
		  _snprintf(szAni, sizeof szAni, "Bni_connon%d", j + 1);
		  ani = resource_manager->GetAnimation(szAni);
		  ani_gun_fire_self_[i][j] = new hgeAnimation(*ani);
	  }
  }

  return true;
}

bool CannonManager::OnFrame(float delta_time) {
  rotate_angle_ += kRotateAngle;
  if (rotate_angle_ > M_PI * 2) {
    rotate_angle_ -= M_PI * 2;
  }

  if (show_seat_tip_>0)
  {
	  show_seat_tip_-=delta_time;
  }
  /*for (WORD i = 0; i < GAME_PLAYER; ++i) {
    ani_gun_fire_[i]->Update(delta_time);
  }*/
  for (WORD i=0; i<GAME_PLAYER; i++)
  {
	  for (WORD j=0; j<BULLET_KIND_COUNT; j++)
	  {
		  ani_gun_fire_[i][j]->Update(delta_time);
		  ani_gun_fire_self_[i][j]->Update(delta_time);
	  }
  }

  return false;
}

bool CannonManager::OnRender(float offset_x, float offset_y, float hscale, float vscale) {
	FPoint cannon_pos, muzzle_pos, cannon_pos1;
  static float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
  static float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));

  for (WORD i = 0; i < GAME_PLAYER; ++i) {
	  cannon_pos1 = GetCannonPos2(i, &muzzle_pos);
    cannon_pos = GetCannonPos(i, &muzzle_pos);
	spr_cannon_base_->RenderEx(cannon_pos1.x, cannon_pos1.y, kChairDefaultAngle[i], hscale, vscale);
    spr_cannon_deco_[i]->RenderEx(kPosDeco[i].x * hscale, kPosDeco[i].y * vscale, kChairDefaultAngle[i], hscale, vscale);

	
	if (show_seat_tip_>0&&mechairID==i)
	{
		if (bhere)
		{
			spr_here1->RenderEx(kPosHere[mechairID].x * hscale, kPosHere[mechairID].y * vscale, kChairDefaultAngle[i], hscale, vscale);		
		}
		else
		{
			 spr_here0->RenderEx(kPosHere[mechairID].x * hscale, kPosHere[mechairID].y * vscale, kChairDefaultAngle[i], hscale, vscale);		
		}
		bhere=!bhere;		
	}
    spr_cannon_plate_->RenderEx(kPosSeat[i].x * hscale, kPosSeat[i].y * vscale, kChairDefaultAngle[i], hscale, vscale);
    RenderCannonNum(i, current_mulriple_[i], kPosCannonMul[i].x * hscale,kPosCannonMul[i].y * vscale,
      kChairDefaultAngle[i], hscale, vscale);

	if (i != mechairID)
	{
		if (ani_gun_fire_[i][current_bullet_kind_[i]]->IsPlaying())
		{
			FPoint fPoint={0};
			GetMuzzlePosExcusion(i,&fPoint);
			ani_gun_fire_[i][current_bullet_kind_[i]]->RenderEx(fPoint.x,fPoint.y,current_angle_[i],hscale,vscale);
		}
		else
		{
			spr_cannon_[current_bullet_kind_[i]]->RenderEx(kPosGun[i].x * hscale, kPosGun[i].y * vscale, current_angle_[i], hscale, vscale);
		}
	}
	else
	{
		if (ani_gun_fire_self_[i][current_bullet_kind_[i]]->IsPlaying())
		{
			FPoint fPoint={0};
			GetMuzzlePosExcusion(i,&fPoint);
			//自己的炮
			ani_gun_fire_self_[i][current_bullet_kind_[i]]->RenderEx(fPoint.x/*kPosGun1[i].x * hscale*/, fPoint.y /*kPosGun1[i].y* vscale*/, current_angle_[i], hscale, vscale);
		}
		else
		{	
			spr_cannon_self_[current_bullet_kind_[i]]->RenderEx(kPosGun[i].x * hscale, kPosGun[i].y * vscale, current_angle_[i], hscale, vscale);
		}
	}
   /* if (ani_gun_fire_[i]->IsPlaying())
      ani_gun_fire_[i]->RenderEx(muzzle_pos.x, muzzle_pos.y, current_angle_[i], hscale, vscale);*/

    spr_score_box_->RenderEx(kPosScoreBox[i].x * hscale, kPosScoreBox[i].y * vscale, kChairDefaultAngle[i], hscale, vscale);
    RenderCreditNum(i, fish_score_[i], kPosScore[i].x * hscale,  kPosScore[i].y * vscale, kChairDefaultAngle[i], hscale, vscale);

    if (current_bullet_kind_[i] >= BULLET_KIND_1_ION) {
      float rotate_x = (kPosCardIon[i].x + kRotateRadius * cosf(rotate_angle_)) * hscale;
      float rotate_y = (kPosCardIon[i].y + kRotateRadius * sinf(rotate_angle_)) * vscale;
      spr_card_ion_->RenderEx(rotate_x, rotate_y, kChairDefaultAngle[i], hscale, vscale);
    }
  }
  return false;
}

void CannonManager::SetCurrentAngle(WORD chair_id, float angle) {
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return;

  current_angle_[chair_id] = angle;
}

void CannonManager::SetCannonMulriple(WORD chair_id, int mulriple) {
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return;

  current_mulriple_[chair_id] = mulriple;
}

float CannonManager::GetCurrentAngle(WORD chair_id) const {
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return 0.f;

  return current_angle_[chair_id];
}

FPoint CannonManager::GetCannonPos(WORD chair_id, FPoint* muzzle_pos) {
  assert(chair_id < GAME_PLAYER);
  static float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
  static float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));
  static float hscale = screen_width / kResolutionWidth;
  static float vscale = screen_height / kResolutionHeight;
  FPoint cannon_pos = { kPosBoard[chair_id].x * hscale, kPosBoard[chair_id].y * vscale };
  
  if (muzzle_pos != NULL) {
    GetMuzzlePosExcusion(chair_id, muzzle_pos);
  }

  return cannon_pos;
} 
FPoint CannonManager::GetCannonPos2(WORD chair_id, FPoint* muzzle_pos )
{
	assert(chair_id < GAME_PLAYER);
	static float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
	static float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));
	static float hscale = screen_width / kResolutionWidth;
	static float vscale = screen_height / kResolutionHeight;
	FPoint cannon_pos = { kPosBoard1[chair_id].x * hscale, kPosBoard1[chair_id].y * vscale };
	return cannon_pos;
}

FPoint CannonManager::GetCannonPos1(WORD chair_id, FPoint* muzzle_pos) {
	assert(chair_id < GAME_PLAYER);
	static float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
	static float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));
	static float hscale = screen_width / kResolutionWidth;
	static float vscale = screen_height / kResolutionHeight;
	FPoint cannon_pos = { kPosGun[chair_id].x * hscale, kPosGun[chair_id].y * vscale };

	if (muzzle_pos != NULL) {
		GetMuzzlePosExcusion(chair_id, muzzle_pos);
	}

	return cannon_pos;
}
void CannonManager::Switch(WORD chair_id, BulletKind bullet_kind) {
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return;

  current_bullet_kind_[chair_id] = bullet_kind;
}

BulletKind CannonManager::GetCurrentBulletKind(WORD chair_id) {
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return BULLET_KIND_2_NORMAL;

  return current_bullet_kind_[chair_id];
}

void CannonManager::Fire(WORD chair_id, BulletKind bullet_kind) {
  assert(chair_id < GAME_PLAYER);
  if (chair_id >= GAME_PLAYER) return;

  current_bullet_kind_[chair_id] = bullet_kind;
 // ani_gun_fire_[chair_id]->Play();
  if ( chair_id != mechairID) ani_gun_fire_[chair_id][bullet_kind]->Play();
  else ani_gun_fire_self_[chair_id][bullet_kind]->Play();
}

void CannonManager::RenderCannonNum(WORD chair_id, int num, float x, float y, float rot, float hscale, float vscale) {
  if (num < 0) num = -num;

  float num_width = spr_cannon_num_->GetWidth();
  float num_height = spr_cannon_num_->GetHeight();

  int num_count = 0;
  int num_temp = num;
  do {
    num_count++;
    num_temp /= 10;
  } while (num_temp > 0);

  if (chair_id == 0 || chair_id == 1 || chair_id == 2) x -= (num_count - 1) * num_width * hscale / 2;
  else if (chair_id == 3) y -= (num_count - 1) * num_width * hscale / 2;
  else if (chair_id == 7) y += (num_count - 1) * num_width * hscale / 2;
  else x += (num_count - 1) * num_width * hscale / 2;

  for (int i = 0; i < num_count; ++i) {
    spr_cannon_num_->SetFrame(num % 10);
    spr_cannon_num_->RenderEx(x, y, rot, hscale, vscale);
    if (chair_id <= 2) x += num_width * hscale;
    else if (chair_id == 3) y += num_width * hscale;
    else if (chair_id == 7) y -= num_width * hscale;
    else x -= num_width * hscale;
    num /= 10;
  }
}

//void CannonManager::GetMuzzlePosExcusion(WORD chair_id, FPoint* muzzle_pos) {
//  if (muzzle_pos == NULL) return;
//
//  static float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
//  static float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));
//  static float hscale = screen_width / kResolutionWidth;
//  static float vscale = screen_height / kResolutionHeight;
//  
//  float hotx, muzzle_excursion;
//  spr_cannon_[current_bullet_kind_[chair_id]]->GetHotSpot(&hotx, &muzzle_excursion);
//  muzzle_excursion *= vscale;
//  char file_name[MAX_PATH] = { 0 };
//  if (current_bullet_kind_[chair_id] <= BULLET_KIND_4_NORMAL) {
//    _snprintf(file_name, arraysize(file_name), "bullet1_norm%d", current_bullet_kind_[chair_id] + 1);
//  } else {
//    _snprintf(file_name, arraysize(file_name), "bullet%d_ion", current_bullet_kind_[chair_id] - BULLET_KIND_1_ION + 1);
//  }
//  hgeAnimation* ani = GameManager::GetInstance().GetResourceManager()->GetAnimation(file_name);
//  if (ani != NULL) {
//    float hoty;
//    ani->GetHotSpot(&hotx, &hoty);
//    muzzle_excursion += hoty * vscale;
//  }
//
//  float angle = current_angle_[chair_id];
//  if (chair_id == 0 || chair_id == 1 || chair_id == 2) {
//    if (angle < M_PI) {
//      muzzle_pos->x = kPosGun[chair_id].x * hscale + sinf(M_PI - angle) * muzzle_excursion;
//      muzzle_pos->y = kPosGun[chair_id].y * vscale + cosf(M_PI - angle) * muzzle_excursion;
//    } else {
//      muzzle_pos->x = kPosGun[chair_id].x * hscale - sinf(angle - M_PI) * muzzle_excursion;
//      muzzle_pos->y = kPosGun[chair_id].y * vscale + cosf(angle - M_PI) * muzzle_excursion;
//    }
//  }
//  else if (chair_id == 3) {
//    if (angle > (M_PI + M_PI_2)) {
//      muzzle_pos->x = kPosGun[chair_id].x * hscale - cosf(angle - M_PI - M_PI_2) * muzzle_excursion;
//      muzzle_pos->y = kPosGun[chair_id].y * vscale - sinf(angle - M_PI - M_PI_2) * muzzle_excursion;
//    } else {
//      muzzle_pos->x = kPosGun[chair_id].x * hscale - cosf(M_PI + M_PI_2 - angle) * muzzle_excursion;
//      muzzle_pos->y = kPosGun[chair_id].y * vscale + sinf(M_PI + M_PI_2 - angle) * muzzle_excursion;
//    }
//  } else if (chair_id == 5 || chair_id == 6 || chair_id == 4) {
//    if (angle == M_PI / 2) {
//      muzzle_pos->x = kPosGun[chair_id].x * hscale + muzzle_excursion;
//      muzzle_pos->y = kPosGun[chair_id].y * vscale;
//    } else if (angle > M_PI * 2) {
//      muzzle_pos->x = kPosGun[chair_id].x * hscale + sinf(angle - M_PI * 2) * muzzle_excursion;
//      muzzle_pos->y = kPosGun[chair_id].y * vscale - cosf(angle - M_PI * 2) * muzzle_excursion;
//    } else {
//      muzzle_pos->x = kPosGun[chair_id].x * hscale - sinf(2 * M_PI - angle) * muzzle_excursion;
//      muzzle_pos->y = kPosGun[chair_id].y * vscale - cosf(2 * M_PI - angle) * muzzle_excursion;
//    }
//  } else {
//    if (angle == M_PI * 2) {
//      muzzle_pos->x = kPosGun[chair_id].x * hscale;
//      muzzle_pos->y = kPosGun[chair_id].y * vscale - muzzle_excursion;
//    } else if (angle > M_PI * 2) {
//      muzzle_pos->x = kPosGun[chair_id].x * hscale + cosf(M_PI * 2 + M_PI_2 - angle) * muzzle_excursion;
//      muzzle_pos->y = kPosGun[chair_id].y * vscale - sinf(M_PI * 2 + M_PI_2 - angle) * muzzle_excursion;
//    } else {
//      muzzle_pos->x = kPosGun[chair_id].x * hscale + cosf(angle - M_PI_2) * muzzle_excursion;
//      muzzle_pos->y = kPosGun[chair_id].y * vscale + sinf(angle - M_PI_2) * muzzle_excursion;
//    }
//  }
//
//}
void CannonManager::GetMuzzlePosExcusion(WORD chair_id, FPoint* muzzle_pos) {
	if (muzzle_pos == NULL) return;

	static float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
	static float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));
	static float hscale = screen_width / kResolutionWidth;
	static float vscale = screen_height / kResolutionHeight;

	float hotx, muzzle_excursion;
	spr_cannon_[current_bullet_kind_[chair_id]]->GetHotSpot(&hotx, &muzzle_excursion);
	muzzle_excursion *= vscale;
	char file_name[MAX_PATH] = { 0 };
	if (current_bullet_kind_[chair_id] <= BULLET_KIND_4_NORMAL) {
		_snprintf(file_name, arraysize(file_name), "bullet1_norm%d", current_bullet_kind_[chair_id] + 1);
	} else {
		_snprintf(file_name, arraysize(file_name), "bullet%d_ion", current_bullet_kind_[chair_id] - BULLET_KIND_1_ION + 1);
	}
	hgeAnimation* ani = GameManager::GetInstance().GetResourceManager()->GetAnimation(file_name);
	if (ani != NULL) {
		float hoty;
		ani->GetHotSpot(&hotx, &hoty);
		muzzle_excursion += hoty * vscale;
	}

	muzzle_excursion /= 5.0;

	float angle = current_angle_[chair_id];
	if (chair_id == 0 || chair_id == 1 || chair_id == 2) {
		if (angle < M_PI) {
			muzzle_pos->x = kPosGun[chair_id].x * hscale + sinf(M_PI - angle) * muzzle_excursion;;
			muzzle_pos->y = kPosGun[chair_id].y * vscale + cosf(M_PI - angle) * muzzle_excursion;;
		} else {
			muzzle_pos->x = kPosGun[chair_id].x * hscale - sinf(angle - M_PI) * muzzle_excursion;;
			muzzle_pos->y = kPosGun[chair_id].y * vscale + cosf(angle - M_PI) * muzzle_excursion;;
		}
	}
	else if (chair_id == 3) {
		if (angle > (M_PI + M_PI_2)) {
			muzzle_pos->x = kPosGun[chair_id].x * hscale - cosf(angle - M_PI - M_PI_2) * muzzle_excursion;;
			muzzle_pos->y = kPosGun[chair_id].y * vscale - sinf(angle - M_PI - M_PI_2) * muzzle_excursion;;
		} else {
			muzzle_pos->x = kPosGun[chair_id].x * hscale - cosf(M_PI + M_PI_2 - angle) * muzzle_excursion;;
			muzzle_pos->y = kPosGun[chair_id].y * vscale + sinf(M_PI + M_PI_2 - angle) * muzzle_excursion;;
		}
	} else if (chair_id == 5 || chair_id == 6 || chair_id == 4) {
		if (angle == M_PI / 2) {
			muzzle_pos->x = kPosGun[chair_id].x * hscale + muzzle_excursion;;
			muzzle_pos->y = kPosGun[chair_id].y * vscale;
		} else if (angle > M_PI * 2) {
			muzzle_pos->x = kPosGun[chair_id].x * hscale + sinf(angle - M_PI * 2) * muzzle_excursion;;
			muzzle_pos->y = kPosGun[chair_id].y * vscale - cosf(angle - M_PI * 2) * muzzle_excursion;;
		} else {
			muzzle_pos->x = kPosGun[chair_id].x * hscale - sinf(2 * M_PI - angle) * muzzle_excursion;;
			muzzle_pos->y = kPosGun[chair_id].y * vscale - cosf(2 * M_PI - angle) * muzzle_excursion;;
		}
	} else {
		if (angle == M_PI * 2) {
			muzzle_pos->x = kPosGun[chair_id].x * hscale;
			muzzle_pos->y = kPosGun[chair_id].y * vscale - muzzle_excursion;;
		} else if (angle > M_PI * 2) {
			muzzle_pos->x = kPosGun[chair_id].x * hscale + cosf(M_PI * 2 + M_PI_2 - angle) * muzzle_excursion;;
			muzzle_pos->y = kPosGun[chair_id].y * vscale - sinf(M_PI * 2 + M_PI_2 - angle) * muzzle_excursion;;
		} else {
			muzzle_pos->x = kPosGun[chair_id].x * hscale + cosf(angle - M_PI_2) * muzzle_excursion;;
			muzzle_pos->y = kPosGun[chair_id].y * vscale + sinf(angle - M_PI_2) * muzzle_excursion;;
		}
	}
}

void CannonManager::GetBackblowExcusion(WORD chair_id, FPoint* muzzle_pos) {
  if (muzzle_pos == NULL) return;

  static float screen_width = static_cast<float>(hge_->System_GetState(HGE_SCREENWIDTH));
  static float screen_height = static_cast<float>(hge_->System_GetState(HGE_SCREENHEIGHT));
  static float hscale = screen_width / kResolutionWidth;
  static float vscale = screen_height / kResolutionHeight;

  float excursion = 8 * vscale;

  float angle = current_angle_[chair_id];
  if (chair_id == 0 || chair_id == 1 || chair_id == 2) {
    if (angle < M_PI) {
		muzzle_pos->x = kPosGun[chair_id].x /** hscale - sinf(M_PI - angle) * excursion;*/;
		muzzle_pos->y = kPosGun[chair_id].y /** vscale - cosf(M_PI - angle) * excursion;*/;
    } else {
		muzzle_pos->x = kPosGun[chair_id].x /** hscale + sinf(angle - M_PI) * excursion;*/;
		muzzle_pos->y = kPosGun[chair_id].y /** vscale - cosf(angle - M_PI) * excursion;*/;
    }
  }
  else if (chair_id == 3) {
    if (angle > (M_PI + M_PI_2)) {
		muzzle_pos->x = kPosGun[chair_id].x /** hscale + cosf(angle - M_PI - M_PI_2) * excursion;*/;
		muzzle_pos->y = kPosGun[chair_id].y /** vscale + sinf(angle - M_PI - M_PI_2) * excursion;*/;
    } else {
		muzzle_pos->x = kPosGun[chair_id].x /** hscale + cosf(M_PI + M_PI_2 - angle) * excursion;*/;
		muzzle_pos->y = kPosGun[chair_id].y /** vscale - sinf(M_PI + M_PI_2 - angle) * excursion;*/;
    }
  } else if (chair_id == 5 || chair_id == 6 || chair_id == 4) {
    if (angle == M_PI / 2) {
		muzzle_pos->x = kPosGun[chair_id].x /** hscale - excursion;*/;
		muzzle_pos->y = kPosGun[chair_id].y /** vscale;*/;
    } else if (angle > M_PI * 2) {
		muzzle_pos->x = kPosGun[chair_id].x /** hscale - sinf(angle - M_PI * 2) * excursion;*/;
			muzzle_pos->y = kPosGun[chair_id].y /** vscale + cosf(angle - M_PI * 2) * excursion;*/;
    } else {
		muzzle_pos->x = kPosGun[chair_id].x /** hscale + sinf(2 * M_PI - angle) * excursion;*/;
		muzzle_pos->y = kPosGun[chair_id].y/* * vscale + cosf(2 * M_PI - angle) * excursion;*/;
    }
  } else {
    if (angle == M_PI * 2) {
		muzzle_pos->x = kPosGun[chair_id].x /** hscale;*/;
		muzzle_pos->y = kPosGun[chair_id].y /** vscale + excursion;*/;
    } else if (angle > M_PI * 2) {
		muzzle_pos->x = kPosGun[chair_id].x /** hscale - cosf(M_PI * 2 + M_PI_2 - angle) * excursion;*/;
		muzzle_pos->y = kPosGun[chair_id].y /** vscale + sinf(M_PI * 2 + M_PI_2 - angle) * excursion;*/;
    } else {
		muzzle_pos->x = kPosGun[chair_id].x /** hscale - cosf(angle - M_PI_2) * excursion;*/;
		muzzle_pos->y = kPosGun[chair_id].y /** vscale - sinf(angle - M_PI_2) * excursion;*/;
    }
  }
}

void CannonManager::RenderCreditNum(WORD chair_id, SCORE num, float x, float y, float rot, float hscale, float vscale) {
  if (num < 0) return;

  static const int kMaxShowNumCount = 7;

  float num_width = spr_credit_num_->GetWidth();
  float num_height = spr_credit_num_->GetHeight();

  int num_count = 0;
  SCORE num_temp = num;
  do {
    num_count++;
    num_temp /= 10;
  } while (num_temp > 0);
  if (num_count > kMaxShowNumCount) {
    hscale *= (float)(kMaxShowNumCount) / (float)(num_count);
  }

  for (int i = 0; i < num_count; ++i) {
    spr_credit_num_->SetFrame(static_cast<int>(num % 10));
    spr_credit_num_->RenderEx(x, y, rot, hscale, vscale);
    if (chair_id == 0 || chair_id == 1 || chair_id == 2) x += num_width * hscale;
    else if (chair_id == 3) y += num_width * hscale;
    else if (chair_id == 7) y -= num_width * hscale;
    else x -= num_width * hscale;
    num /= 10;
  }
}
