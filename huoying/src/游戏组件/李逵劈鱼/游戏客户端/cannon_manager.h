
#ifndef CANNON_MANAGER_H_
#define CANNON_MANAGER_H_
#pragma once

#include "hge.h"
#include "hgesprite.h"
#include "hgeanim.h"

class CannonManager {
 public:
  CannonManager();
  ~CannonManager();

  bool LoadGameResource();

  bool OnFrame(float delta_time);
  bool OnRender(float offset_x, float offset_y, float hscale, float vscale);

  void SetCurrentAngle(WORD chair_id, float angle);
  void SetCannonMulriple(WORD chair_id, int mulriple);
  float GetCurrentAngle(WORD chair_id) const;
  FPoint GetCannonPos(WORD chair_id, FPoint* muzzle_pos = NULL);
  FPoint GetCannonPos1(WORD chair_id, FPoint* muzzle_pos = NULL);
  FPoint GetCannonPos2(WORD chair_id, FPoint* muzzle_pos = NULL);
  void Switch(WORD chair_id, BulletKind bullet_kind);
  BulletKind GetCurrentBulletKind(WORD chair_id);
  void Fire(WORD chair_id, BulletKind bullet_kind);

  void SetFishScore(WORD chair_id, SCORE swap_fish_score);
  void ResetFishScore(WORD chair_id);
  SCORE GetFishScore(WORD chair_id);
  void SetMeChairID(WORD chair_id){mechairID=chair_id;}

 private:
  void RenderCannonNum(WORD chair_id, int num, float x, float y, float rot, float hscale, float vscale);
  void GetMuzzlePosExcusion(WORD chair_id, FPoint* muzzle_pos);
  void GetBackblowExcusion(WORD chair_id, FPoint* backblow_pos);
  void RenderCreditNum(WORD chair_id, SCORE num, float x, float y, float rot, float hscale, float vscale);

 private:
  HGE* hge_;

  hgeSprite* spr_cannon_base_;
  hgeSprite* spr_cannon_plate_;
  hgeSprite* spr_cannon_deco_[GAME_PLAYER];
  //别人没开炮的状态图
  hgeSprite* spr_cannon_[BULLET_KIND_COUNT];
  //自己没开炮的状态图
  hgeSprite* spr_cannon_self_[BULLET_KIND_COUNT];
  hgeAnimation* spr_cannon_num_;
  hgeSprite* spr_score_box_;
  hgeAnimation* spr_credit_num_;
  hgeSprite* spr_card_ion_;
  float rotate_angle_;
  //hgeAnimation* ani_gun_fire_[GAME_PLAYER];
  //变身的普通的别人的开火炮图
  hgeAnimation* ani_gun_fire_[GAME_PLAYER][BULLET_KIND_COUNT];
  //普通的和变身的 自己的开火炮图
  hgeAnimation* ani_gun_fire_self_[GAME_PLAYER][BULLET_KIND_COUNT];
  hgeSprite* spr_here0;
  hgeSprite* spr_here1;
  bool bhere;

  float show_seat_tip_;
  float current_angle_[GAME_PLAYER];
  //子弹种类1~4
  BulletKind current_bullet_kind_[GAME_PLAYER];
  //当前倍率
  int current_mulriple_[GAME_PLAYER];
  WORD mechairID;
  //当前鱼钱
  SCORE fish_score_[GAME_PLAYER];
};

#endif // CANNON_MANAGER_H_
