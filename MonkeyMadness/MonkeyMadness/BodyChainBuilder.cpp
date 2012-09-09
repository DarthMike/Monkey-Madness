/*
	Filename: BodychainBuilder.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a series of bodies connected in a chain-like fashion by rev. joints
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "BodyChainBuilder.h"
#include "SpriteBuilder.h"
#include "IndieLibManager.h"
#include "PhysicsManager.h"
#include "Vector2.h"
#include "Math.h"
#include "MMGame.h"
#include <sstream>

//Read a part of xml info and create a blob object
void BodyChainBuilder:: LoadBodyChain(const ticpp::Element* xmlelement)
{
	/* 
	XML DEFINITION EXPECTED FOR BODYCHAIN ELEMENT 
 	Element: BodyChain Atts: Id (string) AnchorId(string) StartX(number) StartY(number) Length(number) StartW(number)
			 EndW(number) NumBodies(number) OverlappingDist(number) JointsMaxTorque(number) JointsLimMin(number) 
			 JointsLimMax(number) EnableControl(number) EnableAgentsAttaching(number) MaxAttached(number)
			 AngDamping(number) LinDamping(number) Density(number) Friction(number) Restitution(number) ParamsScaling(number)
	*/

	//Build a body chain
	assert(!mBodyChain);   //Be sure body chain is empty before creating one!

	//Get attributes from XML
	std::string id;
	std::string anchorid;
	std::string nameappendix;
	float startx,starty,length,startw,endw,overlappingdist,jointsmaxtorque,jointslimmin,jointslimmax;
	float angdamping,lindamping,density,friction,restitution,paramsscaling;
	bool enablecontrol,enableattaching;
	int maxattached(0);
	int numbodies;
	
	id = xmlelement->GetAttribute("Id");
	anchorid = xmlelement->GetAttribute("AnchorId");
	nameappendix = xmlelement->GetAttribute("NameAppendix");
	xmlelement->GetAttribute("StartX",&startx);
	xmlelement->GetAttribute("StartY",&starty);
	xmlelement->GetAttribute("Length",&length);
	xmlelement->GetAttribute("StartW",&startw);
	xmlelement->GetAttribute("EndW",&endw);
	xmlelement->GetAttribute("NumBodies",&numbodies);
	xmlelement->GetAttribute("OverlappingDist",&overlappingdist);
	xmlelement->GetAttribute("JointsMaxTorque",&jointsmaxtorque);
	xmlelement->GetAttribute("JointsLimMin",&jointslimmin);
	xmlelement->GetAttribute("JointsLimMax",&jointslimmax);
	xmlelement->GetAttribute("EnableControl",&enablecontrol);
	xmlelement->GetAttribute("EnableAgentsAttaching",&enableattaching);
	xmlelement->GetAttribute("AngDamping",&angdamping);
	xmlelement->GetAttribute("LinDamping",&lindamping);
	xmlelement->GetAttribute("Density",&density);
	xmlelement->GetAttribute("Friction",&friction);
	xmlelement->GetAttribute("Restitution",&restitution);
	xmlelement->GetAttribute("ParamsScaling",&paramsscaling);

	if(enableattaching)
		xmlelement->GetAttribute("MaxAttached",&maxattached);   //Optional param if attaching is enabled

	//Checkings
	if(startw <= 0.0f || endw <= 0.0f || overlappingdist <= 0.0f || jointsmaxtorque < 0.0f ||(jointslimmax < jointslimmin)
	   || overlappingdist >= length || paramsscaling <= 0.0f)
		throw GenericException("Element:" + id + "Bad attributes detected! - Out of bounds values",GenericException::FILE_CONFIG_INCORRECT);
	
	b2Body* anchorbody(mGame->GetPhysicsManager()->GetBody(anchorid));
	if(!anchorbody)
			throw GenericException("Element:" + id + "Attribute 'AnchorId' incorrect - Body does not exist (or was not created yet)",GenericException::FILE_CONFIG_INCORRECT);
	
	//Direction vector value get
	ticpp::Element* directionelem = xmlelement->FirstChildElement("Direction");
	std::string directiondata (directionelem->GetText());
	float dirx,diry;
	if(!_getVerticesData(directiondata,dirx,diry,0))
			throw GenericException("Element:" + id + "Bad value 'Direction' detected!",GenericException::FILE_CONFIG_INCORRECT);

	//Create the bodychain object
	mBodyChain = BodyChainPointer(new BodyChain(mGame->GetPhysicsManager()));
	
	//Create a vector object from the values
	Vector2 normdirection(dirx,diry);
	normdirection.Normalise();  //Make it unit vector
	Vector2 normperpdirection(-diry,dirx);  //Make a perpendicular chain dir vector
	Vector2 axis(normdirection * length);//Get the length using the direction given

	//Create a body chain with given parameters, and joint each body with a revolute joint
	//Revolute joint predefinition
	b2RevoluteJointDef revjointdef;
	revjointdef.collideConnected = false;
	revjointdef.enableLimit = true;
	revjointdef.lowerAngle = SingletonMath::Instance()->AngleToRadians(jointslimmin,false);
	revjointdef.upperAngle = SingletonMath::Instance()->AngleToRadians(jointslimmax,false);
	revjointdef.enableMotor = true;
	revjointdef.maxMotorTorque = jointsmaxtorque;
	revjointdef.motorSpeed = 0.0f;

	//Body predefinition
	b2BodyDef bodydef;
	bodydef.allowSleep = true;
	bodydef.linearDamping =  lindamping;
	bodydef.angularDamping = angdamping;
	
	//Shape predefinition
	b2PolygonDef shapedef;
	shapedef.density = density;
	shapedef.friction = friction;
	shapedef.restitution = restitution;
	shapedef.isSensor = false;
	shapedef.vertexCount = 4;
	shapedef.filter.groupIndex = -1;
	shapedef.filter.categoryBits = 0x0002;

	//Intial and end points of shapes calculation
	Vector2 perpcut(normperpdirection * startw/2);  //Perpendicular cut vector (from center)
	Vector2 initialpoint1 (startx + perpcut.x,starty + perpcut.y);
	Vector2 initialpoint2 (startx - perpcut.x,starty - perpcut.y);

	perpcut = normperpdirection * endw/2;
	Vector2 endpoint1 (startx + axis.x + perpcut.x,starty + axis.y + perpcut.y);
	Vector2 endpoint2 (startx + axis.x - perpcut.x,starty + axis.y - perpcut.y);
	
	//Shape of the whole chain vectors
	Vector2 outsegment1 (endpoint1 - initialpoint1);
	Vector2 outsegment2 (endpoint2 - initialpoint2);

	//Precreation of elements inside loop
	
	b2Vec2 bodyvertices[4];  //Each body vertices (4, as they will be a 4 vertices polygon)
	b2Body* createdbody(NULL);   //Body to create
	std::string prevbodyname(anchorid);   //Previous body name
	b2Vec2 startpos (startx,starty);  //Initial position in chain (tracker point)
	b2Vec2 nextpos(0,0);
	PhysicsManagerPointer physicsmgr (mGame->GetPhysicsManager()); //Physics manager pointer from game
	//LOOP - Construct the bodies of the chain
	for(int i = 0; i < numbodies;++i)
	{
		//Precalculate vertex position for start in next loop and finish in this loop (second pair of coords)
		nextpos = b2Vec2(static_cast<float>(axis.x),static_cast<float>(axis.y));
		nextpos *= static_cast<float>(i+1)/static_cast<float>(numbodies);
		nextpos.x += startx;
		nextpos.y += starty;

		//Add a body to physics manager
		std::stringstream bodyname;
		bodyname<<id<<nameappendix<<i;
		bodydef.position = nextpos - startpos;
		bodydef.position.x /= 2;
		bodydef.position.y /= 2;
		bodydef.position += startpos;
		createdbody = physicsmgr->CreateBody(&bodydef,bodyname.str());
		
		//Define shape vertex data
		//TODO: MAKE TRAPEZOID SHAPES!!
		/////////////
		Vector2 perpcut(normperpdirection * startw/2);  //Perpendicular cut vector (from center)
		//Definition of points (CCW manner)
		Vector2 shapepoint1 (startpos.x + perpcut.x,startpos.y + perpcut.y);
		Vector2 shapepoint2 (startpos.x - perpcut.x,startpos.y - perpcut.y);
		Vector2 shapepoint3 (nextpos.x - perpcut.x,nextpos.y - perpcut.y);
		Vector2 shapepoint4 (nextpos.x + perpcut.x,nextpos.y + perpcut.y);	

		//////////////TODO!!!!!!//////////////
		
		//Store in definition of shape and create shape for body (shape vertexs in local coords of body!)
		shapedef.vertices[0] = createdbody->GetLocalPoint(b2Vec2(static_cast<float>(shapepoint1.x),
																 static_cast<float>(shapepoint1.y))
														  );
		shapedef.vertices[1] = createdbody->GetLocalPoint(b2Vec2(static_cast<float>(shapepoint2.x),
																 static_cast<float>(shapepoint2.y))
														  );
		shapedef.vertices[2] = createdbody->GetLocalPoint(b2Vec2(static_cast<float>(shapepoint3.x),
																 static_cast<float>(shapepoint3.y))
														  );
		shapedef.vertices[3] = createdbody->GetLocalPoint(b2Vec2(static_cast<float>(shapepoint4.x),
																 static_cast<float>(shapepoint4.y))
														  );

		//Scale shapes density going up in chain
		shapedef.density *= paramsscaling;
		physicsmgr->CreatePolygonShape(&shapedef, bodyname.str());

		//Create joint between this body and previous body
		b2Vec2 jointpos(startpos - b2Vec2(static_cast<float>(normdirection.x * overlappingdist / 2),   
								   static_cast<float>(normdirection.y * overlappingdist / 2))
								   );
		std::stringstream jointname;
		jointname<<id<<"Joint"<<i;
		physicsmgr->CreateRevoluteJoint(&revjointdef,jointname.str() , bodyname.str(), prevbodyname, jointpos);

		//Store this body as pointer for next
		prevbodyname = bodyname.str();
		createdbody->SetMassFromShapes();  //Compute mass
		mBodyChain->mBodiesList.push_back(createdbody);

		//Store this joint in bodychain object
		mBodyChain->mJointsList.push_back(jointname.str());

		//Overlapping distance only affects next body
		startpos = nextpos;
		startpos -= b2Vec2(static_cast<float>(normdirection.x * overlappingdist),   //Apply the overlay offset between shapes
                           static_cast<float>(normdirection.y * overlappingdist));
		
	}//LOOP END

	//Check if there is a sprite related
	ticpp::Element* spriteelem = xmlelement->FirstChildElement("Sprite",false);  //Optional
	if(spriteelem)
	{
		//Build all GFX sprites needed from this one
		float angle;
		SingletonMath::SignedAngleBetweenVecs(Vector2(0,1),normdirection,angle);

		_buildBodyChainSprites(spriteelem,SingletonMath::RadiansToAngle(angle));
	}

	//Set additional params to body chain
	mBodyChain->mChainId = id;
	mBodyChain->mPreviousBodyLink = anchorbody;
	mBodyChain->mAgentsCanAttach = enableattaching;
	mBodyChain->mMaxAttached = maxattached;
	//Enable control as requested
	mBodyChain->EnableControl(enablecontrol);
}

void BodyChainBuilder::_buildBodyChainSprites(ticpp::Element* spriteelem, float angle)
{
	SpriteBuilder spbuilder;

	//Read sprite params to create
	spbuilder.ReadSpriteParams(spriteelem);

	BodyChain::BodiesListIterator itr;
	int zorder(0);
	//LOOP - Create one sprite per body 
	for(itr = mBodyChain->mBodiesList.begin(); itr != mBodyChain->mBodiesList.end(); ++itr, zorder--)
	{
		//Create a new sprite from defs
		spbuilder.CreateNewSprite();
		ContainedSprite newsprite(spbuilder.GetCreatedSprite());
		//Get the shape and measure its lenght and width
		b2Shape* bodyshape ((*itr)->GetShapeList());
		b2PolygonShape* polshape (static_cast<b2PolygonShape*>(bodyshape));
		
		//Only if polygon shape
		if(polshape)
		{
			const b2Vec2* vertices (polshape->GetVertices());
	
			float globalscale(SingletonIndieLib::Instance()->GetGeneralScale());
			b2Vec2 width (vertices[1] - vertices[0]);
			b2Vec2 height(vertices[3] - vertices[0]);
			SingletonIndieLib::Instance()->ScaleToFit(newsprite.gfxentity.get(),
													  newsprite.gfxentity->GetSurface(),
													  static_cast<int>(width.Length()*globalscale),
													  static_cast<int>(height.Length()*globalscale)
													  );

		}
		//Put angle offset as per body chain
		newsprite.rotoffset = angle;
		//Put in position of body
		int posz (newsprite.gfxentity->GetPosZ() + zorder);
		b2Vec2 pos((*itr)->GetPosition());
		newsprite.gfxentity->SetPosition(pos.x,pos.y,posz);
		//Once parametrized, push it in bodychain definition
		mBodyChain->mSpriteList.push_back(newsprite);
	}//LOOP
}

//Utility function to get the vertices data in float format from a string
bool BodyChainBuilder::_getVerticesData(const std::string& verticesdata,float &xvalue, float &yvalue, int vertex)
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