//
//  SampleView.cpp
//  Gelbox
//
//  Created by Chaim Gingold on 12/6/17.
//
//

#include "SampleView.h"
#include "GelboxApp.h" // ui text, gel picking

using namespace std;
using namespace ci;
using namespace ci::app;

void SampleView::setSource( SampleRef source )
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

void SampleView::draw()
{
	// draw drop target
	if ( mDropTarget )
	{
		// get to root 
		gl::ScopedModelMatrix modelMatrix;
		gl::multModelMatrix( getRootToChildMatrix() );		
		mDropTarget->draw();
	}	
	
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

void SampleView::mouseDrag( ci::app::MouseEvent event )
{
	mDropTarget = GelboxApp::instance()->pickDropTarget( vec2(event.getPos()) );
}

void SampleView::mouseUp( ci::app::MouseEvent event )
{
	if ( mDropTarget )
	{
		// drop!
		mDropTarget->receive( *mSource.get() );
		
		mDropTarget=0;
	}
	else
	{
		// move
		setFrame( getFrame() + getMouseLoc() - getMouseDownLoc() );
	}
}