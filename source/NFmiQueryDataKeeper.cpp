
#include "NFmiQueryDataKeeper.h"
#include "NFmiSmartInfo2.h"
#include "NFmiFileSystem.h"
#include "NFmiFileString.h"
#include "NFmiQueryData.h"
#include <fstream>

// ************* NFmiQueryDataKeeper-class **********************

NFmiQueryDataKeeper::NFmiQueryDataKeeper(boost::shared_ptr<NFmiOwnerInfo> &theOriginalData)
:itsData(theOriginalData)
,itsLastTimeUsedTimer()
,itsKeepInMemoryTime(10)
,itsIndex(0)
,itsIteratorList()
{
}

NFmiQueryDataKeeper::~NFmiQueryDataKeeper(void)
{
}

boost::shared_ptr<NFmiOwnerInfo> NFmiQueryDataKeeper::OriginalData(void) 
{
	return itsData;
}

// T�m� palauttaa vapaana olevan Info-iteraattori kopion dataan.
boost::shared_ptr<NFmiFastQueryInfo> NFmiQueryDataKeeper::GetIter(void)
{
	// TODO: T�m� vaatii sitten s�ie turvallisen lukituksen

	// Katsotaan onko listassa yht��n Info-iteraattoria, joka ei ole k�yt�ss� (ref-count = 1)
	for(size_t i = 0; i < itsIteratorList.size(); i++)
	{
		if(itsIteratorList[i].use_count() <= 1)
			return itsIteratorList[i];
	}

	// Ei l�ytynyt vapaata (tai ollenkaan) Info-iteraattoria, pit�� luoda sellainen ja lis�t� listaan ja paluttaa se
	boost::shared_ptr<NFmiFastQueryInfo> infoIter;
	if(OriginalData()->DataType() == NFmiInfoData::kEditable)
		infoIter = boost::shared_ptr<NFmiFastQueryInfo>(new NFmiSmartInfo2(*(dynamic_cast<NFmiSmartInfo2*>(OriginalData().get())))); // HUOM! Vain editoitu data on smartInfo2 -tyyppi�, ja clone ei sovi t�ss� koska nyt tehd��n 'matala' kopio. Ett� saataisiin kaunis ratkaisu, pit�isi tehd� joku shallowClone virtuaali metodi
	else
		infoIter = boost::shared_ptr<NFmiFastQueryInfo>(new NFmiOwnerInfo(*(OriginalData().get())));

	itsIteratorList.push_back(infoIter);
	return infoIter;
}

// ************* NFmiQueryDataSetKeeper-class **********************

int NFmiQueryDataSetKeeper::gStaticDefaultModelRunTimeGap = 6*60; // s��det��n t�m� 6 tuntiin, koska se on yleisin malliajov�li (RCR, MBE, Arome, GFS)

NFmiQueryDataSetKeeper::NFmiQueryDataSetKeeper(void)
:itsQueryDatas()
,itsMaxLatestDataCount(0)
,itsModelRunTimeGap(gStaticDefaultModelRunTimeGap)
,itsFilePattern()
,itsLatestOriginTime()
,itsDataType(NFmiInfoData::kNoDataType)
{
}

NFmiQueryDataSetKeeper::NFmiQueryDataSetKeeper(boost::shared_ptr<NFmiOwnerInfo> &theData, int theMaxLatestDataCount, int theModelRunTimeGap)
:itsQueryDatas()
,itsMaxLatestDataCount(theMaxLatestDataCount)
,itsModelRunTimeGap(theModelRunTimeGap)
,itsFilePattern()
,itsLatestOriginTime()
{
	AddData(theData, true); // true tarkoittaa ett� kyse on 1. lis�tt�v�st� datasta
}

NFmiQueryDataSetKeeper::~NFmiQueryDataSetKeeper(void)
{
}

// Lis�t�t��n annettu data keeper-settiin.
// Jos	itsMaxLatestDataCount on 0, tyhjennnet��n olemassa olevat listat ja datat ja laitetaan annettu data k�ytt��n.
// Jos	itsMaxLatestDataCount on > 0, katsotaan mihin kohtaan (mille indeksille) data sijoittuu, mahdollisesti vanhimman datan joutuu siivoamaan pois.
void NFmiQueryDataSetKeeper::AddData(boost::shared_ptr<NFmiOwnerInfo> &theData, bool fFirstData)
{
	if(theData)
	{
		itsDataType = theData->DataType();
		if(fFirstData || itsMaxLatestDataCount == 0)
		{
			itsQueryDatas.clear(); 
			itsQueryDatas.push_back(boost::shared_ptr<NFmiQueryDataKeeper>(new NFmiQueryDataKeeper(theData)));
			itsFilePattern = theData->DataFilePattern();
			itsLatestOriginTime = theData->OriginTime();
		}
		else
			AddDataToSet(theData);
	}
}

// Oletus: annettu data lis�t��n t�h�n data settiin niin ett� etsit��n sen paikka (indeksi).
// Siirret��n kaikkia datoja tarpeen mukaan indekseiss�.
// Jos datoja on liikaa setiss�, poistetaan ylim��r�iset (yli max indeksiset).
// Jos sama data l�ytyy jo setist� (= sama origin-aika), korvaa listassa oleva t�ll� (esim. on tehty uusi korjattu malliajo datahaku).
void NFmiQueryDataSetKeeper::AddDataToSet(boost::shared_ptr<NFmiOwnerInfo> &theData)
{
	// etsi ja korvaa, jos setist� l�ytyy jo saman origin-timen data
	NFmiMetTime origTime = theData->OriginTime();
	NFmiMetTime latestOrigTime = origTime;
	for(ListType::iterator it = itsQueryDatas.begin(); it != itsQueryDatas.end(); ++it)
	{
		const NFmiMetTime &currentOrigTime = (*it)->OriginalData()->OriginTime();
		if(latestOrigTime < currentOrigTime)
			latestOrigTime = currentOrigTime; // etsit��n samalla viimeisint� origin aikaa listasta

		if(currentOrigTime == origTime)
		{
			*it = boost::shared_ptr<NFmiQueryDataKeeper>(new NFmiQueryDataKeeper(theData));
			// koska kyseess� oli vain toisen datan korvaava toimenpide, voidaan lis�ys-operaatio lopettaa t�h�n....
			return ;
		}
	}

	// T�m� on data uudella origin ajalla. 
	// 1. Lis�� se listaan.
	itsQueryDatas.push_back(boost::shared_ptr<NFmiQueryDataKeeper>(new NFmiQueryDataKeeper(theData)));
	// 2. Laske kaikille setin datoille indeksit uudestaan.
	RecalculateIndexies(latestOrigTime);
	// 3. Poista listasta kaikki datat joiden indeksi on suurempi kuin itsMaxLatestDataCount:in arvo sallii.
	DeleteTooOldDatas();
}

static int CalcIndex(const NFmiMetTime &theLatestOrigTime, const NFmiMetTime &theOrigCurrentTime, int theModelRunTimeGap)
{
	if(theModelRunTimeGap == 0)
		return 0;
	int diffInMinutes = theLatestOrigTime.DifferenceInMinutes(theOrigCurrentTime);
	return round(-diffInMinutes/theModelRunTimeGap);
}

void NFmiQueryDataSetKeeper::RecalculateIndexies(const NFmiMetTime &theLatestOrigTime)
{
	for(ListType::iterator it = itsQueryDatas.begin(); it != itsQueryDatas.end(); ++it)
		(*it)->Index(::CalcIndex(theLatestOrigTime, (*it)->OriginalData()->OriginTime(), itsModelRunTimeGap));
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
					NFmiFileString fileName(itsFilePattern);
					fileName.FileName(*it);
					std::string usedFileName = fileName;
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
								// TODO lue data t�ss� k�ytt��n
								NFmiQueryData *data = new NFmiQueryData(usedFileName);
								boost::shared_ptr<NFmiOwnerInfo> ownerInfoPtr(new NFmiOwnerInfo(data, itsDataType, usedFileName, itsFilePattern));
								AddDataToSet(ownerInfoPtr);
								return true;
							}
						}
					}
				}
				catch(...)
				{ // pit�� vain varmistaa ett� jos tiedosto on viallinen, poikkeukset napataan kiinni t�ss�
				}
			}
		}
	}
	return false;
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
