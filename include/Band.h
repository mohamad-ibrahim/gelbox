//
//  Band.h
//  Gelbox
//
//  Created by Chaim Gingold on 4/27/18.
//

#pragma once

#include <algorithm>
#include "cinder/CinderMath.h"

class Band
{
public:
	
	// meta-data
	int			mLane		= -1; // what lane are we in?
	int			mFragment	= -1; // what fragment from sample are we from?
	int			mDye		= -1; // if -1 not a dye; otherwise, what dye is it?

	// fragment data (inputs)
	int			mBases		 = 0;	// base count
	int			mAggregate	 = 1;   // monomer by default (so, mBases * mAggregate is effective molecule size)
	int			mDegradeLo	 = 0;	// by how many base pairs are we degraded at the low  end (bottom of band shifted down)?  
	int			mDegradeHi	 = 0;	// by how many base pairs are we degraded at the high end (top of band shifted down)?
	float		mMass		 = 0.f; // needed?
	float		mAspectRatio = 1.f; // needed?
	
	// ui
	ci::Color   mFocusColor;
	
	// basic geometric information
	ci::Rectf	mRect;		// visual. does not include smearing above/below or blurring.
	ci::Rectf	mUIRect;	// ui. should encompass all visual phenomena.
	ci::Rectf	mWellRect;	// start bounds 
	
	// render params (some just for GelRender class)
	float		mSmearAbove = 0.f; // same units as mRect
	float		mSmearBelow = 0.f; // same units as mRect
	float		mSmearBrightnessAbove[2] = {1,0}; // near, far (e.g. 1,0)
	float		mSmearBrightnessBelow[2] = {1,0}; // near, far (e.g. 1,0)
	
	float		mFlameHeight = 0.f; // in unit space, how high to make the flames?
	
	float		mSmileHeight = 0.f; // how high in unit space will max smile peel back?
	float		mSmileExp	 = 0.f; // what exponent to apply to smile curve?
	
	int			mBlur		= 0;
	float		mBrightness = 1.f; // 0..1; effectively mColor's alpha
	ci::Color	mColor		= ci::Color(1,1,1); // almost always white (except dyes)
	
	int			mRandSeed	= 0;	

	float		mWellDamage = 0.f; // 0..1
	int			mWellDamageRandSeed = 0;

	// helpers
	ci::ColorA	getColorA() const { return ci::ColorA( mColor, mBrightness ); }
	
	float		pickSmear( ci::vec2 p ) const
	{
		return std::max( pickSmearAbove(p), pickSmearBelow(p) );
	}
	
	float		pickSmearAbove( ci::vec2 p ) const
	{
		if (    mSmearAbove > 0.f
		     && p.x >= mRect.x1 && p.x <= mRect.x2
		     && p.y >= mRect.y1 - mSmearAbove
		     && p.y <= mRect.y1
		     )
		{
			return ci::lmap( p.y, mRect.y1 - mSmearAbove, mRect.y1, 0.f, 1.f );
		}
		else return 0.f;
	}

	float		pickSmearBelow( ci::vec2 p ) const
	{
		if (    mSmearBelow > 0.f
		     && p.x >= mRect.x1 && p.x <= mRect.x2
		     && p.y >= mRect.y2
		     && p.y <= mRect.y2 + mSmearBelow
		     )
		{
			return ci::lmap( p.y, mRect.y2, mRect.y2 + mSmearBelow, 1.f, 0.f );
		}
		else return 0.f;
	}
	
};

inline int findBandByAggregate( const std::vector<Band>& bands, int aggregate )
{
	int bandi = -1;
	
	for( size_t i=0; i<bands.size(); ++i )
	{
		if ( bands[i].mAggregate == aggregate ) {
			bandi = (int)i;
			break;
		}
	}
	
	return bandi;
}
