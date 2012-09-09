/*
	Filename: GameEvents.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Definition of specific game-related events
	Comments: Shared pointer (boost smart pointer implementation) used to define events data
	Attribution: "Game coding complete" was the inspiration , great book! http://www.mcshaffry.com/GameCode/
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _GAMEEVENTS
#define _GAMEEVENTS

//Library dependencies
#include <vector>

//Classes dependencies
#include "GameEventsDef.h"
#include "GameLogicDefs.h"
#include "Vector2.h"

typedef struct DebugStringInfo //New debug string info
{
	DebugStringInfo(const std::string& thestring):
	message(thestring)
	{}

	//New target position
	std::string message;
}DebugStringInfo;

typedef struct NewMousePosInfo  //New mouse position info
{
	NewMousePosInfo(const Vector2& positionpix):
	pospix(positionpix)
	{}

	Vector2 pospix;
}NewMousePosInfo;


typedef struct NewAgentCreatedInfo   //New agent created info
{
	NewAgentCreatedInfo(const std::string& agentid, AgentType type):
	agentid(agentid),
	type(type)
	{}

	std::string agentid;
	AgentType type;
}NewAgentCreatedInfo;


typedef struct AgentOutInfo   //New agent created info
{
	AgentOutInfo(const std::string& agentid, AgentType type, const Vector2& position):
	agentid(agentid),
	type(type),
	position(position)
	{}

	std::string agentid;
	AgentType type;
	Vector2 position;
}AgentOutInfo;

class b2Body;
typedef struct NewAITargetInfo  //New target info for AI
{
	NewAITargetInfo(const std::string& aiagentname, const Vector2& targetposition, b2Body* targetbody, const std::string& targetbodychain,const std::string& agentname):
	aiagentname(aiagentname),	
	target(targetposition),
	body(targetbody),
	bodychainid(targetbodychain),
	agentname(agentname)
	{}
	
	std::string aiagentname;
	Vector2 target;
	b2Body* body;
	std::string bodychainid;
	std::string agentname;

}NewAITargetInfo;

typedef struct NewTargetRequestInfo  //New target request info from AI
{
	NewTargetRequestInfo(const Vector2& currentposition, const std::string& agentname):
	currentposition(currentposition),
	agentname(agentname)
	{}

	Vector2 currentposition;
	std::string agentname;

}NewTargetRequestInfo;

typedef struct SolidCollisionInfo  //New collision info
{
	SolidCollisionInfo(MaterialType material)
	:
	material(material)
	{}

	MaterialType material;
}SolidCollisionInfo;

typedef struct MonkeyAttachedInfo   //Agent attached to a bodychainagent bodychain
{
	MonkeyAttachedInfo(const std::string& bodychainid, const std::string& agentid):
    bodychainid(bodychainid),
	agentid(agentid)
	{}

	std::string bodychainid;
	std::string agentid;
}MonkeyAttachedInfo;

typedef struct MonkeyDetachedInfo   //Agent detached to a bodychainagent bodychain
{
	MonkeyDetachedInfo(const std::string& bodychainid, const std::string& agentid):
    bodychainid(bodychainid),
	agentid(agentid)
	{}

	std::string bodychainid;
	std::string agentid;
}MonkeyDetachedInfo;

typedef struct BranchBrokeInfo   //A Branch (BodyChain joint) broke
{
	BranchBrokeInfo(const std::string& bodychainid, const std::string& agentid, int remaining):
    bodychainid(bodychainid),
	agentid(agentid),
	remaining(remaining)
	{}

	std::string bodychainid;
	std::string agentid;
	int remaining;
}BranchBrokeInfo;

typedef struct MonkeyKOInfo   //Monkey KO
{
	MonkeyKOInfo(const Vector2& koposition):
	position(koposition)
	{}

	Vector2 position;
}MonkeyKOInfo;

typedef struct GameStatisticsInfo    //Game statistics 
{
	GameStatisticsInfo(int monkeysleft, int branchesleft, unsigned int seconds, unsigned int minutes, int numko, int numout):
	monkeysleft(monkeysleft),
	branchesleft(branchesleft),
	seconds(seconds),
	minutes(minutes),
	numko(numko),
	numout(numout)
	{}
	int monkeysleft, branchesleft, numko, numout;
	unsigned int seconds, minutes;

}GameStatisticsInfo;

typedef struct RenderInLayerInfo //Perform additional per-layer rendering
{
	RenderInLayerInfo(int thelayer):
	layer(thelayer)
	{}
	int layer;  //Layer to render in
}RenderInLayerInfo;

//DATA WHICH WILL BE SENT  - NEW MOVEMENT TARGET //TODO: REMOVE
class DebugMessageEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	DebugMessageEvent(const GameEventType type, const DebugStringInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mDebugInfo(info)
	{}
	virtual ~DebugMessageEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	const std::string& GetString() const { return mDebugInfo.message; }
	
protected:
	//----- INTERNAL VARIABLES -----
	DebugStringInfo mDebugInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT -NEW POSITION OF MOUSE
class NewMousePosEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	NewMousePosEvent(const GameEventType type, const NewMousePosInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~NewMousePosEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	Vector2 GetPosPix() const  {return mInfo.pospix;}
	
protected:
	//----- INTERNAL VARIABLES -----
	NewMousePosInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - NEW AGENT CREATED 
class NewAgentCreatedEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	NewAgentCreatedEvent(const GameEventType type, const NewAgentCreatedInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~NewAgentCreatedEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	std::string GetAgentId() const  {return mInfo.agentid;}
	AgentType GetAgentType() const { return mInfo.type; }	
protected:
	//----- INTERNAL VARIABLES -----
	NewAgentCreatedInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - AGENT OUT OF LIMITS
class AgentOutEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	AgentOutEvent(const GameEventType type, const AgentOutInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~AgentOutEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	std::string GetAgentId() const  {return mInfo.agentid;}
	AgentType GetAgentType() const { return mInfo.type; }	
	Vector2 GetPosition() const { return mInfo.position; }
protected:
	//----- INTERNAL VARIABLES -----
	AgentOutInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - NEW AI TARGET INFO
class NewAITargetEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	NewAITargetEvent(const GameEventType type, const NewAITargetInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~NewAITargetEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	std::string GetAIAgentName() const { return mInfo.aiagentname; }
	Vector2 GetTarget() const  {return mInfo.target;}
	b2Body* GetTargetBody() const  {return mInfo.body;}
	std::string GetBodyChainId() const { return mInfo.bodychainid; }
	std::string GetAgentName() const {return mInfo.agentname; }
	
protected:
	//----- INTERNAL VARIABLES -----
	NewAITargetInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - NEW TARGET REQUEST
class NewTargetRequestEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	NewTargetRequestEvent(const GameEventType type, const NewTargetRequestInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~NewTargetRequestEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	Vector2 GetPosition() const  {return mInfo.currentposition;}
	std::string GetAgentName() const {return mInfo.agentname;} 
	
protected:
	//----- INTERNAL VARIABLES -----
	NewTargetRequestInfo mInfo;
};

//DATA WHICH WILL BE SENT - SOLID COLLISION EVENT
class SolidCollisionEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	SolidCollisionEvent(const GameEventType type, const SolidCollisionInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~SolidCollisionEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	const SolidCollisionInfo& GetInfo() const  {return mInfo;}
	
protected:
	//----- INTERNAL VARIABLES -----
	SolidCollisionInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - MONKEY ATTACHED TO A BODYCHAIN EVENT
class MonkeyAttachedEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	MonkeyAttachedEvent(const GameEventType type, const MonkeyAttachedInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~MonkeyAttachedEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	std::string GetBodyChainId() const {return mInfo.bodychainid; }
	std::string GetAgentId() const { return mInfo.agentid; }
protected:
	//----- INTERNAL VARIABLES -----
	MonkeyAttachedInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - MONKEY DETACHED TO A BODYCHAIN EVENT
class MonkeyDetachedEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	MonkeyDetachedEvent(const GameEventType type, const MonkeyDetachedInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~MonkeyDetachedEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	std::string GetBodyChainId() const {return mInfo.bodychainid; }
	std::string GetAgentId() const { return mInfo.agentid; }
protected:
	//----- INTERNAL VARIABLES -----
	MonkeyDetachedInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - A BODYCHAIN BRANCH BROKE (A JOINT INSIDE BODYCHAIN)
class BranchBrokeEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	BranchBrokeEvent(const GameEventType type, const BranchBrokeInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~BranchBrokeEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	std::string GetBodyChainId() const {return mInfo.bodychainid; }
	std::string GetAgentId() const { return mInfo.agentid; }
	int GetRemainingBranches() const { return mInfo.remaining; }
protected:
	//----- INTERNAL VARIABLES -----
	BranchBrokeInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};


//DATA WHICH WILL BE SENT - MONKEY KO
class MonkeyKOEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	MonkeyKOEvent(const GameEventType type, const MonkeyKOInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~MonkeyKOEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	Vector2 GetPosition() const { return mInfo.position; }

protected:
	//----- INTERNAL VARIABLES -----
	MonkeyKOInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};


//DATA WHICH WILL BE SENT - GAME STATISTICS
class GameStatisticsEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	GameStatisticsEvent(const GameEventType type, const GameStatisticsInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mInfo(info)
	{}
	virtual ~GameStatisticsEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	const GameStatisticsInfo& GetInfo() const { return mInfo; }

protected:
	//----- INTERNAL VARIABLES -----
	GameStatisticsInfo mInfo;
	//----- INTERNAL FUNCTIONS -----
};

//DATA WHICH WILL BE SENT - RENDER DATA IN A CONCRETE LAYER
class RenderInLayerXEvent : public EventData
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	RenderInLayerXEvent(const GameEventType type, const RenderInLayerInfo& info, float timestamp = 0.0f ):
	  EventData(type,timestamp),
	  mLayerInfo(info)
	{}
	virtual ~RenderInLayerXEvent()
	{}
	//----- GET/SET FUNCTIONS -----
	int GetLayer() const { return mLayerInfo.layer; }

protected:
	//----- INTERNAL VARIABLES -----
	RenderInLayerInfo mLayerInfo;
	//----- INTERNAL FUNCTIONS -----
};

#endif