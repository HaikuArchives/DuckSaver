/*
 * Copyrights (c):
 *     2002 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include <Bitmap.h>
#include <Entry.h>
#include <Mime.h>
#include <Resources.h>
#include <Screen.h>
#include <StringView.h>

#include <ctime> 
#include <cstdlib> 

#ifdef DEBUG
#include <iostream>
#include <string.h>
#endif

#include <ScreenSaver.h>

#define APP_SIGNATURE		"application/x-vnd.pecora-ducksaver"

#define	MAX_COUNT_RESOURCES	2
#define SPEED				300000 // Higher = slower!

// Class declaration --------------------------
class DuckSaver : public BScreenSaver
{
public:
				DuckSaver(BMessage *message, image_id id);
				~DuckSaver();
	void		StartConfig(BView *view);
	status_t	StartSaver(BView *v, bool preview);
	void		Draw(BView *v, int32 frame);

	
private:
	
	bool		fInitOk;
	BBitmap		*fImg[MAX_COUNT_RESOURCES];

};

// MAIN INSTANTIATION FUNCTION
extern "C" _EXPORT BScreenSaver *instantiate_screen_saver(BMessage *message, image_id image)
{
	return new DuckSaver(message, image);
}

// Class definition ---------------------------
DuckSaver::DuckSaver(BMessage *message, image_id image)
 :	BScreenSaver(message, image)
{

	fInitOk = false;
	
	srand(time(NULL));

	// Images holen
	for (int i = 0; i<MAX_COUNT_RESOURCES; ++i) fImg[i] = 0;

	BMimeType	mime( APP_SIGNATURE );
	entry_ref	app_ref;
	BResources	resources;

	status_t err;
	if ( (err = resources.SetToImage((const void*)&instantiate_screen_saver)) != B_OK) {
#ifdef DEBUG
		cerr << "Unable to open resource file." << endl;
#endif
		return;
	}
	else {

		size_t		groesse;
		BMessage	msg;
		char		*buf;

		for (int i = 0; i<MAX_COUNT_RESOURCES; ++i) {
			buf = (char *)resources.LoadResource('BBMP', i+1, &groesse);
			if (!buf) {
				
				fImg[i] = 0;
#ifdef DEBUG
				cerr << "Resource not found: " << i+1 << endl;
#endif
			}
			else {
				msg.Unflatten(buf);
				fImg[i] = new BBitmap( &msg );
			}
		}
	}
	
	fInitOk = true;
}

DuckSaver::~DuckSaver() {
	for (int i=0; i<MAX_COUNT_RESOURCES; ++i) delete fImg[i];
}

void DuckSaver::StartConfig(BView *view)
{
	BStringView *child = new BStringView(BRect(10, 10, 200, 25), B_EMPTY_STRING, "DuckSaver");
	child->SetFont(be_bold_font);
	view->AddChild(child);

	view->AddChild(new BStringView(BRect(10, 30, 200, 55), B_EMPTY_STRING, "©2002 by Werner Freytag"));
	
	if (!fInitOk) {
		child = new BStringView(BRect(10, 60, 200, 85), B_EMPTY_STRING, "Error: Can't find resources!");
		child->SetHighColor(255, 0, 0);
		view->AddChild(child);
	}
}

status_t DuckSaver::StartSaver(BView *view, bool preview)
{	
	if (!fInitOk) return B_ERROR;
	
	SetTickSize(SPEED);
	return B_OK;
}

void DuckSaver::Draw(BView *view, int32 frame)
{
	if (frame==0) {
		view->SetDrawingMode( B_OP_ALPHA );
	}
	
	BBitmap	*bmp;
	if ( (bmp = fImg[rand()>(RAND_MAX>>1) ? 1 : 0]) ) {

		BPoint pt( 	(rand() % view->Bounds().IntegerWidth()) - (bmp->Bounds().IntegerWidth()>>1),
					(rand() % view->Bounds().IntegerHeight()) - (bmp->Bounds().IntegerHeight()>>1) );
		view->DrawBitmap( bmp, pt );
		
	}
}
