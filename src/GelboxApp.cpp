#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Gel.h"
#include "GelView.h"
#include "GelParticleSource.h"
#include "GelParticleSourceView.h"
#include "View.h"
#include "TimelineView.h"
#include "ImageView.h"

#include "GelboxApp.h"

using namespace ci;
using namespace ci::app;
using namespace std;

GelboxApp* GelboxApp::mInstance = 0;

GelboxApp::GelboxApp()
{
	assert( !mInstance );
	mInstance = this;
}

GelboxApp::~GelboxApp()
{
	assert( mInstance==this );
	mInstance=0;
}

void GelboxApp::setup()
{
//	glEnable( GL_MULTISAMPLE_ARB );
//	glEnable( GL_LINE_SMOOTH );
//	glEnable( GL_POLYGON_SMOOTH );
	
	// get gel source data
	try {
		mGelSource.loadXml( ci::XmlTree(loadAsset("gel.xml")) );
		makeGel( mGelSource, getWindowCenter() );
	} catch (...) {
		cout << "failed to load gel.xml" << endl;
	}
	
	// ui assets
	mUIFont = gl::TextureFont::create( Font("Avenir",12) );
}

void GelboxApp::makeGel( const GelParticleSource& source, vec2 center )
{	
	// gel
	auto gel = make_shared<Gel>();
	gel->setLayout( 300.f, 400.f, 5 );
	
	// populate it
//	gel->insertSamples( source, 0, 10   );
	int col=0;
	gel->insertSamples( source, col++, 100  );
	gel->insertSamples( source, col++, 100  );
	gel->insertSamples( source, col++, 100  );
	gel->insertSamples( source, col++, 100  );
	gel->insertSamples( source, col++, 1000 );
	
	// gel view
	auto gelView = make_shared<GelView>( gel );
	
	{
		Rectf frame = gelView->getFrame();
		frame += center - frame.getCenter();
		
		gelView->setFrame( frame );
		mViews.addView(gelView);
	}
	
	// timeline
	{
		// in gelview frame coordinate space
		const float kTimelineHeight = 20.f;
		const float kTimelineGutter = 10.f;
		
		vec2 topleft( 0, gelView->getFrame().getHeight() + kTimelineGutter ); 
		vec2 size   ( gelView->getFrame().getWidth(), kTimelineHeight );
		Rectf timelineRect( topleft, topleft + size );
		
		auto timelineView = make_shared<TimelineView>( timelineRect );
		
		timelineView->mGetTime		= [gel](){ return gel->getTime(); };
		timelineView->mSetTime		= [gel]( float t ){ gel->setTime(t); };
		timelineView->mGetDuration	= [gel](){ return gel->getDuration(); };
		timelineView->mGetIsPlaying = [gel](){ return ! gel->getIsPaused(); };
		timelineView->mSetIsPlaying = [gel]( bool v ){ gel->setIsPaused( !v ); };
		
		timelineView->setParent(gelView);
		
		mViews.addView(timelineView);
	}
}

GelViewRef GelboxApp::pickGelView( vec2 loc, int* pickedLane ) const
{
	GelViewRef pick=0;
	
	mViews.pickView( loc, [&pick,pickedLane]( ViewRef view, vec2 inFrameLoc ) -> bool
	{
		GelViewRef gelView = dynamic_pointer_cast<GelView>(view);
		
		bool hit = gelView && view->pick(inFrameLoc);
		
		if ( hit )
		{
			pick = gelView;
			if (pickedLane) *pickedLane = gelView->pickLane(inFrameLoc);
		}
		
		return hit;
	});
	
	return pick;
}

void GelboxApp::mouseDown( MouseEvent event )
{
	mViews.mouseDown(event);
}

void GelboxApp::mouseUp( MouseEvent event )
{
	mViews.mouseUp(event);
}

void GelboxApp::mouseMove( MouseEvent event )
{
	mViews.mouseMove(event);
}

void GelboxApp::mouseDrag( MouseEvent event )
{
	mViews.mouseDrag(event);
}

void GelboxApp::fileDrop ( FileDropEvent event )
{
	auto					files		  = event.getFiles();
	const vec2				pos			  = vec2( event.getPos() );
	const vector<string>	imgExtensions = {".jpg",".png",".gif"};
	
	for( auto i : files )
	{
		std::string ext = i.extension().string();
		
		// xml?
		if ( ext == ".xml" )
		{
			try
			{
				XmlTree xml( loadFile(i) );
				
				// GelParticleSource?
				if ( xml.hasChild(GelParticleSource::kRootXMLNodeName) )
				{
					GelParticleSourceRef	 source = std::make_shared<GelParticleSource>(xml);
					GelParticleSourceViewRef view	= std::make_shared<GelParticleSourceView>(source);
					
					view->setFrame( view->getFrame() + (pos - view->getFrame().getCenter()) );
					
					mViews.addView(view);
				}
			}
			catch (...) {
				cout << "Failed to load .xml '" << i << "'" << endl;
			}
		}
		
		// image?
		if ( find( imgExtensions.begin(), imgExtensions.end(), ext ) != imgExtensions.end() )
		{
			try {
				gl::TextureRef image = gl::Texture::create( loadImage(i) );
				
				if (image) {
					// make view
					auto imageView = make_shared<ImageView>(image);
					
					// center frame on drop loc
					Rectf frame = imageView->getFrame();
					
					frame.offsetCenterTo(pos);
					
					imageView->setFrame( frame );
					
					mViews.addView(imageView);
				}
			} catch (...) {
				cout << "Failed to load image '" << i << "'" << endl;
			}
		}
	}
}

void GelboxApp::update()
{	
	mViews.tick(.1f);
}

void GelboxApp::draw()
{
	gl::clear( Color( 1, 1, 1 ) );
	
	mViews.draw();

	// anti-aliasing/smoothing test
	if (0)
	{
		Rectf r(0,0,10,10);	
		gl::color( 0,0,0 );
		for( int i=0; i<4; ++i )
		{
			gl::drawSolidRect(r);
			r += vec2( r.getWidth() * 2.f, .25f );
		}
		
		gl::drawLine( vec2(0, 0), vec2(50, 200) );
	}
	
	// lane pick test
	{
		int lane;
		GelViewRef gelView = pickGelView( mViews.getMouseLoc(), &lane );
		
		if ( gelView )
		{
			gl::color(1,0,0);
			gl::pushModelMatrix();
			gl::multModelMatrix( gelView->getChildToRootMatrix() );
			gl::drawStrokedRect( gelView->getLaneRect(lane) );
			gl::popModelMatrix();
		}
	}
}

CINDER_APP( GelboxApp, RendererGl )
