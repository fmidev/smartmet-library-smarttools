
#include "NFmiInfoOrganizer.h"
#include "NFmiDrawParamFactory.h"
#include "NFmiSmartInfo.h"
#include "NFmiQueryDataKeeper.h"
#include "NFmiDrawParam.h"
#include "NFmiQueryInfo.h"
#include "NFmiGrid.h"
#include "NFmiLatlonArea.h"
#include "NFmiQueryDataUtil.h"

#ifdef _MSC_VER
#pragma warning(disable : 4239) // poistaa VC++ 2010 varoituksen: warning C4239: nonstandard extension used : 'argument' : conversion from 'boost::shared_ptr<T>' to 'boost::shared_ptr<T> &'
#endif

NFmiInfoOrganizer::NFmiInfoOrganizer(void)
:itsEditedDataKeeper()
,itsCopyOfEditedDataKeeper()
,itsDataMap()
,itsDrawParamFactory()
,itsWorkingDirectory()
,itsMacroParamGridSize(50, 50)
,itsMacroParamMinGridSize(5, 5)
,itsMacroParamMaxGridSize(2000, 2000)
,itsMacroParamData()
,itsMacroParamMissingValueMatrix()
,itsCrossSectionMacroParamData()
,itsCrossSectionMacroParamMissingValueMatrix()
,fCreateEditedDataCopy(false)
{
}

NFmiInfoOrganizer::~NFmiInfoOrganizer(void)
{
}

bool NFmiInfoOrganizer::Init(const std::string &theDrawParamPath, bool createDrawParamFileIfNotExist, bool createEditedDataCopy, bool fUseOnePressureLevelDrawParam)
{
	fCreateEditedDataCopy = createEditedDataCopy;
 	itsDrawParamFactory = boost::shared_ptr<NFmiDrawParamFactory>(new NFmiDrawParamFactory(createDrawParamFileIfNotExist, fUseOnePressureLevelDrawParam));
	itsDrawParamFactory->LoadDirectory(theDrawParamPath);
	UpdateMacroParamDataSize(static_cast<int>(itsMacroParamGridSize.X()), static_cast<int>(itsMacroParamGridSize.Y()));
 	return itsDrawParamFactory->Init();
}

// ***** Datan lis�ykseen liittyvi� metodeja/funktioita ****************

bool NFmiInfoOrganizer::AddData(NFmiQueryData* theData
								 ,const std::string& theDataFileName
								 ,const std::string& theDataFilePattern
								 ,NFmiInfoData::Type theDataType
								 ,int theUndoLevel
								 ,int theMaxLatestDataCount
								 ,int theModelRunTimeGap)
{
	bool status = false;
	if(theData)
	{
		if(theDataType == NFmiInfoData::kEditable)
			status = AddEditedData(new NFmiSmartInfo(theData, theDataType, theDataFileName, theDataFilePattern), theUndoLevel);
		else
			status = Add(new NFmiOwnerInfo(theData, theDataType, theDataFileName, theDataFilePattern), theMaxLatestDataCount, theModelRunTimeGap); // muun tyyppiset datat kuin editoitavat menev�t mappiin
	}
	return status;
}

bool NFmiInfoOrganizer::AddEditedData(NFmiSmartInfo *theEditedData, int theUndoLevel)
{
	if(theEditedData)
	{
		boost::shared_ptr<NFmiOwnerInfo> dataPtr(theEditedData);
		theEditedData->First();
		try
		{
			if(theUndoLevel)
				theEditedData->UndoLevel(theUndoLevel);
		}
		catch(...)
		{
			// jos undo-levelin asetus ep�onnistui syyst� tai toisesta (esim. muisti loppui), asetetaan syvyydeksi 0 ja koitetaan jatkaa...
			theEditedData->UndoLevel(0);
		}

		itsEditedDataKeeper = boost::shared_ptr<NFmiQueryDataKeeper>(new NFmiQueryDataKeeper(dataPtr));
		fCreateEditedDataCopy = theUndoLevel ? true : false; // pit�� p�ivitt�� kopion luomiseen vaikuttavaa muuttujaa undo-levelin mukaan
		UpdateEditedDataCopy();
		return true;
	}
	return false;
}

static void SetFastInfoToZero(boost::shared_ptr<NFmiFastQueryInfo> &theOwnerInfo)
{
	theOwnerInfo = boost::shared_ptr<NFmiFastQueryInfo>(static_cast<NFmiFastQueryInfo*>(0));
}

static void SetDataKeeperToZero(boost::shared_ptr<NFmiQueryDataKeeper> &theDataKeeper)
{
	theDataKeeper = boost::shared_ptr<NFmiQueryDataKeeper>(static_cast<NFmiQueryDataKeeper*>(0));
}

void NFmiInfoOrganizer::UpdateEditedDataCopy(void)
{
	if(fCreateEditedDataCopy)
	{
		if(itsEditedDataKeeper) // Testaa toimiiko t�ll�inen sharep-ptr nolla tarkastus!!!!
		{
			boost::shared_ptr<NFmiOwnerInfo> tmpInfo(itsEditedDataKeeper->OriginalData()->NFmiOwnerInfo::Clone());
			itsCopyOfEditedDataKeeper = boost::shared_ptr<NFmiQueryDataKeeper>(new NFmiQueryDataKeeper(tmpInfo));
			itsCopyOfEditedDataKeeper->OriginalData()->DataType(NFmiInfoData::kCopyOfEdited);
			return ; // onnistuneen operaation j�lkeen paluu, ettei mene datan nollaukseen 
		}
	}
	// muissa tapauksissa varmuuden vuoksi nollataan copy-data
	::SetDataKeeperToZero(itsCopyOfEditedDataKeeper);
}

bool NFmiInfoOrganizer::Add(NFmiOwnerInfo* theInfo, int theMaxLatestDataCount, int theModelRunTimeGap)
{
	if(theInfo)
	{
		boost::shared_ptr<NFmiOwnerInfo> dataPtr(theInfo);
		MapType::iterator pos = itsDataMap.find(theInfo->DataFilePattern());
		if(pos != itsDataMap.end())
		{ // kyseinen data patterni l�ytyi, lis�t��n annettu data sen keeper-settiin
			if(pos->second->MaxLatestDataCount() != theMaxLatestDataCount)
				pos->second->MaxLatestDataCount(theMaxLatestDataCount);	
			if(pos->second->ModelRunTimeGap() != theModelRunTimeGap)
				pos->second->ModelRunTimeGap(theModelRunTimeGap);	
			pos->second->AddData(dataPtr);
		}
		else
		{ // lis�t��n kyseinen data keeper-set listaan
			itsDataMap.insert(std::make_pair(theInfo->DataFilePattern(), boost::shared_ptr<NFmiQueryDataSetKeeper>(new NFmiQueryDataSetKeeper(dataPtr, theMaxLatestDataCount, theModelRunTimeGap))));
		}
		return true;
	}
	else
		return false;
}

// ***** Datan kyselyyn liittyvi� metodeja/funktioita ****************

static bool UseParIdOnly(NFmiInfoData::Type theDataType)
{
	if(theDataType == NFmiInfoData::kEditable || theDataType == NFmiInfoData::kCopyOfEdited || theDataType == NFmiInfoData::kAnyData) // jos editoitava data, ei tuottajalla v�li�
		return true;
	return false;
}

static bool CheckDataType(boost::shared_ptr<NFmiFastQueryInfo> &theInfo, NFmiInfoData::Type theType)
{
	bool anyDataOk = (theType == NFmiInfoData::kAnyData);
	if(theInfo && (theInfo->DataType() == theType || anyDataOk))
		return true;
	return false;
}

static bool CheckDataIdent(boost::shared_ptr<NFmiFastQueryInfo> &theInfo, const NFmiDataIdent& theDataIdent, bool fUseParIdOnly)
{
	if(theInfo && (fUseParIdOnly ? theInfo->Param(static_cast<FmiParameterName>(theDataIdent.GetParamIdent())): theInfo->Param(theDataIdent)))
		return true;
	return false;
}

static bool CheckLevel(boost::shared_ptr<NFmiFastQueryInfo> &theInfo, const NFmiLevel *theLevel)
{
	if(theInfo && (!theLevel || (theLevel && theInfo->Level(*theLevel))))
		return true;
	return false;
}

boost::shared_ptr<NFmiFastQueryInfo> NFmiInfoOrganizer::Info(boost::shared_ptr<NFmiDrawParam> &theDrawParam, bool fCrossSectionInfoWanted, bool fGetLatestIfArchiveNotFound, bool &fGetDataFromServer)
{
	fGetDataFromServer = false;
	boost::shared_ptr<NFmiFastQueryInfo> aInfo = Info(theDrawParam, fCrossSectionInfoWanted);
	if(aInfo == 0 && fGetLatestIfArchiveNotFound && theDrawParam->ModelRunIndex() < 0)
	{
		boost::shared_ptr<NFmiDrawParam> tmpDrawParam(new NFmiDrawParam(*theDrawParam));
		tmpDrawParam->ModelRunIndex(0); 
		aInfo = Info(tmpDrawParam, fCrossSectionInfoWanted); // koetetaan sitten hakea viimeisint� dataa
		if(aInfo)
			fGetDataFromServer = true;
	}
	return aInfo;
}

boost::shared_ptr<NFmiFastQueryInfo> NFmiInfoOrganizer::Info(boost::shared_ptr<NFmiDrawParam> &theDrawParam, bool fCrossSectionInfoWanted, bool fGetLatestIfArchiveNotFound)
{
	bool getDataFromServer = false;
	return Info(theDrawParam, fCrossSectionInfoWanted, fGetLatestIfArchiveNotFound, getDataFromServer);
}

boost::shared_ptr<NFmiFastQueryInfo> NFmiInfoOrganizer::Info(boost::shared_ptr<NFmiDrawParam> &theDrawParam, bool fCrossSectionInfoWanted)
{
	NFmiInfoData::Type dataType = theDrawParam->DataType();
	if(fCrossSectionInfoWanted)
		return CrossSectionInfo(theDrawParam->Param(), dataType, theDrawParam->ModelRunIndex());
	else
		return GetInfo(theDrawParam);
}

boost::shared_ptr<NFmiFastQueryInfo> NFmiInfoOrganizer::GetInfo(boost::shared_ptr<NFmiDrawParam> &theDrawParam)
{
	NFmiLevel* level = &theDrawParam->Level();
	NFmiInfoData::Type dataType = theDrawParam->DataType();
	if(level && level->GetIdent() == 0) // jos t�m� on ns. default-level otus (GetIdent() == 0), annetaan 0-pointteri Info-metodiin
		level = 0;
	return GetInfo(theDrawParam->Param(), level, dataType, ::UseParIdOnly(dataType), theDrawParam->ModelRunIndex());
}

boost::shared_ptr<NFmiFastQueryInfo> NFmiInfoOrganizer::Info(const NFmiDataIdent& theIdent, const NFmiLevel* theLevel, NFmiInfoData::Type theType, bool fUseParIdOnly, bool fLevelData)
{
	if(fLevelData)
		return CrossSectionInfo(theIdent, theType);
	else 
		return GetInfo(theIdent, theLevel, theType, (fUseParIdOnly || ::UseParIdOnly(theType)));
}

boost::shared_ptr<NFmiFastQueryInfo> NFmiInfoOrganizer::GetWantedProducerInfo(NFmiInfoData::Type theType, FmiProducerName theProducerName)
{
	if(itsEditedDataKeeper && theType == NFmiInfoData::kEditable)
		return itsEditedDataKeeper->GetIter();
	else
	{
		boost::shared_ptr<NFmiFastQueryInfo> aInfo;
		for(MapType::iterator iter = itsDataMap.begin(); iter != itsDataMap.end(); ++iter)
		{
			aInfo = iter->second->GetDataKeeper()->GetIter();
			if(aInfo->DataType() == theType && static_cast<FmiProducerName>(aInfo->Producer()->GetIdent()) == theProducerName)
				return aInfo;
		}
	}
	// Jos ei l�ytynyt sopivaa dataa, palautetaan tyhj�.
	return boost::shared_ptr<NFmiFastQueryInfo>();
}

boost::shared_ptr<NFmiFastQueryInfo> NFmiInfoOrganizer::GetSynopPlotParamInfo(NFmiInfoData::Type theType)
{
	return GetWantedProducerInfo(theType, kFmiSYNOP);
}

boost::shared_ptr<NFmiFastQueryInfo> NFmiInfoOrganizer::GetSoundingPlotParamInfo(NFmiInfoData::Type theType)
{
	return GetWantedProducerInfo(theType, kFmiTEMP);
}

// HUOM! N�m� makroParamData jutut pit�� mietti� uusiksi, jos niit� aletaan k�sittelem��n eri s�ikeiss�. T�ll�in
// Niist� pit�� luoda aina ilmeisesti paikalliset kopiot?!?!
boost::shared_ptr<NFmiFastQueryInfo> NFmiInfoOrganizer::MacroParamData(void) 
{
	return itsMacroParamData;
}

boost::shared_ptr<NFmiFastQueryInfo> NFmiInfoOrganizer::CrossSectionMacroParamData(void) 
{
	return itsCrossSectionMacroParamData;
}

static bool MatchData(boost::shared_ptr<NFmiFastQueryInfo> &theInfo, NFmiInfoData::Type theType, const NFmiDataIdent& theDataIdent, bool fUseParIdOnly, const NFmiLevel *theLevel)
{
	if(::CheckDataType(theInfo, theType) && ::CheckDataIdent(theInfo, theDataIdent, fUseParIdOnly) && ::CheckLevel(theInfo, theLevel))
		return true;

	return false;
}

static bool MatchCrossSectionData(boost::shared_ptr<NFmiFastQueryInfo> &theInfo, NFmiInfoData::Type theType, const NFmiDataIdent& theDataIdent, bool fUseParIdOnly)
{
	if(::CheckDataType(theInfo, theType) && ::CheckDataIdent(theInfo, theDataIdent, fUseParIdOnly) && theInfo->SizeLevels() > 1)
		return true;

	return false;
}

// Palauttaa annetun datan, paitsi jos kyse on arkisto datasta, tarkistetaan ett� sellainen l�ytyy ja palautetaan se (parametri asetettuna oikein).
// Jos ei l�ydy oikeaa arkisto dataa, palautetaan 0-pointteri.
static boost::shared_ptr<NFmiFastQueryInfo> DoArchiveCheck(boost::shared_ptr<NFmiFastQueryInfo> &theData, const NFmiDataIdent& theDataIdent, bool fUseParIdOnly, const NFmiLevel* theLevel, int theIndex, NFmiInfoOrganizer::MapType::iterator &theDataKeeperIter)
{
	boost::shared_ptr<NFmiFastQueryInfo> aInfo = theData;
	if(aInfo && theIndex < 0)
	{
		boost::shared_ptr<NFmiQueryDataKeeper> qDataKeeper = theDataKeeperIter->second->GetDataKeeper(theIndex);
		if(qDataKeeper)
			aInfo = qDataKeeper->GetIter(); // t�ss� katsotaan l�ytyyk� viel� haluttu arkisto data
		else
			aInfo = boost::shared_ptr<NFmiFastQueryInfo>(); // ei l�ytynyt arkisto dataa, nollataan info-pointteri, ett� data koetetaan sitten hakea q2serverilt�
		::CheckDataIdent(aInfo, theDataIdent, fUseParIdOnly); // pit�� asettaa arkisto datakin oikeaan parametriin
		::CheckLevel(aInfo, theLevel);
	}
	return aInfo;
}

//--------------------------------------------------------
// GetInfo
// Yritin aluksi tehd� metodin k�ytt�m�ll� parametria 
// bool fIgnoreProducerName = false
// siksi ett� voisi olla samalta tuottajalta useita samantyyppisi�
// datoja k�yt�ss� yht�aikaa, mutta t�m� osoittautui liian
// haavoittuvaiseksi koska eri tilanteissa datoilla voi olla erilaisia nimi�
// ja tuottajien nimet pit�� pysty� vaihtamaan ilman ongelmia
// querydatojen tuotanto ketjuissa. 
// Kun t�rm�sin nyt kahteen eri ongelmaan:
// 1. querydatan tuottaja nimi vaihdetaan, mutta se on laitettu view-makroon.
// 2. EC:n 3 vrk pinta datassa on 12 utc ajossa eri nimi kuin 00 ajossa.
// SIKSI nyt metodi toimii siten ett� se yritt�� etsi� dataa oikealla tuottaja nimell�.
// Mutta jos oikealla nimell� ei l�ytynyt, otetaan talteen 1. muilta kriteereilt�
// oikea data ja palautetaan se.
//
//--------------------------------------------------------
boost::shared_ptr<NFmiFastQueryInfo> NFmiInfoOrganizer::GetInfo(const NFmiDataIdent& theDataIdent
									   , const NFmiLevel* theLevel
									   , NFmiInfoData::Type theType
									   , bool fUseParIdOnly
									   , int theIndex)
{
	boost::shared_ptr<NFmiFastQueryInfo> backupData; // etsit�� t�h�n 1. data joka muuten sopii kriteereihin, mutta 
									// jonka tuottaja nimi on eri kuin haluttu. Jos oikealla nimell� ei l�ydy dataa, k�ytet��n t�t�.
	if(theDataIdent.GetParamIdent() == NFmiInfoData::kFmiSpSynoPlot || theDataIdent.GetParamIdent() == NFmiInfoData::kFmiSpMinMaxPlot) // synop plot paramille pit�� tehd� kikka (ja min/max plot 9996)
		return GetSynopPlotParamInfo(theType);
	if(theLevel && theLevel->LevelType() == kFmiSoundingLevel) // sounding plot paramille pit�� tehd� kikka
		return GetSoundingPlotParamInfo(theType);
	if(theType == NFmiInfoData::kMacroParam || theType == NFmiInfoData::kQ3MacroParam) // macro- parametrit lasketaan t�ll�
		return MacroParamData(); // t�ss� ei parametreja ja leveleit� ihmetell�, koska ne muutetaan aina lennossa tarpeen vaatiessa
	if(theType == NFmiInfoData::kCrossSectionMacroParam)
		return CrossSectionMacroParamData(); // t�ss� ei parametreja ja leveleit� ihmetell�, koska ne muutetaan aina lennossa tarpeen vaatiessa

	boost::shared_ptr<NFmiFastQueryInfo> foundData;
	if(::MatchData(itsEditedDataKeeper->GetIter(), theType, theDataIdent, fUseParIdOnly, theLevel))
		foundData = itsEditedDataKeeper->GetIter();
	else if(::MatchData(itsCopyOfEditedDataKeeper->GetIter(), theType, theDataIdent, fUseParIdOnly, theLevel))
		foundData = itsCopyOfEditedDataKeeper->GetIter();
	else
	{
		// tutkitaan ensin l�ytyyk� theParam suoraan joltain listassa olevalta NFmiSmartInfo-pointterilta
		for(MapType::iterator iter = itsDataMap.begin(); iter != itsDataMap.end(); ++iter)
		{
			boost::shared_ptr<NFmiFastQueryInfo> aInfo = iter->second->GetDataKeeper(0)->GetIter(); // t�ss� haetaan ensin viimeisin data!!
			if(::MatchData(aInfo, theType, theDataIdent, fUseParIdOnly, theLevel))
			{
				if(!(theLevel == 0 && aInfo->SizeLevels() > 1))
				{
					if(theDataIdent.GetProducer()->GetName() == aInfo->Param().GetProducer()->GetName())
					{
						foundData = ::DoArchiveCheck(aInfo, theDataIdent, fUseParIdOnly, theLevel, theIndex, iter); // t�m� saa olla 0-pointteri, jos kyse oli arkistodatasta
						if(foundData)
							break;
					}
					else if(backupData == 0)
						backupData = ::DoArchiveCheck(aInfo, theDataIdent, fUseParIdOnly, theLevel, theIndex, iter);
				}
			}
		}
	}
	if(foundData == 0 && backupData != 0)
		foundData = backupData;

	if(foundData)
	{
		if(foundData->SizeLevels() == 1)
			foundData->FirstLevel();
	}
	return foundData;
}

// Etsi haluttu crossSection-data. Eli pit�� olla yli 1 leveli�
// eik� etsit� tietty� leveli�.
// HUOM! Tein t�h�n CrossSectionInfo-metodiin saman tuottaja nimi ohitus virityksen kuin
// Info-metodiin. Ks. kommenttia sielt�.
boost::shared_ptr<NFmiFastQueryInfo> NFmiInfoOrganizer::CrossSectionInfo(const NFmiDataIdent& theDataIdent, NFmiInfoData::Type theType, int theIndex)
{
	if(theType == NFmiInfoData::kCrossSectionMacroParam)
		return CrossSectionMacroParamData();
	boost::shared_ptr<NFmiFastQueryInfo> backupData; // etsit�� t�h�n 1. data joka muuten sopii kriteereihin, mutta 
									// jonka tuottaja nimi on eri kuin haluttu. Jos oikealla nimell� ei l�ydy dataa, k�ytet��n t�t�.
	boost::shared_ptr<NFmiFastQueryInfo> foundData;
	if(::MatchCrossSectionData(itsEditedDataKeeper->GetIter(), theType, theDataIdent, true))
		foundData = itsEditedDataKeeper->GetIter();
	else
	{
		// tutkitaan ensin l�ytyyk� theParam suoraan joltain listassa olevalta NFmiSmartInfo-pointterilta
		for(MapType::iterator iter = itsDataMap.begin(); iter != itsDataMap.end(); ++iter)
		{
			boost::shared_ptr<NFmiFastQueryInfo> aInfo = iter->second->GetDataKeeper(0)->GetIter(); // t�ss� haetaan ensin viimeisin data!!
			if(::MatchCrossSectionData(aInfo, theType, theDataIdent, false))
			{
				if(theDataIdent.GetProducer()->GetName() == aInfo->Param().GetProducer()->GetName())
				{
					foundData = ::DoArchiveCheck(aInfo, theDataIdent, false, 0, theIndex, iter); // t�m� saa olla 0-pointteri, jos kyse oli arkistodatasta
					if(foundData)
						break;
				}
				else if(backupData == 0)
					backupData = ::DoArchiveCheck(aInfo, theDataIdent, false, 0, theIndex, iter); // t�h�n laitetaan siis vain prod-namesta poikkeava data (t�m� tapahtuu mm. kun k�ytt�j� tekee changeAllProducers-toiminnon)
			}
		}
	}

	if(foundData == 0 && backupData != 0)
		foundData = backupData;

	return foundData;
}

boost::shared_ptr<NFmiFastQueryInfo> NFmiInfoOrganizer::FindInfo(NFmiInfoData::Type theDataType, int theIndex) // Hakee indeksin mukaisen tietyn datatyypin infon
{
	if(itsEditedDataKeeper && theDataType == NFmiInfoData::kEditable)
		return itsEditedDataKeeper->GetIter();
	else if(itsCopyOfEditedDataKeeper && theDataType == NFmiInfoData::kCopyOfEdited)
		return itsCopyOfEditedDataKeeper->GetIter();
	else
	{
		int ind = 0;
		for(MapType::iterator iter = itsDataMap.begin(); iter != itsDataMap.end(); ++iter)
		{
			boost::shared_ptr<NFmiFastQueryInfo> aInfo = iter->second->GetDataKeeper()->GetIter();
			if(aInfo->DataType() == theDataType)
			{
				if(ind == theIndex)
					return aInfo;
				ind++;
			}
		}
	}
	return boost::shared_ptr<NFmiFastQueryInfo>();
}

// Haetaan halutun datatyypin, tuottajan joko pinta tai level dataa (mahd indeksi kertoo sitten konfliktin
// yhteydess�, monesko otetaan)
boost::shared_ptr<NFmiFastQueryInfo> NFmiInfoOrganizer::FindInfo(NFmiInfoData::Type theDataType, const NFmiProducer &theProducer, bool fGroundData, int theIndex)
{
	if(itsEditedDataKeeper && theDataType == NFmiInfoData::kEditable)
		return itsEditedDataKeeper->GetIter();
	else if(itsCopyOfEditedDataKeeper && theDataType == NFmiInfoData::kCopyOfEdited)
		return itsCopyOfEditedDataKeeper->GetIter();
	else
	{
		int ind = 0;
		for(MapType::iterator iter = itsDataMap.begin(); iter != itsDataMap.end(); ++iter)
		{
			boost::shared_ptr<NFmiFastQueryInfo> aInfo = iter->second->GetDataKeeper()->GetIter();
			if(aInfo && aInfo->DataType() == theDataType)
			{
				aInfo->FirstParam(); // pit�� varmistaa, ett� producer l�ytyy
				if(*(aInfo->Producer()) == theProducer)
				{
					int levSize = aInfo->SizeLevels();
					if((levSize == 1 && fGroundData) || (levSize > 1 && (!fGroundData)))
					{
						if(ind == theIndex)
							return aInfo;
						ind++;
					}
				}
			}
		}
	}
	return boost::shared_ptr<NFmiFastQueryInfo>();
}

// vastaus 0 = ei ole
// 1 = on sounding dataa, mutta ei v�ltt�m�tt� paras mahd.
// 2 = on hyv�� dataa
// T�m� on malli datojen kanssa  niin ett� painepinta data on 1 ja hybridi on 2
static int IsGoodSoundingData(boost::shared_ptr<NFmiFastQueryInfo> &theInfo, const NFmiProducer &theProducer, bool ignoreProducer)
{
	if(theInfo)
	{
		if(ignoreProducer || (*(theInfo->Producer()) == theProducer))
		{
			if(theInfo->SizeLevels() > 3) // pit�� olla v�h 4 leveli� ennen kuin kelpuutetaan sounding dataksi
			{
				if(theInfo->DataType() == NFmiInfoData::kHybridData)
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
boost::shared_ptr<NFmiFastQueryInfo> NFmiInfoOrganizer::FindSoundingInfo(const NFmiProducer &theProducer, int theIndex)
{
	boost::shared_ptr<NFmiFastQueryInfo> exceptableInfo;
	for(MapType::iterator iter = itsDataMap.begin(); iter != itsDataMap.end(); ++iter)
	{
		boost::shared_ptr<NFmiFastQueryInfo> aInfo = iter->second->GetDataKeeper()->GetIter();
		int result = ::IsGoodSoundingData(aInfo, theProducer, false);
		if(result != 0 && theIndex < 0)
		{ // haetaan vanhempaa malliajo dataa
			boost::shared_ptr<NFmiQueryDataKeeper> qDataKeeper = iter->second->GetDataKeeper(theIndex);
			if(qDataKeeper)
				aInfo = qDataKeeper->GetIter();
			else
				aInfo = boost::shared_ptr<NFmiFastQueryInfo>(); // ei l�ytynyt vanhoista malliajoista, pit�� nollata pointteri 
		}
		if(aInfo)
		{
			if(result == 2)
				return aInfo;
			else if(result == 1)
				exceptableInfo = aInfo;
		}
	}

	if(exceptableInfo)
		return exceptableInfo;

	boost::shared_ptr<NFmiFastQueryInfo> aInfo = FindInfo(NFmiInfoData::kEditable);
	if(aInfo)
	{
		if(theProducer.GetIdent() == kFmiMETEOR || (*aInfo->Producer() == theProducer)) // t�ss� hanskataan 'editoitu' data, jolloin ignoorataan tuottaja
		{
			int result = ::IsGoodSoundingData(aInfo, theProducer, true);
			if(result != 0)
				exceptableInfo = aInfo;
		}
	}

	return exceptableInfo;
}

// Haetaan infoOrganizerista kaikki ne SmartInfot, joihin annettu fileNameFilter sopii.
// Mielest�ni vastauksia pit�isi tulla korkeintaan yksi, mutta ehk� tulevaisuudessa voisi tulla lista.
// HUOM! Palauttaa vectorin halutunlaisia infoja, vectori ei omista pointtereita, joten infoja ei saa tuhota delete:ll�.
// Ei k�y l�pi kEditable, eik� kCopyOfEdited erikois datoja!
checkedVector<boost::shared_ptr<NFmiFastQueryInfo> > NFmiInfoOrganizer::GetInfos(const std::string &theFileNameFilter)
{
	checkedVector<boost::shared_ptr<NFmiFastQueryInfo> > infoVector;

	if(theFileNameFilter.empty() == false)
	{
		for(MapType::iterator iter = itsDataMap.begin(); iter != itsDataMap.end(); ++iter)
		{
			if(iter->second->FilePattern() == theFileNameFilter)
				infoVector.push_back(iter->second->GetDataKeeper()->GetIter());
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
checkedVector<boost::shared_ptr<NFmiFastQueryInfo> > NFmiInfoOrganizer::GetInfos(int theProducerId, int theProducerId2, int theProducerId3, int theProducerId4)
{
	checkedVector<boost::shared_ptr<NFmiFastQueryInfo> > infoVector;

	int currentProdId = 0;
	if(itsEditedDataKeeper)
	{
		boost::shared_ptr<NFmiFastQueryInfo> editedDataIter = itsEditedDataKeeper->GetIter();
		if(editedDataIter && editedDataIter->IsGrid() == false) // laitetaan my�s mahdollisesti editoitava data, jos kyseess� on asema dataa eli havainto
		{
			currentProdId = editedDataIter->Producer()->GetIdent();
			if(::IsProducerWanted(currentProdId, theProducerId, theProducerId2, theProducerId3, theProducerId4))
				infoVector.push_back(editedDataIter);
		}
	}

	for(MapType::iterator iter = itsDataMap.begin(); iter != itsDataMap.end(); ++iter)
	{
		boost::shared_ptr<NFmiFastQueryInfo> aInfo = iter->second->GetDataKeeper()->GetIter();
	    currentProdId = static_cast<int>(aInfo->Producer()->GetIdent());
		if(::IsProducerWanted(currentProdId, theProducerId, theProducerId2, theProducerId3, theProducerId4))
			infoVector.push_back(aInfo);
	}
	return infoVector;
}

// HUOM! T�st� pit�� tehd� multithreaddauksen kest�v�� koodia, eli
// iteraattorista pit�� tehd� lokaali kopio.
checkedVector<boost::shared_ptr<NFmiFastQueryInfo> > NFmiInfoOrganizer::GetInfos(NFmiInfoData::Type theType, bool fGroundData, int theProducerId, int theProducerId2)
{
	checkedVector<boost::shared_ptr<NFmiFastQueryInfo> > infoVector;
	for(MapType::iterator iter = itsDataMap.begin(); iter != itsDataMap.end(); ++iter)
	{
		boost::shared_ptr<NFmiFastQueryInfo> info = iter->second->GetDataKeeper()->GetIter();
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

// Palauttaa vectorin viewable infoja, vectori ei omista pointtereita,
// joten infoja ei saa tuhota.
checkedVector<boost::shared_ptr<NFmiFastQueryInfo> > NFmiInfoOrganizer::GetInfos(NFmiInfoData::Type theDataType)
{
	checkedVector<boost::shared_ptr<NFmiFastQueryInfo> > infoVector;

	if(itsEditedDataKeeper && theDataType == NFmiInfoData::kEditable)
	{
		boost::shared_ptr<NFmiFastQueryInfo> editedDataIter = itsEditedDataKeeper->GetIter();
		if(editedDataIter)
			infoVector.push_back(editedDataIter);
	}
	else if(itsCopyOfEditedDataKeeper && theDataType == NFmiInfoData::kCopyOfEdited)
	{
		boost::shared_ptr<NFmiFastQueryInfo> copyOfEditedDataIter = itsCopyOfEditedDataKeeper->GetIter();
		if(copyOfEditedDataIter)
			infoVector.push_back(copyOfEditedDataIter);
	}
	else
	{
		for(MapType::iterator iter = itsDataMap.begin(); iter != itsDataMap.end(); ++iter)
		{
			boost::shared_ptr<NFmiFastQueryInfo> info = iter->second->GetDataKeeper()->GetIter();
			if(info->DataType() == theDataType)
				infoVector.push_back(info);
		}
	}
	return infoVector;
}

NFmiParamBag NFmiInfoOrganizer::GetParams(int theProducerId1)
{
	NFmiParamBag paramBag;
	checkedVector<boost::shared_ptr<NFmiFastQueryInfo> > infos(GetInfos(theProducerId1));
	size_t size = infos.size();
	if(size > 0)
	{
		for(size_t i=0; i<size; i++)
		{
			paramBag = paramBag.Combine(infos[i]->ParamBag());
		}
	}

    return paramBag;
}

//--------------------------------------------------------
// CreateDrawParam(NFmiDataIdent& theDataIdent)
//--------------------------------------------------------
// Tutkii l�ytyyk� listasta itsList infoa, jossa on theDataIdent - siis
// etsit��n info, jonka tuottaja ja parametri saadaan theDataIdent:st�.
// Jos t�ll�inen info l�ytyy, pyydet��n itsDrawParamFactory luomaan
// drawParam kyseiselle parametrille l�ydetyn infon avulla.
boost::shared_ptr<NFmiDrawParam> NFmiInfoOrganizer::CreateDrawParam(const NFmiDataIdent& theIdent, const NFmiLevel* theLevel, NFmiInfoData::Type theType)
{
// Huomaa, ett� itsDrawParamFactory luo pointterin drawParam new:ll�, joten
// drawParam pit�� muistaa tuhota  NFmiInfoOrganizer:n ulkopuolella
	boost::shared_ptr<NFmiDrawParam> drawParam;
	if(theType == NFmiInfoData::kSatelData || theType == NFmiInfoData::kConceptualModelData) // spesiaali keissi satelliitti kuville, niill� ei ole infoa
	{
		drawParam = boost::shared_ptr<NFmiDrawParam>(new NFmiDrawParam(theIdent, NFmiLevel(), 1, theType));
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
boost::shared_ptr<NFmiDrawParam> NFmiInfoOrganizer::CreateCrossSectionDrawParam(const NFmiDataIdent& theDataIdent, NFmiInfoData::Type theType)
{
	boost::shared_ptr<NFmiDrawParam> drawParam = itsDrawParamFactory->CreateCrossSectionDrawParam(theDataIdent);
	if(drawParam)
		drawParam->DataType(theType); // data tyyppi pit�� my�s asettaa!!
	return drawParam;
}

boost::shared_ptr<NFmiDrawParam> NFmiInfoOrganizer::CreateSynopPlotDrawParam(const NFmiDataIdent& theDataIdent
														  ,const NFmiLevel* theLevel
														  ,NFmiInfoData::Type theType)
{
	boost::shared_ptr<NFmiDrawParam> drawParam = itsDrawParamFactory->CreateDrawParam(theDataIdent, theLevel); // false merkitsee, ett� parametria ei taas aseteta tuolla metodissa
	if(drawParam)
		drawParam->DataType(theType);
	return drawParam;
}

//--------------------------------------------------------
// Clear
//--------------------------------------------------------
// tuhoaa aina datan
bool NFmiInfoOrganizer::Clear(void)
{
	itsDataMap.clear();
	return true; // t�m� paluu arvo on turha
}


// TODO: T�m�n k�ytt�� pit�� mietti�, halutaanko siivota pois koko setti vai mit�?
void NFmiInfoOrganizer::ClearData(NFmiInfoData::Type theDataType)
{
	if(theDataType == NFmiInfoData::kEditable)
	{
		::SetDataKeeperToZero(itsEditedDataKeeper);
		UpdateEditedDataCopy();
	}
	else
	{ // k�yd��n lista l�pi ja tuhotaan halutun tyyppiset datat
		MapType::iterator iter = itsDataMap.begin();
		for(;;)
		{
			if(iter == itsDataMap.end())
				break;

			if(iter->second->GetDataKeeper()->GetIter()->DataType() == theDataType)
			{
				itsDataMap.erase(iter); // t�m� poisto-operaatio siirt�� iteraattorin seuraavaan
			}
			else
				++iter; // jos ei poistettu objektia, pit�� iteraattoria edist��....
		}
	}
}

// this kind of m��ritell��n t�ll� hetkell�:
// parametrien, leveleiden ja mahdollisen gridin avulla (ei location bagin avulla)
// TODO: tarvitaanko t�ll�ist� tarkastelua, parametritlistat tai levelit voiva muuttua jonain p�iv�n� saman tyyppisess� datassa, pit�isik� tehd� t�st� l�ysempi tarkastelu?!?
bool NFmiInfoOrganizer::IsInfosTwoOfTheKind(NFmiQueryInfo* theInfo1, NFmiInfoData::Type theType1, const std::string &theFileNamePattern, boost::shared_ptr<NFmiFastQueryInfo> &theInfo2)
{
	// parametrit ja tuottajat samoja
	if(theInfo1 && theInfo2)
	{
		if(theType1 == theInfo2->DataType())
		{
			const std::string &dataFilePatternStr = dynamic_cast<NFmiOwnerInfo*>(theInfo2.get())->DataFilePattern();
			if(theFileNamePattern.empty() == false && dataFilePatternStr.empty() == false && theFileNamePattern == dataFilePatternStr)
				return true; // jos filepatternit eiv�t olleet tyhji� ja ne ovat samanlaisia, pidet��n datoja samanlaisina (t�m� auttaa, jos datat muuttuvat (paramereja lis��, asemia lis�� jne.))
			if(theInfo1->ParamBag() == theInfo2->ParamBag())
			{
				// Level tyypit pit��olla samoja ja niiden lukum��r�t niin ett� joko leveleit� on molemmissa
				// tasan yksi tai molemmissa on yli yksi.
				// Ongelmia muuten data p�ivityksen kanssa jos esim. luotaus datoissa on eri m��r� leveleit�
				// tai p�ivitet��n hybridi dataa ja level m��r� muuttuu tms.
				FmiLevelType levelType1 = theInfo1->VPlaceDescriptor().Level(0)->LevelType();
				FmiLevelType levelType2 = theInfo2->VPlaceDescriptor().Level(0)->LevelType();
				unsigned long size1 = theInfo1->VPlaceDescriptor().Size();
				unsigned long size2 = theInfo2->VPlaceDescriptor().Size();
				if(levelType1 == levelType2 && (size1 == size2 || (size1 > 1 && size2 > 1)))
				{
					// mahdollinen gridi samoja
					bool status3 = true;
					if(theInfo1->Grid() && theInfo2->Grid())
					{
						status3 = (theInfo1->Grid()->AreGridsIdentical(*(theInfo2->Grid()))) == true;
					}
					if(status3)
					{
						theInfo1->FirstParam(); // varmistaa, ett� producer l�ytyy
						theInfo2->FirstParam();
						if(*theInfo1->Producer() == *theInfo2->Producer())
							return true;
					}
				}
			}
		}
	}
	return false;
}

// TODO: T�m�n k�ytt�� pit�� mietti�, halutaanko siivota pois koko setti vai mit�? NYT AddData tekee siirrot DataSetKeeper:iss� jo automaattisesti!!!
// ******
// theRemovedDatasTimesOut -parametri k�ytet��n optimoidaan esim. havaittua hila datan kanssa teht�vi� ruudun p�ivityksi�,
// editoitavasta datasta ei oteta mit��n aikoja talteen.
void NFmiInfoOrganizer::ClearThisKindOfData(NFmiQueryInfo* theInfo, NFmiInfoData::Type theDataType, const std::string &theFileNamePattern, NFmiTimeDescriptor &theRemovedDatasTimesOut)
{
	if(theInfo)
	{
		if(itsEditedDataKeeper)
		{
			if(IsInfosTwoOfTheKind(theInfo, theDataType, theFileNamePattern, itsEditedDataKeeper->GetIter()))
			{
				::SetDataKeeperToZero(itsEditedDataKeeper);
				UpdateEditedDataCopy();
				return;
			}
		}

		for(MapType::iterator iter = itsDataMap.begin(); iter != itsDataMap.end(); ++iter)
		{
			boost::shared_ptr<NFmiFastQueryInfo> &info = iter->second->GetDataKeeper()->GetIter();
			if(IsInfosTwoOfTheKind(theInfo, theDataType, theFileNamePattern, info))
			{
				theRemovedDatasTimesOut = info->TimeDescriptor();
				// TODO en tied� mit� pit�isi tehd�?!?!? tuhota data vai mit�? Onko se jo tehty/tehd��nk� se muualla?
				break;
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
	for(MapType::iterator iter = itsDataMap.begin(); iter != itsDataMap.end(); )
	{
		if(std::find(ignoreTypesVector.begin(), ignoreTypesVector.end(), iter->second->GetDataKeeper()->GetIter()->DataType()) == ignoreTypesVector.end())
			itsDataMap.erase(iter); // jos data poistetaan, menee iter seuraavaan pisteeseen
		else
			++iter; // jos dataa ei poistettu, pit�� sit� siirt�� t�ss� pyk�l� eteenp�in
	}
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

void NFmiInfoOrganizer::SetMacroParamDataGridSize(int x, int y)
{
	x = FmiMin(x, static_cast<int>(itsMacroParamMaxGridSize.X()));
	y = FmiMin(y, static_cast<int>(itsMacroParamMaxGridSize.Y()));
	x = FmiMax(x, static_cast<int>(itsMacroParamMinGridSize.X()));
	y = FmiMax(y, static_cast<int>(itsMacroParamMinGridSize.Y()));
	itsMacroParamGridSize = NFmiPoint(x, y);
	UpdateMacroParamDataSize(x, y);
//	UpdateMacroParamData();
}
void NFmiInfoOrganizer::SetMacroParamDataMinGridSize(int x, int y)
{
	itsMacroParamMinGridSize = NFmiPoint(x, y);
}
void NFmiInfoOrganizer::SetMacroParamDataMaxGridSize(int x, int y)
{
	itsMacroParamMaxGridSize = NFmiPoint(x, y);
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

void NFmiInfoOrganizer::UpdateMacroParamDataSize(int x, int y)
{
	static std::auto_ptr<NFmiArea> dummyArea(new NFmiLatLonArea(NFmiPoint(19,57), NFmiPoint(32,71)));

	// Luo uusi data jossa on yksi aika,param ja level ja luo hplaceDesc annetusta areasta ja hila koosta
	NFmiQueryData* data = CreateDefaultMacroParamQueryData(dummyArea.get(), x, y);
	if(data)
	{
		itsMacroParamData = boost::shared_ptr<NFmiFastQueryInfo>(new NFmiOwnerInfo(data, NFmiInfoData::kMacroParam, "", ""));
		itsMacroParamMissingValueMatrix.Resize(itsMacroParamData->Grid()->XNumber(), itsMacroParamData->Grid()->YNumber(), kFloatMissing);
		return ;
	}

	// virhetilanteissa nollataan data
	::SetFastInfoToZero(itsMacroParamData);
}

void NFmiInfoOrganizer::UpdateCrossSectionMacroParamDataSize(int x, int y)
{
	static std::auto_ptr<NFmiArea> dummyArea(new NFmiLatLonArea(NFmiPoint(19,57), NFmiPoint(32,71)));

	// Luo uusi data jossa on yksi aika,param ja level ja luo hplaceDesc annetusta areasta ja hila koosta
	NFmiQueryData* data = CreateDefaultMacroParamQueryData(dummyArea.get(), x, y);
	if(data)
	{
		itsCrossSectionMacroParamData = boost::shared_ptr<NFmiFastQueryInfo>(new NFmiOwnerInfo(data, NFmiInfoData::kCrossSectionMacroParam, "", ""));
		itsCrossSectionMacroParamMissingValueMatrix.Resize(itsCrossSectionMacroParamData->Grid()->XNumber(), itsCrossSectionMacroParamData->Grid()->YNumber(), kFloatMissing);
		return ;
	}

	// virhetilanteissa nollataan data
	::SetFastInfoToZero(itsCrossSectionMacroParamData);
}

int NFmiInfoOrganizer::CountData(void)
{
	int count = 0;
	if(itsEditedDataKeeper)
		count++;
	if(itsCopyOfEditedDataKeeper)
		count++;

	for(MapType::iterator iter = itsDataMap.begin(); iter != itsDataMap.end(); ++iter)
		count += static_cast<int>(iter->second->DataCount());

	return count;
}

double NFmiInfoOrganizer::CountDataSize(void)
{
	double dataSize = 0;
	if(itsEditedDataKeeper)
		dataSize += itsEditedDataKeeper->OriginalData()->Size() * sizeof(float);
	if(itsCopyOfEditedDataKeeper)
		dataSize += itsCopyOfEditedDataKeeper->OriginalData()->Size() * sizeof(float);

	for(MapType::iterator iter = itsDataMap.begin(); iter != itsDataMap.end(); ++iter)
		dataSize += iter->second->DataByteCount();

	return dataSize;
}

int NFmiInfoOrganizer::CleanUnusedDataFromMemory(void)
{
	int dataRemovedCounter = 0;
	for(MapType::iterator iter = itsDataMap.begin(); iter != itsDataMap.end(); ++iter)
		dataRemovedCounter += iter->second->CleanUnusedDataFromMemory();
	return dataRemovedCounter;
}

// Jos kyse ns. editoidusta datasta (esim. kepa-data), joilla on ep�s��nn�llinen ilmestymis aikav�li,
// etsit��n qdatakeeperin listoilta l�hin origin aika ennen annettua aikaa ja palautetaan sen indeksi.
// Jos ei l�ydy aikaa ennen annettua aikaa, palautetaan viimeinen indeksi (eli vanhimman ajan indeksi).
// Jos ei l�ytynyt sopivaa eps��nn�llist� dataKeeperi�, palautetaan arvo 99, joka kertoo ett� ei l�ydy.
int NFmiInfoOrganizer::GetNearestUnRegularTimeIndex(boost::shared_ptr<NFmiDrawParam> &theDrawParam, const NFmiMetTime &theTime)
{
	for(MapType::iterator iter = itsDataMap.begin(); iter != itsDataMap.end(); ++iter)
	{
		if(iter->second->ModelRunTimeGap() == -1)
		{
			boost::shared_ptr<NFmiFastQueryInfo> aInfo = GetInfo(theDrawParam);
			if(aInfo)
			{ // l�ytyi haluttu dataKeeper, nyt katsotaan mink� indeksin saadaan palautettua
				return iter->second->GetNearestUnRegularTimeIndex(theTime);
			}
		}
	}
	return 0;
}
