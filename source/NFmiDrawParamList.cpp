//**********************************************************
// C++ Class Name : NFmiDrawParamList
// ---------------------------------------------------------
// Filetype: (SOURCE)
// Filepath: D:/projekti/GDPro/GDTemp/NFmiDrawParamList.cpp
//
//
// GDPro Properties
// ---------------------------------------------------
//  - GD Symbol Type    : CLD_Class
//  - GD Method         : UML ( 2.1.4 )
//  - GD System Name    : Met-editor Plan 2
//  - GD View Type      : Class Diagram
//  - GD View Name      : Markon ehdotus
// ---------------------------------------------------
//  Author         : pietarin
//  Creation Date  : Thur - Jan 28, 1999
//
//
//  Description:
//   Voi olla ett� t�ss� riitt�� suoraan uuden
//   template lista luokan ilmentym�.
//
//  Change Log:
//
//**********************************************************
#ifdef _MSC_VER
#pragma warning(disable : 4786 4512) // poistaa n kpl VC++ k��nt�j�n "liian pitk� tyyppi nimi" varoitusta
#endif

#include "NFmiDrawParamList.h"
#include "NFmiDrawParam.h"
#include <algorithm>

//--------------------------------------------------------
// Constructor  NFmiDrawParamList
//--------------------------------------------------------
NFmiDrawParamList::NFmiDrawParamList(void)
:fBeforeFirstItem(true),
itsList(),
itsIter(itsList.begin()),
fDirtyList(true),
fHasBorrowedParams(false)
{
}

//--------------------------------------------------------
// Destructor  ~NFmiDrawParamList
//--------------------------------------------------------
NFmiDrawParamList::~NFmiDrawParamList(void)
{
	Clear();
}

//--------------------------------------------------------
// Add
//--------------------------------------------------------
bool NFmiDrawParamList::Add(boost::shared_ptr<NFmiDrawParam> &theParam)
{
	if(theParam)
	{
		if(theParam->DataType() == NFmiInfoData::kSatelData) // kSatelData tarkoittaa oikeasti vain kuva muotoista dataa eli ei v�ltt�m�tt� satelliitti dataa siis
		{ // satelliitti-kuvat pit�� laitaa aina 1. n�yt�lle, koska ne peitt�v�t muut
			itsList.push_front(theParam);
			fDirtyList = true;
			return true;
		}
		else
		{
			itsList.push_back(theParam);
			fDirtyList = true;
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------
// Add
// Index count starts from 1 i.e first item in list is with index number 1
// and second is 2 and so on.
// If index is bigger than list, addition is done to end of list.
// If given index <=0 addition is done to the start of list.
//--------------------------------------------------------
bool NFmiDrawParamList::Add(boost::shared_ptr<NFmiDrawParam> &theParam, unsigned long theIndex)
{
   if(theParam)
   {
	   if(theIndex <= 1)
			itsList.push_front(theParam);
	   else if(theIndex > itsList.size())
			itsList.push_back(theParam);
	   else
	   {
		   IterType pos = itsList.begin();
		   std::advance(pos, theIndex - 1);
		   itsList.insert(pos, theParam);
	   }
		fDirtyList = true;
		return true;
   }
   return false;
}

// ottaa toiselta listalta parametrit lainaan (tekee kopiot, mutta merkit�� lainatuiksi)
void NFmiDrawParamList::BorrowParams(NFmiDrawParamList &theList)
{
	for(theList.Reset(); theList.Next(); )
	{
		DataType tmp = boost::shared_ptr<NFmiDrawParam>(new NFmiDrawParam(*(theList.Current())));
		tmp->BorrowedParam(true);
		Add(tmp);
		HasBorrowedParams(true);
	}
}

// poistaa listasta lainatut parametrit
void NFmiDrawParamList::ClearBorrowedParams(void)
{
	for(Reset(); Next(); )
	{
		if(Current()->BorrowedParam())
			Remove();
	}
	HasBorrowedParams(false);
}

//--------------------------------------------------------
// Current
//--------------------------------------------------------
boost::shared_ptr<NFmiDrawParam> NFmiDrawParamList::Current(void)
{
	if(fBeforeFirstItem || itsIter == itsList.end())
		return boost::shared_ptr<NFmiDrawParam>();
	else 
		return *itsIter;
}
//--------------------------------------------------------
// Reset
//--------------------------------------------------------
bool NFmiDrawParamList::Reset(void)
{
	itsIter = itsList.begin();
	fBeforeFirstItem = true;
	return true;
}
//--------------------------------------------------------
// Next
//--------------------------------------------------------
bool NFmiDrawParamList::Next(void)
{
	if(itsIter == itsList.end())
		return false;
	if(fBeforeFirstItem)
	{
		fBeforeFirstItem = false;
		return true;
	}
	else
	{
		++itsIter;
		return itsIter != itsList.end();
	}
}
//--------------------------------------------------------
// Clear
//--------------------------------------------------------
void NFmiDrawParamList::Clear()
{
	itsList.clear();
	fDirtyList = true;
}
//--------------------------------------------------------
// Remove
//--------------------------------------------------------
bool NFmiDrawParamList::Remove(void)
{
	if(fBeforeFirstItem || itsIter == itsList.end())
		return false;
	else
	{
		itsIter = itsList.erase(itsIter);
		fDirtyList = true;
		return true;
   }
}
//--------------------------------------------------------
// Index
//--------------------------------------------------------
bool NFmiDrawParamList::Index(unsigned long index)
{
	if(index <= 0)
	{
		Reset();
		return false;
	}
	else if(index > itsList.size())
	{
		fBeforeFirstItem = false;
		itsIter = itsList.end();
		return false;
	}
	else
	{
		fBeforeFirstItem = false;
		itsIter = itsList.begin();
		std::advance(itsIter, index - 1);
		return true;
	}
}
//--------------------------------------------------------
// Find
//--------------------------------------------------------
/*
bool NFmiDrawParamList::Find(NFmiDrawParam* item)
{
	if(item)
	{
		itsIter = itsList.Find(item);
		if(Current())
			return true;
		Reset();
	}
	return false;
}
*/
//--------------------------------------------------------
// Update
//--------------------------------------------------------
void NFmiDrawParamList::Update(void)
{

}

void NFmiDrawParamList::HideAllParams(bool newState)
{
	for(Reset(); Next();)
		Current()->HideParam(newState);
}

// asettaa kaikkien listalla olevien tilan 'ei editoitaviksi' (toisin p�in ei ole j�rkev�� tehd�)
void NFmiDrawParamList::DisableEditing(void)
{
	for(Reset(); Next();)
		Current()->EditParam(false);
}

void NFmiDrawParamList::DeactivateAll(void)
{
	for(Reset(); Next();)
		Current()->Activate(false);
}

bool NFmiDrawParamList::Find(const NFmiDataIdent& theParam, const NFmiLevel* theLevel, NFmiInfoData::Type theDataType, bool fUseOnlyParamId, bool fIgnoreLevelInfo)
{
	for(Reset(); Next();)
	{
		boost::shared_ptr<NFmiDrawParam> drawParam = Current();
		if(theParam.GetParamIdent() == NFmiInfoData::kFmiSpSynoPlot) // pirun synop-parametrille pit�� taas tehd� virityksi�
		{
			if(fUseOnlyParamId || drawParam->Param() == theParam)
				return true;
		}
		else
		{
			if(fUseOnlyParamId ? drawParam->Param().GetParamIdent() == theParam.GetParamIdent() : drawParam->Param() == theParam)
			{
				if(theDataType == NFmiInfoData::kAnyData || drawParam->DataType() == theDataType)
				{
					if(fIgnoreLevelInfo)
						return true;
					if(theLevel == 0 && drawParam->Level().LevelType() == 0)
						return true;
					if(drawParam->Level().LevelType() == 0 && (theLevel->LevelType() == kFmiAnyLevelType || theLevel->LevelType() == kFmiMeanSeaLevel))
						return true; // t�m� case tulee kun nyky��n tehd��n pinta parametreja
					if(theLevel && (*(theLevel) == drawParam->Level()))
						return true;
				}
			}
		}
	}
	return false;
}

// Poistaa listalta kaikki halutun tuottajan ja mahd. annetun levelin parametrit.
// Ei poista niit� parametreja, jotka ovat theParamIdsNotRemoved-listalla. L�y-
// tynyt paramId poistetaan theParamIdsNotRemoved-listalta, ett� niit� ei
// lis�tt�isi my�hemmin t�h�n listaan.
void NFmiDrawParamList::Clear(const NFmiProducer& theProducer, checkedVector<int>& theParamIdsNotRemoved, NFmiLevel* theLevel)
{
	std::list<int> tmpParIdList;
	int size = static_cast<int>(theParamIdsNotRemoved.size());
	for(int i=0; i<size; i++)
		tmpParIdList.push_back(theParamIdsNotRemoved[i]);
	std::list<int>::iterator it;
	if(theLevel)
	{
		for(Reset(); Next();)
		{
			if(*(Current()->Param().GetProducer()) == theProducer)
			{
				if(Current()->Level() == *theLevel)
				{
					it = std::find(tmpParIdList.begin(), tmpParIdList.end(), static_cast<int>(Current()->Param().GetParamIdent()));
					if(it == tmpParIdList.end())
						Remove();
					else
						tmpParIdList.erase(it);
				}
			}
		}
	}
	else
	{
		for(Reset(); Next();)
		{
			if(*(Current()->Param().GetProducer()) == theProducer)
			{
				if(Current()->Level().LevelType() == 0) // jos drawParamilla on joku 'oikea' leveli, ei kosketa siihen, koska nyt k�sittelemme pinta datoja
				{
					it = std::find(tmpParIdList.begin(), tmpParIdList.end(), static_cast<int>(Current()->Param().GetParamIdent()));
					if(it == tmpParIdList.end())
						Remove();
					else
						tmpParIdList.erase(it);
				}
			}
		}
	}
	theParamIdsNotRemoved.clear();
	for(it=tmpParIdList.begin(); it != tmpParIdList.end(); ++it)
		theParamIdsNotRemoved.push_back(*it);
}

/*!
 * Poistaa listalta kaikki halutun tuottajan ja mahd. annetun levelin parametrit.
 * Ei poista niit� parametreja, jotka ovat theParamIdsNotRemoved-listalla. L�y-
 * tynyt paramId poistetaan theParamIdsNotRemoved-listalta, ett� niit� ei
 * lis�tt�isi my�hemmin t�h�n listaan.
 */
void NFmiDrawParamList::Clear(const NFmiProducer& theProducer, std::list<std::pair<int, NFmiLevel> >& theParamIdsAndLevelsNotRemoved)
{
	std::list<std::pair<int, NFmiLevel> >::iterator it;
	for(Reset(); Next();)
	{
		if(*(Current()->Param().GetProducer()) == theProducer)
		{
			if(Current()->Level().LevelType() == kFmiPressureLevel) // koska kyse vain painepinta parametreista, pit�� level tyypin olla painepinta
			{
				std::pair<int, NFmiLevel> tmp(Current()->Param().GetParamIdent(), Current()->Level());
				it = std::find(theParamIdsAndLevelsNotRemoved.begin(), theParamIdsAndLevelsNotRemoved.end(), tmp);
				if(it == theParamIdsAndLevelsNotRemoved.end())
					Remove();
				else
					theParamIdsAndLevelsNotRemoved.erase(it);
			}
		}
	}
}

/*!
 * Poistaa halutun macroParamin listalta. Eli DrawParamin tyyppi pit�� olla oikein
 * Ja sitten tutkitaan drawParamin nimen lyhennyst�, joka on macroparametrien tunnus.
 * Palauttaa true, jos poistettiin mit��n, muuten false.
 */
bool NFmiDrawParamList::RemoveMacroParam(const std::string &theName)
{
	bool status = false;
	std::string wantedName(theName);
	for(Reset(); Next();)
	{
		if(Current()->DataType() == NFmiInfoData::kMacroParam)
		{
			if(Current()->ParameterAbbreviation() == wantedName)
			{
				status = true;
				Remove();
			}
		}
	}
	return status;
}

/*!
 * Liikuttaa aktiivista parametri� listalla halutun verran als/yl�s p�in.
 * Tarkoitus on muuttaa esim. karttan�yt�ll� parametrien piirtoj�rjestyst�.
 * Palauttaa true jos lista meni likaiseksi eli siirto tapahtui.
 * Lis�sin alusta loppuun ja lopusta alkuun erikois k�sittelyt, koska vanha koodi ei toiminut.
 * TODO Koodi n�ytt�� nyt tosi hankalalta ja sen saisi varmaan yksinkertaisemminkin tehty�.
 */
bool NFmiDrawParamList::MoveActiveParam(int theMovement)
{
	int paramCount = NumberOfItems();
	if(theMovement && paramCount > 1)
	{
		int index = FindActive();
		// pit�� huolehtia kahdest� erikoistapauksesta
		// 1. siirret��n alusta loppuun
		if(index == 1 && theMovement < 0)
		{
			IterType iter = itsList.begin();
			DataType drawParam = *iter;
			itsList.pop_front();
			itsList.push_back(drawParam);
			fDirtyList = true;
			return true;
		}
		//  2. siirret��n lopusta alkuun
		else if(index == paramCount && theMovement > 0)
		{
			IterType iter = itsList.end();
			--iter;
			DataType drawParam = *iter;
			itsList.pop_back();
			itsList.push_front(drawParam);
			fDirtyList = true;
			return true;
		}
		else
		{
			int oldIndex = index;
			if(index)
			{
				index += theMovement;
				if(index < 1)
					index = (index + paramCount); // menn��n listan ymp�ri tarvittaessa
				else if(index > paramCount)
					index = (index - paramCount); // menn��n listan ymp�ri tarvittaessa

				// tarkistus koodia sille jos theMovement on suurempi kuin paramCount
				index = FmiMax(index, 1);
				index = FmiMin(index, paramCount);
				if(index != oldIndex) // jos todella tapahtuu siirto, tehd��n se ja laitetaan lista likaiseksi
				{
					Swap(index, oldIndex);
					return true;
				}
			}
		}
	}
	return false;
}

// Siirret��n osoitettua parametria osoitettuun paikkaan listassa,
// jos listassa on kaksi tai enemm�n parametreja
// jos indeksit ovat kunnollisia ja eri suuruisia
bool NFmiDrawParamList::MoveParam(int theMovedParamIndex, int theMoveToPosition)
{
    if(NumberOfItems() >= 2)
    {
        if(theMovedParamIndex != theMoveToPosition)
        {
            if(Index(theMovedParamIndex))
            {
                boost::shared_ptr<NFmiDrawParam> movedDrawPAram = Current(); // Otetaan osoitetulta kohdalta drawParam talteen
                Remove(); // poistetaan siirrett�v� drawParam
                Add(movedDrawPAram, theMoveToPosition); // lis�t��n se haluttuun kohtaan ennen jo poistettua drawParamiajolloin voidaan k�ytt�� annettua indeksi�
                return fDirtyList;
            }
        }
    }
    return false;
}

// T�m� tekee tarkistamattoman kahden itemin swapin listassa.
// indeksit alkoivat 1:st�, eli listan 1. itemi on indeksi 1:ss�.
void NFmiDrawParamList::Swap(int index1, int index2)
{
	if(Index(index1))
	{
		IterType iter1 = itsIter;
		if(Index(index2))
		{
			IterType iter2 = itsIter;
			(*iter1).swap(*iter2);
			fDirtyList = true;
		}
	}
}

/*!
 * Etsii aktiivisen parametrin listalta. Jos l�ytyy, Currentti osoittaa siihen
 * ja sen indeksi palautetaan (indeksit alkavat 1:st�). Jos ei ole aktiivista
 * palautetaan 0.
 */
int NFmiDrawParamList::FindActive(void)
{
	int index = 1;
	for(Reset(); Next(); index++)
	{
		if(Current()->IsActive())
			return index;
	}
	return 0;
}

int NFmiDrawParamList::FindEdited(void)
{
	int index = 1;
	for(Reset(); Next(); index++)
	{
		if(Current()->IsParamEdited())
			return index;
	}
	return 0;
}

/*!
 * Tekee annetusta listasta kopioidut drawParamit omaan listaansa. clearList parametrilla voidaan
 * 'this' -lista tyhjent�� ennen kopiointia.
*/
void NFmiDrawParamList::CopyList(NFmiDrawParamList &theList, bool clearFirst)
{
	if(clearFirst)
		Clear();

	for(theList.Reset(); theList.Next(); )
	{
		DataType tmp = boost::shared_ptr<NFmiDrawParam>(new NFmiDrawParam(*(theList.Current())));
		Add(tmp);
	}
	ActivateOnlyOne();
}

// T�m� metodi varmistaa, ett� listassa on vain yksi aktivoitu DrawParami.
// Jos listassa on useita aktiivisia, ensimm�inen niist� j�� aktiiviseksi.
// Jos listassa ei ole yht��n aktiivista, ensimm�inen aktivoidaan.
void NFmiDrawParamList::ActivateOnlyOne(void)
{
	if(NumberOfItems() == 0)
		return;
	int activeIndex = FindActive();
	DeactivateAll();
	if(activeIndex == 0)
		activeIndex++;
	if(Index(activeIndex)) // pit�isi l�yty�!!
		Current()->Activate(true);
}
