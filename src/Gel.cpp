//
//  Gel.cpp
//  Gelbox
//
//  Created by Chaim Gingold on 12/4/17.
//
//

#include "Gel.h"
#include "Sample.h"
#include "Tuning.h"

using namespace std;
using namespace ci;

const vec2 kLaneVec(1,0);
const vec2 kPosVec (0,1);

void Gel::setLayout(
	float		lane_dimension, 
	float		pos_elec_dimension,
	int			numLanes,
	float		ymargin )
{
	mYMargin = ymargin;
	
	mSize = vec2( lane_dimension, pos_elec_dimension );
	
	mNumLanes  = numLanes;
	mLaneWidth = mSize.x / (float)numLanes;
	
	mSamples.resize(numLanes);
}

void Gel::syncBandsToSample( SampleRef sample )
{
	int lane=-1;
	
	// find
	for( int i=0; i<mNumLanes; ++i )
	{
		if ( mSamples[i] == sample )
		{
			lane=i;
			break;
		}
	}
	
	// check
	assert( lane >= 0 && lane < mNumLanes );
	
	// update
	clearSamples(lane);
	
	if ( mSamples[lane] )
	{
		insertSample( *mSamples[lane], lane );
	}
}

void Gel::insertSample( const Sample& src, int lane )
{
	vec2 laneLoc = vec2(0.f,mYMargin) + kLaneVec * ((float)lane*mLaneWidth + mLaneWidth/2.f);
	
	vec2 size;
	size.x = mLaneWidth * .25f;
	size.y = mLaneWidth * .05f;
	
	for( auto frag : src.mFragments )
	{
		Band b;

		b.mLane			= lane;
		
		b.mBases		= frag.mBases * frag.mAggregate;
		b.mMass			= frag.mMass  * (float)frag.mAggregate;
		b.mDegrade		= frag.mDegrade;
		
		b.mStartLoc		= laneLoc;
		
		b.mAspectRatioYNormBonus = (frag.mAspectRatio - 1.f) / 16.f;
		
		// calculate alpha
		float a = constrain( b.mMass / kSampleMassHigh, 0.1f, 1.f ); // alpha set to mass / 125ml
		
		if (b.mDegrade > 1.f) a *= 1.f - min( b.mDegrade - 1.f, 1.f ); // degrade alpha
		
		
		b.mCreateTime	= 0.f; // always start it at the start (not mTime); otherwise is kind of silly...
		b.mExists		= true;
		b.mColor		= ColorA(1.f,1.f,1.f,a);
		
		b.mBounds		= calcBandBounds(b);
		
		mBands.push_back(b);
	}
}

void Gel::clearSamples( int lane )
{
	std::vector<Band> nb;
	
	for( const auto &b : mBands )
	{
		if (b.mLane != lane) nb.push_back(b);
	}
	
	mBands = nb;
}

void Gel::stepTime ( float dt )
{
	mTime += dt;
	mTime = min( mTime, mDuration );
	updateBandsWithTime(mTime);
}

void Gel::setTime( float t )
{
	mTime = t;
	updateBandsWithTime(mTime);
}

void Gel::updateBandsWithTime( float t )
{
	for( auto &b : mBands )
	{
		b.mExists = t >= b.mCreateTime;
		
		b.mBounds = calcBandBounds(b);
	}
}

ci::Rectf Gel::calcBandBounds( const Band& b ) const
{
	const float bandTime = max(mTime - b.mCreateTime,0.f);

	const float w2 = mLaneWidth * .25f;
	const float h2 = mLaneWidth * .05f;
	
	// what base pair location to use for y1 and y2
	float y1b = constrain( normalizeBases(b.mBases), 0.f, 1.f );
	float y2b = y1b;
	
	y1b -= b.mAspectRatioYNormBonus * bandTime;
	y2b -= b.mAspectRatioYNormBonus * bandTime;
	
	// degrade base pair location
	y2b -= min( 1.f, b.mDegrade ); // as degrade goes 0..1, y2 moves to end of chart--shorter base pairs
	if ( b.mDegrade > 1.f ) y1b -= min( 1.f, b.mDegrade - 1.f ); // as degrade goes 1..2, y1 moves to end of chart--shorter bp  
	
	// get bounds in cm
	Rectf r;
	
	r.x1 = b.mStartLoc.x - w2;
	r.x2 = b.mStartLoc.x + w2;
	
	r.y1 = b.mStartLoc.y - h2 + getYForNormalizedBases( y1b, bandTime );
	r.y2 = b.mStartLoc.y + h2 + getYForNormalizedBases( y2b, bandTime );
	
	return r;
}

float Gel::normalizeBases( int bases ) const
{
	return (float)bases / 10000.f;	
}

float Gel::getYForNormalizedBases( float normalizedBases, float t ) const
{
	float y = normalizedBases;
	// y=1 means lots of bases, slow, top of gel
	// y=0 means few bases, fast, bottom of gel
	
	// time (doing time here seems to mess things up, causing bands to disappear)
//	y = 1.f - t * (1.f - y);
	
	// output is non-linear
	if (1)
	{
		const float K = 3.7f;
	//	float K = lerp( 0.f, 2.f, t );
		
		y = .05f + powf( 1.f - y, K );
	}
	
	// flip it, so that y=1 puts us at bottom of gel
//	y = 1.f - y; 

	// time
	y *= t;
	
	// scale
	y *= mSize.y - mYMargin*2.f;
	
	// done
	return y;
}