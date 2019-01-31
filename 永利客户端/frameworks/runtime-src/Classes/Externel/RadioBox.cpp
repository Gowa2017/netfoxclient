#include "RadioBox.h"

static const float OUT_OF_BOUNDARY_BREAKING_FACTOR = 0.05f;

#define fltEqualZero(point) (fabsf(point.x) <= 0.0001f && fabsf(point.y) <= 0.0001f)

RadioBox::RadioBox()
{
	m_pRadioGroup = nullptr;
	m_pLastSelected = nullptr;
	m_SelectScaleX = 1;
	m_SelectScaleY = 1;

	m_SolidPaddingLeftX = 0;
	m_SolidPaddingTopY = 0;
	m_SolidPaddingRightX = 0;
	m_SolidPaddingBottomY = 0;

	m_MarginX = 0;
	m_GeneralMarginX = 0;

	m_bFocus = true;

	//m_MarginY = 0;

	m_nShowRadioCount = 1;

	m_RadioFocusCallBack = nullptr;

	m_bSelectedEvent = true;
}

RadioBox::~RadioBox()
{

}

bool RadioBox::init()
{
	if (!ui::ScrollView::init())
		return false;

	m_pRadioGroup = ui::RadioButtonGroup::create();
	this->addChild(m_pRadioGroup);

	m_pPanel = Layout::create();
	this->addChild(m_pPanel);


	this->setDirection(ScrollView::Direction::HORIZONTAL);
	this->setScrollBarEnabled(false);
	this->setBounceEnabled(true);

	m_pRadioGroup->addEventListener(std::bind(&RadioBox::onSelectedRadio, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	return true;
}

void RadioBox::setSelectedScale(const float scaleX, const float scaleY)
{
	m_SelectScaleX = scaleX;
	m_SelectScaleY = scaleY;
}

void RadioBox::setSolidPaddingLeftX(const float x)
{
	m_SolidPaddingLeftX = x;

	retifyUI();
}

void RadioBox::setSolidPaddingTopY(const float y)
{
	m_SolidPaddingTopY = y;

	retifyUI();
}

void RadioBox::setSolidPaddingRightX(const float x)
{
	m_SolidPaddingRightX = x;

	retifyUI();
}

void RadioBox::setSolidPaddingBottomY(const float y)
{
	m_SolidPaddingBottomY = y;

	retifyUI();
}

void RadioBox::setMarginX(const float x)
{
	m_MarginX = x;

	retifyUI();
}

//void RadioBox::setMarginY(const float y)
//{
//	m_MarginY = y;
//
//	retifyUI();
//}

void RadioBox::setShowRadioCount(const unsigned int count)
{
	m_nShowRadioCount = count;

	retifyUI();
}

void RadioBox::addRadioButton(RadioButton* radioButton, bool bRetify)
{
	assert(m_pRadioGroup != nullptr);
	if (m_pRadioGroup == nullptr)
		return;
	m_pRadioGroup->addRadioButton(radioButton);
	m_pPanel->addChild(radioButton);

	auto pLastSelectedRadio = m_pRadioGroup->getRadioButtonByIndex(m_pRadioGroup->getSelectedButtonIndex());
	//m_pPanel->stopAllActions();
	//m_pPanel->setPosition(Vec2::ZERO);
	if (m_pLastSelected)
	{
		if (bRetify)
			m_pRadioGroup->setSelectedButton(m_pLastSelected);
	}
	if (bRetify)
		pLastSelectedRadio->setScale(m_SelectScaleX, m_SelectScaleY);
	m_pLastSelected = (RadioButton*)pLastSelectedRadio;

	retifyUI();
}

void RadioBox::refresh()
{
	auto idx = m_pRadioGroup->getSelectedButtonIndex();
	if (idx == -1)
		return;

	auto pLastSelectedRadio = m_pRadioGroup->getRadioButtonByIndex(m_pRadioGroup->getSelectedButtonIndex());
	//m_pPanel->stopAllActions();
	//m_pPanel->setPosition(Vec2::ZERO);
	if (m_pLastSelected)
	{
		m_pRadioGroup->setSelectedButton(m_pLastSelected);
	}
	pLastSelectedRadio->setScale(1, 1);
}

void RadioBox::addFocusEventListener(const RadioButtonGroup::ccRadioButtonGroupCallback& callback)
{
	m_RadioFocusCallBack = callback;
}

ssize_t RadioBox::getNumberOfRadioButtons()
{
	return m_pRadioGroup->getNumberOfRadioButtons();
}

void RadioBox::startAttenuatingAutoScroll(const Vec2& deltaMove, const Vec2& initialVelocity)
{
	float time = sqrtf(sqrtf(initialVelocity.length() / 5));

	Vec2 adjustedDeltaMove = flattenVectorByDirection(deltaMove);

	_autoScrolling = true;
	_autoScrollTargetDelta = adjustedDeltaMove;
	_autoScrollAttenuate = true;
	_autoScrollStartPosition = _innerContainer->getPosition();
	_autoScrollTotalTime = time;
	_autoScrollAccumulatedTime = 0;
	_autoScrollBraking = false;
	_autoScrollBrakingStartPosition = Vec2::ZERO;

	// If the destination is also out of boundary of same side, start brake from beginning.
	Vec2 currentOutOfBoundary = getHowMuchOutOfBoundary();
	if (!fltEqualZero(currentOutOfBoundary))
	{
		_autoScrollCurrentlyOutOfBoundary = true;
		Vec2 afterOutOfBoundary = getHowMuchOutOfBoundary(adjustedDeltaMove);
		if (currentOutOfBoundary.x * afterOutOfBoundary.x > 0 || currentOutOfBoundary.y * afterOutOfBoundary.y > 0)
		{
			_autoScrollBraking = true;
		}
	}
	//startAutoScroll(deltaMove, time, true);
}

void RadioBox::onSizeChanged()
{
	ui::ScrollView::onSizeChanged();
	//m_pPanel->setContentSize(_contentSize);
	retifyUI();
}

void RadioBox::update(float dt)
{
	if (_autoScrolling)
	{
		processAutoScrolling(dt);
	}
}

void RadioBox::retifyUI()
{
	if (!m_pRadioGroup)
		return;

	auto radioCount = m_pRadioGroup->getNumberOfRadioButtons();

	if (radioCount <= 0)
		return;

	if (m_nShowRadioCount <= 0)
		return;

	//x÷· ≈≈∞Ê
	auto InnerWidth = _contentSize.width - (m_SolidPaddingLeftX + m_SolidPaddingRightX);
	float iMarginX = 0;
	float iPaddingX = 0;

	float iAllItemWidth = 0;
	for (unsigned int i = 0; i < m_nShowRadioCount && i < (unsigned int)radioCount; ++i)
	{
		auto pObj = m_pRadioGroup->getRadioButtonByIndex(i);
		auto objSize = pObj->getContentSize();
		//auto objRect = pObj->getBoundingBox();
		iAllItemWidth += objSize.width;
	}

	if (m_nShowRadioCount == 1)
	{
		iMarginX = m_MarginX;
		iPaddingX = (m_SolidPaddingLeftX || m_SolidPaddingRightX) ? m_SolidPaddingLeftX : ((InnerWidth - iAllItemWidth) / 2);
	}
	else
	{
		iMarginX = m_MarginX ? m_MarginX : ((InnerWidth - iAllItemWidth) / ((m_nShowRadioCount > radioCount ? radioCount : m_nShowRadioCount) + 1));
		iPaddingX = (m_SolidPaddingLeftX || m_SolidPaddingRightX) ? m_SolidPaddingLeftX : ((InnerWidth - iAllItemWidth) / ((m_nShowRadioCount > radioCount ? radioCount : m_nShowRadioCount) + 1));
	}

	//y÷· ≈≈∞Ê
	auto InnerHeight = _contentSize.height - (m_SolidPaddingTopY + m_SolidPaddingBottomY);
	float iPaddingY = 0;

	float iAllItemCount = 0;
	for (auto i = 0; i < radioCount; ++i)
	{
		auto pObj = m_pRadioGroup->getRadioButtonByIndex(i);
		auto objRect = pObj->getBoundingBox();
		auto objSize = pObj->getContentSize();

		float xOffset = fabs(objSize.width - objRect.size.width);
		float yOffset = fabs(objSize.height - objRect.size.height);
		iPaddingY = (m_SolidPaddingTopY || m_SolidPaddingBottomY) ? m_SolidPaddingTopY : ((InnerHeight - objRect.size.height) / 2);

		//if (m_pLastSelected == pObj)
		//{
		//	pObj->setPosition(Vec2(iPaddingX + i * (iMarginX + objRect.size.width) + objRect.size.width * pObj->getAnchorPoint().x, _contentSize.height - iPaddingY - objRect.size.height * pObj->getAnchorPoint().y));
		//}
		//else
		//{
		//	pObj->setPosition(Vec2(iPaddingX + i * (iMarginX + objSize.width) + objSize.width * pObj->getAnchorPoint().x, _contentSize.height - iPaddingY - objSize.height * pObj->getAnchorPoint().y));
		//}

		//auto posTarget = Vec2(iPaddingX + i * (iMarginX + objSize.width) + objSize.width * pObj->getAnchorPoint().x, _contentSize.height - iPaddingY - objRect.size.height * pObj->getAnchorPoint().y);
		//pObj->runAction(MoveTo::create(0.2f, posTarget));
		float mediumOffset = _contentSize.width / 2 - (iPaddingX + objSize.width * 0.5);
		pObj->setPosition(Vec2(mediumOffset + iPaddingX + i * (iMarginX + objSize.width) + objSize.width * pObj->getAnchorPoint().x, _contentSize.height - iPaddingY - objRect.size.height * pObj->getAnchorPoint().y));
		iAllItemCount += objRect.size.width;
		//pObj->runAction(MoveBy::create(0.2f, Vec2(iMarginX, _contentSize.height - iPaddingY - objRect.size.height * pObj->getAnchorPoint().y)));
	}

	m_GeneralMarginX = iMarginX;

	float offsetX = _contentSize.width / 2 - (m_pRadioGroup->getRadioButtonByIndex(0)->getContentSize().width * 0.5 + iPaddingX);
	this->setInnerContainerSize(Size(iAllItemCount + iMarginX * (radioCount + 1) + 2 * offsetX, _contentSize.height));
}

void RadioBox::onSelectedRadio(RadioButton* radioButton, int index, RadioButtonGroup::EventType eType)
{
	//if (m_pLastSelected)
	//	m_pLastSelected->setScale(1, 1);

	//m_pLastSelected = (RadioButtonEx*)radioButton;

	//radioButton->setScale(m_SelectScaleX, m_SelectScaleY);

	//retifyUI();

	//auto offsetX = _contentSize.width / 2 - m_pRadioGroup->getRadioButtonByIndex(m_pRadioGroup->getSelectedButtonIndex())->getPosition().x;
	//moveInnerContainer(Vec2(offsetX, getInnerContainerPosition().y), false);
	//startAttenuatingAutoScroll(Vec2(offsetX, 0),)
	//float w = _innerContainer->getContentSize().width - _contentSize.width;
	Vec2 midPoint = _contentSize / 2;
	startAutoScrollToDestination(Vec2(midPoint.x - radioButton->getPosition().x, _innerContainer->getPosition().y), 0.2f, true);

	m_bFocus = true;

	if (m_RadioFocusCallBack)
	{
		if (m_bSelectedEvent)
		{
			m_RadioFocusCallBack(radioButton, index, eType);
		}
		else
		{
			m_bSelectedEvent = true;
		}
	}
		
	//m_pPanel->runAction(MoveTo::create(0.2f, Vec2(offsetX, m_pPanel->getPositionY())));
}

void RadioBox::processAutoScrolling(float deltaTime)
{
	// Make auto scroll shorter if it needs to deaccelerate.
	float brakingFactor = (isNecessaryAutoScrollBrake() ? OUT_OF_BOUNDARY_BREAKING_FACTOR : 1);

	// Elapsed time
	_autoScrollAccumulatedTime += deltaTime * (1 / brakingFactor);

	// Calculate the progress percentage
	float percentage = MIN(1, _autoScrollAccumulatedTime / _autoScrollTotalTime);
	if (_autoScrollAttenuate)
	{
		// Use quintic(5th degree) polynomial
		percentage = tweenfunc::quintEaseOut(percentage);
	}

	// Calculate the new position
	Vec2 newPosition = _autoScrollStartPosition + (_autoScrollTargetDelta * percentage);
	bool reachedEnd = std::abs(percentage - 1) <= FLT_EPSILON;

	if (reachedEnd)
	{
		newPosition = _autoScrollStartPosition + _autoScrollTargetDelta;
	}

	if (_bounceEnabled)
	{
		// The new position is adjusted if out of boundary
		newPosition = _autoScrollBrakingStartPosition + (newPosition - _autoScrollBrakingStartPosition) * brakingFactor;
	}
	else
	{
		// Don't let go out of boundary
		Vec2 moveDelta = newPosition - getInnerContainerPosition();
		Vec2 outOfBoundary = getHowMuchOutOfBoundary(moveDelta);
		if (!fltEqualZero(outOfBoundary))
		{
			newPosition += outOfBoundary;
			reachedEnd = true;
		}
	}

	// Finish auto scroll if it ended
	if (reachedEnd)
	{
		_autoScrolling = false;
	}

	moveInnerContainer(newPosition - getInnerContainerPosition(), reachedEnd);


	Vec2 midPoint = _contentSize / 2;
	RadioButtonEx * pObj = nullptr;
	for (auto i = 0; i < m_pRadioGroup->getNumberOfRadioButtons(); ++i)
	{

		pObj = (RadioButtonEx*)m_pRadioGroup->getRadioButtonByIndex(i);
		//cocos2d::log("%0.2f", fabs(_innerContainer->getPosition().x + pObj->getPositionX() - midPoint.x));
		if (fabs(_innerContainer->getPosition().x + pObj->getPositionX() - midPoint.x) <= m_GeneralMarginX / 2)
		{
			if (m_pLastSelected)
				m_pLastSelected->setScale(1, 1);

			m_pLastSelected = pObj;

			pObj->setScale(m_SelectScaleX, m_SelectScaleY);
			//cocos2d::log("%d", i);
			m_pRadioGroup->setSelectedButtonWithoutEvent(pObj);
			//onSelectedRadio(pObj, i, RadioButtonGroup::EventType::SELECT_CHANGED);
			break;
		}
	}

	if (reachedEnd)
	{
		//log("%0.2f-%0.2f-%0.2f", midPoint.x, m_pLastSelected->getPositionX(), _innerContainer->getPositionX());
		auto effect = fabsf(midPoint.x - m_pLastSelected->getPositionX() - _innerContainer->getPositionX());
		if (effect < 0.0001f)
		{
			if (m_bFocus)
			{
				//cocos2d::log("effect rec1 : %s", m_pLastSelected->getTitleText().c_str());
				m_bFocus = false;
			}
			else
			{
				//cocos2d::log("effect rec2 : %s", m_pLastSelected->getTitleText().c_str());
				if (m_bSelectedEvent)
				{
					m_pRadioGroup->setSelectedButton(m_pLastSelected);
				}
				m_bFocus = true;
			}
		}
		else
		{
			//cocos2d::log("effect no : %s", m_pLastSelected->getTitleText().c_str());
			m_pRadioGroup->setSelectedButton(m_pLastSelected);
			//startAutoScrollToDestination(Vec2(midPoint.x - m_pLastSelected->getPosition().x, _innerContainer->getPosition().y), 0.2f, true);
		}
	}
}

void RadioBox::setSelectedButton(int index)
{
	if (m_pRadioGroup)
		m_pRadioGroup->setSelectedButton(index);
}

void RadioBox::setSelectedButton(RadioButton* radioButton)
{
	if (m_pRadioGroup)
		m_pRadioGroup->setSelectedButton(radioButton);
}

void RadioBox::setSelectedButtonWithoutEvent(int index)
{
	if (m_pRadioGroup)
	{
		m_bSelectedEvent = false;
		m_pRadioGroup->setSelectedButton(index);
	}
}

void RadioBox::setSelectedButtonWithoutEvent(RadioButton* radioButton)
{
	if (m_pRadioGroup)
	{
		m_bSelectedEvent = false;
		m_pRadioGroup->setSelectedButton(radioButton);
	}
}