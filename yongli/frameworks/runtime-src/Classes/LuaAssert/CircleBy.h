//
//  CircleBy.hpp
//  GameProject
//
//  Created by 汤淼 on 16/9/22.
//
//

#ifndef CircleBy_h
#define CircleBy_h
#include <stdio.h>
#include "cocos2d.h"
USING_NS_CC;

int register_all_Circleasset();
class CirCleBy:public ActionInterval
{
    
public:
    //参数 : 圆心 , 半径
    static CirCleBy *create(float t,Vec2 circleCenter,float radius);
    
    bool init(float t,Vec2 circleCenter,float radius);
    
    virtual void update(float dt) ;
    
protected:
    Vec2        m_circleCenter;
    float       m_raduis;
    float       m_radian;
    float       m_moveTimes;
    
};

#endif /* CircleBy_hpp */
