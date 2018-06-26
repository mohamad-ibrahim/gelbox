//
//  GelboxApp.h
//  Gelbox
//
//  Created by Chaim Gingold on 12/11/17.
//
//

#pragma once

#include "cinder/app/App.h"

#include "View.h"
#include "Sample.h"
#include "GelView.h"
#include "FileWatch.h"

class AppSettingsView;
typedef std::shared_ptr<AppSettingsView> AppSettingsViewRef;

class GelboxApp : public ci::app::App {
  public:
	static GelboxApp* instance() { return mInstance; }
	
	GelboxApp();
	~GelboxApp();
	
	void setup() override;
	void mouseDown( ci::app::MouseEvent event ) override;
	void mouseUp  ( ci::app::MouseEvent event ) override;
	void mouseMove( ci::app::MouseEvent event ) override;
	void mouseDrag( ci::app::MouseEvent event ) override;
	void fileDrop ( ci::app::FileDropEvent event ) override;
	void keyDown  ( ci::app::KeyEvent ) override;
	void keyUp    ( ci::app::KeyEvent ) override;
	void resize   () override;

	void update() override;
	void draw() override;
	
	static void prepareSettings( Settings *settings );

	
	ci::gl::TextureFontRef getUIFont( int scale=1, int *oScale=0 );
	
	int getModifierKeys() const { return mModifierKeys; }

	ci::fs::path getOverloadedAssetPath() const { return mOverloadedAssetPath; }
	FileWatch&   getFileWatch() { return mFileWatch; }
	
  private:
  	
  	int						mModifierKeys=0; // e.g. ci::app::KeyEvent::SHIFT_DOWN
  	
	ci::gl::TextureFontRef	mUIFont, mUIFont2x;

	ViewCollection			mViews;
	GelViewRef				mGelView;
	ButtonViewRef	 		mSettingsBtn, mHelpBtn;
	AppSettingsViewRef		mSettingsView;
	
	static GelboxApp*		mInstance;

	void		makeSettingsBtn();
	void		makeHelpBtn();
  	int			getModifierKeys( ci::app::KeyEvent ) const;
	SampleRef	loadSample( ci::fs::path ) const;	

	void promptUserToSaveSample( SampleRef );
	void promptUserToSaveGel   ( GelRef );
	void promptUserToOpenFile();

	int pickLaneForDroppedSample( vec2 dropPos ) const;
	void makeIconForDroppedSample( SampleRef, vec2 dropPos );
	void makeIconForSample( SampleRef );

	SampleRef tryXmlToSample ( ci::XmlTree ) const;
	GelRef    tryXmlToGel    ( ci::XmlTree ) const;
	SampleRef tryJsonToSample( ci::JsonTree ) const;
	GelRef	  tryJsonToGel	 ( ci::JsonTree ) const;
	
	void makeGel();

	ci::fs::path			calcOverloadedAssetPath() const;
	ci::fs::path			mOverloadedAssetPath;
	
	FileWatch				mFileWatch;
	
};
