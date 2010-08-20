// NFmiHelpDataInfo.cpp

#ifdef _MSC_VER
#pragma warning(disable : 4996) // 4996 poistaa ep�turvallisten string manipulaatio funktioiden k�yt�st� tulevat varoitukset. En aio k�ytt�� ehdotettuja turvallisia _s -funktioita (esim. sprintf_s), koska ne eiv�t ole linux yhteensopivia.
#endif

#include "NFmiHelpDataInfo.h"
#include "NFmiArea.h"
#include "NFmiAreaFactory.h"
#include "NFmiStereographicArea.h"
#include "NFmiSettings.h"
#include "NFmiFileString.h"

using namespace std;

// ----------------------------------------------------------------------
/*!
 *  sy� spacet pois streamista ja palauttaa true:n jos ei olla lopussa
 *
 * \param theInput The input stream
 * \return Undocumented
 */
// ----------------------------------------------------------------------

NFmiHelpDataInfo::NFmiHelpDataInfo(void)
:itsName()
,itsFileNameFilter()
,fForceFileFilterName(false)
,itsLatestFileName()
,itsDataType(NFmiInfoData::kNoDataType)
,itsLatestFileTimeStamp(0)
,itsFakeProducerId(0)
,itsImageProjectionString()
,itsImageDataIdent()
,itsImageArea(0)
,fNotifyOnLoad(false)
,itsNotificationLabel()
,itsCustomMenuFolder()
,itsReportNewDataTimeStepInMinutes(0)
,itsReportNewDataLabel()
,itsCombineDataPathAndFileName()
,itsCombineDataMaxTimeSteps(0)
,fMakeSoundingIndexData(false)

,itsBaseNameSpace()
{}

NFmiHelpDataInfo::NFmiHelpDataInfo(const NFmiHelpDataInfo &theOther)
:itsName(theOther.itsName)
,itsFileNameFilter(theOther.itsFileNameFilter)
,fForceFileFilterName(theOther.fForceFileFilterName)
,itsLatestFileName(theOther.itsLatestFileName)
,itsDataType(theOther.itsDataType)
,itsLatestFileTimeStamp(theOther.itsLatestFileTimeStamp)
,itsFakeProducerId(theOther.itsFakeProducerId)
,itsImageProjectionString(theOther.itsImageProjectionString)
,itsImageDataIdent(theOther.itsImageDataIdent)
,itsImageArea(theOther.itsImageArea ? theOther.itsImageArea->Clone() : 0)
,fNotifyOnLoad(theOther.fNotifyOnLoad)
,itsNotificationLabel(theOther.itsNotificationLabel)
,itsCustomMenuFolder(theOther.itsCustomMenuFolder)
,itsReportNewDataTimeStepInMinutes(theOther.itsReportNewDataTimeStepInMinutes)
,itsReportNewDataLabel(theOther.itsReportNewDataLabel)
,itsCombineDataPathAndFileName(theOther.itsCombineDataPathAndFileName)
,itsCombineDataMaxTimeSteps(theOther.itsCombineDataMaxTimeSteps)
,fMakeSoundingIndexData(theOther.fMakeSoundingIndexData)

,itsBaseNameSpace(theOther.itsBaseNameSpace)
{}

NFmiHelpDataInfo& NFmiHelpDataInfo::operator=(const NFmiHelpDataInfo &theOther)
{
	if(this != &theOther)
	{
		Clear(); // l�hinn� area-otuksen tuhoamista varten kutsutaan
		itsName = theOther.itsName;
		itsFileNameFilter = theOther.itsFileNameFilter;
		fForceFileFilterName = theOther.fForceFileFilterName;
		itsLatestFileName = theOther.itsLatestFileName;
		itsDataType = theOther.itsDataType;
		itsLatestFileTimeStamp = theOther.itsLatestFileTimeStamp;
		itsFakeProducerId = theOther.itsFakeProducerId;
		itsImageProjectionString = theOther.itsImageProjectionString;
		itsImageDataIdent = theOther.itsImageDataIdent;
		itsImageArea = theOther.itsImageArea ? theOther.itsImageArea->Clone() : 0;
		fNotifyOnLoad = theOther.fNotifyOnLoad;
		itsNotificationLabel = theOther.itsNotificationLabel;
		itsCustomMenuFolder = theOther.itsCustomMenuFolder;
		itsReportNewDataTimeStepInMinutes = theOther.itsReportNewDataTimeStepInMinutes;
		itsReportNewDataLabel = theOther.itsReportNewDataLabel;
		itsCombineDataPathAndFileName = theOther.itsCombineDataPathAndFileName;
		itsCombineDataMaxTimeSteps = theOther.itsCombineDataMaxTimeSteps;
		fMakeSoundingIndexData = theOther.fMakeSoundingIndexData;

		itsBaseNameSpace = theOther.itsBaseNameSpace;
	}
	return *this;
}

void NFmiHelpDataInfo::Clear(void)
{
	itsName = "";
	itsFileNameFilter = "";
	fForceFileFilterName = false;
	itsLatestFileName = "";
	itsDataType = NFmiInfoData::kNoDataType;
	itsLatestFileTimeStamp = 0;
	itsFakeProducerId = 0;
	itsImageProjectionString = "";
	itsImageDataIdent = NFmiDataIdent();
	delete itsImageArea;
	itsImageArea = 0;
	fNotifyOnLoad = false;
	itsNotificationLabel = "";
	itsCustomMenuFolder = "";
	itsBaseNameSpace = "";
	itsReportNewDataTimeStepInMinutes = 0;
	itsReportNewDataLabel = "";
	itsCombineDataPathAndFileName = "";
	itsCombineDataMaxTimeSteps = 0;
	fMakeSoundingIndexData = false;
}

void NFmiHelpDataInfo::InitFromSettings(const std::string &theBaseKey, const std::string &theName)
{
	fForceFileFilterName = false;
	itsName = theName;
	itsBaseNameSpace = theBaseKey + "::" + theName;

	std::string fileNameFilterKey = itsBaseNameSpace + "::FilenameFilter";
	if(NFmiSettings::IsSet(fileNameFilterKey))
	{
		// Read configuration
		itsFileNameFilter = NFmiSettings::Require<std::string>(fileNameFilterKey);
		itsDataType = static_cast<NFmiInfoData::Type> (NFmiSettings::Require<int>(itsBaseNameSpace + "::DataType"));
		itsFakeProducerId = NFmiSettings::Optional<int>(itsBaseNameSpace + "::ProducerId", 0);
		fNotifyOnLoad = NFmiSettings::Optional<bool>(itsBaseNameSpace + "::NotifyOnLoad", false);
		itsNotificationLabel = NFmiSettings::Optional<string>(itsBaseNameSpace + "::NotificationLabel", "");
		itsCustomMenuFolder = NFmiSettings::Optional<string>(itsBaseNameSpace + "::CustomMenuFolder", "");
		itsReportNewDataTimeStepInMinutes = NFmiSettings::Optional<int>(itsBaseNameSpace + "::ReportNewDataTimeStepInMinutes", 0);
		itsReportNewDataLabel = NFmiSettings::Optional<string>(itsBaseNameSpace + "::ReportNewDataLabel", "");
		itsCombineDataPathAndFileName = NFmiSettings::Optional<string>(itsBaseNameSpace + "::CombineDataPathAndFileName", "");
		itsCombineDataMaxTimeSteps = NFmiSettings::Optional<int>(itsBaseNameSpace + "::CombineDataMaxTimeSteps", 0);
		fMakeSoundingIndexData = NFmiSettings::Optional<bool>(itsBaseNameSpace + "::MakeSoundingIndexData", false);

		std::string imageProjectionKey(itsBaseNameSpace + "::ImageProjection");
		if (NFmiSettings::IsSet(imageProjectionKey))
		{
			NFmiArea *area = NFmiAreaFactory::Create(NFmiSettings::Require<std::string>(imageProjectionKey)).release();
			if(area)
			{
				if(area->XYArea().Width() != 1 || area->XYArea().Height() != 1)
				{
					area->SetXYArea(NFmiRect(0,0,1,1));
				}
				itsImageArea = area;
			}
			NFmiParam param(NFmiSettings::Require<int>(itsBaseNameSpace + "::ParameterId")
						   ,NFmiSettings::Require<std::string>(itsBaseNameSpace + "::ParameterName"));
			itsImageDataIdent = NFmiDataIdent(param, itsFakeProducerId);
		}
	}
}

static std::string MakeCacheFilePattern(const NFmiHelpDataInfo &theDataInfo, const NFmiHelpDataInfoSystem &theHelpDataSystem)
{
	NFmiFileString fileStr(theDataInfo.FileNameFilter());
	std::string cachePattern(theHelpDataSystem.CacheDirectory());
	cachePattern += static_cast<char*>(fileStr.FileName());
	return cachePattern;
}

void NFmiHelpDataInfo::FileNameFilter(const std::string &newValue, bool forceFileNameFilter) 
{
	itsFileNameFilter = newValue;
	fForceFileFilterName = forceFileNameFilter;
}

// t�m� on viritys, ett� olisi funktio, jolla voidaan pyyt�� k�ytetty fileFilter, riippuen siit� onko cache k�yt�ss� vai ei
const std::string NFmiHelpDataInfo::UsedFileNameFilter(const NFmiHelpDataInfoSystem &theHelpDataInfoSystem) const
{
	if(fForceFileFilterName || theHelpDataInfoSystem.UseQueryDataCache() == false)
		return FileNameFilter();
	else
		return ::MakeCacheFilePattern(*this, theHelpDataInfoSystem);
}

NFmiHelpDataInfo& NFmiHelpDataInfoSystem::DynamicHelpDataInfo(int theIndex)
{
	static NFmiHelpDataInfo dummy;
	if(!itsDynamicHelpDataInfos.empty() && theIndex >= 0 && theIndex < static_cast<int>(itsDynamicHelpDataInfos.size()))
		return itsDynamicHelpDataInfos[theIndex];
	return dummy;
}
NFmiHelpDataInfo& NFmiHelpDataInfoSystem::StaticHelpDataInfo(int theIndex)
{
	static NFmiHelpDataInfo dummy;
	if(!itsStaticHelpDataInfos.empty() && theIndex >= 0 && theIndex < static_cast<int>(itsStaticHelpDataInfos.size()))
		return itsStaticHelpDataInfos[theIndex];
	return dummy;
}

// Etsii seuraavan satel-kuvan kanavan. Pit�� olla sama tuottaja.
// Palauttaa uuden dataidentin, miss� uusi parametri.
NFmiDataIdent NFmiHelpDataInfoSystem::GetNextSatelChannel(const NFmiDataIdent &theDataIdent, FmiDirection theDir)
{
	NFmiDataIdent returnDataIdent(theDataIdent);
	FmiProducerName prodId = static_cast<FmiProducerName>(theDataIdent.GetProducer()->GetIdent());
	size_t count = itsDynamicHelpDataInfos.size();
	std::vector<NFmiDataIdent> dataIdentVec;
	int counter = 0;
	int currentIndex = -1;
	for(size_t i=0; i<count; i++)
	{
		if(itsDynamicHelpDataInfos[i].DataType() == NFmiInfoData::kSatelData)
		{
			if(static_cast<unsigned long>(prodId) == itsDynamicHelpDataInfos[i].ImageDataIdent().GetProducer()->GetIdent())
			{
				dataIdentVec.push_back(itsDynamicHelpDataInfos[i].ImageDataIdent());
				if(theDataIdent.GetParamIdent() == itsDynamicHelpDataInfos[i].ImageDataIdent().GetParamIdent())
					currentIndex = counter; // laitetaan currentti param index talteen
				counter++;
			}
		}
	}
	if(counter > 1)
	{
		if(theDir == kForward)
			currentIndex++;
		else
			currentIndex--;
		if(currentIndex < 0)
			currentIndex = counter-1;
		if(currentIndex >= counter)
			currentIndex = 0;
		returnDataIdent = dataIdentVec[currentIndex];
	}
	return returnDataIdent;
}


// etsii sateldata-tyypit (oikeasti imagedatatype) l�pi ja etsii haluttua parametria
// jolle l�ytyy ns. piirrett�v� projektio. Palauttaa kyseisen datan projektion
// ja filepatternin.
// Jos fDemandMatchingArea on true, pit�� areoiden sopia niin ett� piirto onnistuu blittaamalla.
// jos false, palautetaan area ja filepattern jos datatyyppi ja parametri ovat halutut.
const NFmiArea* NFmiHelpDataInfoSystem::GetDataFilePatternAndArea(NFmiInfoData::Type theDataType, FmiProducerName theProdId, FmiParameterName theParamId, const NFmiArea *theZoomedArea, std::string &theFilePattern, bool fDemandMatchingArea)
{
	if(theDataType == NFmiInfoData::kSatelData)
	{
		size_t count = itsDynamicHelpDataInfos.size();
		for(size_t i=0; i<count; i++)
		{
			if(itsDynamicHelpDataInfos[i].DataType() == theDataType)
			{
				if(static_cast<unsigned long>(theProdId) == itsDynamicHelpDataInfos[i].ImageDataIdent().GetProducer()->GetIdent() &&
				   static_cast<unsigned long>(theParamId) == itsDynamicHelpDataInfos[i].ImageDataIdent().GetParamIdent())
				{
					if(fDemandMatchingArea && IsSameTypeProjections(theZoomedArea, itsDynamicHelpDataInfos[i].ImageArea()))
					{
						theFilePattern = itsDynamicHelpDataInfos[i].FileNameFilter();
						return itsDynamicHelpDataInfos[i].ImageArea();
					}
					else if(fDemandMatchingArea == false)
					{
						theFilePattern = itsDynamicHelpDataInfos[i].FileNameFilter();
						return itsDynamicHelpDataInfos[i].ImageArea();
					}
			}
			}
		}
	}
	return 0;
}

// Ovatko kaksi annettua projektiota siin� mieless� samanlaisia, ett� 
// voidaan sijoittaa rasterikuva oikein toisen rasterikuvan p��lle.
// Eli palauta false = ei voi piirt�� ja true = voi, 
// jos projektio luokat ovat samoja ja jos kyseess� oli stereographic areat, niiden
// orientaatiot pit�� olla samat.
bool NFmiHelpDataInfoSystem::IsSameTypeProjections(const NFmiArea *theFirst, const NFmiArea *theSecond)
{
	if(theFirst && theSecond)
	{
		if(theFirst->ClassId() == theSecond->ClassId() && theFirst->ClassId() == kNFmiStereographicArea)
		{
			if(static_cast<const NFmiStereographicArea*>(theFirst)->Orientation() == static_cast<const NFmiStereographicArea*>(theSecond)->Orientation())
				return true;
			else
				return false;
		}
		else if(theFirst->ClassId() == theSecond->ClassId())
			return true;
	}
	return false;
}

void NFmiHelpDataInfoSystem::AddDynamic(const NFmiHelpDataInfo &theInfo)
{
	itsDynamicHelpDataInfos.push_back(theInfo);
}

void NFmiHelpDataInfoSystem::AddStatic(const NFmiHelpDataInfo &theInfo)
{
	itsStaticHelpDataInfos.push_back(theInfo);
}

void NFmiHelpDataInfoSystem::InitDataType(const std::string &theBaseKey, checkedVector<NFmiHelpDataInfo> &theHelpDataInfos)
{
	std::vector<std::string> dataKeys = NFmiSettings::ListChildren(theBaseKey);
	std::vector<std::string>::iterator iter = dataKeys.begin();
	for( ; iter != dataKeys.end(); ++iter)
	{
		NFmiHelpDataInfo hdi;
		hdi.InitFromSettings(theBaseKey, *iter);
		theHelpDataInfos.push_back(hdi);
	}
}

static void FixPathEndWithSeparator(std::string &theFixedPathStr)
{
	if(theFixedPathStr.empty() == false)
	{
		std::string::value_type lastLetter = theFixedPathStr[theFixedPathStr.size()-1];
		if((lastLetter == '/' || lastLetter == '\\') == false)
			theFixedPathStr.push_back(kFmiDirectorySeparator);
	}
}

void NFmiHelpDataInfoSystem::InitFromSettings(const std::string &theBaseNameSpaceStr, std::string theHelpEditorFileNameFilter)
{
	itsBaseNameSpace = theBaseNameSpaceStr;
	itsCacheDirectory = NFmiSettings::Require<std::string>(itsBaseNameSpace + "::CacheDirectory");
	::FixPathEndWithSeparator(itsCacheDirectory);
	itsCacheTmpDirectory = NFmiSettings::Require<std::string>(itsBaseNameSpace + "::CacheTmpDirectory");
	::FixPathEndWithSeparator(itsCacheTmpDirectory);
	itsCacheTmpFileNameFix = NFmiSettings::Require<std::string>(itsBaseNameSpace + "::CacheTmpFileNameFix");
	fUseQueryDataCache = NFmiSettings::Require<bool>(itsBaseNameSpace + "::UseQueryDataCache");

	fDoCeanCache = NFmiSettings::Require<bool>(itsBaseNameSpace + "::DoCeanCache");
	itsCacheFileKeepMaxDays = NFmiSettings::Require<float>(itsBaseNameSpace + "::CacheFileKeepMaxDays");
	itsCacheMaxFilesPerPattern = NFmiSettings::Require<int>(itsBaseNameSpace + "::CacheMaxFilesPerPattern");

	// Read static helpdata configurations
	InitDataType(itsBaseNameSpace + "::Static", itsStaticHelpDataInfos);

	// Read dynamic helpdata configurations
	InitDataType(itsBaseNameSpace + "::Dynamic", itsDynamicHelpDataInfos);

	// Lis�t��n help editor mode datan luku jos niin on haluttu
	if(theHelpEditorFileNameFilter.empty() == false)
	{
		NFmiHelpDataInfo helpDataInfo;
		helpDataInfo.FileNameFilter(theHelpEditorFileNameFilter);
		helpDataInfo.DataType(NFmiInfoData::kEditingHelpData);
		AddDynamic(helpDataInfo);
	}
}

void NFmiHelpDataInfoSystem::StoreToSettings(void)
{
	if(itsBaseNameSpace.empty() == false)
	{
		// HUOM! t�ss� on toistaiseksi vain cacheen liittyvien muutosten talletukset
		NFmiSettings::Set(std::string(itsBaseNameSpace + "::CacheDirectory"), itsCacheDirectory);
		NFmiSettings::Set(std::string(itsBaseNameSpace + "::CacheTmpDirectory"), itsCacheTmpDirectory);
		NFmiSettings::Set(std::string(itsBaseNameSpace + "::CacheTmpFileNameFix"), itsCacheTmpFileNameFix);
		NFmiSettings::Set(std::string(itsBaseNameSpace + "::UseQueryDataCache"), NFmiStringTools::Convert(fUseQueryDataCache));
		NFmiSettings::Set(std::string(itsBaseNameSpace + "::DoCeanCache"), NFmiStringTools::Convert(fDoCeanCache));
		NFmiSettings::Set(std::string(itsBaseNameSpace + "::CacheFileKeepMaxDays"), NFmiStringTools::Convert(itsCacheFileKeepMaxDays));
		NFmiSettings::Set(std::string(itsBaseNameSpace + "::CacheMaxFilesPerPattern"), NFmiStringTools::Convert(itsCacheMaxFilesPerPattern));
	}
	else
		throw std::runtime_error("Error in NFmiHelpDataInfoSystem::StoreToSettings, unable to store setting.");
}

void NFmiHelpDataInfoSystem::InitSettings(const NFmiHelpDataInfoSystem &theOther, bool fDoHelpDataInfo)
{
	this->itsCacheDirectory = theOther.itsCacheDirectory;
	this->itsCacheTmpDirectory = theOther.itsCacheTmpDirectory;
	this->itsCacheTmpFileNameFix = theOther.itsCacheTmpFileNameFix;
	this->fUseQueryDataCache = theOther.fUseQueryDataCache;
	this->fDoCeanCache = theOther.fDoCeanCache;
	this->itsCacheFileKeepMaxDays = theOther.itsCacheFileKeepMaxDays;
	this->itsCacheMaxFilesPerPattern = theOther.itsCacheMaxFilesPerPattern;
	this->itsBaseNameSpace = theOther.itsBaseNameSpace;

	if(fDoHelpDataInfo)
	{
		this->itsDynamicHelpDataInfos = theOther.itsDynamicHelpDataInfos;
		this->itsStaticHelpDataInfos = theOther.itsStaticHelpDataInfos;
	}
}

void NFmiHelpDataInfoSystem::MarkAllDynamicDatasAsNotReaded()
{
	size_t ssize = itsDynamicHelpDataInfos.size();
	for(size_t i = 0; i<ssize; i++)
		itsDynamicHelpDataInfos[i].LatestFileTimeStamp(-1);
}

static NFmiHelpDataInfo* FindHelpDataInfo(checkedVector<NFmiHelpDataInfo> &theHelpInfos, const std::string &theFileNameFilter, const NFmiHelpDataInfoSystem &theHelpDataInfoSystem)
{
	size_t ssize = theHelpInfos.size();
	for(size_t i = 0; i<ssize; i++)
	{
		// Siis jos joko FileNameFilter tai CombineDataPathAndFileName (yhdistelm� datoissa t�m� 
		// on se data joka luetaan sis��n SmartMetiin) on etsitty, palautetaan helpInfo.
		if(theHelpInfos[i].UsedFileNameFilter(theHelpDataInfoSystem) == theFileNameFilter || theHelpInfos[i].CombineDataPathAndFileName() == theFileNameFilter)
			return &theHelpInfos[i];
	}
	return 0;
}

// Etsii annetun fileNameFilterin avulla HelpDataInfon ja palauttaa sen, jos l�ytyi.
// Jos ei l�ytynyt vastaavaa filePatternia, palauttaa 0-pointterin.
// K�y ensin l�pi dynaamiset helpDataInfot ja sitten staattiset.
NFmiHelpDataInfo* NFmiHelpDataInfoSystem::FindHelpDataInfo(const std::string &theFileNameFilter)
{
	if(theFileNameFilter.empty())
		return 0;

	NFmiHelpDataInfo *helpInfo = ::FindHelpDataInfo(itsDynamicHelpDataInfos, theFileNameFilter, *this);
	if(helpInfo == 0)
		helpInfo = ::FindHelpDataInfo(itsStaticHelpDataInfos, theFileNameFilter, *this);

	return helpInfo;
}

static void CollectCustomMenuItems(const checkedVector<NFmiHelpDataInfo> &theHelpInfos, std::set<std::string> &theMenuSet)
{
	size_t ssize = theHelpInfos.size();
	for(size_t i = 0; i<ssize; i++)
	{
		if(theHelpInfos[i].CustomMenuFolder().empty() == false)
			theMenuSet.insert(theHelpInfos[i].CustomMenuFolder());
	}
}

// ker�� uniikki lista mahdollisista custom Menu folder asetuksista
std::vector<std::string> NFmiHelpDataInfoSystem::GetUniqueCustomMenuList(void)
{
	std::set<std::string> menuSet;
	::CollectCustomMenuItems(itsDynamicHelpDataInfos, menuSet);
	::CollectCustomMenuItems(itsStaticHelpDataInfos, menuSet);

	std::vector<std::string> menuList(menuSet.begin(), menuSet.end());
	return menuList;
}

static void CollectCustomMenuHelpDatas(const checkedVector<NFmiHelpDataInfo> &theHelpInfos, const std::string &theCustomFolder, std::vector<NFmiHelpDataInfo> &theCustomHelpDatas)
{
	size_t ssize = theHelpInfos.size();
	for(size_t i = 0; i<ssize; i++)
	{
		if(theHelpInfos[i].CustomMenuFolder().empty() == false)
			if(theHelpInfos[i].CustomMenuFolder() == theCustomFolder)
				theCustomHelpDatas.push_back(theHelpInfos[i]);
	}
}

// ker�t��n list� niista helpDataInfoissta, joissa on asetettu kyseinen customFolder
std::vector<NFmiHelpDataInfo> NFmiHelpDataInfoSystem::GetCustomMenuHelpDataList(const std::string &theCustomFolder)
{
	std::vector<NFmiHelpDataInfo> helpDataList;
	if(theCustomFolder.empty() == false)
	{
		::CollectCustomMenuHelpDatas(itsDynamicHelpDataInfos, theCustomFolder, helpDataList);
		::CollectCustomMenuHelpDatas(itsStaticHelpDataInfos, theCustomFolder, helpDataList);
	}
	return helpDataList;
}
