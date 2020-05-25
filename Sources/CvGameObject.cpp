//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvGameObject.cpp
//
//  PURPOSE: Wrapper classes for Civ4 game objects
//
//------------------------------------------------------------------------------------------------

#include "CvGameCoreDLL.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "CyCity.h"
#include "CyGame.h"
#include "CyPlayer.h"
#include "CyPlot.h"
#include "CyTeam.h"
#include "CyUnit.h"

CvGameObjectGame::CvGameObjectGame()
{
}

CvGameObjectTeam::CvGameObjectTeam(CvTeam *pTeam)
{
	m_pTeam = pTeam;
}

CvGameObjectPlayer::CvGameObjectPlayer(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

CvGameObjectCity::CvGameObjectCity(CvCity* pCity)
{
	m_pCity = pCity;
}

CvGameObjectUnit::CvGameObjectUnit(CvUnit* pUnit)
{
	m_pUnit = pUnit;
}

CvGameObjectPlot::CvGameObjectPlot(CvPlot* pPlot)
{
	m_pPlot = pPlot;
}

GameObjectTypes CvGameObjectGame::getGameObjectType() const
{
	return GAMEOBJECT_GAME;
}

GameObjectTypes CvGameObjectTeam::getGameObjectType() const
{
	return GAMEOBJECT_TEAM;
}

GameObjectTypes CvGameObjectPlayer::getGameObjectType() const
{
	return GAMEOBJECT_PLAYER;
}

GameObjectTypes CvGameObjectCity::getGameObjectType() const
{
	return GAMEOBJECT_CITY;
}

GameObjectTypes CvGameObjectUnit::getGameObjectType() const
{
	return GAMEOBJECT_UNIT;
}

GameObjectTypes CvGameObjectPlot::getGameObjectType() const
{
	return GAMEOBJECT_PLOT;
}

CvProperties* CvGameObjectGame::getProperties() const
{
	return GC.getGame().getProperties();
}

const CvProperties* CvGameObjectGame::getPropertiesConst() const
{
	return GC.getGame().getPropertiesConst();
}

CvProperties* CvGameObjectTeam::getProperties() const
{
	return m_pTeam->getProperties();
}

const CvProperties* CvGameObjectTeam::getPropertiesConst() const
{
	return m_pTeam->getPropertiesConst();
}

CvProperties* CvGameObjectPlayer::getProperties() const
{
	return m_pPlayer->getProperties();
}

const CvProperties* CvGameObjectPlayer::getPropertiesConst() const
{
	return m_pPlayer->getPropertiesConst();
}

CvProperties* CvGameObjectCity::getProperties() const
{
	return m_pCity->getProperties();
}

const CvProperties* CvGameObjectCity::getPropertiesConst() const
{
	return m_pCity->getPropertiesConst();
}

CvProperties* CvGameObjectUnit::getProperties() const
{
	return m_pUnit->getProperties();
}

const CvProperties* CvGameObjectUnit::getPropertiesConst() const
{
	return m_pUnit->getPropertiesConst();
}

CvProperties* CvGameObjectPlot::getProperties() const
{
	return m_pPlot->getProperties();
}

const CvProperties* CvGameObjectPlot::getPropertiesConst() const
{
	return m_pPlot->getPropertiesConst();
}

// helper function to call foreach on the object
void callForeach(CvGameObject* pObject, GameObjectTypes eType, bst::function<void (CvGameObject*)> func)
{
	pObject->foreach(eType, func);
}

void CvGameObjectGame::foreach(GameObjectTypes eType, bst::function<void (CvGameObject*)> func)
{
	switch(eType)
	{
		case GAMEOBJECT_TEAM:
			for (int iI = 0; iI < MAX_TEAMS; iI++)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive())
				{
					func(GET_TEAM((TeamTypes)iI).getGameObject());
				}
			}
			break;

		case GAMEOBJECT_PLAYER:
			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
				if (kLoopPlayer.isAlive())
				{
					func(kLoopPlayer.getGameObject());
				}
			}
			break;

		case GAMEOBJECT_CITY:
			foreach(GAMEOBJECT_PLAYER, bst::bind(callForeach, _1, GAMEOBJECT_CITY, func));
			break;

		case GAMEOBJECT_UNIT:
			foreach(GAMEOBJECT_PLAYER, bst::bind(callForeach, _1, GAMEOBJECT_UNIT, func));
			break;

		case GAMEOBJECT_PLOT:
			for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
			{
				func(GC.getMap().plotByIndex(iI)->getGameObject());
			}
			break;

		case GAMEOBJECT_GAME:
			func(this);
			break;
	}
}

void CvGameObjectTeam::foreach(GameObjectTypes eType, bst::function<void (CvGameObject*)> func)
{
	switch(eType)
	{
		case GAMEOBJECT_GAME:
			func(GC.getGame().getGameObject());
			break;

		case GAMEOBJECT_PLAYER:
			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
				if (kLoopPlayer.isAlive())
				{
					if (kLoopPlayer.getTeam() == m_pTeam->getID())
					{
						func((CvGameObject*)&CvGameObjectPlayer(&kLoopPlayer));
					}
				}
			}
			break;

		case GAMEOBJECT_CITY:
			foreach(GAMEOBJECT_PLAYER, bst::bind(callForeach, _1, GAMEOBJECT_CITY, func));
			break;

		case GAMEOBJECT_UNIT:
			foreach(GAMEOBJECT_PLAYER, bst::bind(callForeach, _1, GAMEOBJECT_UNIT, func));
			break;

		case GAMEOBJECT_PLOT:
			for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
			{
				CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);
				if (pLoopPlot->getTeam() == m_pTeam->getID())
				{
					func(pLoopPlot->getGameObject());
				}
			}
			break;

		case GAMEOBJECT_TEAM:
			func(this);
			break;
	}
}

void CvGameObjectPlayer::foreach(GameObjectTypes eType, bst::function<void (CvGameObject*)> func)
{
	int iLoop;
	switch(eType)
	{
		case GAMEOBJECT_GAME:
			func(GC.getGame().getGameObject());
			break;

		case GAMEOBJECT_TEAM:
			func(GET_TEAM(m_pPlayer->getTeam()).getGameObject());
			break;

		case GAMEOBJECT_CITY:
			for (CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
			{
				func(pCity->getGameObject());
			}
			break;

		case GAMEOBJECT_UNIT:
			for (CvUnit* pUnit = m_pPlayer->firstUnit(&iLoop); pUnit != NULL; pUnit = m_pPlayer->nextUnit(&iLoop))
			{
				func(pUnit->getGameObject());
			}
			break;

		case GAMEOBJECT_PLOT:
			for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
			{
				CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);
				if (pLoopPlot->getOwner() == m_pPlayer->getID())
				{
					func(pLoopPlot->getGameObject());
				}
			}
			break;

		case GAMEOBJECT_PLAYER:
			func(this);
			break;
	}
}

void CvGameObjectCity::foreach(GameObjectTypes eType, bst::function<void (CvGameObject*)> func)
{
	switch(eType)
	{
		case GAMEOBJECT_GAME:
			func(GC.getGame().getGameObject());
			break;

		case GAMEOBJECT_TEAM:
			func(GET_TEAM(m_pCity->getTeam()).getGameObject());
			break;

		case GAMEOBJECT_PLAYER:
			func(GET_PLAYER(m_pCity->getOwner()).getGameObject());
			break;

		case GAMEOBJECT_UNIT:
			m_pCity->plot()->getGameObject()->foreach(eType, func);
			break;

		case GAMEOBJECT_PLOT:
			func(m_pCity->plot()->getGameObject());
			break;

		case GAMEOBJECT_CITY:
			func(this);
			break;
	}
}

void CvGameObjectUnit::foreach(GameObjectTypes eType, bst::function<void (CvGameObject*)> func)
{
	switch(eType)
	{
		case GAMEOBJECT_GAME:
			func(GC.getGame().getGameObject());
			break;

		case GAMEOBJECT_TEAM:
			func(GET_TEAM(m_pUnit->getTeam()).getGameObject());
			break;

		case GAMEOBJECT_PLAYER:
			func(GET_PLAYER(m_pUnit->getOwner()).getGameObject());
			break;

		case GAMEOBJECT_CITY:
			m_pUnit->plot()->getGameObject()->foreach(eType, func);
			break;

		case GAMEOBJECT_PLOT:
			func(m_pUnit->plot()->getGameObject());
			break;

		case GAMEOBJECT_UNIT:
			func(this);
			break;
	}
}

void CvGameObjectPlot::foreach(GameObjectTypes eType, bst::function<void (CvGameObject*)> func)
{
	switch(eType)
	{
		case GAMEOBJECT_GAME:
			func(GC.getGame().getGameObject());
			break;

		case GAMEOBJECT_TEAM:
			if (m_pPlot->getTeam() != NO_TEAM)
				func(GET_TEAM(m_pPlot->getTeam()).getGameObject());
			break;

		case GAMEOBJECT_PLAYER:
			if (m_pPlot->getOwner() != NO_PLAYER)
				func(GET_PLAYER(m_pPlot->getOwner()).getGameObject());
			break;

		case GAMEOBJECT_CITY:
			if (m_pPlot->getPlotCity())
				func(m_pPlot->getPlotCity()->getGameObject());
			break;

		case GAMEOBJECT_PLOT:
			func(this);
			break;

		case GAMEOBJECT_UNIT:
			CLLNode<IDInfo>* pUnitNode = m_pPlot->headUnitNode();
			while (pUnitNode != NULL)
			{
				CvUnit* pUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = m_pPlot->nextUnitNode(pUnitNode);
				func(pUnit->getGameObject());
			}
			break;
	}
}

CvGameObjectPlayer* CvGameObjectGame::getOwner()
{
	return NULL;
}

CvGameObjectPlayer* CvGameObjectTeam::getOwner() 
{
	return NULL;
}

CvGameObjectPlayer* CvGameObjectPlayer::getOwner() 
{
	return this;
}

CvGameObjectPlayer* CvGameObjectCity::getOwner() 
{
	return GET_PLAYER(m_pCity->getOwner()).getGameObject();
}

CvGameObjectPlayer* CvGameObjectUnit::getOwner() 
{
	return GET_PLAYER(m_pUnit->getOwner()).getGameObject();
}

CvGameObjectPlayer* CvGameObjectPlot::getOwner() 
{
	if (m_pPlot->getOwner() != NO_PLAYER)
		return GET_PLAYER(m_pPlot->getOwner()).getGameObject();
	return NULL;
}

CvGameObjectTeam* CvGameObjectGame::getTeam() 
{
	return NULL;
}

CvGameObjectTeam* CvGameObjectTeam::getTeam() 
{
	return this;
}

CvGameObjectTeam* CvGameObjectPlayer::getTeam() 
{
	return GET_TEAM(m_pPlayer->getTeam()).getGameObject();
}

CvGameObjectTeam* CvGameObjectCity::getTeam() 
{
	return GET_TEAM(m_pCity->getTeam()).getGameObject();
}

CvGameObjectTeam* CvGameObjectUnit::getTeam() 
{
	return GET_TEAM(m_pUnit->getTeam()).getGameObject();
}

CvGameObjectTeam* CvGameObjectPlot::getTeam() 
{
	if (m_pPlot->getTeam() != NO_TEAM)
		return GET_TEAM(m_pPlot->getTeam()).getGameObject();
	return NULL;
}

CvGameObjectPlot* CvGameObjectGame::getPlot() 
{
	return NULL;
}

CvGameObjectPlot* CvGameObjectTeam::getPlot() 
{
	return NULL;
}

CvGameObjectPlot* CvGameObjectPlayer::getPlot() 
{
	return NULL;
}

CvGameObjectPlot* CvGameObjectCity::getPlot() 
{
	return m_pCity->plot()->getGameObject();
}

CvGameObjectPlot* CvGameObjectUnit::getPlot() 
{
	return m_pUnit->plot()->getGameObject();
}

CvGameObjectPlot* CvGameObjectPlot::getPlot() 
{
	return this;
}

void CvGameObject::foreachOn(GameObjectTypes eType, bst::function<void(CvGameObject *)> func)
{
	CvGameObjectPlot* pPlot = getPlot();
	if (pPlot)
		pPlot->foreachOn(eType, func);
}

void CvGameObject::foreachNear(GameObjectTypes eType, bst::function<void(CvGameObject *)> func, int iDistance)
{
	CvGameObjectPlot* pPlot = getPlot();
	if (pPlot)
		pPlot->foreachNear(eType, func, iDistance);
}

void CvGameObject::foreachRelated(GameObjectTypes eType, RelationTypes eRelation, bst::function<void(CvGameObject *)> func, int iData)
{
	switch (eRelation)
	{
		case RELATION_ASSOCIATED:
			foreach(eType, func);
			break;
		case RELATION_NEAR:
			foreachNear(eType, func, iData);
			break;
		case RELATION_SAME_PLOT:
			foreachOn(eType, func);
			break;
	}
}

// helper function to call function if expression true
void callFuncIf(CvGameObject* pObject, BoolExpr* pExpr, bst::function<void (CvGameObject*)> func)
{
	if (pExpr->evaluate(pObject))
		func(pObject);
}

void CvGameObject::foreachRelatedCond(GameObjectTypes eType, RelationTypes eRelation, bst::function<void(CvGameObject *)> func, BoolExpr* pExpr, int iData)
{
	if (pExpr)
	{
		foreachRelated(eType, eRelation, bst::bind(callFuncIf, _1, pExpr, func), iData);
	}
	else
	{
		foreachRelated(eType, eRelation, func, iData);
	}
}

//helper function to add game object to vector
void addToVector(CvGameObject* pObject, std::vector<CvGameObject*> *kEnum)
{
	kEnum->push_back(pObject);
}

void CvGameObject::enumerate(std::vector<CvGameObject*> &kEnum, GameObjectTypes eType)
{
	foreach(eType, bst::bind(addToVector, _1, &kEnum));
}

void CvGameObject::enumerateOn(std::vector<CvGameObject*> &kEnum, GameObjectTypes eType)
{
	foreachOn(eType, bst::bind(addToVector, _1, &kEnum));
}

void CvGameObject::enumerateNear(std::vector<CvGameObject*> &kEnum, GameObjectTypes eType, int iDistance)
{
	foreachNear(eType, bst::bind(addToVector, _1, &kEnum), iDistance);
}

void CvGameObject::enumerateRelated(std::vector<CvGameObject*>& kEnum, GameObjectTypes eType, RelationTypes eRelation, int iData)
{
	foreachRelated(eType, eRelation, bst::bind(addToVector, _1, &kEnum), iData);
}

void CvGameObject::enumerateRelatedCond(std::vector<CvGameObject*>& kEnum, GameObjectTypes eType, RelationTypes eRelation, BoolExpr* pExpr, int iData)
{
	foreachRelatedCond(eType, eRelation, bst::bind(addToVector, _1, &kEnum), pExpr, iData);
}

void CvGameObjectPlot::foreachOn(GameObjectTypes eType, bst::function<void(CvGameObject *)> func)
{
	if (eType == GAMEOBJECT_PLOT)
	{
		func(this);
	}
	else
	{
		foreach(eType, func);
	}
}

void CvGameObjectPlot::foreachNear(GameObjectTypes eType, bst::function<void(CvGameObject *)> func, int iDistance)
{
	const int iPlotX = m_pPlot->getX();
	const int iPlotY = m_pPlot->getY();

	for (int iX=iPlotX - iDistance; iX <= iPlotX + iDistance; iX++)
	{
		for (int iY=iPlotY - iDistance; iY <= iPlotY + iDistance; iY++)
		{
			CvPlot* pPlot = GC.getMap().plot(iX, iY);
			if (pPlot)
				pPlot->getGameObject()->foreachOn(eType, func);
		}
	}
}

void CvGameObjectCity::foreachRelated(GameObjectTypes eType, RelationTypes eRelation, bst::function<void(CvGameObject *)> func, int iData)
{
	if (eRelation == RELATION_TRADE)
	{
		if (eType == GAMEOBJECT_CITY)
		{
			const int iRoutes = m_pCity->getTradeRoutes();
			for (int i=0; i<iRoutes; i++)
			{
				CvCity* pTradeCity = m_pCity->getTradeCity(i);
				if (pTradeCity)
				{
					func(pTradeCity->getGameObject());
				}
			}
		}
	}
	else if (eRelation == RELATION_WORKING)
	{
		if (eType == GAMEOBJECT_PLOT)
		{
			for (int iI = 0; iI < m_pCity->getNumCityPlots(); iI++)
			{
				CvPlot* pLoopPlot = plotCity(m_pCity->getX(), m_pCity->getY(), iI);
				if (pLoopPlot)
				{
					if (pLoopPlot->getWorkingCity() == m_pCity)
					{
						func(pLoopPlot->getGameObject());
					}
				}
			}
		}
	}
	else
	{
		CvGameObject::foreachRelated(eType, eRelation, func, iData);
	}
}

void CvGameObjectPlot::foreachRelated(GameObjectTypes eType, RelationTypes eRelation, bst::function<void(CvGameObject *)> func, int iData)
{
	if (eRelation == RELATION_WORKING)
	{
		if (eType == GAMEOBJECT_CITY)
		{
			func(m_pPlot->getWorkingCity()->getGameObject());
		}
	}
	else
	{
		CvGameObject::foreachRelated(eType, eRelation, func, iData);
	}
}

void CvGameObjectPlayer::foreachManipulator(ManipCallbackFn func) const
{
	// Civics
	for (int i=0; i<GC.getNumCivicOptionInfos(); i++)
	{
		func(GC.getCivicInfo(m_pPlayer->getCivics((CivicOptionTypes)i)).getPropertyManipulators());
	}

	// State religion
	if (m_pPlayer->getStateReligion() != NO_RELIGION)
	{
		func(GC.getReligionInfo(m_pPlayer->getStateReligion()).getPropertyManipulators());
	}

	// Leader traits
	for (int i=0; i<GC.getNumTraitInfos(); i++)
	{
		//TB Traits begin
		if (m_pPlayer->hasTrait((TraitTypes)i))
		//TB Traits end
		{
			func(GC.getTraitInfo((TraitTypes)i).getPropertyManipulators());
		}
	}

	// Handicap
	func(GC.getHandicapInfo(m_pPlayer->getHandicapType()).getPropertyManipulators());
}

void CvGameObjectCity::foreachManipulator(ManipCallbackFn func) const
{
	// Building manipulators apply to cities
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if(m_pCity->getNumActiveBuilding((BuildingTypes)iI) > 0)
		{
			//Team Project (5)
			if (!m_pCity->isReligiouslyDisabledBuilding((BuildingTypes)iI))
			{
				func(GC.getBuildingInfo((BuildingTypes)iI).getPropertyManipulators());
			}
		}
	}

	// Religions
	for (int i=0; i< GC.getNumReligionInfos(); i++)
	{
		if (m_pCity->isHasReligion((ReligionTypes)i))
		{
			func(GC.getReligionInfo((ReligionTypes)i).getPropertyManipulators());
		}
	}

	// Corporations
	for (int i=0; i< GC.getNumCorporationInfos(); i++)
	{
		if (m_pCity->isHasCorporation((CorporationTypes)i))
		{
			func(GC.getCorporationInfo((CorporationTypes)i).getPropertyManipulators());
		}
	}

	// Specialists
	for (int i=0; i< GC.getNumSpecialistInfos(); i++)
	{
		const int iCount = m_pCity->getSpecialistCount((SpecialistTypes)i) + m_pCity->getFreeSpecialistCount((SpecialistTypes)i);
		for (int j=0; j<iCount; j++)
		{
			func(GC.getSpecialistInfo((SpecialistTypes)i).getPropertyManipulators());
		}
	}
}

void CvGameObjectUnit::foreachManipulator(ManipCallbackFn func) const
{
	// Unit Type
	func(m_pUnit->getUnitInfo().getPropertyManipulators());

	// Promotions
	for (int i=0; i<GC.getNumPromotionInfos(); i++)
	{
		if (m_pUnit->isHasPromotion((PromotionTypes)i))
		{
			func(GC.getPromotionInfo((PromotionTypes)i).getPropertyManipulators());
		}
	}
}

void CvGameObjectPlot::foreachManipulator(ManipCallbackFn func) const
{
	// Terrain Type
	func(GC.getTerrainInfo(m_pPlot->getTerrainType()).getPropertyManipulators());

	// Feature Type
	if (m_pPlot->getFeatureType() != NO_FEATURE)
	{
		func(GC.getFeatureInfo(m_pPlot->getFeatureType()).getPropertyManipulators());
	}

	// Improvement Type
	if (m_pPlot->getImprovementType() != NO_IMPROVEMENT)
	{
		func(GC.getImprovementInfo(m_pPlot->getImprovementType()).getPropertyManipulators());
	}

	// Route Type
	if (m_pPlot->getRouteType() != NO_ROUTE)
	{
		func(GC.getRouteInfo(m_pPlot->getRouteType()).getPropertyManipulators());
	}

	// Bonus Type
	if (m_pPlot->getBonusType() != NO_BONUS)
	{
		func(GC.getBonusInfo(m_pPlot->getBonusType()).getPropertyManipulators());
	}
}

void CvGameObject::eventPropertyChanged(PropertyTypes eProperty, int iNewValue)
{
}

void CvGameObjectCity::eventPropertyChanged(PropertyTypes eProperty, int iNewValue)
{
	//CvString szBuffer;
	const CvPropertyInfo& kInfo = GC.getPropertyInfo(eProperty);
	const int iNum = kInfo.getNumPropertyBuildings();
	//TB Combat Mods (disease special manifestation and removal system)
	//PropertyTypes eDiseaseType = (PropertyTypes)GC.getInfoTypeForString("PROPERTY_DISEASE");

	//if (eProperty != eDiseaseType)
	//{
	//
	if (!GC.getGame().isOption(GAMEOPTION_OUTBREAKS_AND_AFFLICTIONS) || !kInfo.isOAType())
	{
		//TB Combat Mods end
		if (!GET_PLAYER(m_pCity->getOwner()).isNPC())
		{
			for (int i=0; i<iNum; i++)
			{
				const PropertyBuilding& kBuilding = kInfo.getPropertyBuilding(i);
				const bool bHasBuilding = m_pCity->getNumRealBuilding(kBuilding.eBuilding) > 0;
				const bool bInRange = (iNewValue >= kBuilding.iMinValue) && (iNewValue <= kBuilding.iMaxValue);
				if (!bInRange)
				{
					if (bHasBuilding)
					{//TBWORKINGHERE
						//szBuffer.format("Removing Building, Player %s, Building %s, iNewValue %i.", GET_PLAYER(m_pCity->getOwner()).getNameKey(), GC.getBuildingInfo(kBuilding.eBuilding).getTextKeyWide(), iNewValue);
						//gDLL->logMsg("PropertyBuildingOOS.log", szBuffer.c_str(), false, false);
						m_pCity->setNumRealBuilding(kBuilding.eBuilding, 0);
					}
				}
				else
				{
					if (m_pCity->canConstruct(kBuilding.eBuilding, false, false, true, true))
					{
						if (!bHasBuilding)
						{
							//szBuffer.format("Adding Building, Player %s, Building %s, iNewValue %i.", GET_PLAYER(m_pCity->getOwner()).getNameKey(), GC.getBuildingInfo(kBuilding.eBuilding).getTextKeyWide(), iNewValue);
							//gDLL->logMsg("PropertyBuildingOOS.log", szBuffer.c_str(), false, false);
							m_pCity->setNumRealBuilding(kBuilding.eBuilding, 1);
						}
					}
					else
					{
						if (bHasBuilding)
						{
							//szBuffer.format("Removing Building - no longer qualified, Player %s, Building %s, iNewValue %i.", GET_PLAYER(m_pCity->getOwner()).getNameKey(), GC.getBuildingInfo(kBuilding.eBuilding).getTextKeyWide(), iNewValue);
							//gDLL->logMsg("PropertyBuildingOOS.log", szBuffer.c_str(), false, false);
							m_pCity->setNumRealBuilding(kBuilding.eBuilding, 0);
						}
					}
				}
			}
		}
	}
}

void CvGameObjectUnit::eventPropertyChanged(PropertyTypes eProperty, int iNewValue)
{
	PROFILE_FUNC();

	const CvPropertyInfo& kInfo = GC.getPropertyInfo(eProperty);
	const int iNum = kInfo.getNumPropertyPromotions();

	for (int i=0; i<iNum; i++)
	{
		const PropertyPromotion& kPromotion = kInfo.getPropertyPromotion(i);
		const bool bHasPromotion = m_pUnit->isHasPromotion(kPromotion.ePromotion);
		const bool bInRange = (iNewValue >= kPromotion.iMinValue) && (iNewValue <= kPromotion.iMaxValue);
		if (!bInRange)
		{
			if (bHasPromotion)
			{
				m_pUnit->setHasPromotion(kPromotion.ePromotion, false);
			}
		}
		else
		{
			//TB Combat Mods begin
			PromotionRequirements::flags promoFlags = PromotionRequirements::IgnoreHas;
			if (GC.getPromotionInfo(kPromotion.ePromotion).isEquipment())
				promoFlags |= PromotionRequirements::Equip;
			if(GC.getPromotionInfo(kPromotion.ePromotion).isAffliction())
				promoFlags |= PromotionRequirements::Afflict;

			if (!GC.getPromotionInfo(kPromotion.ePromotion).isEquipment() &&
				!GC.getPromotionInfo(kPromotion.ePromotion).isAffliction())
			{
				promoFlags |= PromotionRequirements::Promote;
			}
			if (m_pUnit->canAcquirePromotion(kPromotion.ePromotion, promoFlags))
			//TB Combat Mods end
			{
				if (!bHasPromotion)
				{
					m_pUnit->setHasPromotion(kPromotion.ePromotion, true);
				}
			}
			else
			{
				if (bHasPromotion)
				{
					m_pUnit->setHasPromotion(kPromotion.ePromotion, false);
				}
			}
		}
	}
}

bool CvGameObject::isTag(TagTypes eTag) const
{
	return false;
}

bool CvGameObjectPlayer::isTag(TagTypes eTag) const
{
	switch (eTag)
	{
		case TAG_ANARCHY:
			return m_pPlayer->isAnarchy();
			break;
	}
	return false;
}

bool CvGameObjectCity::isTag(TagTypes eTag) const
{
	switch (eTag)
	{
		case TAG_ANARCHY:
			return m_pCity->isDisorder();
			break;
	}
	return false;
}

bool CvGameObjectUnit::isTag(TagTypes eTag) const
{
	switch (eTag)
	{
		case TAG_ONLY_DEFENSIVE:
			return m_pUnit->isOnlyDefensive();
			break;

		case TAG_SPY:
			return m_pUnit->isSpy();
			break;

		case TAG_FIRST_STRIKE_IMMUNE:
			return m_pUnit->immuneToFirstStrikes();
			break;

		case TAG_NO_DEFENSIVE_BONUS:
			return m_pUnit->noDefensiveBonus();
			break;

		case TAG_CAN_MOVE_IMPASSABLE:
			return m_pUnit->canMoveImpassable();
			break;

		case TAG_HIDDEN_NATIONALITY:
			return m_pUnit->isHiddenNationality();
			break;

		case TAG_BLITZ:
			return m_pUnit->isBlitz();
			break;

		case TAG_ALWAYS_HEAL:
			return m_pUnit->isAlwaysHeal();
			break;

		case TAG_ENEMY_ROUTE:
			return m_pUnit->isEnemyRoute();
			break;

		case TAG_FRESH_WATER:
		case TAG_WATER:
		case TAG_PEAK:
		case TAG_HILL:
		case TAG_FLATLAND:
		case TAG_OWNED:
		case TAG_CITY:
		case TAG_ANARCHY:
		case TAG_COASTAL:
			return m_pUnit->plot()->getGameObject()->isTag(eTag);
			break;
	}
	return false;
}

bool CvGameObjectPlot::isTag(TagTypes eTag) const
{
	const CvCity* pCity = m_pPlot->getPlotCity();
	switch (eTag)
	{
		case TAG_FRESH_WATER:
			return m_pPlot->isFreshWater();
			break;

		case TAG_WATER:
			return m_pPlot->isRiver() || m_pPlot->isWater();
			break;

		case TAG_PEAK:
			return m_pPlot->isPeak();
			break;

		case TAG_HILL:
			return m_pPlot->isHills();
			break;

		case TAG_FLATLAND:
			return m_pPlot->isFlatlands();
			break;

		case TAG_OWNED:
			return m_pPlot->isOwned();
			break;

		case TAG_CITY:
			return m_pPlot->isCity();
			break;

		case TAG_ANARCHY:
			if (pCity)
			{
				return pCity->isDisorder();
			}
			break;

		case TAG_COASTAL:
			return m_pPlot->isCoastalLand();
			break;
	}
	return false;
}

int CvGameObject::getAttribute(AttributeTypes eAttribute) const
{
	return 0;
}

int CvGameObjectCity::getAttribute(AttributeTypes eAttribute) const
{
	switch (eAttribute)
	{
		case ATTRIBUTE_POPULATION:
			return m_pCity->getPopulation();
			break;

		case ATTRIBUTE_HEALTH:
			return m_pCity->healthRate();
			break;

		case ATTRIBUTE_HAPPINESS:
			return m_pCity->happyLevel();
			break;
	}
	return 0;
}

int CvGameObjectGame::getAttribute(AttributeTypes eAttribute) const
{
	int iCount = 0; 

	switch (eAttribute)
	{
	case ATTRIBUTE_PLAYERS:

		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			if (kLoopPlayer.isAlive())
			{
				++iCount;
			}
		}
		return iCount;
		break;

	case ATTRIBUTE_TEAMS:
		for (int iI = 0; iI < MAX_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				++iCount;
			}
		}
		return iCount;
		break;
	}

	return 0;
}

void aggregateHasGOM(CvGameObject* pObject, GOMTypes eType, int iID, bool* bHasGOM)
{
	*bHasGOM = *bHasGOM || pObject->hasGOM(eType, iID);
}

bool CvGameObjectGame::hasGOM(GOMTypes eType, int iID)
{
	switch (eType)
	{
		case GOM_BUILDING:
		{
			// If there is any building of that type created, return true
			return GC.getGame().getBuildingCreatedCount((BuildingTypes)iID) > 0;
		}

		case GOM_PROMOTION:
			// This is not stored in a readily accessible way for the entire game, so return false
			return false;
			//break;
	
		case GOM_TRAIT:
		{
			// Defer to players, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_PLAYER, bst::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_FEATURE:
			// This is not stored in a readily accessible way for the entire game, so return false
			return false;
			//break;

		case GOM_OPTION:
		{
			const GameOptionTypes eOption = (GameOptionTypes) iID;
			return GC.getGame().isOption(eOption);
			//break;
		}

		case GOM_TERRAIN:
			// This is not stored in a readily accessible way for the entire game, so return false
			return false;
			//break;

		case GOM_GAMESPEED:
		{
			const GameSpeedTypes eSpeed = (GameSpeedTypes) iID;
			return GC.getGame().getGameSpeedType() == eSpeed;
			//break;
		}

		case GOM_ROUTE:
			// This is not stored in a readily accessible way for the entire game, so return false
			return false;
			//break;

		case GOM_BONUS:
			// This is not stored in a readily accessible way for the entire game, so return false
			return false;
			//break;

		case GOM_UNITTYPE:
		{
			// If there is any unit of that type created, return true
			return GC.getGame().getUnitCreatedCount((UnitTypes) iID) > 0;
			//break;
		}

		case GOM_TECH:
		{
			// If any team has researched that tech, return true
			const TechTypes eTech = (TechTypes) iID;
			return GC.getGame().countKnownTechNumTeams(eTech) > 0;
			//break;
		}

		case GOM_CIVIC:
		{
			// Defer to players, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_PLAYER, bst::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_RELIGION:
		{
			// True if the religion has been founded
			return GC.getGame().isReligionFounded((ReligionTypes) iID);
			//break;
		}

		case GOM_CORPORATION:
		{
			// True if the corporation has been founded
			return GC.getGame().isCorporationFounded((CorporationTypes) iID);
			//break;
		}

		case GOM_IMPROVEMENT:
			// This is not stored in a readily accessible way for the entire game, so return false
			return false;
			//break;

		case GOM_UNITCOMBAT:
			// This only makes sense on units and maybe sometimes on plots and cities
			return false;
			//break;

		case GOM_HANDICAP:
		{
			const HandicapTypes eHandicap = (HandicapTypes) iID;
			return GC.getGame().getHandicapType() == eHandicap;
			//break;
		}
	}
	return false;
}

bool CvGameObjectTeam::hasGOM(GOMTypes eType, int iID)
{
	switch (eType)
	{
		case GOM_BUILDING:
		{
			// If there is any building of that type in the team, return true
			return m_pTeam->getBuildingCount(static_cast<BuildingTypes>(iID)) > 0;
		}

		case GOM_PROMOTION:
			// This is not stored in a readily accessible way for the entire team, so return false
			return false;
			//break;
	
		case GOM_TRAIT:
		{
			// Defer to players, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_PLAYER, bst::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_FEATURE:
			// This is not stored in a readily accessible way for the entire team, so return false
			return false;
			//break;

		case GOM_OPTION:
		{
			const GameOptionTypes eOption = (GameOptionTypes) iID;
			return GC.getGame().isOption(eOption);
			//break;
		}

		case GOM_TERRAIN:
			// This is not stored in a readily accessible way for the entire team, so return false
			return false;
			//break;

		case GOM_GAMESPEED:
		{
			const GameSpeedTypes eSpeed = (GameSpeedTypes) iID;
			return GC.getGame().getGameSpeedType() == eSpeed;
			//break;
		}

		case GOM_ROUTE:
			// This is not stored in a readily accessible way for the entire team, so return false
			return false;
			//break;

		case GOM_BONUS:
		{
			// Defer to players, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_PLAYER, bst::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_UNITTYPE:
		{
			// If there is any unit of that type in the team, return true
			return m_pTeam->getUnitCount((UnitTypes) iID) > 0;
			//break;
		}

		case GOM_TECH:
		{
			// If the team has researched that tech, return true
			const TechTypes eTech = (TechTypes) iID;
			return m_pTeam->isHasTech(eTech);
			//break;
		}

		case GOM_CIVIC:
		{
			// Defer to players, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_PLAYER, bst::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_RELIGION:
		{
			// Defer to players, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_PLAYER, bst::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_CORPORATION:
		{
			// Defer to players, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_PLAYER, bst::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_IMPROVEMENT:
			// This is not stored in a readily accessible way for the entire team, so return false
			return false;
			//break;

		case GOM_UNITCOMBAT:
			// This only makes sense on units and maybe sometimes on plots and cities
			return false;
			//break;

		case GOM_HANDICAP:
		{
			const HandicapTypes eHandicap = (HandicapTypes) iID;
			return m_pTeam->getHandicapType() == eHandicap;
			//break;
		}
	}
	return false;
}

bool CvGameObjectPlayer::hasGOM(GOMTypes eType, int iID)
{
	switch (eType)
	{
		case GOM_BUILDING:
		{
			// If there is any building of that type of the player, return true
			return m_pPlayer->getBuildingCount(static_cast<BuildingTypes>(iID)) > 0;
		}

		case GOM_PROMOTION:
			// This is not stored in a readily accessible way for the player, so return false
			return false;
			//break;
	
		case GOM_TRAIT:
		{
			// Return true if this player has the trait
			const TraitTypes eTrait = (TraitTypes) iID;
			return m_pPlayer->hasTrait(eTrait);
			//break;
		}

		case GOM_FEATURE:
			// This is not stored in a readily accessible way for the player, so return false
			return false;
			//break;

		case GOM_OPTION:
		{
			const GameOptionTypes eOption = (GameOptionTypes) iID;
			return GC.getGame().isOption(eOption);
			//break;
		}

		case GOM_TERRAIN:
			// This is not stored in a readily accessible way for the player, so return false
			return false;
			//break;

		case GOM_GAMESPEED:
		{
			const GameSpeedTypes eSpeed = (GameSpeedTypes) iID;
			return GC.getGame().getGameSpeedType() == eSpeed;
			//break;
		}

		case GOM_ROUTE:
			// This is not stored in a readily accessible way for the player, so return false
			return false;
			//break;

		case GOM_BONUS:
		{
			// Use getNumAvailableBonuses which takes the bonuses in the capitals plot group, alternative would be hasBonus which loops through all cities
			const BonusTypes eBonus = (BonusTypes) iID;
			return m_pPlayer->getNumAvailableBonuses(eBonus) > 0;
			//break;
		}

		case GOM_UNITTYPE:
		{
			// If there is any unit of that type of the player, return true
			return m_pPlayer->getUnitCount((UnitTypes) iID) > 0;
			//break;
		}

		case GOM_TECH:
		{
			// If the team has researched that tech, return true
			const TechTypes eTech = (TechTypes) iID;
			return GET_TEAM(m_pPlayer->getTeam()).isHasTech(eTech);
			//break;
		}

		case GOM_CIVIC:
		{
			// Return true if this player has the civic active
			const CivicTypes eCivic = (CivicTypes) iID;
			return m_pPlayer->isCivic(eCivic);
			//break;
		}

		case GOM_RELIGION:
		{
			// True if the religion is state religion
			return m_pPlayer->getStateReligion() == (ReligionTypes)iID;
			//break;
		}

		case GOM_CORPORATION:
		{
			// True if the corporation is active
			return m_pPlayer->isActiveCorporation((CorporationTypes)iID);
			//break;
		}

		case GOM_IMPROVEMENT:
			// This is not stored in a readily accessible way for the player, so return false
			return false;
			//break;

		case GOM_UNITCOMBAT:
			// This only makes sense on units and maybe sometimes on plots and cities
			return false;
			//break;

		case GOM_HANDICAP:
		{
			const HandicapTypes eHandicap = (HandicapTypes) iID;
			return m_pPlayer->getHandicapType() == eHandicap;
			//break;
		}
	}
	return false;
}

bool CvGameObjectCity::hasGOM(GOMTypes eType, int iID)
{
	switch (eType)
	{
		case GOM_BUILDING:
		{
			// return true if the building is present and active
			const BuildingTypes eBuilding = (BuildingTypes) iID;
			return m_pCity->getNumActiveBuilding(eBuilding) > 0;
			//break;
		}

		case GOM_PROMOTION:
		{
			// return true if that is a free promotion from the city
			const PromotionTypes ePromotion = (PromotionTypes) iID;
			return m_pCity->isFreePromotion(ePromotion);
			//break;
		}
	
		case GOM_TRAIT:
		{
			// Return true if the owner has the trait
			const TraitTypes eTrait = (TraitTypes) iID;
			return GET_PLAYER(m_pCity->getOwner()).hasTrait(eTrait);
			//break;
		}

		case GOM_FEATURE:
		{
			// Defer to city plots, combine with OR
			bool bHasGOM = false;
			foreachRelated(GAMEOBJECT_PLOT, RELATION_WORKING, bst::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_OPTION:
		{
			const GameOptionTypes eOption = (GameOptionTypes) iID;
			return GC.getGame().isOption(eOption);
			//break;
		}

		case GOM_TERRAIN:
		{
			// Defer to city plots, combine with OR
			bool bHasGOM = false;
			foreachRelated(GAMEOBJECT_PLOT, RELATION_WORKING, bst::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_GAMESPEED:
		{
			const GameSpeedTypes eSpeed = (GameSpeedTypes) iID;
			return GC.getGame().getGameSpeedType() == eSpeed;
			//break;
		}

		case GOM_ROUTE:
		{
			// Defer to city plots, combine with OR
			bool bHasGOM = false;
			foreachRelated(GAMEOBJECT_PLOT, RELATION_WORKING, bst::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_BONUS:
		{
			// Vicinity not required, only connection
			const BonusTypes eBonus = (BonusTypes) iID;
			return m_pCity->hasBonus(eBonus);
			//break;
		}

		case GOM_UNITTYPE:
		{
			// If there is any unit of that type in the city, return true, defer to the plot for that
			return m_pCity->plot()->getGameObject()->hasGOM(eType, iID);
			//break;
		}

		case GOM_TECH:
		{
			// If the team has researched that tech, return true
			const TechTypes eTech = (TechTypes) iID;
			return GET_TEAM(m_pCity->getTeam()).isHasTech(eTech);
			//break;
		}

		case GOM_CIVIC:
		{
			// Return true if the owning player has the civic active
			const CivicTypes eCivic = (CivicTypes) iID;
			return GET_PLAYER(m_pCity->getOwner()).isCivic(eCivic);
			//break;
		}

		case GOM_RELIGION:
		{
			// True if the religion is in the city
			return m_pCity->isHasReligion((ReligionTypes)iID);
			//break;
		}

		case GOM_CORPORATION:
		{
			// True if the corporation is in the city
			return m_pCity->isHasCorporation((CorporationTypes)iID);
			//break;
		}

		case GOM_IMPROVEMENT:
		{
			// Defer to city plots, combine with OR
			bool bHasGOM = false;
			foreachRelated(GAMEOBJECT_PLOT, RELATION_WORKING, bst::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_UNITCOMBAT:
		{
			// If there is any unit of that type in the city, return true, defer to the plot for that
			return m_pCity->plot()->getGameObject()->hasGOM(eType, iID);
			//break;
		}

		case GOM_HANDICAP:
		{
			const HandicapTypes eHandicap = (HandicapTypes) iID;
			return m_pCity->getHandicapType() == eHandicap;
			//break;
		}
	}
	return false;
}

bool CvGameObjectUnit::hasGOM(GOMTypes eType, int iID)
{
	switch (eType)
	{
		case GOM_BUILDING:
		{
			// return true if the building is present in the city the unit is in and active
			const CvCity* pCity = m_pUnit->plot()->getPlotCity();
			if (pCity)
			{
				const BuildingTypes eBuilding = (BuildingTypes) iID;
				return pCity->getNumActiveBuilding(eBuilding) > 0;
			}
			else
			{
				return false;
			}
			//break;
		}

		case GOM_PROMOTION:
		{
			// return true if the unit has that promotion
			const PromotionTypes ePromotion = (PromotionTypes) iID;
			return m_pUnit->isHasPromotion(ePromotion);
			//break;
		}
	
		case GOM_TRAIT:
		{
			// Return true if the owner has the trait
			const TraitTypes eTrait = (TraitTypes) iID;
			return GET_PLAYER(m_pUnit->getOwner()).hasTrait(eTrait);
			//break;
		}

		case GOM_FEATURE:
		{
			// Check plot on which the unit is
			const FeatureTypes eFeature = (FeatureTypes) iID;
			return m_pUnit->plot()->getFeatureType() == eFeature;
			//break;
		}

		case GOM_OPTION:
		{
			const GameOptionTypes eOption = (GameOptionTypes) iID;
			return GC.getGame().isOption(eOption);
			//break;
		}

		case GOM_TERRAIN:
		{
			// Check plot on which the unit is
			const TerrainTypes eTerrain = (TerrainTypes) iID;
			return m_pUnit->plot()->getTerrainType() == eTerrain;
			//break;
		}

		case GOM_GAMESPEED:
		{
			const GameSpeedTypes eSpeed = (GameSpeedTypes) iID;
			return GC.getGame().getGameSpeedType() == eSpeed;
			//break;
		}

		case GOM_ROUTE:
		{
			// Check plot on which the unit is
			const RouteTypes eRoute = (RouteTypes) iID;
			return m_pUnit->plot()->getRouteType() == eRoute;
			//break;
		}

		case GOM_BONUS:
		{
			// Check plot on which the unit is
			const BonusTypes eBonus = (BonusTypes) iID;
			return m_pUnit->plot()->getBonusType() == eBonus;
			//break;
		}

		case GOM_UNITTYPE:
		{
			// Check the type of the unit
			const UnitTypes eType = (UnitTypes) iID;
			return m_pUnit->getUnitType() == eType;
			//break;
		}

		case GOM_TECH:
		{
			// If the team has researched that tech, return true
			const TechTypes eTech = (TechTypes) iID;
			return GET_TEAM(m_pUnit->getTeam()).isHasTech(eTech);
			//break;
		}

		case GOM_CIVIC:
		{
			// Return true if the owning player has the civic active
			const CivicTypes eCivic = (CivicTypes) iID;
			return GET_PLAYER(m_pUnit->getOwner()).isCivic(eCivic);
			//break;
		}

		case GOM_RELIGION:
		{
			// True if the religion is state religion
			return GET_PLAYER(m_pUnit->getOwner()).getStateReligion() == (ReligionTypes)iID;
			//break;
		}

		case GOM_CORPORATION:
		{
			// True if the corporation is active
			return GET_PLAYER(m_pUnit->getOwner()).isActiveCorporation((CorporationTypes)iID);
			//break;
		}

		case GOM_IMPROVEMENT:
		{
			// Check plot on which the unit is
			const ImprovementTypes eImprovement = (ImprovementTypes) iID;
			return m_pUnit->plot()->getImprovementType() == eImprovement;
			//break;
		}

		case GOM_UNITCOMBAT:
		{
			// Check the combat types of the unit
			const UnitCombatTypes eType = (UnitCombatTypes) iID;
			return m_pUnit->isHasUnitCombat(eType);
			//break;
		}

		case GOM_HANDICAP:
		{
			const HandicapTypes eHandicap = (HandicapTypes) iID;
			return m_pUnit->getHandicapType() == eHandicap;
			//break;
		}
	}
	return false;
}

bool CvGameObjectPlot::hasGOM(GOMTypes eType, int iID)
{
	switch (eType)
	{
		case GOM_BUILDING:
		{
			// return true if the building is present in the city on this plot and active
			const CvCity* pCity = m_pPlot->getPlotCity();
			if (pCity)
			{
				const BuildingTypes eBuilding = (BuildingTypes) iID;
				return pCity->getNumActiveBuilding(eBuilding) > 0;
			}
			else
			{
				return false;
			}
			//break;
		}

		case GOM_PROMOTION:
		{
			// Defer to units on the plot, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_UNIT, bst::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}
	
		case GOM_TRAIT:
		{
			// Return true if the owner has the trait
			const PlayerTypes ePlayer = m_pPlot->getOwner();
			if (ePlayer != NO_PLAYER)
			{
				const TraitTypes eTrait = (TraitTypes) iID;
				return GET_PLAYER(ePlayer).hasTrait(eTrait);
			}
			else
			{
				return false;
			}
			//break;
		}

		case GOM_FEATURE:
		{
			// Check feature type
			const FeatureTypes eFeature = (FeatureTypes) iID;
			return m_pPlot->getFeatureType() == eFeature;
			//break;
		}

		case GOM_OPTION:
		{
			const GameOptionTypes eOption = (GameOptionTypes) iID;
			return GC.getGame().isOption(eOption);
			//break;
		}

		case GOM_TERRAIN:
		{
			// Check terrain type
			const TerrainTypes eTerrain = (TerrainTypes) iID;
			return m_pPlot->getTerrainType() == eTerrain;
			//break;
		}

		case GOM_GAMESPEED:
		{
			const GameSpeedTypes eSpeed = (GameSpeedTypes) iID;
			return GC.getGame().getGameSpeedType() == eSpeed;
			//break;
		}

		case GOM_ROUTE:
		{
			// Check route type
			const RouteTypes eRoute = (RouteTypes) iID;
			return m_pPlot->getRouteType() == eRoute;
			//break;
		}

		case GOM_BONUS:
		{
			// Check bonus type
			const BonusTypes eBonus = (BonusTypes) iID;
			return m_pPlot->getBonusType() == eBonus;
			//break;
		}

		case GOM_UNITTYPE:
		{
			// Defer to units on the plot, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_UNIT, bst::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_TECH:
		{
			// If the team has researched that tech, return true
			const TeamTypes eTeam = m_pPlot->getTeam();
			if (eTeam != NO_TEAM)
			{
				const TechTypes eTech = (TechTypes) iID;
				return GET_TEAM(eTeam).isHasTech(eTech);
			}
			else
			{
				return false;
			}
			//break;
		}

		case GOM_CIVIC:
		{
			// Return true if the owning player has the civic active
			const PlayerTypes ePlayer = m_pPlot->getOwner();
			if (ePlayer != NO_PLAYER)
			{
				const CivicTypes eCivic = (CivicTypes) iID;
				return GET_PLAYER(ePlayer).isCivic(eCivic);
			}
			else
			{
				return false;
			}
			//break;
		}

		case GOM_RELIGION:
		{
			// return true if the religion is present in the city on this plot
			const CvCity* pCity = m_pPlot->getPlotCity();
			if (pCity)
			{
				return pCity->isHasReligion((ReligionTypes)iID);
			}
			else
			{
				return false;
			}
			//break;
		}

		case GOM_CORPORATION:
		{
			// return true if the corporation is present in the city on this plot
			const CvCity* pCity = m_pPlot->getPlotCity();
			if (pCity)
			{
				return pCity->isHasCorporation((CorporationTypes)iID);
			}
			else
			{
				return false;
			}
			//break;
		}

		case GOM_IMPROVEMENT:
		{
			// Check improvement type
			const ImprovementTypes eImprovement = (ImprovementTypes) iID;
			return m_pPlot->getImprovementType() == eImprovement;
			//break;
		}

		case GOM_UNITCOMBAT:
		{
			// Defer to units on the plot, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_UNIT, bst::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_HANDICAP:
		{
			// Return true if the owning player has the handicap
			const PlayerTypes ePlayer = m_pPlot->getOwner();
			if (ePlayer != NO_PLAYER)
			{
				const HandicapTypes eHandicap = (HandicapTypes) iID;
				return GET_PLAYER(ePlayer).getHandicapType() == eHandicap;
			}
			else
			{
				return false;
			}
			//break;
		}
	}
	return false;
}

void* CvGameObjectGame::createPythonWrapper(PyObject*& pyObj)
{
	CyGame* wrapper = new CyGame();
	pyObj = gDLL->getPythonIFace()->makePythonObject(wrapper);
	return wrapper;
}

void CvGameObjectGame::disposePythonWrapper(void *pArgument)
{
	delete (CyGame*)pArgument;
}

void* CvGameObjectTeam::createPythonWrapper(PyObject*& pyObj)
{
	CyTeam* wrapper = new CyTeam(m_pTeam);
	pyObj = gDLL->getPythonIFace()->makePythonObject(wrapper);
	return wrapper;
}

void CvGameObjectTeam::disposePythonWrapper(void *pArgument)
{
	delete (CyTeam*)pArgument;
}

void* CvGameObjectPlayer::createPythonWrapper(PyObject*& pyObj)
{
	CyPlayer* wrapper = new CyPlayer(m_pPlayer);
	pyObj = gDLL->getPythonIFace()->makePythonObject(wrapper);
	return wrapper;
}

void CvGameObjectPlayer::disposePythonWrapper(void *pArgument)
{
	delete (CyPlayer*)pArgument;
}

void* CvGameObjectCity::createPythonWrapper(PyObject*& pyObj)
{
	CyCity* wrapper = new CyCity(m_pCity);
	pyObj = gDLL->getPythonIFace()->makePythonObject(wrapper);
	return wrapper;
}

void CvGameObjectCity::disposePythonWrapper(void *pArgument)
{
	delete (CyCity*)pArgument;
}

void* CvGameObjectUnit::createPythonWrapper(PyObject*& pyObj)
{
	CyUnit* wrapper = new CyUnit(m_pUnit);
	pyObj = gDLL->getPythonIFace()->makePythonObject(wrapper);
	return wrapper;
}

void CvGameObjectUnit::disposePythonWrapper(void *pArgument)
{
	delete (CyUnit*)pArgument;
}

void* CvGameObjectPlot::createPythonWrapper(PyObject*& pyObj)
{
	CyPlot* wrapper = new CyPlot(m_pPlot);
	pyObj = gDLL->getPythonIFace()->makePythonObject(wrapper);
	return wrapper;
}

void CvGameObjectPlot::disposePythonWrapper(void *pArgument)
{
	delete (CyPlot*)pArgument;
}


int CvGameObject::adaptValueToGame(int iID, int iValue) const
{
	iValue = (iValue * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getPercent(iID)) / 100;
	return (iValue * GC.getWorldInfo(GC.getMap().getWorldSize()).getPercent(iID)) / 100;
}

int CvGameObjectPlayer::adaptValueToGame(int iID, int iValue) const
{
	iValue = (iValue * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getPercent(iID)) / 100;
	iValue = (iValue * GC.getWorldInfo(GC.getMap().getWorldSize()).getPercent(iID)) / 100;
	return (iValue * GC.getHandicapInfo(m_pPlayer->getHandicapType()).getPercent(iID)) / 100;
}

int CvGameObjectCity::adaptValueToGame(int iID, int iValue) const
{
	iValue = (iValue * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getPercent(iID)) / 100;
	iValue = (iValue * GC.getWorldInfo(GC.getMap().getWorldSize()).getPercent(iID)) / 100;
	return (iValue * GC.getHandicapInfo(GET_PLAYER(m_pCity->getOwner()).getHandicapType()).getPercent(iID)) / 100;
}

int CvGameObjectUnit::adaptValueToGame(int iID, int iValue) const
{
	iValue = (iValue * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getPercent(iID)) / 100;
	iValue = (iValue * GC.getWorldInfo(GC.getMap().getWorldSize()).getPercent(iID)) / 100;
	return (iValue * GC.getHandicapInfo(GET_PLAYER(m_pUnit->getOwner()).getHandicapType()).getPercent(iID)) / 100;
}