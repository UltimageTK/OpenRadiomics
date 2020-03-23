/******************************************************** 
* @file    : DrawBase.cpp
* @brief   : 
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-5-8
*********************************************************/

#include "DrawBase.h"

PEN_TYPE    DrawBase::m_emPenType = PenNone;
int         DrawBase::m_nAlphaValue = 100;

DrawBase::DrawBase()
{

}

DrawBase::~DrawBase()
{

}

PEN_TYPE DrawBase::getCurPenType()
{
    return m_emPenType;
}

void DrawBase::setCurPenType(PEN_TYPE emPenType)
{
    m_emPenType = emPenType;    
}

void DrawBase::setAlphaValue(int nAlphaValue)
{
    m_nAlphaValue = nAlphaValue;
}

