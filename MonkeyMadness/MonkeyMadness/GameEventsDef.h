/*
	Filename: GameEventsDef.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Definitions of general classes for events and events data related
	Comments: Shared pointer (boost smart pointer implementation) used to define events data
	Attribution: "Game coding complete" was the inspiration , great book! http://www.mcshaffry.com/GameCode/
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _GAMEEVENTSDEF
#define _GAMEEVENTSDEF

//Libraries dependencies
#include <string>
#include "boost\shared_ptr.hpp"  //Smart pointer facilites - not using std because it cant go inside an 
								 //STL container as we do in event manager...

//-------------------Definitions-----------------------------
//The big event type enumeration
//THIS ENUMERATION IS USED TO CREATE ALL TYPES OF MESSAGES IN-GAME
typedef enum GameEventType
{
	UNDEFINED = 0,
	Event_DebugString,	//Debug string to show in overlay
	Event_NewCollision, //Collision events
	Event_PersistantCollision,
	Event_DeletedCollision,
	Event_CollisionResult,
	Event_OutOfLimits,  //Out of limits physics event
	Event_DestroyedJoint, //Destroyed joint physics event
	Event_DestroyedShape, //Destroyed shape physics event
	Event_RenderInLayer, //Render other stuff than entities in a given layer
	Event_NewAgentCreated,  //New Agent Created (for any one interested in register it)
	Event_AgentOut,        //An agent is out of limits
	Event_GameStart,  //Start a new game
	Event_NewMousePos, //Update mouse position
	Event_TakeObjectCommand,  //Take an object
	Event_ReleaseObjectCommand, //Relese an object
	Event_SolidCollision,  //Solid collision
	Event_NewAITarget,       //New target body / position to an AI Agent from game
	Event_NewTargetRequest,  //An Agent requests new target to game
	Event_MonkeyAttached,   //A Monkey was attached to a branch
	Event_MonkeyDetached,   //A Monkey was attached to a branch
	Event_MonkeyKO,			//A Monkey is KO!
	Event_BranchBroke,      //A joint from BodyChain object broke
	Event_GameStatistics,  //Change in counters for game
	Event_LevelCompleted,		//A level was completed!
	Event_RestartLevel,			//Restart the level
	Event_NextLevel,			//Load next level
	Event_GameOver,  //Game state events
	Event_ExitGame,
	NUM_MSG
}GameEventType;

//-------------------Classes---------------------------------
//An Event Data base empty event class
//DATA WHICH WILL BE SENT 
class EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	EventData(const GameEventType type, float timestamp = 0.0f ):
	  mType(type),
	  mEventTime(timestamp)
	{}
	virtual ~EventData()
	{}
	//----- GET/SET FUNCTIONS -----
	const GameEventType &GetEventType() const { return mType; } 
	float GetEventTimeStamp() const {return mEventTime; }

protected:
	//----- INTERNAL VARIABLES -----
	const float mEventTime;				//When this message occured
	const GameEventType mType;			//The type of message	
	//----- INTERNAL FUNCTIONS -----
private:	
	//ASSIGNMENT OPERATOR IS DISABLED
	EventData& operator=(EventData const&){}

};

//Definition of auto pointer for events data - important to make it this way, as events are created 
//inside clients, but managed finally (asyncronously or not) by the event manager.
typedef boost::shared_ptr<EventData> EventDataPointer;

//An event listener Base abstract class
//ANY MESSAGE LISTENER CLASS SHOULD DERIVE FROM THIS CLASS TO IMPLEMENT 
//HANDLING OF MESSAGES
class IEventListener
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	IEventListener()
	{}
	virtual ~IEventListener()
	{}
	//----- OTHER FUNCTIONS -----
	//Get the name of listener (useful for events manager)
	virtual const std::string& GetName() = 0;
	//Prompts listener to handle an event, contained in event data. If event is processed
	//returns true, if not , false. If a generic implementation is needed to answer "message processed"
	//allways, the class can use this implementation here.
	#pragma warning(push)  //Store warnings state
	#pragma warning(disable : 4100)  //Disable this warning - Unreference local variable - it is a dummy function!
	virtual bool HandleEvent(const EventData& theevent) = 0
	{
		return true;
	}
	#pragma warning(pop)  //Restore warnings state
};
#endif