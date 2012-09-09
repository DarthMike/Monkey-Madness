/*
	Filename: MMGameListener.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Event listener for game
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _MMGAMELISTENER
#define _MMGAMELISTENER
//Library dependencies

//Class dependencies
#include "GameEventManager.h"

//Forward declarations
class MMGame;
class EventData;

class MMGameListener : public IEventListener
{

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	MMGameListener(MMGame* gameptr)
		:mName("MMGameListener"),
		mGame(gameptr)
	{
		SingletonGameEventMgr::Instance()->AddListener(this,Event_GameStart); //Game start
		SingletonGameEventMgr::Instance()->AddListener(this,Event_RestartLevel); //Restart level
		SingletonGameEventMgr::Instance()->AddListener(this,Event_AgentOut);  //Agent out
		SingletonGameEventMgr::Instance()->AddListener(this,Event_MonkeyKO);  //Monkey ko
		SingletonGameEventMgr::Instance()->AddListener(this,Event_BranchBroke);  //Branch broke
		//Game AI
		SingletonGameEventMgr::Instance()->AddListener(this,Event_NewTargetRequest);
		//Game agents creation
		SingletonGameEventMgr::Instance()->AddListener(this,Event_NewAgentCreated);  
	}
	~MMGameListener()
	{
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_GameStart); //Game start
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_RestartLevel); //Restart level
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_AgentOut);  //Agent out
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_MonkeyKO);  //Monkey ko
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_BranchBroke);  //Branch broke
		//Game AI
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_NewTargetRequest);
		//Game agents creation
		SingletonGameEventMgr::Instance()->RemoveListener(this,Event_NewAgentCreated);
	}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	//Implementation of handling
	virtual bool HandleEvent(const EventData& theevent);
	//Implementation of name
	virtual const std::string& GetName()
	{
		return mName;
	}
	//----- PUBLIC VARIABLES ------	

protected:
	//----- INTERNAL VARIABLES -----
	std::string mName;
	MMGame* mGame;
	//----- INTERNAL FUNCTIONS -----
};

#endif
