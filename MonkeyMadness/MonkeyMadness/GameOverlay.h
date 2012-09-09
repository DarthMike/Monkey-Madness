/*
	Filename: GameOverlay.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to overlay presentation (UI)
	Comments: It depends of IndieLib
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
/*TODO:
	- Make mouse and keyboard part of this class, not the game logic
*/


#ifndef _GAMEOVERLAY
#define _GAMEOVERLAY

//Library dependencies
#include <string>
#include <sstream>
#include <list>
//Class dependencies
#include "Vector2.h"
#include "Shared_Resources.h"
#include "GFXDefs.h"
#include "LogManager.h"
#include "GenericException.h"
#include "AnimationController.h"

//Forward declarations
class EventData;
class GameOverlayListener;
class MMGame;
class GameMouse;
class GameKeyBoard;

class GameOverlay
{
	friend class GameOverlayListener; //Listener is friend to this, as it has access to internals... (just a handle really)

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GameOverlay(MMGame* thegame):
	  mGameMouse(NULL), 
	  mGameKeyBoard(NULL),
	  mScaleFactor(0),
	  mResX(0),
	  mResY(0),
	  mCounter(0.0f),
	  mListener(NULL),
	  mDisplayNextMessages(true),
	  mDebugLines(0),
	  mGameOver(false),
	  mLevelCompleted(false),
	  mGameStarted(false),
	  mTutorialMSGCounter(0.0f),
	  mFirstMsg(false),
	  mSecondMsg(false),
	  mThirdMsg(false),
	  mFourthMsg(false),
	  mAllMessages(false),
	  mNumKO(0),
	  mNumOut(0),
	  mMinutes(0),
	  mSeconds(0),
	  mMonkeysLeft(0)
	{	
		_init();
		SingletonLogMgr::Instance()->AddNewLine("GameOverlay","Overlay created and ready",LOGNORMAL);
	}
	virtual ~GameOverlay()
	{
		_release();
		SingletonLogMgr::Instance()->AddNewLine("GameOverlay","Overlay destroyed",LOGNORMAL);
	}

	//----- GET/SET FUNCTIONS -----
	
	//----- OTHER FUNCTIONS -----
	void Update(float dt);		//Update method
	void Render();				//Render necessary elements
	//----- PUBLIC VARIABLES ------

protected:
	//----- INTERNAL VARIABLES -----
	float mScaleFactor;			//Transform scale to get pixel units x
	float mResX;				//Screen X resolution
	float mResY;				//Screen Y resolution
	float mCounter;				//Timing variabler

	GameMouse* mGameMouse;			//Input controllers pointers
	GameKeyBoard* mGameKeyBoard;

	static const std::string mOverlayAssetsFileName;  //Filename hard-coded
	GameLevelPointer mOverlayAssets;	//The entities loaded from xml file
	std::list<AnimationControllerPointer> mDynamicAnimations;	//Dynamic entities created in-game
	std::list<SpritePointer> mDynamicSprites;    //Dynamic sprites created in-game

	GameOverlayListener* mListener;			//Events listener
	
	bool mDisplayNextMessages;
	bool mGameOver;					//Tracking of game over
	bool mLevelCompleted;			//Tracking of level completed
	bool mGameStarted;				//Playing game

	//Text for font entities in overlay
	std::string mDebugText;
	std::string mMessagesText;
	std::string mStatisticsText;

	//Debug messages
	int mDebugLines;
	std::stringstream mDebugMSGstream;

	//IN-game tutorial
	float mTutorialMSGCounter;
	bool mFirstMsg;
	bool mSecondMsg;
	bool mThirdMsg;
	bool mFourthMsg;
	bool mAllMessages;

	//Statistics
	int mNumKO;
	int mNumOut;
	unsigned int mMinutes;
	unsigned int mSeconds;
	int mMonkeysLeft;

	//----- INTERNAL FUNCTIONS -----
	void _init();
	void _resetVariables();
	void _release();
	//Event handling
	bool _handleEvents(const EventData& theevent);
};

#endif