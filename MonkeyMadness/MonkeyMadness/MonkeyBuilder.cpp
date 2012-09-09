/*
	Filename: MonkeyBuilder.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a MONKEY for the game
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "MonkeyBuilder.h"
#include "IndieLibManager.h"
#include "ResourceManager.h"
#include "AgentsManager.h"
#include <sstream>
#include "MMGame.h"

//Statics
int MonkeyBuilder::mNumCreated = 0;

//Read a part of xml info and store parameters
void MonkeyBuilder::LoadParams(const ticpp::Element* xmlelement)
{
	//AI to use
	std::string ai (xmlelement->GetAttribute("AI"));
	//Params for movement
	float steerforce, maxspeedx,maxspeedy;
	xmlelement->GetAttribute("SteerForce",&steerforce);
	xmlelement->GetAttribute("MaxSpeedX",&maxspeedx);
	xmlelement->GetAttribute("MaxSpeedY",&maxspeedy);
	
	mAgentParams.agentAI = ai;
	mAgentParams.maxspeedx = maxspeedx;
	mAgentParams.maxspeedy = maxspeedy;
	mAgentParams.maxsteerforce = steerforce;
	//NO CORRECTNESS CHECK !!!

	//Pre-load sprite params
	ticpp::Element* xmlsprite (xmlelement->FirstChildElement("Sprite"));
	mSpriteBuilder.ReadSpriteParams(xmlsprite);
	///////
	
	//Body params
	ticpp::Element* xmlbody (xmlelement->FirstChildElement("Body"));
	mBodyParams.angle = 0;
	mBodyParams.allowSleep = true;
	xmlbody->GetAttribute("LinDamping",&mBodyParams.linearDamping);
	xmlbody->GetAttribute("AngDamping",&mBodyParams.angularDamping);

	ticpp::Iterator <ticpp::Element> shapeselements;
	//LOOP - Get elements of entity
	for(shapeselements = shapeselements.begin(xmlbody);shapeselements != shapeselements.end();shapeselements++)
	{
		//IF - Entity type is a polygon
		if(shapeselements->Value() == "PolygonShape")
		{
			//Density, restitution, and Friction
			float32 density, restitution, friction;
			shapeselements->GetAttribute("Density",&density);
			shapeselements->GetAttribute("Restitution",&restitution);
			shapeselements->GetAttribute("Friction",&friction);
			//Sensor?
			bool sensor(false);
			shapeselements->GetAttribute("Sensor",&sensor,false);  //Optional param
			//Order
			int32 order;
			shapeselements->GetAttribute("Order",&order);
			//Vertices data
			std::string verticesdata = shapeselements->GetText();
			int parsedvertices = _getNumberofVertices(verticesdata);
			//Checkings
			if(order > b2_maxPolygonVertices || order < 3  || order != parsedvertices/2 || density < 0 || restitution < 0 || friction < 0)
				throw GenericException("Bad attributes for PolygonShape element",GenericException::FILE_CONFIG_INCORRECT);

			if(sensor)
			{
				mSensorShapeParams.density = density;
				mSensorShapeParams.restitution = restitution;
				mSensorShapeParams.friction = friction;
				mSensorShapeParams.vertexCount = order;
				mSensorShapeParams.isSensor = sensor;
				mSensorShapeParams.filter.categoryBits = 0x0004;
			}
			else
			{
				mShapeParams.density = density;
				mShapeParams.restitution = restitution;
				mShapeParams.friction = friction;
				mShapeParams.vertexCount = order;
				mShapeParams.isSensor = sensor;
				mShapeParams.filter.maskBits = 0x0001;
				mShapeParams.filter.categoryBits = 0x0004;				
				mShapeParams.filter.groupIndex = -1;
			}
			//LOOP - Set vertices data of polygon
			for(int i = 0; i<order; i++)
			{
				float x,y;
				if(!_getVerticesData(verticesdata,x,y,i))
					throw GenericException("Data invalid for polygon shape definition in entity ",GenericException::FILE_CONFIG_INCORRECT);
				
				if(sensor)
					mSensorShapeParams.vertices[i].Set(x,y);
				else
					mShapeParams.vertices[i].Set(x,y);
			}//LOOP
		}
	}//LOOP END
}

//Creates a new agent in given position
void MonkeyBuilder::CreateNewMonkeyAgent(const Vector2& position)  
{
	//Increment creation count
	mNumCreated++;

	std::stringstream agentname;
	agentname<<"Monkey"<<mNumCreated;

	///SPRITE CREATION/////
	mSpriteBuilder.CreateNewSprite();
	ContainedSprite sprite = mSpriteBuilder.GetCreatedSprite();

	//Add to agent params too
	mAgentParams.sprite = sprite;

	SingletonLogMgr::Instance()->AddNewLine("MonkeyBuilder","Sprite for Monkey created",LOGDEBUG);
	///////

	////// CREATION OF BODY AND SHAPES  //////////
	PhysicsManagerPointer physicsmgr (mGame->GetPhysicsManager());
	std::stringstream bodyname;
	bodyname<<agentname.str()<<"Body";
	mBodyParams.position = b2Vec2 (static_cast<float>(position.x), static_cast<float>(position.y));
	mAgentParams.physicbody = physicsmgr->CreateBody(&mBodyParams,bodyname.str());  //Body creation
	physicsmgr->CreatePolygonShape(&mShapeParams,bodyname.str());      //Sensor shape creation
	physicsmgr->CreatePolygonShape(&mSensorShapeParams,bodyname.str());      //Main shape definition	IAgent* newmonkey (mAgentsManager->CreateNewAgent("FirstMonkey",&agentparams));

	//////////////////////////////////////////////

	////// FINALLY, CREATION OF AGENT ////////////
	AgentsManagerPointer agentsmgr (mGame->GetAgentsManager());
	IAgent* newmonkey (agentsmgr->CreateNewAgent(agentname.str(), &mAgentParams));
	mAgentParams.physicbody->SetUserData(newmonkey);   //Link body with created agent
	mAgentParams.physicbody->SetMassFromShapes();  //Mass from shapes (dynamic)

	//////////////////////////////////////////////
	
	SingletonLogMgr::Instance()->AddNewLine("MonkeyBuilder","New Monkey created",LOGDEBUG);
}

bool MonkeyBuilder::_getVerticesData(const std::string& verticesdata,float &xvalue, float &yvalue, int vertex)
{
	int number = 0;
	unsigned int startposition = 0;
	unsigned int nextposition = 0;
	unsigned int position = 0;

	//IF - First vertex
	if(vertex == 0)
	{
		startposition = 0;
		nextposition = verticesdata.find(",");
		//Check found
		if(nextposition == std::string::npos)
			return false;
		else
			nextposition++;
	}//ELSE - Not first vertex
	else
	{
		//LOOP - SEARCH FOR SEPARATOR CHARACTER
		while(position < verticesdata.size())
		{
			//Find an ocurrence of separator character in whole string remaining
			position = verticesdata.find(",",position+1,1);
			//Check found
			if(position != std::string::npos)
				number ++; //Increment num of vertices found
 			else
				return false;

			//IF - Found the vertex asked for (x coord)
			if(static_cast<float>(number/2) == static_cast<float>(vertex))
			//if(number%vertex == 0)
			{
				startposition = position+1;
				nextposition = verticesdata.find(",",startposition,1);
				nextposition++;
				break;
			}
		}//LOOP END
	}
	
	//Check vertices where found
	if(startposition == 0 && nextposition == 0)
		return false;

	//DEBUG ASSERT
	assert(startposition < verticesdata.size() && nextposition < verticesdata.size());

	//From positions inside c-strings, get float data of container data
	xvalue = static_cast<float>(atof(verticesdata.substr(startposition,nextposition-1-startposition).c_str()));
	yvalue = static_cast<float>(atof(verticesdata.substr(nextposition).c_str()));

	return true;
}

//Utility function to parse string and find separator characters to see how many numbers are stored in between
int MonkeyBuilder::_getNumberofVertices(const std::string& verticesdata)
{
	int number = 0;
	unsigned int position = 0;

	//LOOP - SEARCH A STRING FOR SEPARATOR CHARACTER, UPDATE COUNT OF FOUND
	while(position < verticesdata.size() && position != std::string::npos)
	{
		//Find an ocurrence of separator character in whole string remaining
		position = verticesdata.find(",",position+1,1);
		if(position != std::string::npos)
			number ++;
	}//LOOP END

	return(number + 1);
}