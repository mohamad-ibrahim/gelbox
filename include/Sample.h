//
//  Sample.h
//  Gelbox
//
//  Created by Chaim Gingold on 12/5/17.
//
//

#pragma once

#include <vector>
#include "cinder/Rand.h"
#include "cinder/Xml.h"

class Sample;
typedef std::shared_ptr<Sample> SampleRef;

class SampleFragRef
{
public:
	void clear() { mSample=0; mFrag=-1; }
	
	void set( SampleRef s, int frag ) { mSample=s; mFrag=frag; }
	bool is ( SampleRef s, int frag ) const { return mSample==s && mFrag==frag; } 
	bool isa( SampleRef s, int frag ) const; // is identical, or is (s,frag) derived from this 
		// e.g. x->isa(s,f) means is x equal to or a parent of (s,f)
		
	bool isValid() const;
	bool isValidIn( SampleRef inSample ) const;
	
	SampleRef getSample() const { return mSample; }
	int		  getFrag()   const { return mFrag; }
	
	bool setToOrigin(); // returns true if changed
	bool setToRoot();

	bool operator==(const SampleFragRef &rhs) const {
		return mSample == rhs.mSample && mFrag == rhs.mFrag ;
	}
		
private:
	SampleRef	mSample;
	int			mFrag = -1;
};
typedef std::shared_ptr<SampleFragRef> SampleFragRefRef; // :D it is a pointer-pointer after all

class Sample
{
public:

	class Fragment
	{
	public:
		int   mBases		= 0;   // base count
		
		float mMass			= 0.f; // ng
		
		float mDegrade		= 0.f; // 0..2
		float mSampleSizeBias = -1.f; // -1 for none, 0 to skew towards big + slow, 1 for small + fast 
		
		std::vector<float> mAggregate;
		/* - empty means default (monomer)
		   - dimer is {0,1} (0 for size 1, 1 for size 2)
		   - 50/50 dimers and trimers is {0,1,1}
		   - can no non-uniform distributions, e.g.: {.5,1,2}
		   etc...
		*/
		
		float mAspectRatio	= 1.f;
		ci::Color mColor	= ci::Color(.5,.5,.5);
		
		// tracking parentage across sample derivatives
		// -- e.g. for a loupe into a band of another sample
		//		in that case, loupe peers into a new sample that is derived from
		//		a larger sample defining an entire lane
		SampleRef mOriginSample;
		int		  mOriginSampleFrag=-1;
		
		// derived properties
		float calcAggregateSum() const {
			float sum = 0.f;
			for( auto w : mAggregate ) sum += w;
			return sum; 
		}

		float calcAggregateWeightedSum() const {
			float sum = 0.f;
			for( int i=0; i<mAggregate.size(); ++i ) sum += mAggregate[i] * (float)i;
			return sum; 
		}
		
		int calcAggregateRange( int& lo, int& hi ) const
		{
			lo = hi = -1;
			
			int numNonZeroMultimers = 0;
			
			for( int m=0; m<mAggregate.size(); ++m )
			{
				if ( mAggregate[m] > 0.f )
				{
					numNonZeroMultimers++;
					if ( lo == -1 ) lo = m;
					hi = m;					
				}
			}
			
			return numNonZeroMultimers;			
		} 
	};

	std::vector<Fragment> mFragments;
	
	std::string mName;
	int			mID = -1;
	
	std::string mIconFileName;
	float		mIconScale=1.f;
	
	
	
	Sample() {}
	Sample( const ci::XmlTree& xml ) { loadXml(xml); }

	void degrade( float d ) {
		for ( auto& f : mFragments ) f.mDegrade = std::min( 2.f, f.mDegrade + d );
	}
	
	int cloneFragment( int f )
	{
		assert( isValidFragment(f) );
		mFragments.push_back( mFragments[f] );
		return mFragments.size()-1;
	}
	
	void removeFragment( int f )
	{
		assert( isValidFragment(f) );
		mFragments[f] = mFragments.back();
		mFragments.pop_back();
	}
	
	bool isValidFragment( int f ) const { return f >=0 && f < mFragments.size(); }
	
	void   loadXml( const ci::XmlTree& ); // clears existing mFragments first
	
	static const std::string kRootXMLNodeName;
	
};
