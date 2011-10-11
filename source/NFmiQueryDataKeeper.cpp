
#include "NFmiQueryDataKeeper.h"
#include "NFmiSmartInfo.h"
#include "NFmiFileSystem.h"
#include "NFmiFileString.h"
#include "NFmiQueryData.h"
#include <fstream>

#ifdef _MSC_VER
#pragma warning (disable : 4244 4267 4512) // boost:in thread kirjastosta tulee ik�v�sti 4244 varoituksia
#endif
#include <boost/thread.hpp>
#ifdef _MSC_VER
#pragma warning (default : 4244 4267 4512) // laitetaan 4244 takaisin p��lle, koska se on t�rke� (esim. double -> int auto castaus varoitus)
#endif

// ************* NFmiQueryDataKeeper-class **********************

NFmiQueryDataKeeper::NFmiQueryDataKeeper(void)
:itsData()
,itsLastTimeUsedTimer()
,itsIndex(0)
,itsIteratorList()
,itsOriginTime(NFmiMetTime::gMissingTime)
,itsDataFileName()
{
}

NFmiQueryDataKeeper::NFmiQueryDataKeeper(boost::shared_ptr<NFmiOwnerInfo> &theOriginalData)
:itsData(theOriginalData)
,itsLastTimeUsedTimer()
,itsIndex(0)
,itsIteratorList()
,itsOriginTime(theOriginalData->OriginTime())
,itsDataFileName(theOriginalData->DataFileName())
{
}

NFmiQueryDataKeeper::~NFmiQueryDataKeeper(void)
{
}

boost::shared_ptr<NFmiOwnerInfo> NFmiQueryDataKeeper::OriginalData(void) 
{
	itsLastTimeUsedTimer.StartTimer();
	return itsData;
}

// T�m� palauttaa vapaana olevan Info-iteraattori kopion dataan.
boost::shared_ptr<NFmiFastQueryInfo> NFmiQueryDataKeeper::GetIter(void)
{
	// TODO: T�m� vaatii sitten s�ie turvallisen lukituksen

	itsLastTimeUsedTimer.StartTimer();
	// Katsotaan onko listassa yht��n Info-iteraattoria, joka ei ole k�yt�ss� (ref-count = 1)
	for(size_t i = 0; i < itsIteratorList.size(); i++)
	{
		if(itsIteratorList[i].use_count() <= 1)
			return itsIteratorList[i];
	}

	// Ei l�ytynyt vapaata (tai ollenkaan) Info-iteraattoria, pit�� luoda sellainen ja lis�t� listaan ja paluttaa se
	boost::shared_ptr<NFmiFastQueryInfo> infoIter;
	if(OriginalData()->DataType() == NFmiInfoData::kEditable)
		infoIter = boost::shared_ptr<NFmiFastQueryInfo>(new NFmiSmartInfo(*(dynamic_cast<NFmiSmartInfo*>(OriginalData().get())))); // HUOM! Vain editoitu data on smartInfo2 -tyyppi�, ja clone ei sovi t�ss� koska nyt tehd��n 'matala' kopio. Ett� saataisiin kaunis ratkaisu, pit�isi tehd� joku shallowClone virtuaali metodi
	else
		infoIter = boost::shared_ptr<NFmiFastQueryInfo>(new NFmiOwnerInfo(*(OriginalData().get())));

	itsIteratorList.push_back(infoIter);
	return infoIter;
}

int NFmiQueryDataKeeper::LastUsedInMS(void) const
{
	return itsLastTimeUsedTimer.CurrentTimeDiffInMSeconds();
}

// ************* NFmiQueryDataSetKeeper-class **********************

NFmiQueryDataSetKeeper::NFmiQueryDataSetKeeper(void)
:itsQueryDatas()
,itsMaxLatestDataCount(0)
,itsModelRunTimeGap(0)
,itsFilePattern()
,itsLatestOriginTime()
,itsDataType(NFmiInfoData::kNoDataType)
,itsKeepInMemoryTime(5)
{
}

NFmiQueryDataSetKeeper::NFmiQueryDataSetKeeper(boost::shared_ptr<NFmiOwnerInfo> &theData, int theMaxLatestDataCount, int theModelRunTimeGap, int theKeepInMemoryTime)
:itsQueryDatas()
,itsMaxLatestDataCount(theMaxLatestDataCount)
,itsModelRunTimeGap(theModelRunTimeGap)
,itsFilePattern()
,itsLatestOriginTime()
,itsKeepInMemoryTime(theKeepInMemoryTime)
{
	bool dataWasDeleted = false;
	AddData(theData, true, dataWasDeleted); // true tarkoittaa ett� kyse on 1. lis�tt�v�st� datasta
}

NFmiQueryDataSetKeeper::~NFmiQueryDataSetKeeper(void)
{
}

/*
static void QDataListDestroyer(NFmiQueryDataSetKeeper::ListType *theQDataListToBeDestroyed)
{
	if(theQDataListToBeDestroyed)
	{
		theQDataListToBeDestroyed->clear();
		delete theQDataListToBeDestroyed;
	}
}

static void DestroyQDatasInSeparateThread(NFmiQueryDataSetKeeper::ListType &theQDataListToBeDestroyed)
{
	NFmiQueryDataSetKeeper::ListType *swapList = new NFmiQueryDataSetKeeper::ListType;
	theQDataListToBeDestroyed.swap(*swapList); // siirret��n tuhottava lista toiseen lista-olioon
	// K�ynnistet��n uusi threadi, joka hoitaa lopullisen tuhoamisen
	boost::thread wrk_thread(::QDataListDestroyer, swapList);

	// ei j��d� odottamaan lopetusta
}
*/

// Lis�t�t��n annettu data keeper-settiin.
// Jos	itsMaxLatestDataCount on 0, tyhjennnet��n olemassa olevat listat ja datat ja laitetaan annettu data k�ytt��n.
// Jos	itsMaxLatestDataCount on > 0, katsotaan mihin kohtaan (mille indeksille) data sijoittuu, mahdollisesti vanhimman datan joutuu siivoamaan pois.
void NFmiQueryDataSetKeeper::AddData(boost::shared_ptr<NFmiOwnerInfo> &theData, bool fFirstData, bool &fDataWasDeletedOut)
{
	if(theData)
	{
		itsDataType = theData->DataType();
		if(fFirstData || itsMaxLatestDataCount == 0)
		{
			// Halusin siirt�� t�m�n datojen tuhoamisen omaan threadiin, koska ainakin debugatessa salama-kaudella, salama datan tuhomaminen kest��, koska siin� on kymmeni� tuhansia dynaamisesti luotuja NFmiMetTime-olioita tuhottavana
			itsQueryDatas.clear(); 

			//::DestroyQDatasInSeparateThread(itsQueryDatas); // ei riit� ett� tuhoaminen siirret��n omaan threadiin, OwnerInfon rakennuskin kest��!


			itsQueryDatas.push_back(boost::shared_ptr<NFmiQueryDataKeeper>(new NFmiQueryDataKeeper(theData)));
			itsFilePattern = theData->DataFilePattern();
			itsLatestOriginTime = theData->OriginTime();
		}
		else
			AddDataToSet(theData, fDataWasDeletedOut);
	}
}

// Oletus: annettu data lis�t��n t�h�n data settiin niin ett� etsit��n sen paikka (indeksi).
// Siirret��n kaikkia datoja tarpeen mukaan indekseiss�.
// Jos datoja on liikaa setiss�, poistetaan ylim��r�iset (yli max indeksiset).
// Jos sama data l�ytyy jo setist� (= sama origin-aika), korvaa listassa oleva t�ll� (esim. on tehty uusi korjattu malliajo datahaku).
void NFmiQueryDataSetKeeper::AddDataToSet(boost::shared_ptr<NFmiOwnerInfo> &theData, bool &fDataWasDeletedOut)
{
	// etsi ja korvaa, jos setist� l�ytyy jo saman origin-timen data
	NFmiMetTime origTime = theData->OriginTime();
	NFmiMetTime latestOrigTime = origTime;
	bool wasReplace = false;
	for(ListType::iterator it = itsQueryDatas.begin(); it != itsQueryDatas.end(); ++it)
	{
		const NFmiMetTime &currentOrigTime = (*it)->OriginalData()->OriginTime();
		if(latestOrigTime < currentOrigTime)
			latestOrigTime = currentOrigTime; // etsit��n samalla viimeisint� origin aikaa listasta

		if(currentOrigTime == origTime)
		{
			*it = boost::shared_ptr<NFmiQueryDataKeeper>(new NFmiQueryDataKeeper(theData)); // korvataan l�ydetty dataKeeper uudella
			wasReplace = true;
		}
	}

	// T�m� on data uudella origin ajalla. 
	// 1. Lis�� se listaan.
	itsLatestOriginTime = latestOrigTime;
	if(wasReplace == false)
		itsQueryDatas.push_back(boost::shared_ptr<NFmiQueryDataKeeper>(new NFmiQueryDataKeeper(theData)));
	// 2. Laske kaikille setin datoille indeksit uudestaan.
	RecalculateIndexies(itsLatestOriginTime);
	// 3. Ota talteen lis�tyn datan origTime
	NFmiMetTime addedDataOrigTime = theData->OriginTime();
	// 4. Poista listasta kaikki datat joiden indeksi on suurempi kuin itsMaxLatestDataCount:in arvo sallii.
	DeleteTooOldDatas();
	// 5. Tutki l�ytyyk� lis�tty data viel� listalta
	fDataWasDeletedOut = OrigTimeDataExist(addedDataOrigTime) == false;
}

// Etsi annettua origin-aikaa vastaava dataa listalta, jos ei l�ydy, palauta false, muuten true.
bool NFmiQueryDataSetKeeper::OrigTimeDataExist(const NFmiMetTime &theOrigTime)
{
	for(ListType::iterator it = itsQueryDatas.begin(); it != itsQueryDatas.end(); ++it)
	{
		if((*it)->OriginTime() == theOrigTime)
			return true;
	}
	return false;
}

static int CalcIndex(const NFmiMetTime &theLatestOrigTime, const NFmiMetTime &theOrigCurrentTime, int theModelRunTimeGap)
{
	if(theModelRunTimeGap == 0)
		return 0;
	int diffInMinutes = theLatestOrigTime.DifferenceInMinutes(theOrigCurrentTime);
	return static_cast<int>(round(-diffInMinutes/theModelRunTimeGap));
}

static bool IsNewer(const boost::shared_ptr<NFmiQueryDataKeeper> &theDataKeeper1, const boost::shared_ptr<NFmiQueryDataKeeper> &theDataKeeper2)
{
	return theDataKeeper1->OriginTime() > theDataKeeper2->OriginTime();
}

void NFmiQueryDataSetKeeper::RecalculateIndexies(const NFmiMetTime &theLatestOrigTime)
{
	if(itsModelRunTimeGap < 0)
	{ // kyse on editoidusta datasta, jolla ei ole vakio malliajo v�li�. Laitetaan lista vain aikaj�rjestykseen ja indeksoidaan numeroj�rjestyksess�
		itsQueryDatas.sort(::IsNewer);
		int index = 0;
		for(ListType::iterator it = itsQueryDatas.begin(); it != itsQueryDatas.end(); ++it)
		{
			(*it)->Index(index);
			index--;
		}
	}
	else
	{
		for(ListType::iterator it = itsQueryDatas.begin(); it != itsQueryDatas.end(); ++it)
			(*it)->Index(::CalcIndex(theLatestOrigTime, (*it)->OriginalData()->OriginTime(), itsModelRunTimeGap));
	}
}

struct OldDataRemover
{
	OldDataRemover(int theMaxLatestDataCount)
	:itsMaxLatestDataCount(theMaxLatestDataCount)
	{
	}

	bool operator()(boost::shared_ptr<NFmiQueryDataKeeper> &theDataKeeper)
	{
		if(::abs(theDataKeeper->Index()) > itsMaxLatestDataCount)
			return true;
		return false;
	}

	int itsMaxLatestDataCount;
};


void NFmiQueryDataSetKeeper::DeleteTooOldDatas(void)
{
	itsQueryDatas.remove_if(OldDataRemover(itsMaxLatestDataCount));
}

static boost::shared_ptr<NFmiQueryDataKeeper> FindQDataKeeper(NFmiQueryDataSetKeeper::ListType &theQueryDatas, int theIndex)
{
	for(NFmiQueryDataSetKeeper::ListType::iterator it = theQueryDatas.begin(); it != theQueryDatas.end(); ++it)
	{
		if((*it)->Index() == theIndex)
			return (*it);
	}
	return boost::shared_ptr<NFmiQueryDataKeeper>();
}

// TODO: Tulevaisuudessa pit�� viel� hanskata tilanne ett� halutaan uusimman ajon dataa, 
// jota ei v�ltt�m�tt� ole kyseiselle datatyypille kyseisest� mallista. Voisi olla siis arvo 0, joka tarkoittaa
// ett� hae viimeisimman malliajon data, siis Hirlam RCR:sta pintadata on jo 06, mutta jos mallipinta olisi 00 ajosta,
// pit�isi t�ll�in palauttaa 0-data. Jos indeksi olisi 1 (tai suurempi), palautettaisiin viimeisin data, ed. mainitun 
// esimerkin mukaisesti 00 mallipinta data.
boost::shared_ptr<NFmiQueryDataKeeper> NFmiQueryDataSetKeeper::GetDataKeeper(int theIndex)
{
	if(theIndex > 0)
		theIndex = 0; 

	boost::shared_ptr<NFmiQueryDataKeeper> qDataKeeperPtr = ::FindQDataKeeper(itsQueryDatas, theIndex);
	if(qDataKeeperPtr)
		return qDataKeeperPtr;

	if(DoOnDemandOldDataLoad(theIndex))
		return ::FindQDataKeeper(itsQueryDatas, theIndex); // kokeillaan, l�ytyyk� on-demand pyynn�n j�lkeen haluttua dataa 

	// jos ei l�ytynyt, palautetaan tyhj�
	return boost::shared_ptr<NFmiQueryDataKeeper>();
}

static NFmiMetTime CalcWantedOrigTime(const NFmiMetTime &theLatestOrigTime, int theIndex, int theModelRunTimeGap)
{
	NFmiMetTime wantedOrigTime = theLatestOrigTime;
	long diffInMinutes = theModelRunTimeGap * theIndex;
	wantedOrigTime.ChangeByMinutes(diffInMinutes);
	return wantedOrigTime;
}

static std::string GetFullFileName(const std::string &theFileFilter, const std::string &theFileName)
{
	NFmiFileString fileName(theFileFilter);
	fileName.FileName(theFileName);
	return static_cast<char*>(fileName);
}

bool NFmiQueryDataSetKeeper::DoOnDemandOldDataLoad(int theIndex)
{
	if(::abs(theIndex) < itsMaxLatestDataCount) // ei yritet� hakea liian vanhoja datoja
	{
		if(itsModelRunTimeGap > 0)
		{
			NFmiMetTime wantedOrigTime = ::CalcWantedOrigTime(itsLatestOriginTime, theIndex, itsModelRunTimeGap);
			std::list<std::string> files = NFmiFileSystem::PatternFiles(itsFilePattern);
			for(std::list<std::string>::iterator it = files.begin(); it != files.end(); ++it)
			{
				try
				{
					std::string usedFileName = ::GetFullFileName(itsFilePattern, *it);
					NFmiQueryInfo info;
					std::ifstream in(usedFileName.c_str());
					if(in)
					{
						in >> info;
						if(in.good())
						{
							if(info.OriginTime() == wantedOrigTime)
							{
								in.close();
								return ReadDataFileInUse(usedFileName);
							}
						}
					}
				}
				catch(...)
				{ // pit�� vain varmistaa ett� jos tiedosto on viallinen, poikkeukset napataan kiinni t�ss�
				}
			}
		}
		else if(itsModelRunTimeGap < 0)
			ReadAllOldDatasInMemory(); // editoidut datat (tai vastaavat, joilla ei ole siis s��nn�llisi� tekoaikoja) pit�� lukea kaikki muistiin, muuten ei voida laskea niiden indeksej�
	}
	return false;
}

bool NFmiQueryDataSetKeeper::ReadDataFileInUse(const std::string &theFileName)
{
	try
	{
		NFmiQueryData *data = new NFmiQueryData(theFileName);
		boost::shared_ptr<NFmiOwnerInfo> ownerInfoPtr(new NFmiOwnerInfo(data, itsDataType, theFileName, itsFilePattern));
		bool dataWasDeleted = false;
		AddDataToSet(ownerInfoPtr, dataWasDeleted);
		return (dataWasDeleted == false);
	}
	catch(...)
	{ // pit�� vain varmistaa ett� jos tiedosto on viallinen, poikkeukset napataan kiinni t�ss�
	}
	return false;
}

std::set<std::string> NFmiQueryDataSetKeeper::GetAllFileNames(void)
{
	std::set<std::string> fileNames;
	for(ListType::iterator it = itsQueryDatas.begin(); it != itsQueryDatas.end(); ++it)
		fileNames.insert((*it)->DataFileName());
	return fileNames;
}

void NFmiQueryDataSetKeeper::ReadAllOldDatasInMemory(void)
{
	std::set<std::string> filesInMemory = GetAllFileNames();
	std::list<std::string> filesOnDrive = NFmiFileSystem::PatternFiles(itsFilePattern);
	for(std::list<std::string>::iterator it = filesOnDrive.begin(); it != filesOnDrive.end(); ++it)
	{
		std::set<std::string>::iterator it2 = filesInMemory.find(*it);
		if(it2 == filesInMemory.end())
		{ // kyseist� tiedostoa ei l�ytynyt muistista, koitetaan lukea se nyt
			std::string usedFileName = ::GetFullFileName(itsFilePattern, *it);
			ReadDataFileInUse(usedFileName);
		}
	}
}

size_t NFmiQueryDataSetKeeper::DataCount(void)
{
	return itsQueryDatas.size(); 
}

size_t NFmiQueryDataSetKeeper::DataByteCount(void)
{
	size_t sizeInBytes = 0;
	for(ListType::iterator it = itsQueryDatas.begin(); it != itsQueryDatas.end(); ++it)
		sizeInBytes += (*it)->OriginalData()->Size() * sizeof(float);

	return sizeInBytes;
}

// palauttaa true, jos data on jo 'vanhaa' eli sen voi poistaa muistista.
bool NFmiQueryDataSetKeeper::CheckKeepTime(ListType::iterator &it)
{
	if((*it)->Index() != 0)
	{ // vain viimeisin data j�� tutkimatta, koska sit� ei ole tarkoitus poistaa muistista koskaan
		if((*it)->LastUsedInMS() > itsKeepInMemoryTime * 60 * 1000)
			return true;
	}
	return false;
}

// 1. Jos jotain arkisto-dataa ei ole k�ytetty tarpeeksi pitk��n aikaan, poistetaan ne muistista.
// 2. Viimeisint� dataa ei poisteta koskaan muistista.
// 3. Jos kyse editoidusta datasta (esim. kepa-data, ei s��nn�llist� ilmestymis tiheytt�, vaan niit� syntyy satunnaisina aikoina), 
// pit�� tutkia kaikki arkisto ajat ennen niiden siivoamista. Jos yht�k��n arkisto datoista on k�ytetty aikarajan sis�ll�, 
// ei mit��n heitet� pois muistista.
// 4. Palauttaa kuinka monta dataa poistettiin muistista operaation aikana 
int NFmiQueryDataSetKeeper::CleanUnusedDataFromMemory(void)
{
	int dataRemovedCounter = 0;
	if(itsModelRunTimeGap > 0)
	{ // normaalit mallidatat
		for(ListType::iterator it = itsQueryDatas.begin(); it != itsQueryDatas.end(); )
		{
			if(CheckKeepTime(it))
			{
				it = itsQueryDatas.erase(it);
				dataRemovedCounter++;
			}
			else
				++it;
		}
	}
	else if(itsModelRunTimeGap < 0)
	{ // editoitu data
		bool oneUsedTimeNewer = false;
		for(ListType::iterator it = itsQueryDatas.begin(); it != itsQueryDatas.end(); ++it)
		{
			if((*it)->Index() != 0 && CheckKeepTime(it) == false)
			{
				oneUsedTimeNewer = true;
				break;
			}
		}
		if(oneUsedTimeNewer == false)
		{
			for(ListType::iterator it = itsQueryDatas.begin(); it != itsQueryDatas.end(); )
			{
				if((*it)->Index() != 0)
				{
					it = itsQueryDatas.erase(it);
					dataRemovedCounter++;
				}
				else
					++it;
			}
		}
	}

	return dataRemovedCounter;
}



int NFmiQueryDataSetKeeper::GetNearestUnRegularTimeIndex(const NFmiMetTime &theTime)
{
	if(ModelRunTimeGap() == -1)
	{
		ReadAllOldDatasInMemory();
		std::map<long, int> timeDiffsWithIndexies;
		for(ListType::iterator it = itsQueryDatas.begin(); it != itsQueryDatas.end(); ++it)
		{
			// HUOM! etsit��n pienint� negatiivista lukua, sitten nollaa, jos kumpaakaan ei l�ydy, palautetaan pienimm�n positiivisen luvun indeksi
			long diffInMinutes = (*it)->OriginTime().DifferenceInMinutes(theTime);
			int index = (*it)->Index();
			timeDiffsWithIndexies.insert(std::make_pair(diffInMinutes, index));
		}

		if(timeDiffsWithIndexies.size())
		{
			if(timeDiffsWithIndexies.size() <= 1)
				return timeDiffsWithIndexies.begin()->second;

			std::map<long, int>::iterator it2 = timeDiffsWithIndexies.begin();
			long diffValue1 = it2->first;
			int indexValue1 = it2->second;
			long diffValue2 = diffValue1;
			for( ; it2 != timeDiffsWithIndexies.end(); )
			{
				++it2;
				diffValue2 = it2->first;
				if(diffValue1 < 0 && diffValue2 >= 0)
					return indexValue1;
				else if(diffValue1 == 0 && diffValue2 > 0)
					return indexValue1;
				else if(diffValue1 > 0)
					return indexValue1;

				diffValue1 = it2->first;
				indexValue1 = it2->second;
			}
		}
	}
	return 0;
}
