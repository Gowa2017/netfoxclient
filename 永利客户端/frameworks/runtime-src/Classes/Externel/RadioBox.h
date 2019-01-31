#ifndef __H_RADIO_BOX_H__
#define __H_RADIO_BOX_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <unordered_map>

USING_NS_CC;
using namespace ui;
using namespace std;

class RadioButtonEx : public ui::RadioButton
{
public:
	RadioButtonEx() : _titleRenderer(nullptr), _fontSize(10)
	{
		
	};
	~RadioButtonEx() {};

	/**
	* Another factory method to create a RadioButton instance.
	* This method uses less resource to create a RadioButton.
	* @param backGround The background image name in `std::string`.
	* @param cross The cross image name in `std::string`.
	* @param texType  The texture's resource type in `Widget::TextureResType`.
	* @return A RadioButton instance pointer
	*/

	static RadioButtonEx* create(const std::string& backGround,
		const std::string& cross,
		TextureResType texType = TextureResType::LOCAL)
	{
		RadioButtonEx *pWidget = new (std::nothrow) RadioButtonEx;
		if (pWidget && pWidget->init(backGround,
			"",
			cross,
			"",
			"",
			texType))
		{
			pWidget->autorelease();
			return pWidget;
		}
		CC_SAFE_DELETE(pWidget);
		return nullptr;
	}

	std::string getTitleText() const
	{
		if (nullptr == _titleRenderer)
		{
			return "";
		}
		return _titleRenderer->getString();
	}

	void createTitleRenderer()
	{
		_titleRenderer = Label::create();
		_titleRenderer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		addProtectedChild(_titleRenderer, -1, -1);
	}

	void setTitleFontSize(float size)
	{
		if (nullptr == _titleRenderer)
		{
			this->createTitleRenderer();
		}

		_fontSize = size;
		_titleRenderer->setSystemFontSize(_fontSize);
	}

	Size getNormalSize() const
	{
		Size titleSize;
		if (_titleRenderer != nullptr)
		{
			titleSize = _titleRenderer->getContentSize();
		}
		Size imageSize;
		if (_backGroundBoxRenderer != nullptr)
		{
			imageSize = _backGroundBoxRenderer->getContentSize();
		}
		float width = titleSize.width > imageSize.width ? titleSize.width : imageSize.width;
		float height = titleSize.height > imageSize.height ? titleSize.height : imageSize.height;

		return Size(width, height);
	}

	void updateContentSize()
	{
		if (_unifySize)
		{
			Size s = getNormalSize();
			ProtectedNode::setContentSize(s);
			onSizeChanged();
			return;
		}

		if (_ignoreSize)
		{
			this->setContentSize(getVirtualRendererSize());
		}
	}

	void updateTitleLocation()
	{
		_titleRenderer->setPosition(_contentSize.width * 0.5f, _contentSize.height * 0.5f);
	}

	void setTitleText(const std::string& text)
	{
		if (text == getTitleText())
		{
			return;
		}
		if (nullptr == _titleRenderer)
		{
			this->createTitleRenderer();
		}
		_titleRenderer->setString(text);
		this->setTitleFontSize(_fontSize);
		updateContentSize();
		updateTitleLocation();
	}

	CREATE_FUNC(RadioButtonEx);

protected:
	Label* _titleRenderer;

private:
	int _fontSize;
};

class RadioBox : public ui::ScrollView
{
public:
	RadioBox();
	~RadioBox();
	virtual bool init();
	CREATE_FUNC(RadioBox);

	//参数设置
public:
	//设置选中按钮放大比例
	void setSelectedScale(const float scaleX, const float scaleY);

	//设置盒子与包围盒的固定左内边距X
	void setSolidPaddingLeftX(const float x);
	//设置盒子与包围盒的固定顶内边距Y
	void setSolidPaddingTopY(const float y);

	//设置盒子与包围盒的固定右内边距X
	void setSolidPaddingRightX(const float x);
	//设置盒子与包围盒的固定底内边距Y
	void setSolidPaddingBottomY(const float y);

	//设置盒子间的边距X
	void setMarginX(const float x);
	//设置盒子间的边距Y
	//void setMarginY(const float y);

	//设置显示按钮个数
	void setShowRadioCount(const unsigned int count);

	//添加RadioButton
	void addRadioButton(RadioButton* radioButton, bool bRetify = false);

	void refresh();

	//获得焦点的回调函数
	virtual void addFocusEventListener(const RadioButtonGroup::ccRadioButtonGroupCallback& callback);

	virtual ssize_t getNumberOfRadioButtons();

	//重载函数
public:
	void startAttenuatingAutoScroll(const Vec2& deltaMove, const Vec2& initialVelocity);
	//重载函数
protected:
	virtual void onSizeChanged() override;
	virtual void update(float dt) override;
	

	//功能函数
protected:
	//重新布局
	void retifyUI();
	//选中回调
	void onSelectedRadio(RadioButton* radioButton, int index, RadioButtonGroup::EventType eType);
	//处理滑动效果
	void processAutoScrolling(float deltaTime);
public:
	virtual void setSelectedButton(int index);
	virtual void setSelectedButton(RadioButton* radioButton);
	virtual void setSelectedButtonWithoutEvent(int index);
	virtual void setSelectedButtonWithoutEvent(RadioButton* radioButton);

	//ui变量
protected:
	RadioButtonGroup*	m_pRadioGroup;
	RadioButton *			m_pLastSelected;
	Layout *				m_pPanel;

	//状态变量
protected:
	float				m_SelectScaleX;
	float				m_SelectScaleY;
	float				m_SolidPaddingLeftX;
	float				m_SolidPaddingTopY;
	float				m_SolidPaddingRightX;
	float				m_SolidPaddingBottomY;

	float				m_MarginX;
	float				m_GeneralMarginX;

	float				m_InnerWidth;

	bool				m_bFocus;
	bool				m_bSelectedEvent;

	unsigned			m_nShowRadioCount;
	RadioButtonGroup::ccRadioButtonGroupCallback m_RadioFocusCallBack;
};


#endif