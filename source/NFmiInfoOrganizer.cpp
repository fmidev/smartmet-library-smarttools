// ======================================================================
/*!
 * C++ Class Name : NFmiInfoOrganizer
 * ---------------------------------------------------------
 * Filetype: (SOURCE)
 * Filepath: D:/projekti/GDPro/GDTemp/NFmiInfoOrganizer.cpp
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
 *  Creation Date  : Tues - Feb 9, 1999
 *
 *
 *  Description:
 *   Sis�lt�� mahdollisesti listan infoja, joista
 *   luokka osaa antaa tarvittavan
 *   infon pointterin parametrin nimell�. Listalla
 *   ei ole j�rjestyst�.
 *
 *  Change Log:
 * 1999.08.26/Marko	Laitoin organizeriin eri tyyppisten datojen talletuksen
 *					mahdollisuuden. Editoitavaa dataa voi olla vain yksi, mutta
 *					katsottavaa/maskattavaa dataa voi olla useita erilaisia ja statio-
 *					n��risi� (esim. topografia) datoja voi olla my�s useita erilaisia.
 * 1999.08.26/Marko	Laitoin level tiedon DrawParam:in luonti-kutsuun varten.
 * 1999.09.22/Marko	DrawParamin luonnissa otetaan huomioon datan tyyppi t�st�l�htien.
 * 1999.09.22/Marko	Lis�sin EditedDatan kopion, jota k�ytet��n visualisoimaan dataan tehtyj� muutoksia.
 *
 */
// ======================================================================

#ifdef _MSC_VER
#pragma warning(disable : 4786) // poistaa n kpl VC++ k��nt�j�n varoitusta (liian pitk� nimi >255 merkki� joka johtuu 'puretuista' STL-template nimist�)
#endif

//#ifndef UNIX
//  #include "stdafx.h" // DEBUG_NEW
//#endif

#include "NFmiInfoOrganizer.h"
#include "NFmiSmartInfo.h"
#include "NFmiDrawParamFactory.h"
#include "NFmiDrawParam.h"
#include "NFmiGrid.h"
#include "NFmiQueryDataUtil.h"
#include "NFmiQueryData.h"
#include "NFmiLatLonArea.h"
#include "NFmiProducerName.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/
//--------------------------------------------------------
// Constructor/Destructor
//--------------------------------------------------------
// luodaan tyhj� lista
NFmiInfoOrganizer::NFmiInfoOrganizer(void)
:itsList()
//,itsIter(itsList.Start())
,itsDrawParamFactory(0)
,itsWorkingDirectory("")
,itsEditedData(0)
,itsEditedDataCopy(0)
//,itsSoundingParamGridSize(30, 30)
,itsMacroParamGridSize(50, 50)
,itsMacroParamMinGridSize(10, 10)
,itsMacroParamMaxGridSize(200, 200)
,itsMacroParamData(0)
,itsMacroParamMissingValueMatrix()
//,itsSoundingParamData(0)
,itsSoundingParamMissingValueMatrix()
,itsCrossSectionMacroParamData(0)
,itsCrossSectionMacroParamMissingValueMatrix()
,fCreateEditedDataCopy(true)
{
}

// destruktori tuhoaa infot
NFmiInfoOrganizer::~NFmiInfoOrganizer (void)
{
	Clear();
	delete itsDrawParamFactory;
	if(itsEditedData)
		itsEditedData->DestroySharedData();
	delete itsEditedData;

	if(itsEditedDataCopy)
		itsEditedDataCopy->DestroySharedData();
	delete itsEditedDataCopy;

	if(itsMacroParamData)
		itsMacroParamData->DestroySharedData();
	delete itsMacroParamData;
/*
	if(itsSoundingParamData)
		itsSoundingParamData->DestroySharedData();
	delete itsSoundingParamData;
*/
	if(itsCrossSectionMacroParamData)
		itsCrossSectionMacroParamData->DestroySharedData();
	delete itsCrossSectionMacroParamData;
}


//--------------------------------------------------------
// Init
//--------------------------------------------------------
bool NFmiInfoOrganizer::Init(const std::string &theDrawParamPath, bool createDrawParamFileIfNotExist, bool createEditedDataCopy, bool fUseOnePressureLevelDrawParam)
{
	fCreateEditedDataCopy = createEditedDataCopy;
 	itsDrawParamFactory =new NFmiDrawParamFactory(createDrawParamFileIfNotExist, fUseOnePressureLevelDrawParam);
	itsDrawParamFactory->LoadDirectory(theDrawParamPath);
 	return itsDrawParamFactory->Init();
}

NFmiSmartInfo* NFmiInfoOrganizer::GetSynopPlotParamInfo(bool& fSubParameter, NFmiInfoData::Type theType)
{
	fSubParameter = false;
	if(theType == NFmiInfoData::kEditable)
		return itsEditedData;
	else
	{
		NFmiSmartInfo * aInfo = 0;
		NFmiPtrList<NFmiSmartInfo>::Iterator aIter = itsList.Start();
		// tutkitaan ensin l�ytyyk� theParam suoraan joltain listassa olevalta NFmiSmartInfo-pointterilta
		while(aIter.Next())
		{
			aInfo = aIter.CurrentPtr();
			if(aInfo->DataType() == theType && aInfo->Producer()->GetIdent() == kFmiSYNOP)
				return aInfo;
		}
	}
	return 0;
}

NFmiSmartInfo* NFmiInfoOrganizer::GetSoundingPlotParamInfo(bool& fSubParameter, NFmiInfoData::Type theType)
{
	fSubParameter = false;
	if(theType == NFmiInfoData::kEditable)
		return itsEditedData;
	else
	{
		NFmiSmartInfo * aInfo = 0;
		NFmiPtrList<NFmiSmartInfo>::Iterator aIter = itsList.Start();
		// tutkitaan ensin l�ytyyk� theParam suoraan joltain listassa olevalta NFmiSmartInfo-pointterilta
		while(aIter.Next())
		{
			aInfo = aIter.CurrentPtr();
			if(aInfo->DataType() == theType && aInfo->Producer()->GetIdent() == kFmiTEMP)
				return aInfo;
		}
	}
	return 0;
}

//--------------------------------------------------------
// Info
//--------------------------------------------------------

//  Palauttaa smartinfon pointterin, jossa on annettu parametri
//  joko kokonaisena tai jonkin parametrin osaparametrina.
//	Kysyt��n ensimm�isen� listassa olevalta infolta, onko infolla theParam
//	suoraan. Jos milt��n infolta ei suoraan saada theParam:a, tutkitaan
//	l�ytyyk� theParam aliparametrina (esim. pilvisyys voi l�yty� parametrin
//	kWeatherAndCloudiness aliparametrina). Periaatteessa theParam voisi l�yty�
//	my�s aliparametrin aliparametrilta jne - tutkittavia aliparametri-tasoja
//	ei ole rajoitettu. T�ss� metodissa tutkitaan vain parameri ja tarvittaessa
//  parametrin aliparametri.

//  22.02.1999 lis�ttiin  tunnistin bool fSubParameter, josta saadaan tieto
//  siit�, oliko l�ydetty parametri itsen�inen vaiko jonkin parametrin aliparametri.
NFmiSmartInfo* NFmiInfoOrganizer::Info ( const FmiParameterName& theParam
									   , bool& fSubParameter
									   , const NFmiLevel* theLevel
									   , NFmiInfoData::Type theType)
{
	if(theParam == NFmiInfoData::kFmiSpSynoPlot || theParam == NFmiInfoData::kFmiSpMinMaxPlot) // synop plot paramille pit�� tehd� kikka (ja min/max plot 9996)
		return GetSynopPlotParamInfo(fSubParameter, theType);
	if(theLevel && theLevel->LevelType() == kFmiSoundingLevel) // sounding plot paramille pit�� tehd� kikka
		return GetSoundingPlotParamInfo(fSubParameter, theType);
	if(theType == NFmiInfoData::kMacroParam || theType == NFmiInfoData::kQ3MacroParam) // macro- parametrit lasketaan t�ll�
		return itsMacroParamData; // t�ss� ei parametreja ja leveleit� ihmetell�, koska ne muutetaan aina lennossa tarpeen vaatiessa
//	if(theType >= NFmiInfoData::kSoundingParameterData) // sounding parametrit lasketaan t�ll�
//		return itsSoundingParamData; // t�ss� ei parametreja ja leveleit� ihmetell�, koska ne muutetaan aina lennossa tarpeen vaatiessa
	if(theType == NFmiInfoData::kCrossSectionMacroParam)
		return itsCrossSectionMacroParamData; // t�ss� ei parametreja ja leveleit� ihmetell�, koska ne muutetaan aina lennossa tarpeen vaatiessa

	bool anyDataOk = (theType == NFmiInfoData::kAnyData);
	NFmiSmartInfo* aInfo = 0;
	if(itsEditedData && (itsEditedData->DataType() == theType || anyDataOk) && itsEditedData->Param(theParam) && (!theLevel || (theLevel && itsEditedData->Level(*theLevel))))
	{
		fSubParameter = itsEditedData->UseSubParam();
		aInfo = itsEditedData;
	}
	else if(itsEditedDataCopy && (itsEditedDataCopy->DataType() == theType || anyDataOk) && itsEditedDataCopy->Param(theParam) && (!theLevel || (theLevel && itsEditedDataCopy->Level(*theLevel))))
	{
		fSubParameter = itsEditedDataCopy->UseSubParam();
		aInfo = itsEditedDataCopy;
	}
	else
	{
		NFmiPtrList<NFmiSmartInfo>::Iterator aIter = itsList.Start();
		// tutkitaan ensin l�ytyyk� theParam suoraan joltain listassa olevalta NFmiSmartInfo-pointterilta
		while(aIter.Next())
		{
			aInfo = aIter.CurrentPtr();
			if((aInfo->DataType() == theType  || anyDataOk) && aInfo->Param(theParam) && (!theLevel || (theLevel && aInfo->Level(*theLevel))))
			{
				if(theLevel == 0 && aInfo->SizeLevels() > 1) // en osaa nyt laittaa t�t� ehtoa edell� olevaan if-lauseeseen, mutta pelkk� edellinen ei toimi, jos leveldata l�ytyy ennen pintadataa
				{
					aInfo = 0; // pit�� aina t�ss� nollata, muuten viimeisen j�lkeen j�� voimaan
					continue;
				}
				fSubParameter = aInfo->UseSubParam();
				break;
			}
			aInfo = 0; // pit�� aina t�ss� nollata, muuten viimeisen j�lkeen j�� voimaan
		}
	}
	if(aInfo && aInfo->SizeLevels() == 1)
		aInfo->FirstLevel();
	return aInfo; // theParam ei l�ytynyt edes aliparametrina milt��n listassa olevalta aInfo-pointterilta
}
//--------------------------------------------------------
// Info
//--------------------------------------------------------

NFmiSmartInfo* NFmiInfoOrganizer::Info ( const NFmiDataIdent& theDataIdent
									   , bool& fSubParameter
									   , const NFmiLevel* theLevel
									   , NFmiInfoData::Type theType
									   , bool fIgnoreProducerName)
{
	if(theDataIdent.GetParamIdent() == NFmiInfoData::kFmiSpSynoPlot || theDataIdent.GetParamIdent() == NFmiInfoData::kFmiSpMinMaxPlot) // synop plot paramille pit�� tehd� kikka (ja min/max plot 9996)
		return GetSynopPlotParamInfo(fSubParameter, theType);
	if(theLevel && theLevel->LevelType() == kFmiSoundingLevel) // sounding plot paramille pit�� tehd� kikka
		return GetSoundingPlotParamInfo(fSubParameter, theType);
	if(theType == NFmiInfoData::kMacroParam || theType == NFmiInfoData::kQ3MacroParam) // macro- parametrit lasketaan t�ll�
		return itsMacroParamData; // t�ss� ei parametreja ja leveleit� ihmetell�, koska ne muutetaan aina lennossa tarpeen vaatiessa
//	if(theType >= NFmiInfoData::kSoundingParameterData) // sounding parametrit lasketaan t�ll�
//		return itsSoundingParamData; // t�ss� ei parametreja ja leveleit� ihmetell�, koska ne muutetaan aina lennossa tarpeen vaatiessa
	if(theType == NFmiInfoData::kCrossSectionMacroParam)
		return itsCrossSectionMacroParamData; // t�ss� ei parametreja ja leveleit� ihmetell�, koska ne muutetaan aina lennossa tarpeen vaatiessa

	bool anyDataOk = (theType == NFmiInfoData::kAnyData);
	NFmiSmartInfo* aInfo = 0;
	if(itsEditedData && (itsEditedData->DataType() == theType || anyDataOk) && itsEditedData->Param(theDataIdent) && (!theLevel || (theLevel && itsEditedData->Level(*theLevel))))
	{
		fSubParameter = itsEditedData->UseSubParam();
		aInfo = itsEditedData;
	}
	else if(itsEditedDataCopy && (itsEditedDataCopy->DataType() == theType || anyDataOk) && itsEditedDataCopy->Param(theDataIdent) && (!theLevel || (theLevel && itsEditedDataCopy->Level(*theLevel))))
	{
		fSubParameter = itsEditedDataCopy->UseSubParam();
		aInfo = itsEditedDataCopy;
	}
	else
	{
		// tutkitaan ensin l�ytyyk� theParam suoraan joltain listassa olevalta NFmiSmartInfo-pointterilta
		for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
		{
			aInfo = &(iter.Current());
			if((aInfo->DataType() == theType || anyDataOk) && aInfo->Param(theDataIdent) && (!theLevel || (theLevel && aInfo->Level(*theLevel))))
			{
				if(!(theLevel == 0 && aInfo->SizeLevels() > 1))
				{
					if(fIgnoreProducerName || (theDataIdent.GetProducer()->GetName() == aInfo->Param().GetProducer()->GetName()))
					{
						fSubParameter = aInfo->UseSubParam();
						break;
					}
				}
			}
			aInfo = 0; // pit�� aina t�ss� nollata, muuten viimeisen j�lkeen j�� voimaan
		}
	}
	if(aInfo && aInfo->SizeLevels() == 1)
		aInfo->FirstLevel();
	return aInfo; // theParam ei l�ytynyt edes aliparametrina milt��n listassa olevalta aInfo-pointterilta
}

// Etsi haluttu crossSection-data. Eli pit�� olla yli 1 leveli�
// eik� etsit� tietty� leveli�.
NFmiSmartInfo* NFmiInfoOrganizer::CrossSectionInfo(const NFmiDataIdent& theDataIdent
													, bool& fSubParameter
													, NFmiInfoData::Type theType
													, bool fIgnoreProducerName)
{
	if(theType == NFmiInfoData::kCrossSectionMacroParam)
		return itsCrossSectionMacroParamData;
	bool anyDataOk = (theType == NFmiInfoData::kAnyData || theType == NFmiInfoData::kEditable);
	NFmiSmartInfo* aInfo = 0;
	if(itsEditedData && (itsEditedData->DataType() == theType || anyDataOk) && itsEditedData->SizeLevels() > 1 && itsEditedData->Param(static_cast<FmiParameterName>(theDataIdent.GetParam()->GetIdent())))
	{
		fSubParameter = itsEditedData->UseSubParam();
		aInfo = itsEditedData;
	}
	else
	{
		// tutkitaan ensin l�ytyyk� theParam suoraan joltain listassa olevalta NFmiSmartInfo-pointterilta
		for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
		{
			aInfo = &(iter.Current());
			if((aInfo->DataType() == theType || anyDataOk) && aInfo->SizeLevels() > 1 && aInfo->Param(theDataIdent))
			{
				if(fIgnoreProducerName || (theDataIdent.GetProducer()->GetName() == aInfo->Param().GetProducer()->GetName()))
				{
					fSubParameter = aInfo->UseSubParam();
					break;
				}
			}
			aInfo = 0; // pit�� aina t�ss� nollata, muuten viimeisen j�lkeen j�� voimaan
		}
	}
	return aInfo; // theParam ei l�ytynyt edes aliparametrina milt��n listassa olevalta aInfo-pointterilta
}

// itsEditedData infon parambagi
NFmiParamBag NFmiInfoOrganizer::EditedParams(void)
{
	if(itsEditedData)
		return *(itsEditedData->ParamDescriptor().ParamBag());
	return NFmiParamBag();
}

// kaikkien apudatojen parametrit yhdess� bagissa (joita voidaan katsoa/maskata)
NFmiParamBag NFmiInfoOrganizer::ViewableParams(void)
{
	return GetParams(NFmiInfoData::kViewable);
}

// vain halutun indeksin parametrit (HUONO VIRITYS KORJAA!!!!)
NFmiParamBag NFmiInfoOrganizer::ViewableParams(int theIndex)
{
	NFmiSmartInfo* info = ViewableInfo(theIndex);
	if(info)
		return info->ParamBag();
	return NFmiParamBag();
}

// vain halutun indeksin parametrit (HUONO VIRITYS KORJAA!!!!)
NFmiParamBag NFmiInfoOrganizer::ObservationParams(int theIndex)
{
	NFmiSmartInfo* info = ObservationInfo(theIndex);
	if(info)
		return info->ParamBag();
	return NFmiParamBag();
}

NFmiParamBag NFmiInfoOrganizer::GetParams(NFmiInfoData::Type theDataType)
{
	NFmiParamBag paramBag;
	for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
	{
		if(iter.Current().DataType() == theDataType)
			paramBag = paramBag.Combine(iter.Current().ParamBag());
	}

    return paramBag;
}

// kaikkien staattisten (ei muutu ajan mukana) datojen parambag (esim. topografia)
NFmiParamBag NFmiInfoOrganizer::StaticParams(void)
{
	return GetParams(NFmiInfoData::kStationary);
}

// SmartToolModifier tarvitsee ohuen kopion (eli NFmiQueryData ei kopioidu)
// T�m� ignooraa aina tuottajien nimet, koska t�t� k�ytet��n SmartToolModifierissa
// ja siell� k�ytet��n tuottajista aina jotain default nimi�
NFmiSmartInfo* NFmiInfoOrganizer::CreateShallowCopyInfo(const NFmiDataIdent& theDataIdent, const NFmiLevel* theLevel, NFmiInfoData::Type theType, bool fUseParIdOnly, bool fLevelData)
{
	bool aSubParam;
	NFmiSmartInfo* info = 0;
	if(fLevelData)
		info = CrossSectionInfo(theDataIdent, aSubParam, theType, true);
	else
		info = fUseParIdOnly ? Info(static_cast<FmiParameterName>(theDataIdent.GetParamIdent()), aSubParam, theLevel, theType) : Info(theDataIdent, aSubParam, theLevel, theType, true);
	if(info)
	{
		if(theType == NFmiInfoData::kCrossSectionMacroParam || theType == NFmiInfoData::kMacroParam || theType == NFmiInfoData::kQ3MacroParam || (fUseParIdOnly ?  info->Param(static_cast<FmiParameterName>(theDataIdent.GetParamIdent())) : info->Param(theDataIdent)))  // makroparamille ei tarvitse laittaa parametria kohdalleen!
		{
			NFmiSmartInfo* copyOfInfo = new NFmiSmartInfo(*info);
			return copyOfInfo;
		}
	}
	return 0;
}

// T�m� luo SmartInfosta syv� kopion eli k�ytt�� Clone-metodia, eli datakin kopioituu ja se pit�� tuhota!!
NFmiSmartInfo* NFmiInfoOrganizer::CreateInfo(const NFmiDataIdent& theDataIdent, const NFmiLevel* theLevel, NFmiInfoData::Type theType, bool fTryParIdAlso)
{
	bool aSubParam;
	NFmiSmartInfo* info = Info(theDataIdent, aSubParam, theLevel, theType);
	if(info == 0 && fTryParIdAlso)
		info = Info(static_cast<FmiParameterName>(theDataIdent.GetParamIdent()), aSubParam, theLevel, theType);
	if(info)
	{
		if(info->Param(theDataIdent))
			return info->Clone();
	}
	return 0;
}

// T�m� luo SmartInfosta pinta kopioin, eli vain ns. iteraattori (info osuus) kopioituu, data pysyy alkuper�isen kanssa jaettuna.
NFmiSmartInfo* NFmiInfoOrganizer::CreateInfo(NFmiSmartInfo* theUsedInfo, const NFmiDataIdent& theDataIdent, const NFmiLevel* theLevel, NFmiInfoData::Type theType)
{
	if(theUsedInfo && theUsedInfo->DataType() == theType && theUsedInfo->Param(theDataIdent) && (!theLevel || (theLevel && theUsedInfo->Level(*theLevel))))
	{
		NFmiSmartInfo* copyOfInfo = new NFmiSmartInfo(*theUsedInfo);
		return copyOfInfo;
	}
	return 0;
}

//--------------------------------------------------------
// CreateDrawParam(NFmiDataIdent& theDataIdent)
//--------------------------------------------------------
// Tutkii l�ytyyk� listasta itsList infoa, jossa on theDataIdent - siis
// etsit��n info, jonka tuottaja ja parametri saadaan theDataIdent:st�.
// Jos t�ll�inen info l�ytyy, pyydet��n itsDrawParamFactory luomaan
// drawParam kyseiselle parametrille l�ydetyn infon avulla.
NFmiDrawParam* NFmiInfoOrganizer::CreateDrawParam(const NFmiDataIdent& theIdent, const NFmiLevel* theLevel, NFmiInfoData::Type theType)
{
// Huomaa, ett� itsDrawParamFactory luo pointterin drawParam new:ll�, joten
// drawParam pit�� muistaa tuhota  NFmiInfoOrganizer:n ulkopuolella
	NFmiDrawParam* drawParam = 0;
	if(theType == NFmiInfoData::kSatelData || theType == NFmiInfoData::kConceptualModelData) // spesiaali keissi satelliitti kuville, niill� ei ole infoa
	{
		drawParam = new NFmiDrawParam(theIdent, NFmiLevel(), 1, theType);
		drawParam->ParameterAbbreviation(static_cast<char*>(theIdent.GetParamName()));
		return drawParam;
	}
	if(theIdent.GetParamIdent() == NFmiInfoData::kFmiSpSynoPlot) // synop plottia varten taas kikkailua
	{
		return CreateSynopPlotDrawParam(theIdent, theLevel, theType);
	}
	drawParam = itsDrawParamFactory->CreateDrawParam(theIdent, theLevel);
	if(drawParam)
		drawParam->DataType(theType); // data tyyppi pit�� my�s asettaa!!
	return drawParam;
}

// hakee poikkileikkausta varten haluttua dataa ja luo siihen sopivan drawparamin
NFmiDrawParam* NFmiInfoOrganizer::CreateCrossSectionDrawParam(const NFmiDataIdent& theDataIdent, NFmiInfoData::Type theType)
{
	NFmiDrawParam* drawParam = 0;
	drawParam = itsDrawParamFactory->CreateCrossSectionDrawParam(theDataIdent);
	if(drawParam)
		drawParam->DataType(theType); // data tyyppi pit�� my�s asettaa!!
	return drawParam;
}

NFmiDrawParam* NFmiInfoOrganizer::CreateSynopPlotDrawParam(const NFmiDataIdent& theDataIdent
														  ,const NFmiLevel* theLevel
														  ,NFmiInfoData::Type theType)
{
	NFmiDrawParam *drawParam = itsDrawParamFactory->CreateDrawParam(theDataIdent, theLevel); // false merkitsee, ett� parametria ei taas aseteta tuolla metodissa
	if(drawParam)
		drawParam->DataType(theType);
	return drawParam;
}

//--------------------------------------------------------
// AddData
//--------------------------------------------------------
// HUOM!!!! T�nne ei ole sitten tarkoitus antaa kFmiDataTypeCopyOfEdited-tyyppist�
// dataa, koska se luodaan kun t�nne annetaan editoitavaa dataa.
bool NFmiInfoOrganizer::AddData(NFmiQueryData* theData
									 ,const std::string& theDataFileName
									 ,const std::string& theDataFilePattern
									 ,NFmiInfoData::Type theDataType
									 ,int theUndoLevel)
{
	bool status = false;
	if(theData)
	{
		NFmiQueryInfo aQueryInfo(theData);
		NFmiSmartInfo* aSmartInfo = new NFmiSmartInfo(aQueryInfo, theData, theDataFileName, theDataFilePattern, theDataType);// ...otetaan k�ytt��n my�hemmin
		if(aSmartInfo)
			aSmartInfo->First();

		if(theDataType == NFmiInfoData::kEditable)
		{
			try
			{
				if(theUndoLevel)
					aSmartInfo->UndoLevel(theUndoLevel);
			}
			catch(...)
			{
				// jos muisti loppuu tai muu poikkeus, asetetaan undo level 0:ksi ja jatketaan
				theUndoLevel = 0;
				aSmartInfo->DestroySharedData(false); // false t�ss� tarkoittaa ett� ei tuhota queryDataa

				throw ; // heitet��n poikkeus eteenp�in
			}

			if(itsEditedData)
			{
				itsEditedData->DestroySharedData();
				delete itsEditedData;
			}
			itsEditedData = aSmartInfo;
			fCreateEditedDataCopy = theUndoLevel ? true : false; // pit�� p�ivitt�� kopion luomiseen vaikuttavaa muuttujaa undo-levelin mukaan
			UpdateEditedDataCopy();

			status = true;
		}
		else
			status = Add(aSmartInfo); // muun tyyppiset datat kuin editoitavat menev�t listaan

	}
	return status;
}

void NFmiInfoOrganizer::ClearData(NFmiInfoData::Type theDataType)
{
	if(theDataType == NFmiInfoData::kEditable)
	{
		if(itsEditedData)
		{
			itsEditedData->DestroySharedData();
			delete itsEditedData;
			itsEditedData = 0;
		}
	}
	else
	{ // k�yd��n lista l�pi ja tuhotaan halutun tyyppiset datat
		for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
		{
			if(iter.Current().DataType() == theDataType)
			{
				iter.Current().DestroySharedData();
				iter.Remove(true);
			}
		}
	}
}

void NFmiInfoOrganizer::ClearDynamicHelpData()
{ 
	std::vector<NFmiInfoData::Type> ignoreTypesVector;
	ignoreTypesVector.push_back(NFmiInfoData::kStationary); // stationaariset eli esim. topografia data ei kuulu poistettaviin datoihin
	ignoreTypesVector.push_back(NFmiInfoData::kClimatologyData); // klimatologiset datat luetaan vain kerran ohjelman k�ynnistyess�

	// k�yd��n lista l�pi ja tuhotaan dynaamiset help datat
	for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
	{
		if(std::find(ignoreTypesVector.begin(), ignoreTypesVector.end(), iter.Current().DataType()) == ignoreTypesVector.end())
		{
			iter.Current().DestroySharedData();
			iter.Remove(true);
		}
	}
}


// theRemovedDatasTimesOut -parametri k�ytet��n optimoidaan esim. havaittua hila datan kanssa teht�vi� ruudun p�ivityksi�,
// editoitavasta datasta ei oteta mit��n aikoja talteen.
void NFmiInfoOrganizer::ClearThisKindOfData(NFmiQueryInfo* theInfo, NFmiInfoData::Type theDataType, const std::string &theFileNamePattern, NFmiTimeDescriptor &theRemovedDatasTimesOut)
{
	if(theInfo)
	{
		if(itsEditedData)
		{
			if(IsInfosTwoOfTheKind(theInfo, theDataType, theFileNamePattern, itsEditedData))
			{
				itsEditedData->DestroySharedData();
				delete itsEditedData;
				itsEditedData = 0;
				return;
			}
		}

		for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
		{
			if(IsInfosTwoOfTheKind(theInfo, theDataType, theFileNamePattern, &(iter.Current())))
			{
				theRemovedDatasTimesOut = iter.Current().TimeDescriptor();
				iter.Current().DestroySharedData();
				iter.Remove(true);
				break; // tuhotaan vain yksi!
			}
		}
	}
}

int NFmiInfoOrganizer::CountData(void)
{
	int count = 0;
	if(this->itsEditedData)
		count++;
	if(this->itsEditedDataCopy)
		count++;

	count += itsList.NumberOfItems();
	return count;
}

double NFmiInfoOrganizer::CountDataSize(void)
{
	double dataSize = 0;
	if(itsEditedData)
		dataSize += itsEditedData->Size() * sizeof(float);
	if(itsEditedDataCopy)
		dataSize += itsEditedDataCopy->Size() * sizeof(float);

	for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
	{
		dataSize += iter.Current().Size() * sizeof(float);
	}
	return dataSize;
}

// this kind of m��ritell��n t�ll� hetkell�:
// parametrien, leveleiden ja mahdollisen gridin avulla (ei location bagin avulla)
bool NFmiInfoOrganizer::IsInfosTwoOfTheKind(NFmiQueryInfo* theInfo1, NFmiInfoData::Type theType1, const std::string &theFileNamePattern, NFmiSmartInfo* theSmartInfo2)
{
	// parametrit ja tuottajat samoja
	if(theInfo1 && theSmartInfo2)
	{
		if(theType1 == theSmartInfo2->DataType())
		{
			if(theFileNamePattern.empty() == false && theSmartInfo2->DataFilePattern().empty() == false && theFileNamePattern == theSmartInfo2->DataFilePattern())
				return true; // jos filepatternit eiv�t olleet tyhji� ja ne ovat samanlaisia, pidet��n datoja samanlaisina (t�m� auttaa, jos datat muuttuvat (paramereja lis��, asemia lis�� jne.))
			if(theInfo1->ParamBag() == theSmartInfo2->ParamBag())
			{
				// Level tyypit pit��olla samoja ja niiden lukum��r�t niin ett� joko leveleit� on molemmissa
				// tasan yksi tai molemmissa on yli yksi.
				// Ongelmia muuten data p�ivityksen kanssa jos esim. luotaus datoissa on eri m��r� leveleit�
				// tai p�ivitet��n hybridi dataa ja level m��r� muuttuu tms.
				FmiLevelType levelType1 = theInfo1->VPlaceDescriptor().Level(0)->LevelType();
				FmiLevelType levelType2 = theSmartInfo2->VPlaceDescriptor().Level(0)->LevelType();
				unsigned long size1 = theInfo1->VPlaceDescriptor().Size();
				unsigned long size2 = theSmartInfo2->VPlaceDescriptor().Size();
				if(levelType1 == levelType2 && (size1 == size2 || (size1 > 1 && size2 > 1)))
				{
					// mahdollinen gridi samoja
					bool status3 = true;
					if(theInfo1->Grid() && theSmartInfo2->Grid())
					{
						status3 = (theInfo1->Grid()->AreGridsIdentical(*theSmartInfo2->Grid())) == true;
					}
					if(status3)
					{
						theInfo1->FirstParam(); // varmistaa, ett� producer l�ytyy
						theSmartInfo2->FirstParam();
						if(*theInfo1->Producer() == *theSmartInfo2->Producer())
							return true;
					}
				}
			}
		}
	}
	return false;
}

//--------------------------------------------------------
// Add
//--------------------------------------------------------
//   Laittaa alkuper�isen pointterin listaan - uutta pointteria
//   ei luoda vaan NFmiInfoOrganizer::itsList ottaa pointterin
//   'omistukseen'. T�m�n luokan destruktori tuhoaa infot.
bool NFmiInfoOrganizer::Add (NFmiSmartInfo* theInfo)
{
	if(theInfo)
		return itsList.InsertionSort(theInfo);
	return false;
}
//--------------------------------------------------------
// Clear
//--------------------------------------------------------
// tuhoaa aina datan
bool NFmiInfoOrganizer::Clear (void)
{
	for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
	{
		iter.Current().DestroySharedData();
		iter.Remove(true);
	}
	itsList.Clear(false);
	return true; // jotain pit�si varmaan tsekatakin?
}

//--------------------------------------------------------
// Reset
//--------------------------------------------------------
/*
bool NFmiInfoOrganizer::Reset (void)
{
   itsIter = itsList.Start();
   return true;
}
//--------------------------------------------------------
// Next
//--------------------------------------------------------
bool NFmiInfoOrganizer::Next (void)
{
   return itsIter.Next();
}
//--------------------------------------------------------
// Current
//--------------------------------------------------------
NFmiSmartInfo* NFmiInfoOrganizer::Current (void)
{
   return itsIter.CurrentPtr();
}

//--------------------------------------------------------
// Remove
//--------------------------------------------------------
bool NFmiInfoOrganizer::Remove(void)
{
	return itsIter.Remove(true);
}
*/

// t�m� toimii vajavaisesti, koska se palauttaa aina 1. kyseisen tyyppisen infon
NFmiSmartInfo* NFmiInfoOrganizer::ViewableInfo(void)
{
	for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
	{
		if(iter.Current().DataType() == NFmiInfoData::kViewable)
			return &(iter.Current());
	}
	return 0;
}

// T�m� on pikaviritys fuktio (kuten nimest�kin voi p��tell�)
// Tarvitaan viel� kun pelataan parametrin valinnassa popupien kanssa (ja niiden kanssa on vaikea pelata).
NFmiLevelBag* NFmiInfoOrganizer::GetAndCreateViewableInfoWithManyLevelsOrZeroPointer(void)
{
	NFmiLevelBag* levels = 0;
	for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
	{
		if(iter.Current().DataType() == NFmiInfoData::kViewable)
			if(iter.Current().SizeLevels() > 1)
			{
				levels = new NFmiLevelBag(*iter.Current().VPlaceDescriptor().Levels());
				break;
			}
	}
	return levels;
}

// palauttaa halutun indeksin infon (huono viritys, KORJAA!!!!)
NFmiSmartInfo* NFmiInfoOrganizer::ViewableInfo(int theIndex)
{
	int ind = 0;
	for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
	{
		if(iter.Current().DataType() == NFmiInfoData::kViewable)
		{
			if(ind == theIndex)
				return &(iter.Current());
			ind++;
		}
	}
	return 0;
}

// palauttaa halutun indeksin infon (huono viritys, KORJAA!!!!)
NFmiSmartInfo* NFmiInfoOrganizer::ObservationInfo(int theIndex)
{
	int ind = 0;
	for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
	{
		if(iter.Current().DataType() == NFmiInfoData::kObservations)
		{
			if(ind == theIndex)
				return &(iter.Current());
			ind++;
		}
	}
	return 0;
}

// 28.09.1999/Marko Tekee uuden kopion editoitavasta datasta
void NFmiInfoOrganizer::UpdateEditedDataCopy(void)
{
	if(fCreateEditedDataCopy)
	{
		if(itsEditedData)
		{
			if(itsEditedDataCopy)
				itsEditedDataCopy->DestroySharedData();
			delete itsEditedDataCopy;
			itsEditedDataCopy = itsEditedData->Clone();
			itsEditedDataCopy->DataType(NFmiInfoData::kCopyOfEdited);
		}
	}
}

static NFmiQueryData* CreateDefaultMacroParamQueryData(const NFmiArea *theArea, int gridSizeX, int gridSizeY)
{
	NFmiLevelBag levelBag;
	levelBag.AddLevel(NFmiLevel(kFmiGroundSurface, 0)); // ihan mit� puppua vain, ei v�li�
	NFmiVPlaceDescriptor vPlace(levelBag);

	NFmiParamBag parBag;
	parBag.Add(NFmiDataIdent(NFmiParam(998, "macroParam", kFloatMissing, kFloatMissing, 1, 0, NFmiString("%.1f"), kLinearly)));
	NFmiParamDescriptor parDesc(parBag);

	NFmiMetTime originTime;
	NFmiTimeBag validTimes(originTime, originTime, 60); // yhden kokoinen feikki timebagi
	NFmiTimeDescriptor timeDesc(originTime, validTimes);

	NFmiGrid grid(theArea, gridSizeX, gridSizeY);
	NFmiHPlaceDescriptor hPlace(grid);

	NFmiQueryInfo info(parDesc, timeDesc, hPlace, vPlace);
	return NFmiQueryDataUtil::CreateEmptyData(info);
}
/*
void NFmiInfoOrganizer::SetSoundingParamDataGridSize(int x, int y)
{
	itsSoundingParamGridSize = NFmiPoint(x, y);
	UpdateSoundingParamData();
}
*/
void NFmiInfoOrganizer::SetMacroParamDataGridSize(int x, int y)
{
	x = FmiMin(x, static_cast<int>(itsMacroParamMaxGridSize.X()));
	y = FmiMin(y, static_cast<int>(itsMacroParamMaxGridSize.Y()));
	x = FmiMax(x, static_cast<int>(itsMacroParamMinGridSize.X()));
	y = FmiMax(y, static_cast<int>(itsMacroParamMinGridSize.Y()));
	itsMacroParamGridSize = NFmiPoint(x, y);
	UpdateMacroParamData();
}
void NFmiInfoOrganizer::SetMacroParamDataMinGridSize(int x, int y)
{
	itsMacroParamMinGridSize = NFmiPoint(x, y);
}
void NFmiInfoOrganizer::SetMacroParamDataMaxGridSize(int x, int y)
{
	itsMacroParamMaxGridSize = NFmiPoint(x, y);
}

void NFmiInfoOrganizer::UpdateMacroParamData(void)
{
	if(itsMacroParamData)
	{
		std::auto_ptr<NFmiArea> arePtr(itsMacroParamData->Area()->Clone());
		UpdateSpecialDataArea(arePtr.get(), itsMacroParamGridSize, NFmiInfoData::kMacroParam, &itsMacroParamData, itsMacroParamMissingValueMatrix);
	}
}
/*
void NFmiInfoOrganizer::UpdateSoundingParamData(void)
{
	if(itsSoundingParamData)
	{
		std::auto_ptr<NFmiArea> arePtr(itsSoundingParamData->Area()->Clone());
		UpdateSpecialDataArea(arePtr.get(), itsSoundingParamGridSize, NFmiInfoData::kSoundingParameterData, &itsSoundingParamData, itsSoundingParamMissingValueMatrix);
	}
}
*/

void NFmiInfoOrganizer::UpdateMapArea(const NFmiArea *theArea)
{
	UpdateSpecialDataArea(theArea, itsMacroParamGridSize, NFmiInfoData::kMacroParam, &itsMacroParamData, itsMacroParamMissingValueMatrix);
//	UpdateSpecialDataArea(theArea, itsSoundingParamGridSize, NFmiInfoData::kSoundingParameterData, &itsSoundingParamData, itsSoundingParamMissingValueMatrix);
}

void NFmiInfoOrganizer::UpdateSpecialDataArea(const NFmiArea *theArea, const NFmiPoint &theGridSize, NFmiInfoData::Type theType, NFmiSmartInfo ** theData, NFmiDataMatrix<float> &theMissingValueMatrix)
{
	// tuhoa ensin vanha pois alta
	if(*theData)
		(*theData)->DestroySharedData();
	delete *theData;
	*theData = 0;

	// Luo sitten uusi data jossa on yksi aika,param ja level ja luo hplaceDesc annetusta areasta ja hila koosta
	NFmiQueryData* data = CreateDefaultMacroParamQueryData(theArea, static_cast<int>(theGridSize.X()), static_cast<int>(theGridSize.Y()));
	if(data)
	{
		NFmiQueryInfo infoIter(data);
		*theData = new NFmiSmartInfo(infoIter, data, "", "", theType);
		theMissingValueMatrix.Resize((*theData)->Grid()->XNumber(), (*theData)->Grid()->YNumber(), kFloatMissing);
	}
}

void NFmiInfoOrganizer::UpdateCrossSectionMacroParamDataSize(int x, int y)
{
	static std::auto_ptr<NFmiArea> dummyArea(new NFmiLatLonArea(NFmiPoint(19,57), NFmiPoint(32,71)));
	// tuhoa ensin vanha pois alta
	if(itsCrossSectionMacroParamData)
		itsCrossSectionMacroParamData->DestroySharedData();
	delete itsCrossSectionMacroParamData;
	itsCrossSectionMacroParamData = 0;

	// Luo sitten uusi data jossa on yksi aika,param ja level ja luo hplaceDesc annetusta areasta ja hila koosta
	NFmiQueryData* data = CreateDefaultMacroParamQueryData(dummyArea.get(), x, y);
	if(data)
	{
		NFmiQueryInfo infoIter(data);
		itsCrossSectionMacroParamData = new NFmiSmartInfo(infoIter, data, "", "", NFmiInfoData::kCrossSectionMacroParam);
		itsCrossSectionMacroParamMissingValueMatrix.Resize(itsCrossSectionMacroParamData->Grid()->XNumber(), itsCrossSectionMacroParamData->Grid()->YNumber(), kFloatMissing);
	}
}

// kaikkien staattisten (ei muutu ajan mukana) datojen parambag (esim. topografia)
NFmiParamBag NFmiInfoOrganizer::ObservationParams(void)
{
	return GetParams(NFmiInfoData::kObservations);
}

// t�m� toimii vajavaisesti, koska se palauttaa aina 1. kyseisen tyyppisen infon
NFmiSmartInfo* NFmiInfoOrganizer::ObservationInfo(void)
{
	for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
	{
		if(iter.Current().DataType() == NFmiInfoData::kObservations)
			return &(iter.Current());
	}
	return 0;
}

// t�m� toimii vajavaisesti, koska se palauttaa aina 1. kyseisen tyyppisen infon
NFmiSmartInfo* NFmiInfoOrganizer::KepaDataInfo(void)
{
	for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
	{
		if(iter.Current().DataType() == NFmiInfoData::kKepaData)
			return &(iter.Current());
	}
	return 0;
}

// t�m� toimii vajavaisesti, koska se palauttaa aina 1. kyseisen tyyppisen infon
NFmiSmartInfo* NFmiInfoOrganizer::ClimatologyInfo(void)
{
	for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
	{
		if(iter.Current().DataType() == NFmiInfoData::kClimatologyData)
			return &(iter.Current());
	}
	return 0;
}

NFmiSmartInfo* NFmiInfoOrganizer::AnalyzeDataInfo(const NFmiProducer& theProducer) // t�m� toimii vajavaisesti, koska se palauttaa aina 1. kyseisen tyyppisen infon
{
	return FindInfo(NFmiInfoData::kAnalyzeData, theProducer, true);
}

// palauttaa tietyss� prioriteetti j�rjestyksess� jonkun analyysi datan mit� systeemist� l�ytyy
NFmiSmartInfo* NFmiInfoOrganizer::AnalyzeDataInfo(void)
{
	checkedVector<NFmiSmartInfo*> infos = GetInfos(NFmiInfoData::kAnalyzeData);
	if(infos.size() == 0)
		return 0;
	else if(infos.size() == 1)
		return infos[0];
	else
	{
		std::vector<unsigned long> analyzeProdIds;
		analyzeProdIds.push_back(160); // 160 eli mesan2 on prioriteetti 1.
		analyzeProdIds.push_back(104); // 104 eli mesan on prioriteetti 2.

		for(size_t i = 0; i<analyzeProdIds.size(); i++)
		{
			for(size_t j = 0; j<infos.size(); j++)
			{
				if(infos[j]->Producer()->GetIdent() == analyzeProdIds[i])
					return infos[j];
			}
		}
	}
	return 0;
}

NFmiSmartInfo* NFmiInfoOrganizer::FindInfo(NFmiInfoData::Type theDataType) // Hakee 1. tietyn datatyypin infon
{
	return FindInfo(theDataType, 0); // indeksi alkaa 0:sta!!!
}

NFmiSmartInfo* NFmiInfoOrganizer::FindInfo(NFmiInfoData::Type theDataType, int theIndex) // Hakee indeksin mukaisen tietyn datatyypin infon
{
	int ind = 0;
	for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
	{
		if(iter.Current().DataType() == theDataType)
		{
			if(ind == theIndex)
				return &(iter.Current());
			ind++;
		}
	}
	return 0;
}

// Palauttaa vectorin viewable infoja, vectori ei omista pointtereita,
// joten infoja ei saa tuhota.
checkedVector<NFmiSmartInfo*> NFmiInfoOrganizer::GetInfos(NFmiInfoData::Type theDataType)
{
	checkedVector<NFmiSmartInfo*> infoVector;

	if(theDataType == NFmiInfoData::kEditable)
	{
		if(itsEditedData)
			infoVector.push_back(itsEditedData);
	}
	else if(theDataType == NFmiInfoData::kCopyOfEdited)
	{
		if(itsEditedDataCopy)
			infoVector.push_back(itsEditedDataCopy);
	}
	else
	{
		for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
		{
			if(iter.Current().DataType() == theDataType)
				infoVector.push_back(&(iter.Current()));
		}
	}
	return infoVector;
}

// Haetaan infoOrganizerista kaikki ne SmartInfot, joihin annettu fileNameFilter sopii.
// Mielest�ni vastauksia pit�isi tulla korkeintaan yksi, mutta ehk� tulevaisuudessa voisi tulla lista.
// HUOM! Palauttaa vectorin halutunlaisia infoja, vectori ei omista pointtereita, joten infoja ei saa tuhota delete:ll�.
// Ei k�y l�pi kEditable, eik� kCopyOfEdited erikois datoja!
checkedVector<NFmiSmartInfo*> NFmiInfoOrganizer::GetInfos(const std::string &theFileNameFilter)
{
	checkedVector<NFmiSmartInfo*> infoVector;

	if(theFileNameFilter.empty() == false)
	{
		for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
		{
			if(iter.Current().DataFilePattern() == theFileNameFilter)
				infoVector.push_back(&(iter.Current()));
		}
	}
	return infoVector;
}

static bool IsProducerWanted(int theCurrentProdId, int theProducerId1, int theProducerId2, int theProducerId3 = -1, int theProducerId4 = -1)
{
	if(theCurrentProdId == theProducerId1)
		return true;
	else if(theProducerId2 != -1 && theCurrentProdId == theProducerId2)
		return true;
	else if(theProducerId3 != -1 && theCurrentProdId == theProducerId3)
		return true;
	else if(theProducerId4 != -1 && theCurrentProdId == theProducerId4)
		return true;
	return false;
}

// Palauttaa vectorin halutun tuottajan infoja, vectori ei omista pointtereita, joten infoja ei saa tuhota.
// Ei katso tuottaja datoja editable infosta eik� sen kopioista!
// voi antaa kaksi eri tuottaja id:t� jos haluaa, jos esim. hirlamia voi olla kahden eri tuottaja id:n alla
checkedVector<NFmiSmartInfo*> NFmiInfoOrganizer::GetInfos(int theProducerId, int theProducerId2, int theProducerId3, int theProducerId4)
{
	checkedVector<NFmiSmartInfo*> infoVector;

	int currentProdId = 0;
	if(itsEditedData && itsEditedData->IsGrid() == false) // laitetaan my�s mahdollisesti editoitava data, jos kyseess� on asema dataa eli havainto
	{
		currentProdId = itsEditedData->Producer()->GetIdent();
		if(::IsProducerWanted(currentProdId, theProducerId, theProducerId2, theProducerId3, theProducerId4))
			infoVector.push_back(itsEditedData);
	}

	for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); ) // HUOM! t�ss� ei kiinnosta editoitu data tai sen kopio!!!!
	{
	    currentProdId = static_cast<int>(iter.Current().Producer()->GetIdent());
		if(::IsProducerWanted(currentProdId, theProducerId, theProducerId2, theProducerId3, theProducerId4))
			infoVector.push_back(&(iter.Current()));
	}
	return infoVector;
}

// HUOM! T�st� pit�� tehd� multithreaddauksen kest�v�� koodia, eli
// iteraattorista pit�� tehd� lokaali kopio.
checkedVector<NFmiSmartInfo*> NFmiInfoOrganizer::GetInfos(NFmiInfoData::Type theType, bool fGroundData, int theProducerId, int theProducerId2)
{
	checkedVector<NFmiSmartInfo*> infoVector;
	NFmiSortedPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start();
	for( ; iter.Next(); ) // HUOM! t�ss� ei kiinnosta editoitu data tai sen kopio!!!!
	{
		NFmiSmartInfo *info = iter.CurrentPtr();
		if(info && info->DataType() == theType)
		{
			if((fGroundData == true && info->SizeLevels() == 1) || (fGroundData == false && info->SizeLevels() > 1))
			{
				// HUOM! info->Producer() on potentiaalisti vaarallinen kutsu multi-threaddaavassa tilanteessa.
				int currentProdId = static_cast<int>(info->Producer()->GetIdent());
				if(::IsProducerWanted(currentProdId, theProducerId, theProducerId2))
					infoVector.push_back(info);
			}
		}
	}
	return infoVector;
}

// Haetaan halutun datatyypin, tuottajan joko pinta tai level dataa (mahd indeksi kertoo sitten konfliktin
// yhteydess�, monesko otetaan)
NFmiSmartInfo* NFmiInfoOrganizer::FindInfo(NFmiInfoData::Type theDataType, const NFmiProducer &theProducer, bool fGroundData, int theIndex)
{
	if(theDataType == NFmiInfoData::kEditable)
		return EditedInfo();
	else if(theDataType == NFmiInfoData::kCopyOfEdited)
		return EditedInfoCopy();
	else
	{
		int ind = 0;
		for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
		{
			NFmiSmartInfo* info = &(iter.Current());
			if(info && info->DataType() == theDataType)
			{
				info->FirstParam(); // pit�� varmistaa, ett� producer l�ytyy
				if(*(info->Producer()) == theProducer)
				{
					int levSize = info->SizeLevels();
					if((levSize == 1 && fGroundData) || (levSize > 1 && (!fGroundData)))
					{
						if(ind == theIndex)
							return info;
						ind++;
					}
				}
			}
		}
	}
	return 0;
}

// vastaus 0 = ei ole
// 1 = on sounding dataa, mutta ei v�ltt�m�tt� paras mahd.
// 2 = on hyv�� dataa
// T�m� on malli datojen kanssa  niin ett� painepinta data on 1 ja hybridi on 2
static int IsGoodSoundingData(NFmiSmartInfo* info, const NFmiProducer &theProducer, bool ignoreProducer)
{
	if(info)
	{
		if(ignoreProducer || (*(info->Producer()) == theProducer))
		{
			if(info->SizeLevels() > 3) // pit�� olla v�h 4 leveli� ennen kuin kelpuutetaan sounding dataksi
			{
				if(info->DataType() == NFmiInfoData::kHybridData)
					return 2;
				else
					return 1;
			}
		}
	}
	return 0;
}

// Hakee parhaan luotaus infon tuottajalle. Eli jos kyseess� esim hirlam tuottaja, katsotaan l�ytyyk�
// hybridi dataa ja sitten tyydyt��n viewable-dataa (= painepinta)
NFmiSmartInfo* NFmiInfoOrganizer::FindSoundingInfo(const NFmiProducer &theProducer)
{
	// TODO Ei ota huomioon viel� editoitavaa dataa
	NFmiSmartInfo* exceptableInfo = 0;
	for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
	{
		NFmiSmartInfo* info = &(iter.Current());
		int result = ::IsGoodSoundingData(info, theProducer, false);
		if(result == 2)
			return info;
		else if(result == 1)
			exceptableInfo = info;
	}

	NFmiSmartInfo* info = EditedInfo();
	if(info)
	{
		if(theProducer.GetIdent() == kFmiMETEOR || (*info->Producer() == theProducer)) // t�ss� hanskataan 'editoitu' data, jolloin ignoorataan tuottaja
		{
			info = EditedInfo();
			int result = ::IsGoodSoundingData(info, theProducer, true);
			if(result != 0)
				exceptableInfo = info;
		}
	}

	return exceptableInfo;
}

void NFmiInfoOrganizer::SetDrawParamPath(const std::string &theDrawParamPath)
{
	if(itsDrawParamFactory)
		itsDrawParamFactory->LoadDirectory(theDrawParamPath);
}

const std::string NFmiInfoOrganizer::GetDrawParamPath(void)
{
	std::string retValue;
	if(itsDrawParamFactory)
		retValue = itsDrawParamFactory->LoadDirectory();
	return retValue;
}
NFmiSmartInfo* NFmiInfoOrganizer::Info(NFmiDrawParam &theDrawParam, bool fCrossSectionInfoWanted)
{
	bool subParameter = false;
	NFmiInfoData::Type dataType = theDrawParam.DataType();
	if(fCrossSectionInfoWanted)
		return CrossSectionInfo(theDrawParam.Param(), subParameter, dataType, true);
	else
	{
		NFmiLevel* level = &theDrawParam.Level();
		if(level && level->GetIdent() == 0) // jos t�m� on ns. default-level otus (GetIdent() == 0), annetaan 0-pointteri Info-metodiin
			level = 0;
		if(dataType == NFmiInfoData::kEditable || dataType == NFmiInfoData::kCopyOfEdited || dataType == NFmiInfoData::kAnyData) // jos editoitava data, ei tuottajalla v�li�
			return Info(static_cast<FmiParameterName>(theDrawParam.Param().GetParamIdent()), subParameter, level, dataType);
		else
			return Info(theDrawParam.Param(), subParameter, level, dataType);
	}
}

NFmiSmartInfo* NFmiInfoOrganizer::Info(const NFmiDataIdent& theIdent, const NFmiLevel* theLevel, NFmiInfoData::Type theType)
{
	bool subParameter = false;
	if(theType == NFmiInfoData::kEditable || theType == NFmiInfoData::kCopyOfEdited || theType == NFmiInfoData::kAnyData) // jos editoitava data, ei tuottajalla v�li�
		return Info(static_cast<FmiParameterName>(theIdent.GetParamIdent()), subParameter, theLevel, theType);
	else
		return Info(theIdent, subParameter, theLevel, theType);
}

NFmiParamBag NFmiInfoOrganizer::GetParams(int theProducerId1, int theProducerId2, NFmiInfoData::Type theIgnoreDataType1, NFmiInfoData::Type theIgnoreDataType2, NFmiInfoData::Type theIgnoreDataType3)
{
	NFmiParamBag paramBag;
	checkedVector<NFmiSmartInfo*> infos(GetInfos(theProducerId1, theProducerId2));
	size_t size = infos.size();
	if(size > 0)
	{
		for(size_t i=0; i<size; i++)
		{
			if(infos[i]->DataType() == theIgnoreDataType1 || infos[i]->DataType() == theIgnoreDataType2 || infos[i]->DataType() == theIgnoreDataType3)
				continue; // tiettyj� data tyyppeja ei haluttukkaan listaan
			paramBag = paramBag.Combine(infos[i]->ParamBag());
		}
	}

    return paramBag;
}
