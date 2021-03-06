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
	std::string			mIconName;
	
	ci::Rectf			mIconRect[2], mIconPickRect[2];
	ci::gl::TextureRef  mIcon[2];
	glm::vec2			mIconSize[2]; // in points
	
	glm::vec2			mEndpoint[2];
	ci::Rectf			mBar;
	float				mBarCornerRadius=0.f;
	
	ci::ColorA			mTextLabelColor = ci::Color::gray(.0f);
	ci::ColorA			mBarFillColor   = ci::Color::gray(.3f);
	ci::ColorA			mBarEmptyColor  = ci::Color::gray(.8f);
	
	bool				mEnabled = true;
	
	bool		loadIcons( ci::fs::path lo, ci::fs::path hi );
	void		setIcon( int icon, ci::gl::TextureRef tex, int pixelsPerPoint=1 ); // sets mIcon, mIconSize; handles tex==0
	
	void		doLayoutInWidth( float fitInWidth, float iconGutter, glm::vec2 notionalIconSize=glm::vec2(0.f) );
	/*
		[ ] ----- [ ]
		   ^     ^      iconGutter
		|-----------|   fitInWidth
	
		topleft is at origin
		notionalIconSize fits whatever icons are present within a box of that size, for consistent layouts regardless of icon sizes
	*/
	
	void		doLayoutFromBar( ci::vec2 barSize, float iconGutter );
	// origin will be at topleft of slider bar; so texture/icon at left will be in negative space 
	
	//
	enum class Style
	{
		Slider,
		Graph,
		Bar
	};
	Style mStyle = Style::Slider;
	
	// notches
//	int			mNotches=0;
	std::vector<float> mNotches; // at arbitrary points on line; does not need to be sorted. 0..1
	void  clearNotches() { mNotches.clear(); }
	void  addFixedNotches( int numNotches );
	void  addNotchAtMappedValue( float v );
	float getNearestNotch( float toNormV ) const;
	
	enum class Notch
	{
		None,
		DrawOnly, // just draw them
		Nearest, // will always snap to a notch
		Snap    // will snap if close to a notch
	};
	Notch		mNotchAction = Notch::None;
	float		mNotchSnapToDist = 4.f;
	
	// value mapping
	float		mValue=.5f; // 0..1
	float		mValueMappedLo=0.f, mValueMappedHi=1.f;
	float		mValueQuantize=0.f;
	
	// graph
	float		mGraphHeight	=	32.f;
	float		mGraphValueMappedLo=0.f, mGraphValueMappedHi=1.f; // per notch graph
	std::vector<float> mGraphValues; // each is 0..1
	bool		mGraphDrawAsColumns = true;
	
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
	void		draw( int highlightIcon=-1 ) const; // call this one; others are internal
	void		drawGraph() const;
	void		drawTextLabel() const;
	void		drawNotches() const;
	
	bool		hasHandle() const { return mStyle==Style::Slider; }
	
	ci::Rectf	calcHandleRect() const;
	ci::Rectf	calcPickRect() const; // just of interactive areas
	ci::Rectf   calcBounds() const; // of everything (line, icons, etc...)
	
	void		setValueWithMouse ( ci::vec2 pos ); 
	void		setNormalizedValue( float normValue );
	void		setMappedValue( float value );
	void		setLimitValue( int v );  // v is 0 (for low), 1 (for high); works on graphs + normal sliders
	
	float		notch   ( float normValue, float v1, float v2 ) const; // for x axis, (v1,v2) are endpoints[0,1].x
	float		quantize( float value, float quantize ) const; // q<=0 means none 
	float		quantizeNormalizedValue( float valueNorm, float valueMapLo, float valueMapHi, float quantizeMapStep ) const;
	
	int			pickIcon( ci::vec2 ) const; // -1 for none, 0,1 for which
	
	float		calcNormalizedValue( float mappedValue ) const;
	float		getMappedValue() const;
	void		flipXAxis();
	
};
