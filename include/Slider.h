//
//  Slider.h
//  Gelbox
//
//  Created by Chaim Gingold on 1/24/18.
//
//

#pragma once

class Slider
{
public:
	
	// icon, layout
	std::string mIconName;
	
	ci::Rectf	mIconRect[2];
	ci::gl::TextureRef  mIcon[2];
	glm::vec2			mIconSize[2]; // in points
	
	glm::vec2	mEndpoint[2];
	
	bool		loadIcons( ci::fs::path lo, ci::fs::path hi );
	void		doLayoutInWidth( float fitInWidth, float iconGutter );
	/*
		[ ] ----- [ ]
		   ^     ^      iconGutter
		|-------------| fitInWidth
	
		topleft is at origin
	*/
	
	// notches
	int			mNotches=0;
	
	enum class Notch
	{
		None,
		DrawOnly, // just draw them
		Nearest, // will always snap to a notch
		Snap    // will snap if close to a notch
	};
	Notch		mNotchAction = Notch::None;
	
	// value mapping
	float		mValue=.5f; // 0..1
	float		mValueMappedLo=0.f, mValueMappedHi=1.f;

	// graph
	bool		mIsGraph		=	false;
	float		mGraphHeight	=	32.f;
	float		mGraphValueMappedLo=0.f, mGraphValueMappedHi=1.f; // per notch graph		
	std::vector<float> mGraphValues;
	
	// getter-setters
	typedef std::function< void ( float ) > tSetter;
	typedef std::function< float() > tGetter;

	typedef std::function< void (std::vector<float> ) > tGraphSetter;
	typedef std::function< std::vector<float>() > tGraphGetter;	
	
	tSetter		mSetter;
	tGetter		mGetter;
	
	tGraphSetter	mGraphSetter;
	tGraphGetter	mGraphGetter;
	bool			mAreGraphValuesReversed = false;

	void pushValueToSetter() const; // implicitly called by setValueWithMouse, setNormalizedValue
	void pullValueFromGetter();
	
	// label generation
	std::function<std::string(float v)> mMappedValueToStr;

	// methods
	void		draw() const;
	
	ci::Rectf	calcHandleRect() const;
	ci::Rectf	calcPickRect() const; // just of interactive areas
	ci::Rectf   calcBounds() const; // of everything (line, icons, etc...)
	
	void		setValueWithMouse ( ci::vec2 pos ); 
	void		setNormalizedValue( float normValue );
	
	float		getMappedValue() const { return ci::lerp( mValueMappedLo, mValueMappedHi, mValue ); }
	void		flipXAxis();
	
};
