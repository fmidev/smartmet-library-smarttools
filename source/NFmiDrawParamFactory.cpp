// ======================================================================
/*!
 *
 * C++ Class Name : NFmiDrawParamFactory 
 * ---------------------------------------------------------
 * Filetype: (SOURCE)
 * Filepath: D:/projekti/GDPro/GDTemp/NFmiDrawParamFactory.cpp 
 * 
 * 
 * GDPro Properties 
 * ---------------------------------------------------
 *  - GD Symbol Type    : CLD_Class 
 *  - GD Method         : UML ( 2.1.4 ) 
 *  - GD System Name    : Met-editor Plan 2 
 *  - GD View Type      : Class Diagram 
 *  - GD View Name      : Markon ehdotus 
 * ---------------------------------------------------  
 *  Author         : pietarin 
 *  Creation Date  : Thur - Jan 28, 1999 
 * 
 * 
 *  Description: 
 * 
 *  Change Log: 
 * 1999.08.26/Marko	Laitoin DrawParam:in (oikeastaan DataParam tarvitsee) 
 *					luontiin mukaan tiedon levelist�.
 * 
 */
// ======================================================================

//#ifndef UNIX
//  #include "stdafx.h" // DEBUG_NEW
//#endif

#include "NFmiDrawParamFactory.h"
#include "NFmiSmartInfo.h"
#include "NFmiValueString.h"
#include "NFmiDrawParam.h"
#include <assert.h>
/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

static void InitDrawParam(NFmiDrawParam* theDrawParam, const NFmiString &theFileName, bool createDrawParamFileIfNotExist)
{
	if(theDrawParam)
	{
		if(!theDrawParam->Init(theFileName))
		{
			if(createDrawParamFileIfNotExist)
			{
				if(!theDrawParam->StoreData(theFileName))
				{
					// tiedostoa ei voitu luoda, mit� pit�isi tehd�?
				}
			}
		}
	}
}

//--------------------------------------------------------
// NFmiDrawParamFactory(void) 
//--------------------------------------------------------
NFmiDrawParamFactory::NFmiDrawParamFactory(bool createDrawParamFileIfNotExist) 
:itsLoadDirectory("")
,fCreateDrawParamFileIfNotExist(createDrawParamFileIfNotExist)
{
}
//--------------------------------------------------------
// ~FmiDrawParamFactory(void) 
//--------------------------------------------------------
NFmiDrawParamFactory::~NFmiDrawParamFactory(void)
{
}

//--------------------------------------------------------
// DrawParam 
//--------------------------------------------------------
NFmiDrawParam* NFmiDrawParamFactory::CreateDrawParam ( NFmiSmartInfo* theInfo 
													 , const NFmiDataIdent& theIdent
													 , bool& fSubParam
													 , const NFmiLevel* theLevel
													 , bool setParam)

//  T�ss� metodissa valitaan s��parametrin theParam perusteella piirtoa
//  varten sopiva drawParam. Kostruktorin NFmiDrawParam vaatima pointteri
//  NFmiDataParam* dataParam saadaan attribuutilta itsDataParamFactory.
//  Huomaa, ett� palautettava drawParam = new NFmiDrawParam, joten drawParam   
//  pit�� muistaa tuhota ulkopuolella.

{
	if(!theInfo)
		return 0;

	if(setParam)
	theInfo->Param(theIdent);
	if(theLevel)
		theInfo->Level(*theLevel);
	else
		theInfo->FirstLevel();

//	NFmiDrawParam* drawParam = new NFmiDrawParam(theInfo, theIdent, 1); // 1 = priority
// 7.1.2002/Marko Muutin dataidentin alustuksen niin, ett� se otetaan annetusta
// infosta, jolloin se on aina oikein. Info on aina asetettu halutun parametrin 
// kohdalle, kun se tulee t�nne.
	NFmiDrawParam* drawParam = new NFmiDrawParam(theInfo, setParam ? theInfo->Param() : theIdent, 1); // 1 = priority

	if(drawParam)
	{
		NFmiString fileName = CreateFileName(drawParam);
		InitDrawParam(drawParam, fileName, fCreateDrawParamFileIfNotExist);
	}
	return drawParam;
}

// luodaan drawparam crossSectionDataa varten. Huom k�ytetyt tiedostonimet
// poikkeavat muista drawparamien tiedostonimist�.
// Eli esim. DrawParam_4_CrossSection.dpa
NFmiDrawParam * NFmiDrawParamFactory::CreateCrossSectionDrawParam( NFmiSmartInfo* theInfo
																, const NFmiDataIdent& theIdent
																, bool& fSubParam)
{
	if(!theInfo)
		return 0;
	theInfo->Param(theIdent);
	NFmiDrawParam* drawParam = new NFmiDrawParam(theInfo, theInfo->Param(), 1); // 1 = priority
	if(drawParam)
	{
		NFmiString fileName = CreateFileName(drawParam, true);
		InitDrawParam(drawParam, fileName, fCreateDrawParamFileIfNotExist);
	}
	return drawParam;
}

NFmiDrawParam* NFmiDrawParamFactory::CreateEmptyInfoDrawParam(const NFmiDataIdent& theIdent)
{
	NFmiDrawParam* drawParam = new NFmiDrawParam;

	if(drawParam)
	{
		drawParam->Param(theIdent);
		NFmiString fileName = CreateFileName(drawParam);
		InitDrawParam(drawParam, fileName, fCreateDrawParamFileIfNotExist);
	}
	return drawParam;
}

//--------------------------------------------------------
// Init 
//--------------------------------------------------------
bool NFmiDrawParamFactory::Init()
{
 	return true;
}

//--------------------------------------------------------
// CreateFileName, private 
//--------------------------------------------------------
NFmiString NFmiDrawParamFactory::CreateFileName(NFmiDrawParam* drawParam, bool fCrossSectionCase)
{
	NFmiString fileName(itsLoadDirectory);
	fileName += "DrawParam_";

	if(drawParam)
	{
		int paramId = drawParam->Param().GetParam()->GetIdent();
		NFmiValueString idStr(paramId, "%d");
		fileName += idStr;
		if(fCrossSectionCase)
			fileName += "_CrossSection";
		else
		{
			if(drawParam && drawParam->Info() && drawParam->Info()->SizeLevels() > 1)
			{ // jos leveleit� on useita, niill� on omat tiedostot
				fileName += "_level_";
				const NFmiLevel* level = drawParam->Info()->Level();
				int levelTypeId = 0;
				if(level)
					levelTypeId = level->LevelTypeId();
				NFmiValueString levelTypeIdStr(levelTypeId, "%d");
				fileName += levelTypeIdStr;
				fileName += "_";
				int levelValue = level->LevelValue();
				NFmiValueString levelValueStr(levelValue, "%d");
				fileName += levelValueStr;
			}
			else
			{
				// normaali parametrille ei taida olla tiedosto nimess� mit��n ekstraa
			}
		}
		fileName +=".dpa";
	}
	return fileName;
}
