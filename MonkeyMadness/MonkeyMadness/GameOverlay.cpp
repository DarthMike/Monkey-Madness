/*
	Filename: GameOverlay.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to overlay presentation (UI)
	Comments: It depends of IndieLib
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/		

#include "GameOverlay.h"
#include "GameOverlayListener.h"
#include "Math.h"
#include "GameMouse.h"
#include "GameKeyboard.h"
#include "ConfigOptions.h"
#include "GameEventManager.h"
#include "IndieLibManager.h"
#include "GFXEffects.h"
#include "GameLevel.h"
#include "LevelBuilder.h"
#include "GameEvents.h"
#include "Camera2D.h"
#include "SoundManager.h"
#include "ResourceManager.h"

//Global config options declaration
extern ConfigOptions g_ConfigOptions;  //Global properties of game

//Definition of entities file to editor
const std::string GameOverlay::mOverlayAssetsFileName = "OverlayAssets.xml";

//Update method
void GameOverlay::Update(float dt)
{
	static bool firstplay (true);
	//Local variables
	IndieLibManager* Ilib (SingletonIndieLib::Instance());
	GFXEffects* effecsmgr (SingletonIndieLib::Instance()->GFXEffectsManager);

	//Update elements of overlay
	//Update input controllers
	mGameMouse->Update(dt);
	mGameKeyBoard->Update(dt);
	
	//Update internal timers
	mCounter += dt;
	if(mGameStarted)
		mTutorialMSGCounter +=dt;

	//Update created dynamic animations
	std::list<AnimationControllerPointer>::iterator anitr = mDynamicAnimations.begin();
	//LOOP - For each element created
	while(anitr != mDynamicAnimations.end())
	{
		//Update controller
		(*anitr)->Update(dt);
		if((*anitr)->IsCurrentAnimationFinished())	
		{
			anitr = mDynamicAnimations.erase(anitr);
		}
		else
		{
			++anitr;
		}

	}//LOOP END

	//Update created dynamic sprites
	std::list<SpritePointer>::iterator spitr = mDynamicSprites.begin();
	//LOOP - For each element created
	while(spitr != mDynamicSprites.end())
	{
		if(effecsmgr->IsEffectComplete((*spitr)))
		{
			//Delete it from indielib and destroy it
			Ilib->Entity2dManager->Delete((*spitr).get());
			spitr = mDynamicSprites.erase(spitr);
		}
		else
		{
			++spitr;
		}

	}//LOOP END
	
	//--------First play only elements---------------------
	if(firstplay)
	{
		SingletonSoundMgr::Instance()->PlayMusic("BackMusic",true);
		//Fade in the title of the game
		//Fade in title
		SpritePointer title (mOverlayAssets->GetEntity("Title"));
		SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(title,1.5f,Vector3(0,0,0));

		firstplay = false;
	}
	
	//--------Tutorial messages control--------------------
	if(mTutorialMSGCounter > 500.0f && !mFirstMsg)
	{
		//Fade in 1st message
		SpritePointer msg1 (mOverlayAssets->GetEntity("TutMSG1"));
		SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(msg1,1.5f,Vector3(0,0,0));
		mFirstMsg = true;
	}
	else if(mTutorialMSGCounter > 6000.0f && !mSecondMsg)
	{
		//Dont show previous message
		SpritePointer msg1(mOverlayAssets->GetEntity("TutMSG1"));
		msg1->SetShow(false);
		//Fade in 2nd message
		SpritePointer msg2 (mOverlayAssets->GetEntity("TutMSG2"));
		SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(msg2,1.5f,Vector3(0,0,0));
		mSecondMsg = true;
	}	
	else if(mTutorialMSGCounter > 15000.0f && !mThirdMsg)
	{
		//Dont show previous message
		SpritePointer msg2(mOverlayAssets->GetEntity("TutMSG2"));
		msg2->SetShow(false);
		//Fade in 3rd message
		SpritePointer msg3 (mOverlayAssets->GetEntity("TutMSG3"));
		SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(msg3,1.5f,Vector3(0,0,0));
		mThirdMsg = true;
	}
	else if(mTutorialMSGCounter > 24000.0f && !mFourthMsg)
	{
		//Dont show previous message
		SpritePointer msg3(mOverlayAssets->GetEntity("TutMSG3"));
		msg3->SetShow(false);
		//Fade in 4th message
		SpritePointer msg4 (mOverlayAssets->GetEntity("TutMSG4"));
		SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(msg4,1.5f,Vector3(0,0,0));
		mFourthMsg = true;
	}
	else if(mTutorialMSGCounter > 33000.0f && !mAllMessages)
	{
		SpritePointer msg4(mOverlayAssets->GetEntity("TutMSG4"));
		msg4->SetShow(false);
		mAllMessages = true;

	}//IF

	//Debug text is updated by messages
	//Messages text is updated by events

	//---Game start---
	//IF - Game wasnt started
	if(!mGameStarted)
	{
		//IF - User presses input or elapsed time
		if(Ilib->Input->OnKeyPress(IND_SPACE)
		   ||
		   Ilib->Input->OnKeyPress(IND_RETURN)
		   ||
		   Ilib->Input->OnMouseButtonPress(IND_MBUTTON_LEFT)
		   ||
		   Ilib->Input->OnMouseButtonPress(IND_MBUTTON_RIGHT)
		   ||
	       mCounter > 8000.0f
		   )
		{

			//Dont show title
			SpritePointer title = mOverlayAssets->GetEntity("Title");
			title->SetShow(false);

			//Fadeout back sounds
			SingletonSoundMgr::Instance()->FadeOutMusic("BackMusic",5.0f);

			//Play music
			SingletonSoundMgr::Instance()->PlayMusic("LevelMusic",true);

			//Send message to start game!
			SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new EventData(Event_GameStart))
														  );
			mGameStarted = true;
		}//IF
	}//IF

	//---Game Over--
	//IF - Game over
	if(mGameOver)
	{
		//Local control variable
		static bool processed(false);
		//Display message to pass to next level
		//IF - Processed text display for next level
		if(!processed)
		{
			//Show corresponding message
			SpritePointer messagetext = mOverlayAssets->GetEntity("MessagesText");
			mMessagesText = mMessagesText + "\n\nPress SPACE to try again\nESC to exit";
			messagetext->SetText(const_cast<char*>(mMessagesText.c_str()));
			SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(messagetext,1.0f,Vector3(0,0.5,0));
			processed = true;
		}
		else if(processed)
		{
			//IF - Restart
			if(Ilib->Input->OnKeyPress(IND_SPACE))
			{
				//Send message to restart level
				SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new EventData(Event_RestartLevel))
											);
				//Play Music
				SingletonSoundMgr::Instance()->PlayMusic("LevelMusic",true);
				_resetVariables();
				processed = false;
			}//IF	
		}//IF
	}//IF
		
	//---Level completed--
	//IF - Level completed
	if(mLevelCompleted)
	{
		//Local control variable
		static bool processed(false);
		//Display message to pass to next level
		//IF - Processed text display for next level
		if(!processed)
		{
			//Show corresponding message
			SpritePointer messagetext = mOverlayAssets->GetEntity("MessagesText");
			mMessagesText = mMessagesText + "\n\nPress SPACE to play again\nESC to quit";
			messagetext->SetText(const_cast<char*>(mMessagesText.c_str()));
			SingletonIndieLib::Instance()->GFXEffectsManager->FadeInEntity(messagetext,0.5f,Vector3(0,0,0));
			processed = true;
		}
		else if(processed)
		{
			//IF - Restart
			if(Ilib->Input->OnKeyPress(IND_SPACE))
			{
				//Send message to restart level
				SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new EventData(Event_RestartLevel))
											);
				//Play Music
				SingletonSoundMgr::Instance()->PlayMusic("LevelMusic",true);
				_resetVariables();
				processed = false;
			}//IF
		}//IF
	}//IF
		
}

//Render necessary elements
void GameOverlay::Render()	
{
	mGameMouse->Render();
	mGameKeyBoard->Render();
}

//Event handling
bool GameOverlay::_handleEvents(const EventData& theevent)
{
	//Local variables
	bool eventprocessed (false);

	//Receiving of events:
	//Check type of event
	//IF - "Exit game" event
	if(theevent.GetEventType() == Event_ExitGame)
	{
		//Just stop music
		SingletonSoundMgr::Instance()->StopMusic("LevelMusic",true);
	}//ELSE - Game Start//ELSE - Game Over
	else if(theevent.GetEventType() == Event_GameOver)
	{
		SingletonSoundMgr::Instance()->StopMusic("LevelMusic",true);
		//Play a defeat music
		SingletonSoundMgr::Instance()->PlayMusic("LaughingBirdsMusic",false);
		
		std::stringstream msg;
		msg<<"Oh! These monkeys are tough!\n"
		   <<"You Knocked Out Monkeys "<<mNumKO<<" Times\n"
		   <<"Threw out "<<mNumOut<<" Monkeys\n"
		   <<"And resisted "<<mMinutes<<" minutes "<<mSeconds<<" seconds";
		
		mMessagesText = msg.str();

		mGameOver = true;

	}//ELSE - Level complete
	else if(theevent.GetEventType() == Event_LevelCompleted)
	{
		SingletonSoundMgr::Instance()->StopMusic("LevelMusic",true);
		//Play laughing birds!
		SingletonSoundMgr::Instance()->PlayMusic("LaughingBirdsMusic",false);
		
		std::stringstream msg;
		msg<<"Great! Your tree is safe... for now...\n"
		   <<"You Knocked Out Monkeys "<<mNumKO<<" Times\n"
		   <<"Threw out "<<mNumOut<<" Monkeys\n"
		   <<"And defeated them in "<<mMinutes<<" minutes "<<mSeconds<<" seconds";
		mMessagesText = msg.str();

		mLevelCompleted = true;		
	}
	else if(theevent.GetEventType() == Event_DebugString)
	{
		const DebugMessageEvent& data = static_cast<const DebugMessageEvent&>(theevent);
		static int linenum = 0;
		if(mDebugLines <= 5)
		{
			mDebugLines += 1;
		}
		else
		{
			mDebugMSGstream.seekp(0);
			mDebugMSGstream.seekg(0);
			mDebugMSGstream.clear();
			mDebugLines = 0;
		}
		linenum++;
		std::string newmsg = data.GetString();;
		mDebugMSGstream<<"\n"<<linenum<<":";
		mDebugMSGstream<<newmsg;
		//Just display the messages
		mDebugText = mDebugMSGstream.str();
		SpritePointer debugtext = mOverlayAssets->GetEntity("DebugText");
		debugtext->SetText(const_cast<char*>(mDebugText.c_str()));
		eventprocessed = true;
	}//ELSE - Branch Broke
	else if(theevent.GetEventType() ==  Event_BranchBroke)
	{
		//Just play a sound of branch breaking
		SingletonSoundMgr::Instance()->PlayAction("BrokenBranch");
	}//ELSE - New Agent Created
	else if(theevent.GetEventType() == Event_NewAgentCreated)
	{
		const NewAgentCreatedEvent& eventdata (static_cast<const NewAgentCreatedEvent&>(theevent));

		//IF - AI agent (Monkey)
		if(eventdata.GetAgentType() == AI)
		{		
			//Play a random monkey chat sound
			if(SingletonMath::Instance()->NewRandom(0,100) < 50)
				SingletonSoundMgr::Instance()->PlayAction("MonkeyChat1");
			else
				SingletonSoundMgr::Instance()->PlayAction("MonkeyChat2");
		}//IF
		
	}//ELSE - Monkey is Hanging
	else if(theevent.GetEventType() == Event_MonkeyAttached)
	{
		//Play a random cry sound
		int soundsel(SingletonMath::Instance()->NewRandom(0,100));
		if(soundsel < 25)
			SingletonSoundMgr::Instance()->PlayAction("MonkeyCry1");
		else if(soundsel < 50)
			SingletonSoundMgr::Instance()->PlayAction("MonkeyCry2");
		else if(soundsel < 75)
			SingletonSoundMgr::Instance()->PlayAction("MonkeyCry3");
		else	
			SingletonSoundMgr::Instance()->PlayAction("MonkeyCry4");
	}//ELSE - Monkey Fell
	else if(theevent.GetEventType() == Event_MonkeyDetached)
	{
		//Play a random hit sound
		if(SingletonMath::Instance()->NewRandom(0,100) < 50)
			SingletonSoundMgr::Instance()->PlayAction("Slap1");
		else
			SingletonSoundMgr::Instance()->PlayAction("Slap2");
	}//ELSE - Monkey is KO
	else if(theevent.GetEventType() == Event_MonkeyKO)
	{
		//Play a random hit sound
		if(SingletonMath::Instance()->NewRandom(0,100) < 50)
			SingletonSoundMgr::Instance()->PlayAction("Punch1");
		else
			SingletonSoundMgr::Instance()->PlayAction("Punch2");

		
		const MonkeyKOEvent& eventdata (static_cast<const MonkeyKOEvent&>(theevent));

		//Show sprite where it collided
		//Get surface resource from resource manager	
		SurfacePointer kosurf (SingletonResourceMgr::Instance()->GetSurfaceResource("KOMSG"));
		//IF - Check surface exists
		if(kosurf)
		{
			//Create new sprite entity
			SpritePointer newentity(new IND_Entity2d());
			int targetlayer = SingletonIndieLib::Instance()->GetCamera("General")->GetLayer();  //Render in "General" layer
			SingletonIndieLib::Instance()->Entity2dManager->Add(targetlayer,newentity.get());
			newentity->SetSurface(kosurf.get());
			newentity->SetHotSpot(0.5f,0.5f);
			newentity->ShowGridAreas(false); 
			newentity->ShowCollisionAreas(false);

			//Scale it
			float mGlobalScale (SingletonIndieLib::Instance()->GetGeneralScale());
			SingletonIndieLib::Instance()->ScaleToFit(newentity.get(),kosurf.get(),static_cast<int>(4*mGlobalScale),static_cast<int>(4*mGlobalScale));

			//Position it
			Vector2 position (eventdata.GetPosition());
			position.y +=5.0; 
			Vector2 pospix = SingletonIndieLib::Instance()->FromCoordToPix(position);
			newentity->SetPosition(static_cast<float>(pospix.x),static_cast<float>(pospix.y),1000);  //With Z big enough to be over everything in layer

			//Fade it out!
			SingletonIndieLib::Instance()->GFXEffectsManager->FadeOutEntity(newentity,4.0f,Vector3(0.0f,-10.0f,0.0f));

			mDynamicSprites.push_front(newentity);
		}//IF
	}//ELSE - Agent out
	else if(theevent.GetEventType() == Event_AgentOut)
	{
		const AgentOutEvent& eventdata (static_cast<const AgentOutEvent&>(theevent));

		//IF - AI agent (Monkey)
		if(eventdata.GetAgentType() == AI)
		{
			//Show sprite where it went out (OF SCREEN)
			//Get surface resource from resource manager	
			SurfacePointer outsurf (SingletonResourceMgr::Instance()->GetSurfaceResource("OUTMSG"));
			//IF - Check surface exists
			if(outsurf)
			{
				//Create new sprite entity
				SpritePointer newentity(new IND_Entity2d());
				int targetlayer = SingletonIndieLib::Instance()->GetCamera("General")->GetLayer();  //Render in "General" layer
				SingletonIndieLib::Instance()->Entity2dManager->Add(targetlayer,newentity.get());
				newentity->SetSurface(outsurf.get());
				newentity->SetHotSpot(0.5f,0.5f);
				newentity->ShowGridAreas(false); 
				newentity->ShowCollisionAreas(false);

				//Scale it
				float mGlobalScale (SingletonIndieLib::Instance()->GetGeneralScale());
				SingletonIndieLib::Instance()->ScaleToFit(newentity.get(),outsurf.get(),static_cast<int>(4*mGlobalScale),static_cast<int>(4*mGlobalScale));

				//Position it
				Vector2 position (eventdata.GetPosition());
				
				//HACK////////////
				//Put it on limits of screen
				Vector2 minaabb(15,8), maxaabb(65,45);	
				//Check boundaries of camera vision
				position.x = SingletonMath::ClampNumber(position.x,maxaabb.x,minaabb.x);
				position.y = SingletonMath::ClampNumber(position.y,maxaabb.y,minaabb.y);
				/////////////////

				Vector2 pospix = SingletonIndieLib::Instance()->FromCoordToPix(position);
				newentity->SetPosition(static_cast<float>(pospix.x),static_cast<float>(pospix.y),1000);  //With Z big enough to be over everything in layer

				//Fade it out!
				SingletonIndieLib::Instance()->GFXEffectsManager->FadeOutEntity(newentity,4.0f,Vector3(0.0f,0.0f,0.0f));

				mDynamicSprites.push_front(newentity);
			}
		}
	}//ELSE - Game statistics update
	else if(theevent.GetEventType() == Event_GameStatistics)
	{
		const GameStatisticsEvent& eventdata (static_cast<const GameStatisticsEvent&>(theevent));
		const GameStatisticsInfo& eventinfo (eventdata.GetInfo());
		
		//Update trackings
		mNumKO = eventinfo.numko;
		mNumOut = eventinfo.numout;
		mMinutes = eventinfo.minutes;
		mSeconds = eventinfo.seconds;
		mMonkeysLeft = eventinfo.monkeysleft;

		//Update display of values
		std::stringstream message;
		message<<"Time - "<<mMinutes<<":"<<mSeconds
			   <<"\n"<<"Remaining Monkeys: "<<mMonkeysLeft<<"\n"
			   <<"Tree Branches: "<<eventinfo.branchesleft;
		//Store text
		mStatisticsText = message.str();
		mOverlayAssets->GetEntity("StatisticsText")->SetText(const_cast<char*>(mStatisticsText.c_str()));   //Indielib incompatibility with string! (const_cast)
		mOverlayAssets->GetEntity("StatisticsText")->SetShow(true);
	}

	return (eventprocessed);
}

void GameOverlay::_init()
{
	//**********************LOAD NEEDED ASSETS*************************************
	//Load entities related to this screen
	std::string assetsfilepath = g_ConfigOptions.GetScriptsPath() + mOverlayAssetsFileName;
	
	//---Creation of entities through level builder---
	LevelBuilder thebuilder; //Load level assets - Container mode
			
	//Call creation of object (level)
	thebuilder.LoadLevel(assetsfilepath,"-**NOEDIT**OVERLAYASSETS-",true);  //Only assets loading (last "true")
	
	//No errors, get level pointer (used to store entities assets)
	mOverlayAssets = thebuilder.GetCreatedLevel();
	//**********************************ASSETS LOADED*******************************
	
	//**********************SET INTERNAL VARIABLES**********************************
	mScaleFactor = SingletonIndieLib::Instance()->GetGeneralScale();
	mResX = static_cast<float>(SingletonIndieLib::Instance()->Window->GetWidth());
	mResY = static_cast<float>(SingletonIndieLib::Instance()->Window->GetHeight());

	//Check all entities were loaded
	SpritePointer pt;
	pt = mOverlayAssets->GetEntity("DebugText");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'DebugText'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	#ifdef _DEBUGGING
	pt->SetText("--DEBUG TEXT HERE--");
	#endif

	pt = mOverlayAssets->GetEntity("MessagesText");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'MessagesText'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	pt->SetTint(0,0,0);

	pt = mOverlayAssets->GetEntity("StatisticsText");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'StatisticsText'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}
	pt->SetTint(0,0,0);

	pt = mOverlayAssets->GetEntity("Title");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'Title'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}	
	pt->SetShow(false);

	pt = mOverlayAssets->GetEntity("TutMSG1");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'TutMSG1'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}	
	pt->SetShow(false);

	pt = mOverlayAssets->GetEntity("TutMSG2");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'TutMSG2'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}	
	pt->SetShow(false);

	pt = mOverlayAssets->GetEntity("TutMSG3");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'TutMSG3'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}	
	pt->SetShow(false);

	pt = mOverlayAssets->GetEntity("TutMSG4");
	if(!pt)
	{
		throw GenericException("File '" + assetsfilepath +"' Doesnt contain necessary entity 'TutMSG4'. Game needs to close inmediately. Review this file, you shouldnt edit it!",GenericException::FILE_CONFIG_INCORRECT);
	}	
	pt->SetShow(false);

	_resetVariables();
	//*********************CREATE INPUT CONTROLLERS********************************
	mGameMouse = new GameMouse(this);
	mGameKeyBoard = new GameKeyBoard(this);

	//*****************INPUT CONTROLLERS CREATED***********************************

	//Register game overlay listener
	if(!mListener)
		mListener = new GameOverlayListener(this);
}

void GameOverlay::_resetVariables()
{
	mMessagesText = "";
	mDisplayNextMessages = true;
	mCounter = 0.0f;
	mGameOver = false;
	mLevelCompleted = false;
	mTutorialMSGCounter= 0.0f;
	mFirstMsg = false;
	mSecondMsg = false;
	mThirdMsg = false;
	mFourthMsg = false;
	mAllMessages = false;

	mNumKO = 0;
	mNumOut = 0;
	mMinutes = 0;
	mSeconds = 0;
	mMonkeysLeft = 0;

	SpritePointer msg(mOverlayAssets->GetEntity("TutMSG1"));
	msg->SetShow(false);
	msg = mOverlayAssets->GetEntity("TutMSG2");
	msg->SetShow(false);
	msg = mOverlayAssets->GetEntity("TutMSG3");
	msg->SetShow(false);
	msg = mOverlayAssets->GetEntity("TutMSG4");
	msg->SetShow(false);
	msg = mOverlayAssets->GetEntity("MessagesText");
	msg->SetShow(false);
}

void GameOverlay::_release()
{

	//Deregister from IndieLib any dynamically created sprites (animations is done in animationcontroller)
	std::list<SpritePointer>::iterator itr;
	//LOOP - Deregister remaining sprites
	for(itr = mDynamicSprites.begin(); itr != mDynamicSprites.end(); ++itr)
	{
		SingletonIndieLib::Instance()->GFXEffectsManager->StopEffect((*itr));
		SingletonIndieLib::Instance()->Entity2dManager->Delete((*itr).get());
	}//LOOP END

	//Input controllers
	if(mGameMouse)
	{
		delete mGameMouse;
		mGameMouse = NULL;
	}
	if(mGameKeyBoard)
	{
		delete mGameKeyBoard;
		mGameKeyBoard = NULL; 
	}

	if(mListener)
	{
		delete mListener;
		mListener = NULL;
	}
}