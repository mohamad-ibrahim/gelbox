//
//  SampleTubeView.cpp
//  Gelbox
//
//  Created by Chaim Gingold on 12/6/17.
//
//

#include "SampleTubeView.h"
#include "GelboxApp.h" // ui text, gel picking
#include "Interactions.h"

using namespace std;
using namespace ci;
using namespace ci::app;

void SampleTubeView::setSource( SampleRef source )
{
	// layout params
	vec2 center(0,0);
	vec2 size(32,32);

	// keep old center?
	if (mSource) center = getFrame().getCenter();
	
	// set source
	mSource = source;
	
	// load image
	try
	{
		mIcon = gl::Texture::create( loadImage( getAssetPath(mSource->mIconFileName) ) );
		
		size = vec2( mIcon->getSize() );
		
		size *= mSource->mIconScale;
	}
	catch (...)
	{
		mIcon = 0;
	}

	// layout
	Rectf bounds( vec2(0,0), size );
	setBounds(bounds);
	
	Rectf frame = bounds;
	frame.offsetCenterTo( center );
	setFrame(frame);
}

void SampleTubeView::draw()
{
	// image
	auto drawImage = [this]( vec2 delta, float alpha )
	{
		if (mIcon)
		{
			gl::color(1,1,1,alpha);
			gl::draw( mIcon, getBounds() + delta );
		}
		else
		{
			gl::color(.5,.5,.5,alpha);
			gl::drawSolidRect( getBounds() + delta );
		}
	};
	
	// this alpha logic dovetails with DragSampleInteraction
	// (not really ideal...)
	if ( getHasMouseDown() )
	{
		drawImage( getMouseLoc() - getMouseDownLoc(), 1.f );
		drawImage( vec2(0,0), .25f );
	}
	else
	{
		drawImage( vec2(0,0), 1.f );
	}
	
	// text label
	gl::color(0,0,0);
	auto font = GelboxApp::instance()->getUIFont(); 
	vec2 strSize = font->measureString(mSource->mName); 
	font->drawString(
		mSource->mName,
		getBounds().getCenter()
		+ vec2( -strSize.x/2.f, getBounds().getHeight()/2.f + font->getDescent() + font->getAscent() ),
		gl::TextureFont::DrawOptions().pixelSnap()
		);
}

void SampleTubeView::mouseDown( ci::app::MouseEvent event )
{
	auto i = make_shared<DragSampleInteraction>(shared_from_this());
	i->begin();
}