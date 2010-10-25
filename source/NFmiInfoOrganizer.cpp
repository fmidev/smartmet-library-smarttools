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

#include "NFmiInfoOrganizer.h"
#include "NFmiSmartInfo.h"
#include "NFmiDrawParamFactory.h"
#include "NFmiDrawParam.h"
#include "NFmiGrid.h"
#include "NFmiQueryDataUtil.h"
#include "NFmiQueryData.h"
#include "NFmiLatLonArea.h"
#include "NFmiProducerName.h"

//--------------------------------------------------------
// Constructor/Destructor
//--------------------------------------------------------
// luodaan tyhj� lista
NFmiInfoOrganizer::NFmiInfoOrganizer(void)
:itsList()
,itsDrawParamFactory(0)
,itsWorkingDirectory("")
,itsEditedData(0)
,itsEditedDataCopy(0)
,itsMacroParamGridSize(50, 50)
,itsMacroParamMinGridSize(10, 10)
,itsMacroParamMaxGridSize(200, 200)
,itsMacroParamData(0)
,itsMacroParamMissingValueMatrix()
,itsCrossSectionMacroParamData(0)
,itsCrossSectionMacroParamMissingValueMatrix()
,fCreateEditedDataCopy(true)
{
	NFmiLatLonArea defaultArea(NFmiPoint(0,0), NFmiPoint(50,50)); // tehd��n vain dummy area, kun dataa piirret��n ja lasketaan, halutun karttan�yt�n alue asetetaan NFmiEditMapGeneralDataDoc::MakeDrawedInfoVectorForMapView -metodissa 
	UpdateSpecialDataArea(&defaultArea, itsMacroParamGridSize, NFmiInfoData::kMacroParam, &itsMacroParamData, itsMacroParamMissingValueMatrix);
}

// destruktori tuhoaa infot
NFmiInfoOrganizer::~NFmiInfoOrganizer (void)
{
	Clear();
	delete itsDrawParamFactory;
	if(itsEditedData)
		itsEditedData->DestroyData();
	delete itsEditedData;

	if(itsEditedDataCopy)
		itsEditedDataCopy->DestroyData();
	delete itsEditedDataCopy;

	if(itsMacroParamData)
		itsMacroParamData->DestroyData();
	delete itsMacroParamData;

	if(itsCrossSectionMacroParamData)
		itsCrossSectionMacroParamData->DestroyData();
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

NFmiSmartInfo* NFmiInfoOrganizer::GetSynopPlotParamInfo(NFmiInfoData::Type theType)
{
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

NFmiSmartInfo* NFmiInfoOrganizer::GetSoundingPlotParamInfo(NFmiInfoData::Type theType)
{
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

static bool CheckDataType(NFmiSmartInfo *theInfo, NFmiInfoData::Type theType)
{
	bool anyDataOk = (theType == NFmiInfoData::kAnyData);
	if(theInfo && (theInfo->DataType() == theType || anyDataOk))
		return true;
	return false;
}

static bool CheckDataIdent(NFmiSmartInfo *theInfo, const NFmiDataIdent& theDataIdent, bool fUseParIdOnly)
{
	if(theInfo && (fUseParIdOnly ? theInfo->Param(static_cast<FmiParameterName>(theDataIdent.GetParamIdent())): theInfo->Param(theDataIdent)))
		return true;
	return false;
}

static bool CheckLevel(NFmiSmartInfo *theInfo, const NFmiLevel *theLevel)
{
	if(theInfo && (!theLevel || (theLevel && theInfo->Level(*theLevel))))
		return true;
	return false;
}

//--------------------------------------------------------
// GetInfo
// Yritin aluksi tehd� metodin k�ytt�m�ll� parametria 
// bool fIgnoreProducerName = false
// siksi ett� voisi olla samalta tuottajalta useita samaantyyppisi�
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
NFmiSmartInfo* NFmiInfoOrganizer::GetInfo(const NFmiDataIdent& theDataIdent
									   , const NFmiLevel* theLevel
									   , NFmiInfoData::Type theType
									   , bool fUseParIdOnly)
{
	NFmiSmartInfo *backupData = 0; // etsit�� t�h�n 1. data joka muuten sopii kriteereihin, mutta 
									// jonka tuottaja nimi on eri kuin haluttu. Jos oikealla nimell� ei l�ydy dataa, k�ytet��n t�t�.
	if(theDataIdent.GetParamIdent() == NFmiInfoData::kFmiSpSynoPlot || theDataIdent.GetParamIdent() == NFmiInfoData::kFmiSpMinMaxPlot) // synop plot paramille pit�� tehd� kikka (ja min/max plot 9996)
		return GetSynopPlotParamInfo(theType);
	if(theLevel && theLevel->LevelType() == kFmiSoundingLevel) // sounding plot paramille pit�� tehd� kikka
		return GetSoundingPlotParamInfo(theType);
	if(theType == NFmiInfoData::kMacroParam || theType == NFmiInfoData::kQ3MacroParam) // macro- parametrit lasketaan t�ll�
		return itsMacroParamData; // t�ss� ei parametreja ja leveleit� ihmetell�, koska ne muutetaan aina lennossa tarpeen vaatiessa
	if(theType == NFmiInfoData::kCrossSectionMacroParam)
		return itsCrossSectionMacroParamData; // t�ss� ei parametreja ja leveleit� ihmetell�, koska ne muutetaan aina lennossa tarpeen vaatiessa

	NFmiSmartInfo* foundData = 0;
	if(::CheckDataType(itsEditedData, theType) && ::CheckDataIdent(itsEditedData, theDataIdent, fUseParIdOnly) && ::CheckLevel(itsEditedData, theLevel))
		foundData = itsEditedData;
	else if(::CheckDataType(itsEditedDataCopy, theType) && ::CheckDataIdent(itsEditedDataCopy, theDataIdent, fUseParIdOnly) && ::CheckLevel(itsEditedDataCopy, theLevel))
		foundData = itsEditedDataCopy;
	else
	{
		// tutkitaan ensin l�ytyyk� theParam suoraan joltain listassa olevalta NFmiSmartInfo-pointterilta
		for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
		{
			NFmiSmartInfo *aInfo = &(iter.Current());
			if(::CheckDataType(aInfo, theType) && ::CheckDataIdent(aInfo, theDataIdent, fUseParIdOnly) && ::CheckLevel(aInfo,theLevel))
			{
				if(!(theLevel == 0 && aInfo->SizeLevels() > 1))
				{
					if(theDataIdent.GetProducer()->GetName() == aInfo->Param().GetProducer()->GetName())
					{
						foundData = aInfo;
						break;
					}
					else if(backupData == 0)
						backupData = aInfo;
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
NFmiSmartInfo* NFmiInfoOrganizer::CrossSectionInfo(const NFmiDataIdent& theDataIdent
													, NFmiInfoData::Type theType)
{
	if(theType == NFmiInfoData::kCrossSectionMacroParam)
		return itsCrossSectionMacroParamData;
	NFmiSmartInfo *backupData = 0; // etsit�� t�h�n 1. data joka muuten sopii kriteereihin, mutta 
									// jonka tuottaja nimi on eri kuin haluttu. Jos oikealla nimell� ei l�ydy dataa, k�ytet��n t�t�.
	NFmiSmartInfo *foundData = 0;
	if(::CheckDataType(itsEditedData, theType) && itsEditedData->SizeLevels() > 1 && ::CheckDataIdent(itsEditedData, theDataIdent, true))
		foundData = itsEditedData;
	else
	{
		// tutkitaan ensin l�ytyyk� theParam suoraan joltain listassa olevalta NFmiSmartInfo-pointterilta
		for(NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start(); iter.Next(); )
		{
			NFmiSmartInfo *aInfo = &(iter.Current());
			if(::CheckDataType(aInfo, theType) && aInfo->SizeLevels() > 1 && ::CheckDataIdent(aInfo, theDataIdent, false))
			{
				if(theDataIdent.GetProducer()->GetName() == aInfo->Param().GetProducer()->GetName())
				{
					foundData = aInfo;
					break;
				}
				else if(backupData == 0)
					backupData = aInfo;
			}
		}
	}

	if(foundData == 0 && backupData != 0)
		foundData = backupData;

	return foundData;
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
				aSmartInfo->DestroyData(false); // false t�ss� tarkoittaa ett� ei tuhota queryDataa

				throw ; // heitet��n poikkeus eteenp�in
			}

			if(itsEditedData)
			{
				itsEditedData->DestroyData();
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
			itsEditedData->DestroyData();
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
				iter.Current().DestroyData();
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
			iter.Current().DestroyData();
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
				itsEditedData->DestroyData();
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
				iter.Current().DestroyData();
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
		return itsList.AddEnd(theInfo);
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
		iter.Current().DestroyData();
		iter.Remove(true);
	}
	itsList.Clear(false);
	return true; // jotain pit�si varmaan tsekatakin?
}

// 28.09.1999/Marko Tekee uuden kopion editoitavasta datasta
void NFmiInfoOrganizer::UpdateEditedDataCopy(void)
{
	if(fCreateEditedDataCopy)
	{
		if(itsEditedData)
		{
			if(itsEditedDataCopy)
				itsEditedDataCopy->DestroyData();
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

void NFmiInfoOrganizer::UpdateSpecialDataArea(const NFmiArea *theArea, const NFmiPoint &theGridSize, NFmiInfoData::Type theType, NFmiSmartInfo ** theData, NFmiDataMatrix<float> &theMissingValueMatrix)
{
	// tuhoa ensin vanha pois alta
	if(*theData)
		(*theData)->DestroyData();
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
		itsCrossSectionMacroParamData->DestroyData();
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

NFmiSmartInfo* NFmiInfoOrganizer::FindInfo(NFmiInfoData::Type theDataType, int theIndex) // Hakee indeksin mukaisen tietyn datatyypin infon
{
	if(theDataType == NFmiInfoData::kEditable)
		return itsEditedData;
	else if(theDataType == NFmiInfoData::kCopyOfEdited)
		return itsEditedDataCopy;
	else
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
	}
	return 0;
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
	NFmiPtrList<NFmiSmartInfo>::Iterator iter = itsList.Start();
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
		return itsEditedData;
	else if(theDataType == NFmiInfoData::kCopyOfEdited)
		return itsEditedDataCopy;
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

	NFmiSmartInfo* info = FindInfo(NFmiInfoData::kEditable);
	if(info)
	{
		if(theProducer.GetIdent() == kFmiMETEOR || (*info->Producer() == theProducer)) // t�ss� hanskataan 'editoitu' data, jolloin ignoorataan tuottaja
		{
			info = FindInfo(NFmiInfoData::kEditable);
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

static bool UseParIdOnly(NFmiInfoData::Type theDataType)
{
	if(theDataType == NFmiInfoData::kEditable || theDataType == NFmiInfoData::kCopyOfEdited || theDataType == NFmiInfoData::kAnyData) // jos editoitava data, ei tuottajalla v�li�
		return true;
	return false;
}

NFmiSmartInfo* NFmiInfoOrganizer::Info(NFmiDrawParam &theDrawParam, bool fCrossSectionInfoWanted)
{
	NFmiInfoData::Type dataType = theDrawParam.DataType();
	if(fCrossSectionInfoWanted)
		return CrossSectionInfo(theDrawParam.Param(), dataType);
	else
	{
		NFmiLevel* level = &theDrawParam.Level();
		if(level && level->GetIdent() == 0) // jos t�m� on ns. default-level otus (GetIdent() == 0), annetaan 0-pointteri Info-metodiin
			level = 0;
		return GetInfo(theDrawParam.Param(), level, dataType, ::UseParIdOnly(dataType));
	}
}

NFmiSmartInfo* NFmiInfoOrganizer::Info(const NFmiDataIdent& theIdent, const NFmiLevel* theLevel, NFmiInfoData::Type theType, bool fUseParIdOnly, bool fLevelData)
{
	if(fLevelData)
		return CrossSectionInfo(theIdent, theType);
	else 
		return GetInfo(theIdent, theLevel, theType, (fUseParIdOnly || ::UseParIdOnly(theType)));
}

NFmiParamBag NFmiInfoOrganizer::GetParams(int theProducerId1)
{
	NFmiParamBag paramBag;
	checkedVector<NFmiSmartInfo*> infos(GetInfos(theProducerId1));
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
