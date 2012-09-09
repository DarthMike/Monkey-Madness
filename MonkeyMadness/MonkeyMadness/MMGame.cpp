/*
	Filename: MMGame.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Main game class
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "MMGame.h"
#include "MMGameListener.h"
#include "ConfigOptions.h"
#include "XMLParser.h"
#include "ResourceManager.h"
#include "IndieLibManager.h"
#include "Camera2D.h"
#include "AgentsManager.h"
#include "PhysicsManager.h"
#include "GameLevel.h"
#include "LevelBuilder.h"
//HACK!!!
#include "MonkeyBuilder.h"
//

//Global config options declaration
extern ConfigOptions g_ConfigOptions;  //Global properties of game

//Static variables declarationc
const float MMGame::FIRSTTICKDELAY = 600.0f;  
//Logic tick update
void MMGame::UpdateGameState(float dt)
{	
	//*************FIRST-TIME LEVEL LOGIC**************************
	//IF - First time execution
	if(mFirstStart)
	{
		//Logic only on first update
		///HACK ////
		//Put camera in interesting position and zoom 
		Camera2DPointer camera (SingletonIndieLib::Instance()->GetCamera("General"));
		camera->SetZoom(1.26f);
		camera->SetPosition(Vector2(40,24.5));
		/// HACK END ////
		//Reset first time execution
		mFirstStart = false;
	}//IF
	//**************************************************************

	if(mFirstTickDelayCounter < FIRSTTICKDELAY)
		mFirstTickDelayCounter += dt;

	//***********GAME STATE UPDATE**********************************
	if(mFirstTickDelayCounter >= FIRSTTICKDELAY
	   && !mLevelCompleted && !mGameOver)
	{
		//Update physics with dt supplied
		mPhysicsMgr->Update(dt);
		if(mPhysicsMgr->IsPhysicsStepped())
		{
			float steppedtime (mPhysicsMgr->GetSteppedTime());

			//Update agents
			mAgentsManager->UpdateAgents(steppedtime);
			//Update Level Logic
			mCurrentLevelPointer->Update(steppedtime);
			//Update camera in course
			SingletonIndieLib::Instance()->UpdateCameras(steppedtime);
		}
	}
	//***************************************************************

	//Create a monkey entity AI 
	mCounter +=dt;
	static bool speedchange1(false);
	static bool speedchange2(false);
	static bool speedchange3(false);
	if(mGameStarted && !mGameOver)
	{
	
		if(mCounter > mNewMonkeyTime && nNumMonkeysCreated < 100)
		{
			int siderand (SingletonMath::Instance()->NewRandom(1,100));
			if(siderand > 50)
			{
				mMonkeyBuilder->CreateNewMonkeyAgent(Vector2(75,15));
			}
			else
			{
				mMonkeyBuilder->CreateNewMonkeyAgent(Vector2(5,15));
			}
			
			nNumMonkeysCreated++;

			
			if(nNumMonkeysCreated > 10 && !speedchange1)
			{
				mNewMonkeyTime -= 2000.0f;
				speedchange1 = true;
			}


			if(nNumMonkeysCreated > 20 && !speedchange2)
			{
				mNewMonkeyTime -= 2000.0f;
				speedchange2 = true;
			}
			

			if(nNumMonkeysCreated > 30 && !speedchange3)
			{
				mNewMonkeyTime = 1000.0f;
				speedchange3 = true;
			}

			mCounter = 0.0f;
		}
	}
	else
	{
		speedchange1 = false;
		speedchange2 = false;
		speedchange3 = false;
	}

	//Update game statistics
	if(mGameStarted)
		mGameElapsedTime += dt;
	
	mUpdateCounter += dt;
	if(!mGameOver && mGameStarted && mUpdateCounter > 1000.0f)
	{
		//Send event with statistics
		unsigned int elapsedminutes (static_cast<unsigned int>((mGameElapsedTime/ 60000 )));  
		unsigned int elapsedseconds (static_cast<unsigned int>((mGameElapsedTime/ 1000))%60); 
		GameStatisticsInfo eventinfo(100 - nNumMonkeysCreated,mBranchesLeft,elapsedseconds,elapsedminutes,mNumKO,mNumOut);
		
		SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer( new GameStatisticsEvent(Event_GameStatistics,eventinfo))
											);
		mUpdateCounter = 0.0f;

	}//IF

	//Game over condition
	if(mBranchesLeft == 0 && mGameStarted && !mGameOver)
	{
		mGameOver = true;
		SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer( new EventData(Event_GameOver))
											);

	}

	//Game complete condition
	if(mBranchesLeft > 0 && nNumMonkeysCreated >100 && mCounter > 5000.0f)
	{
		mLevelCompleted = true;
		SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer( new EventData(Event_GameOver))
											);
	}
	//*******************OTHER LEVEL LOGIC***************************
	//IF - Level completed
	if(mLoadNextLevel)
	{
		LoadNextLevel();
		mLoadNextLevel = false;
	}//IF	

	//IF - Restart level
	if(mRestartLevel)
	{
		//Apply command
		RestartCurrentLevel();
		mRestartLevel = false;
	}//IF 
	//****************************************************************
}

//Drawing of whole scene
void MMGame::RenderScene ()
{
	SingletonIndieLib::Instance()->GetCamera("General")->SetAsCurrent();
	//Debug render of physics
	mPhysicsMgr->DebugRender();

	if(mFirstTickDelayCounter < FIRSTTICKDELAY)
		SingletonIndieLib::Instance()->Render->ClearViewPort(0,0,0);
}	

//Load the first level in sequence of levels
void MMGame::LoadFirstLevel()
{
	//Load first level from vector info
	assert (!mLevels.empty());
	_resetAndPrepareLevel(mLevels[0].name,mLevels[0].fullpath);
}

//Load next level in sequence of levels
void MMGame::LoadNextLevel()
{
	assert(mCurrentLevelPointer);

	//Local variables
	std::string levelname (mCurrentLevelPointer->GetName());
	
	//Search this level in levels vector
	LevelsVectorIterator levelitr;
	//LOOP - Search level
	for(levelitr = mLevels.begin(); levelitr != mLevels.end(); ++levelitr)
	{
		if((*levelitr).name == mCurrentLevelPointer->GetName())
		{
			//Mark as completed
			(*levelitr).completed = true;
			break;
		}
	}//LOOP END

	//-----Mark this level as completed in file------
	//Reset, delete all elements from level
	
	//Open,load document, modify and close
	ticpp::Document levelsdoc(mLevelsFilePath);
	levelsdoc.LoadFile();	//Parsing			
	ticpp::Element* levelssection = levelsdoc.FirstChildElement("Levels");  //Read levels section
	ticpp::Iterator<ticpp::Element> levelsecitr;
	//LOOP - Search for level
	for(levelsecitr = levelsecitr.begin(levelssection); levelsecitr != levelsecitr.end(); levelsecitr++)
	{
		//If id is the same
		if((*levelsecitr).GetAttribute("Id") == levelname)
		{
			(*levelsecitr).SetAttribute("C",true);
			break;
		}	
	}//LOOP END
	levelsdoc.SaveFile();

	//-----Find next level and load it-----
	++levelitr;

	if(levelitr != mLevels.end())
	{
		//Once found, restart it by loading it again from scratch
		_resetAndPrepareLevel((*levelitr).name,(*levelitr).fullpath);
	}
	else
	{
		//TODO: PROCESS END OF LEVELS
	}
}

//Load a level given an index in sequence of levels
void MMGame::LoadLevel(int)
{
	//assert(static_cast<size_t>(index) < mLevels.size());
	//TODO
}

void MMGame::RestartCurrentLevel()
{
	//IF - A previous level existed
	if(mCurrentLevelPointer)
	{
		//Local variables
		std::string levelname (mCurrentLevelPointer->GetName());
	
		//Search this level in levels vector
		LevelsVectorIterator levelitr;
		//LOOP - Search level
		for(levelitr = mLevels.begin(); levelitr != mLevels.end(); ++levelitr)
		{
			if((*levelitr).name == mCurrentLevelPointer->GetName())
			{
				break;
			}
		}//LOOP END

		assert(levelitr != mLevels.end());

		//Once found, restart it by loading it again from scratch
		_resetAndPrepareLevel((*levelitr).name,(*levelitr).fullpath);
	}//IF
}

//Initialize game
void MMGame::_init()
{
	//Init levels filepath
	//Read from xml levels file, the path of this level
	mLevelsFilePath = g_ConfigOptions.GetScriptsPath() + "Levels.xml";  //Path Hard-coded
	_loadLevels();  //Parse levels file
	
	//Create monkey builder
	mMonkeyBuilder = new MonkeyBuilder(this);

	//Create event listener
	mEventListener = new MMGameListener(this);

}

//Release resources
void MMGame::_release()
{
	//IF - A level was loaded
	if(mCurrentLevelPointer)
	{
		//Reset, delete all elements from level
		std::string prevlevelname = mCurrentLevelPointer->GetName();
		mCurrentLevelPointer.reset();
		//Release resources for this level
		SingletonResourceMgr::Instance()->ReleaseLevelResources(prevlevelname);

	}//IF
	
	//Monkey Definitions
	if(mMonkeyBuilder)
	{
		delete mMonkeyBuilder;
		mMonkeyBuilder = NULL;
	}

	//Event Listener
	if(mEventListener)
	{
		delete mEventListener;
		mEventListener = NULL;
	}


}

//Parse levels file
void MMGame::_loadLevels()
{
	//********************LOAD LEVELS***************************************
	//Load the correct level
	//Open and load document
	//-----------Load level path and name from levels file----
	ticpp::Document levelsdoc(mLevelsFilePath);
	levelsdoc.LoadFile();	//Parsing	
	ticpp::Element* levelssection = levelsdoc.FirstChildElement("Levels");

	ticpp::Iterator<ticpp::Element> itr;
	//LOOP - Load all levels
	for(itr = itr.begin(levelssection); itr != itr.end(); itr++)
	{
		std::string levelid ((*itr).GetAttribute("Id"));
		//TODO: CHECK CORRECTNESS
		/*
		//Id
		std::string spriteId = child->GetAttribute("Id");
		if(
			spriteId == ""
			||
			mEntitiesMap.find(spriteId) != mEntitiesMap.end()
		 )
		 throw GenericException("Failure while reading '" + filepath + "'Id '" + spriteId + "' not correct (repeated or empty)!",GenericException::FILE_CONFIG_INCORRECT);
		*/
		std::string levelfilepath ((*itr).GetAttribute("Path"));
		//TODO: CHECK CORRECTNESS
		/*
		//Id
		std::string spriteId = child->GetAttribute("Id");
		if(
			spriteId == ""
			||
			mEntitiesMap.find(spriteId) != mEntitiesMap.end()
		 )
		 throw GenericException("Failure while reading '" + filepath + "'Id '" + spriteId + "' not correct (repeated or empty)!",GenericException::FILE_CONFIG_INCORRECT);	
		*/
		std::string fullpath (g_ConfigOptions.GetWorkingPath() + levelfilepath);
		bool completed(false);
		(*itr).GetAttribute("C",&completed);

		mLevels.push_back(LevelInfo(levelid,completed,fullpath));
	}//LOOP END

	if(mLevels.empty())
		throw GenericException("No levels to load!! Review corresponding file!",GenericException::FILE_CONFIG_INCORRECT);

		//******************LEVELS LOADED***********************************************	
}
//Init game state and load a level
void MMGame::_resetAndPrepareLevel(const std::string& levelname, const std::string& levelfilepath)
{
	//***********************INIT OBJECTS MANAGERS******************************
	//Create physics manager with game parameters
	//Read game parameters for physics
	const PhysicsConfig physicsconf = g_ConfigOptions.GetPhysicsConfiguration(); //Physics
	mPhysicsMgr.reset();
	#ifdef _DEBUGGING //DEBUG MODE: REGISTER DEBUG DRAW
		mPhysicsMgr = PhysicsManagerPointer(
					new PhysicsManager(physicsconf.gravity,
										  physicsconf.timestep,
										  physicsconf.iterations,
										  physicsconf.worldaabbmax,
										  physicsconf.worldaabbmin,
										  SingletonIndieLib::Instance()->Box2DDebugRender)
					);
	#else //NOT DEBUG MODE: DONT REGISTER DEBUG DRAW
		mPhysicsMgr = PhysicsManagerPointer(
			         new PhysicsManager(physicsconf.gravity,
										  physicsconf.timestep,
										  physicsconf.iterations,
										  physicsconf.worldaabbmax,
										  physicsconf.worldaabbmin)
					);
	#endif
	
	//Create agents manager
	mAgentsManager.reset();
	mAgentsManager = AgentsManagerPointer(new AgentsManager(mPhysicsMgr));
	//***********************MANAGERS CREATED***********************************

	//***************************LOAD LEVEL*************************************
	//IF - A previous level existed
	if(mCurrentLevelPointer)
	{
		//Reset, delete all elements from level
		std::string prevlevelname = mCurrentLevelPointer->GetName();
		mCurrentLevelPointer.reset();
		//Release resources for this level
		SingletonResourceMgr::Instance()->ReleaseLevelResources(prevlevelname);
	}//IF
	
	//As we restarted level, empty queues of events
	SingletonGameEventMgr::Instance()->EmptyEventQueues();
	
	//MMGAME: Tracking of bodychain trees deleting
	mBodyChainAgents.clear();

	//--------Load resources for this level---------------------
	//Load resources for level
	SingletonResourceMgr::Instance()->LoadLevelResources(levelname);

	//---Level creation with builder object---
	LevelBuilder thebuilder(this);
			
	//Call creation of object
	thebuilder.LoadLevel(levelfilepath,levelname);
	
	//No errors, get level pointer
	mCurrentLevelPointer = thebuilder.GetCreatedLevel();
	//*****************************LEVEL LOADED*********************************

	//************************READ MONKEY DEFINITIONS **************************
	//XML def file
	ticpp::Document deffile(g_ConfigOptions.GetScriptsPath() + "MonkeyDef.xml");   //Hard-coded
	deffile.LoadFile();  

	ticpp::Element* monkeysdef (deffile.FirstChildElement("Monkeys"));

	mMonkeyBuilder->LoadParams(monkeysdef);

	//**************************************************************************

	//************************RESET INTERNAL LOGIC VARIABLES********************
	mFirstStart = true;
	mGameOver = false;
	mLoadNextLevel = false;
	mRestartLevel = false;
	mLevelCompleted = false;
	mFirstTickDelayCounter = 0.0f;
	mNewMonkeyTime = 8000.0f;

	//Reset statistics 
	nNumMonkeysCreated = 0;
	mGameElapsedTime = 0.0f;
	mUpdateCounter = 0.0f;
	mBranchesLeft = 0;		  
	mNumKO = 0;			  
	mNumOut = 0;	

	std::vector<BodyChainAgent*>::iterator itr;
	//LOOP - Traverse all created bodychain agents
	for(itr = mBodyChainAgents.begin();itr != mBodyChainAgents.end();++itr)
	{
		mBranchesLeft += (*itr)->GetActiveBranches();
	}//LOOP END		
	//*************************VARIABLES RESET**********************************
}

bool MMGame::_handleEvents(const EventData& theevent)
{
	bool eventprocessed(false);

	//IF - New target request from a new AI agent
	if(theevent.GetEventType() == Event_NewTargetRequest)
	{
		const NewTargetRequestEvent& eventdata (static_cast<const NewTargetRequestEvent&>(theevent));
		Vector2 aiagentpos (eventdata.GetPosition());
		std::string aiagentname(eventdata.GetAgentName());

		//LITTLE HACK TO SELECT A NEAR BODY!
		/*
			From all trees (bodychain agents) created, the positions are taken, just of base body. This will
			say (inaccurately) the nearest tree (enough precission for this game). Then, a branch must be chosen,
			one which has less monkeys on it, or one which has the most monkeys on it, just randomly.
		*/

		SingletonLogMgr::Instance()->AddNewLine("MMGame","Calculating new target for monkey...",LOGDEBUG);

		b2Vec2 closestdistance(1000000,1000000);
		BodyChainAgent* closestagent(NULL);

		std::vector<BodyChainAgent*>::iterator itr;
		//LOOP - Traverse all created bodychain agents
		for(itr = mBodyChainAgents.begin();itr != mBodyChainAgents.end();++itr)
		{
			BodyChainPointer basechain((*itr)->GetRootChain());
			if(basechain && (*itr)->GetActiveBranches() > 0)
			{
				BodyChain::BodiesList::const_iterator bodyitr (basechain->GetFirstBodyIterator());
				b2Vec2 bodypos((*bodyitr)->GetPosition());
				
				Vector2 distance(Vector2(bodypos.x,bodypos.y) - aiagentpos);
				//IF - Closer in X			
				if(distance.x < closestdistance.x)
				{
					closestdistance.x = static_cast<float>(distance.x);
					closestagent = (*itr);
				}//IF
			}
		}//LOOP END

		//IF - There was some agent
		if(closestagent)
		{
			//Now, decide in which branch to hang
			int strategydecision (SingletonMath::Instance()->NewRandom(1,100));
			BodyChainPointer selectedbranch;
			
			//IF - Random selection
			if(strategydecision > 10)
			{
				int branchdecision(SingletonMath::Instance()->NewRandom(1,100));
				//Search the first branch where an agent can attach
				BodyChain::BodyChainList::const_iterator bchitr(closestagent->GetFirstBodyChainItr());
				BodyChain::BodyChainList::const_iterator endbchitr(closestagent->GetLastBodyChainItr());
				assert(bchitr != endbchitr);
				
				selectedbranch = *bchitr;

				//LOOP - Search for a valid chain
				while(branchdecision > 15 || !selectedbranch->AgentsCanAttach() || selectedbranch->IsBroken())
				{
					//Generate new random
					branchdecision = SingletonMath::Instance()->NewRandom(1,100);
					//Get next branch in list
					++bchitr;
					if(bchitr != endbchitr)
						selectedbranch =  *bchitr;
					else
						break;

				}//LOOP END

			}//ELSE - Branch with most monkeys
			else
			{
				//Search all branches and find where there are more monkeys
				BodyChain::BodyChainList::const_iterator bchitr(closestagent->GetFirstBodyChainItr());
				BodyChain::BodyChainList::const_iterator endbchitr(closestagent->GetLastBodyChainItr());	
				int mostmonkeys(0);
				
				//LOOP - Search for a valid chain
				while(bchitr != endbchitr )
				{
					//IF - Selected branch is not good
					if(!(*bchitr)->AgentsCanAttach() || (*bchitr)->IsBroken())
					{
						++bchitr;
						continue;
					}//IF
					
					int monkeysthisbranch ((*bchitr)->GetNumAttachedAgents());
					//IF - More monkeys in this branch
					if(monkeysthisbranch >= mostmonkeys)
					{
						selectedbranch = (*bchitr);
						mostmonkeys = monkeysthisbranch;
					}//IF
					++bchitr;
				}//LOOP END
			}//IF

			assert(selectedbranch);

			//Branch selected - Choose a body - Random
			BodyChain::BodiesList::const_iterator bodyitr (selectedbranch->GetFirstBodyIterator());
			BodyChain::BodiesList::const_iterator lastbodyitr (selectedbranch->GetLastBodyIterator());
			int bodydecision(SingletonMath::Instance()->NewRandom(1,100));
			b2Body* targetbody (*bodyitr);

			//Dont choose first body
			++bodyitr;
			//LOOP - Search for a valid body
			while(bodydecision > 80)
			{
				++bodyitr;
				if(bodyitr != lastbodyitr)
					targetbody = (*bodyitr);
				else 
					break;

				bodydecision = SingletonMath::Instance()->NewRandom(1,100);
			}//LOOP END

			assert(targetbody);

			//Send event back to agent with selected target data
			//Send back response to agent AI
			b2Vec2 target(targetbody->GetPosition());

			NewAITargetInfo eventinfo(aiagentname,Vector2(target.x,target.y),targetbody,selectedbranch->GetId(),closestagent->GetId());

			SingletonGameEventMgr::Instance()->QueueEvent(
										EventDataPointer(new NewAITargetEvent(Event_NewAITarget,eventinfo))
													);	
			
			SingletonLogMgr::Instance()->AddNewLine("MMGame","New target for monkey calculated and sent",LOGDEBUG);
		}//IF - Closest agent
		eventprocessed = true;
	}//ELSE - New agent created event
	else if (theevent.GetEventType() == Event_NewAgentCreated)
	{
		const NewAgentCreatedEvent& eventdata (static_cast<const NewAgentCreatedEvent&>(theevent));
	
		//IF - Body chain agent
		if(eventdata.GetAgentType() == BODYCHAIN)
		{
			IAgent* theagent (mAgentsManager->GetAgent(eventdata.GetAgentId()));
			mBodyChainAgents.push_back(static_cast<BodyChainAgent*>(theagent));
		}//IF
	}//ELSE - "Start game" event
	else if(theevent.GetEventType() == Event_GameStart)
	{
		//Store it
		mGameStarted = true;
	}//ELSE - Restart level
	else if(theevent.GetEventType() == Event_RestartLevel)
	{
		mRestartLevel = true;  //Memorize it	
	}//ELSE - Agent out (monkey out)
	else if(theevent.GetEventType() == Event_AgentOut)
	{	
		const AgentOutEvent& eventdata (static_cast<const AgentOutEvent&>(theevent));
		
		//IF - AI (Monkey only for this game)
		if(eventdata.GetAgentType() == AI)
			mNumOut ++;  //Increment count

	}//ELSE - Monkey KO
	else if(theevent.GetEventType() == Event_MonkeyKO)
	{
		mNumKO++;   //Increment count
	}//ELSE - Branch broke
	else if(theevent.GetEventType() == Event_BranchBroke)
	{
		mBranchesLeft--;
	}

	return eventprocessed;
}