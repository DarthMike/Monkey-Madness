/*
	Filename: MonkeyBuilder.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a MONKEY for the game
	Comments: Fast dirty class to get a creation of a monkey for the game inside an object
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

/*
	VERY HUGE HACK TO GET THE WORK DONE IN TIME....! NOT NICE CODE
*/
#ifndef _MONKEYBUILDER
#define _MONKEYBUILDER

#include <string>
//Class dependencies
#include "XMLParser.h"
#include "LogManager.h"
#include "GenericException.h"
#include "SpriteBuilder.h"
#include "AIAgent.h"
#include "Shared_Resources.h"
#include "PhysicsManager.h"

//Forward declarations
class MMGame;

class MonkeyBuilder
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	MonkeyBuilder(MMGame* game):
	mGame(game)
	{
	}
	~MonkeyBuilder()
	{
	}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	void LoadParams(const ticpp::Element* xmlelement);   //Read a part of xml info and store parameters
	void CreateNewMonkeyAgent(const Vector2& position);  //Creates a new agent in given position
private:
	//----- INTERNAL VARIABLES -----
	MMGame* mGame;
	AIAgentPar mAgentParams;   //A pointer to created bodychain
	b2BodyDef mBodyParams;
	b2PolygonDef mShapeParams;
	b2PolygonDef mSensorShapeParams;

	SpriteBuilder mSpriteBuilder;
	static int mNumCreated;
	//----- INTERNAL FUNCTIONS -----
	bool _getVerticesData(const std::string& verticesdata,float &xvalue, float &yvalue, int vertex);
	int _getNumberofVertices(const std::string& verticesdata);
};

#endif