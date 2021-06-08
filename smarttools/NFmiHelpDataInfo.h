// NFmiHelpDataInfo.h
//
// NFmiHelpDataInfo-luokka hoitaa yhden Meteorologin editorin apu-datan
// tiedot. Se sisältää tiedon poluista, tiedosto filttereistä ja mikä
// on viimeksi ladatun datan tiedoston aikaleima jne.
//
// NFmiHelpDataInfoSystem-luokka taas pitää sisällään kaikki Metkun editorin
// käyttämät apudatat.

#pragma once

#include <boost/shared_ptr.hpp>
#include <newbase/NFmiDataIdent.h>
#include <newbase/NFmiDataMatrix.h>
#include <newbase/NFmiInfoData.h>
#include <newbase/NFmiProducerName.h>

class NFmiArea;
class NFmiHelpDataInfoSystem;

const long kTimeInterpolationRangeDefaultValueInMinutes = 6 * 60;

class NFmiHelpDataInfo
{
 public:
  NFmiHelpDataInfo(void) = default;
  NFmiHelpDataInfo(const NFmiHelpDataInfo &theOther);
  ~NFmiHelpDataInfo(void) = default;
  void InitFromSettings(const std::string &theBaseKey,
                        const std::string &theName,
                        const NFmiHelpDataInfoSystem &theHelpDataSystem);
  // void StoreToSettings(void);  // HUOM! ei toteuteta ainakaan vielä talletusta

  NFmiHelpDataInfo &operator=(const NFmiHelpDataInfo &theOther);

  bool IsCombineData(void) const { return itsCombineDataPathAndFileName.empty() == false; }
  const std::string &Name(void) const { return itsName; }
  void Name(const std::string &newValue) { itsName = newValue; }
  const std::string &FileNameFilter(void) const { return itsFileNameFilter; }
  void FileNameFilter(const std::string &newValue, bool forceFileNameFilter = false);
  // tämä on viritys, että olisi funktio, jolla voidaan pyytää käytetty fileFilter, riippuen siitä
  // onko cache käytössä vai ei
  const std::string UsedFileNameFilter(const NFmiHelpDataInfoSystem &theHelpDataInfoSystem) const;
  const std::string &LatestFileName(void) const { return itsLatestFileName; }
  void LatestFileName(const std::string &newName) { itsLatestFileName = newName; }
  std::string &LatestErroneousFileName(void) { return itsLatestErroneousFileName; }
  void LatestErroneousFileName(const std::string &newValue)
  {
    itsLatestErroneousFileName = newValue;
  }
  NFmiInfoData::Type DataType(void) const { return itsDataType; }
  void DataType(NFmiInfoData::Type newValue) { itsDataType = newValue; }
  time_t LatestFileTimeStamp(void) const { return itsLatestFileTimeStamp; }
  void LatestFileTimeStamp(time_t newValue) { itsLatestFileTimeStamp = newValue; }
  int FakeProducerId(void) const { return itsFakeProducerId; }
  void FakeProducerId(int newValue) { itsFakeProducerId = newValue; }
  const NFmiDataIdent &ImageDataIdent(void) const { return itsImageDataIdent; }
  void ImageDataIdent(const NFmiDataIdent &newValue) { itsImageDataIdent = newValue; }
  boost::shared_ptr<NFmiArea> ImageArea(void) const { return itsImageArea; }
  void ImageArea(boost::shared_ptr<NFmiArea> &newValue);
  bool NotifyOnLoad(void) const { return fNotifyOnLoad; }
  void NotifyOnLoad(bool newValue) { fNotifyOnLoad = newValue; }
  const std::string &NotificationLabel(void) const { return itsNotificationLabel; }
  void NotificationLabel(const std::string &newValue) { itsNotificationLabel = newValue; }
  const std::string &CustomMenuFolder(void) const { return itsCustomMenuFolder; }
  void CustomMenuFolder(const std::string &newValue) { itsCustomMenuFolder = newValue; }
  int ReportNewDataTimeStepInMinutes(void) const { return itsReportNewDataTimeStepInMinutes; }
  void ReportNewDataTimeStepInMinutes(int newValue)
  {
    itsReportNewDataTimeStepInMinutes = newValue;
  }
  const std::string &ReportNewDataLabel(void) const { return itsReportNewDataLabel; }
  void ReportNewDataLabel(const std::string &newValue) { itsReportNewDataLabel = newValue; }
  const std::string &CombineDataPathAndFileName(void) const
  {
    return itsCombineDataPathAndFileName;
  }
  void CombineDataPathAndFileName(const std::string &newValue)
  {
    itsCombineDataPathAndFileName = newValue;
  }
  int CombineDataMaxTimeSteps(void) const { return itsCombineDataMaxTimeSteps; }
  void CombineDataMaxTimeSteps(int newValue) { itsCombineDataMaxTimeSteps = newValue; }
  bool MakeSoundingIndexData(void) const { return fMakeSoundingIndexData; }
  void MakeSoundingIndexData(bool newValue) { fMakeSoundingIndexData = newValue; }
  bool ForceFileFilterName(void) const { return fForceFileFilterName; }
  void ForceFileFilterName(bool newValue) { fForceFileFilterName = newValue; }
  int AdditionalArchiveFileCount(void) const { return itsAdditionalArchiveFileCount; }
  void AdditionalArchiveFileCount(int newValue) { itsAdditionalArchiveFileCount = newValue; }
  bool IsEnabled(void) const { return fEnable; }
  void Enable(bool newValue) { fEnable = newValue; }
  bool NonFixedTimeGab(void) const { return fNonFixedTimeGab; }
  void NonFixedTimeGab(bool newValue) { fNonFixedTimeGab = newValue; }
  float ModelRunTimeGapInHours(void) const { return itsModelRunTimeGapInHours; }
  void ModelRunTimeGapInHours(float newValue) { itsModelRunTimeGapInHours = newValue; }
  long TimeInterpolationRangeInMinutes() const { return itsTimeInterpolationRangeInMinutes; }
  void TimeInterpolationRangeInMinutes(long newValue)
  {
    itsTimeInterpolationRangeInMinutes = newValue;
  }
  bool ReloadCaseStudyData() const { return fReloadCaseStudyData; }
  void ReloadCaseStudyData(bool newValue) { fReloadCaseStudyData = newValue; }

  const std::string &PartialDataCacheFileNameFilter(void) const
  {
    return itsPartialDataCacheFileNameFilter;
  }
  void PartialDataCacheFileNameFilter(const std::string &newValue)
  {
    itsPartialDataCacheFileNameFilter = newValue;
  }
  std::string GetCleanedName() const;
  const std::string &RequiredGroundDataFileFilterForSoundingIndexCalculations() const
  {
    return itsRequiredGroundDataFileFilterForSoundingIndexCalculations;
  }
  void RequiredGroundDataFileFilterForSoundingIndexCalculations(const std::string &newValue)
  {
    itsRequiredGroundDataFileFilterForSoundingIndexCalculations = newValue;
  }

  const std::string &LegacyAreaString() const { return itsLegacyAreaString; }
  void LegacyAreaString(const std::string &newValue) { itsLegacyAreaString = newValue; }
  void FixCombinedDataPath(const std::string &absoluteControlBasePath);

 private:
  // tällä nimellä erotetaan konffi-tiedostoissa eri datat
  std::string itsName;
  // tiedostonimi filtteri polun kera esim. d:\weto\wrk\data\in\data_1_*.sqd
  std::string itsFileNameFilter;
  // cachen käyttö ja partial-datat ovat oma lukunsa ja niitä varten pitää tehdä filefiltteri
  std::string itsPartialDataCacheFileNameFilter;
  // jos tämä on erikoistilanteissa asetettu true:ksi, ei välitetä mahdillisista cachen käytöistä,
  // vaan UsedFilefilter-metodi palauttaa itsFileNameFilter:in arvon
  bool fForceFileFilterName = false;
  // kun filtterillä on haettu tiedostot ja uusin on löytynyt, talletetaan se tähän
  std::string itsLatestFileName;
  // Jos datatiedosto on jotenkin korruptoitunut, talletetaan viimeisimmän sellaisen nimi, jotta
  // lokituksen yhteydessä ei tarvitse minuutin välein raportoida saman tiedoston kanssa olevista
  // ongelmista.
  std::string itsLatestErroneousFileName;
  // esim. analyysi, havainto jne.
  NFmiInfoData::Type itsDataType = NFmiInfoData::kNoDataType;
  // viimeksi luetus
  time_t itsLatestFileTimeStamp = 0;
  // joskus pitää muuttaa tuottaja id ennenkuin data otetaan käyttöön esim. kepa data, jos tämä on
  // 0, ei tehdä mitään
  int itsFakeProducerId = 0;

  // Seuraavat 3 asetusta koskevat vain image-tyyppisiä juttuja (muissa näitä ei lueta/talleteta
  // tiedostoon)
  // ============================================================================================

  // jos data on image-tyyppistä (satel,radar, jne.), on tiedostossa
  // stringi, joka sisältää projektio tiedon jonka NFmiAreaFactory ymmärtää
  std::string itsImageProjectionString;
  // tieto image paramin id:stä, nimestä (vain id ja nimi talletetaan tiedostoon)
  NFmiDataIdent itsImageDataIdent;
  // tähän luodaan ed. stringin avulla projektio, tämä ei ole tallessa tiedostossa
  // Edelliset koskevat vain image-tyyppisiä juttuja
  boost::shared_ptr<NFmiArea> itsImageArea;
  // Otetaan talteen legacy newbase area stringi, koska uusi wgs84 pohjainen systeemi (PROJ + gdal
  // kirjastot) ei voi tuottaa niitä enää järkevästi
  std::string itsLegacyAreaString;
  // Jos datan latauksen yhteydessä halutaan tehdä ilmoitus, tämä on true. Oletus arvo on false
  bool fNotifyOnLoad = false;
  // Jos notifikaatioon halutaan tietty sanoma, se voidaan antaa tähän. Defaulttina annetaan
  // tiedoston nimi
  std::string itsNotificationLabel;
  // Jos data halutaan laittaa haluttuun hakemistoon param-popupeissa, tehdää sellainen asetus
  // helpdata konffeihin. Defaulttina tyhjä, jolloin data menee 'normaaliin' paikkaansa valikoissa.
  std::string itsCustomMenuFolder;
  // Default arvo on 0, jolloin tällä ei ole vaikutusta. Tämän avulla voidaan sanoa
  // että SmartMetin pitää tehdä raportointia 'puhekuplilla', kun tulee uutta dataa esim. uudelle
  // tunnille.
  int itsReportNewDataTimeStepInMinutes = 0;
  // Jos halutaan tietty teksti viestiin, se lisätään tähän.
  std::string itsReportNewDataLabel;
  // jos tämä on määritelty, tehdään fileNameFilteristä (hakemistosta) löytyvistä datoista
  // yhdistelmä data ja se talletetaan tähän hakemistoon annetulla nimellä ja aikaleimalla (nimessä
  // tähden tilalle laitetaan aikaleima)
  std::string itsCombineDataPathAndFileName;

  // jos ei haluaa rajoittaa kuinka iso yhdistelmä datasta tehdään, tämän voi määritellä
  int itsCombineDataMaxTimeSteps = 0;
  // jos tämä on true, SmartMet tekee datasta oman sounding-index datan
  bool fMakeSoundingIndexData = false;
  // Halutessa voidaan vaatia, että kun tehdään SI dataa, pitää löytyä siihen sopiva maanpintadata:
  // 1. Se on samalta malliajolta
  // 2. Siinä on mukana paine maanpinnalla parametri (kFmiPressureAtStationLevel = 472)
  // Jos tämä on tyhjä (oletus), ei tarvita maanpinta dataa
  std::string itsRequiredGroundDataFileFilterForSoundingIndexCalculations;
  std::string itsBaseNameSpace;
  // defaultti on 0, joitakin datoja (esim. kepa-datoja, joita tuotetaan n. 15-20 per päivä)
  // halutaan säilyttää enemmän kuin muita NFmiInfoOrganizer:issa. Tällä säädöllä saadaan pidettyä
  // ylimääräiset datat.
  int itsAdditionalArchiveFileCount = 0;
  // Tämä on pakollinen säätö optio siitä, että onko data käytössä. Tämä säätää
  // ladataanko dataa cacheen, SmartMEtiin, tehdäänkö soundinIndex-dataa,
  // yhdistelmä datoja. Lisäksi tuleeko data popup-valikoihin, tai mihinkään muihin datavalikoihin.
  bool fEnable = true;
  // Tämä on valinnainen asetus. Onko datan malliajo väli määrittelemätön. Esim. silam-datat ovat
  // epämääräisesti ajettuja, myös virallinen editoitu data on määrittelemätön, mutta se hoidetaan
  // erikois tapauksena.
  bool fNonFixedTimeGab = false;
  // Joskus pitää laittaa tietyille datoille omat malliajo välit, jos ne poikkeavat totutuista
  // Esim. EC:llä on normaalisti 12 h, mutta nyt 3vrk EC datat tulevat 6 tunnin välein, joten 3 vrk
  // datoille pitää asettaa tämä 6:ksi. Default arvo on 0, jolloin tästä ei välitetä.
  float itsModelRunTimeGapInHours = 0;

  // Kuinka pitkältä aikajänteeltä SmartMet sallii aikainterpolaation tapahtuvan (käytetään ainakin
  // hilapiirroissa). Tämän arvo on optionaalinen ja sen oletuspituus on 6 tuntia. SmartMet käyttää
  // hila muotoisille havaintodatoille arvoa 1 tunti, jos tätä ei ole määritelty.
  long itsTimeInterpolationRangeInMinutes = kTimeInterpolationRangeDefaultValueInMinutes;
  // Joitakin datoja ei haluta poistaa ja uudelleen ladata case-study tapauksissa,
  // niille datoilla tämä pitää laittaa false:ksi konfiguraatioista. Esim. ERA-data,
  // observed-climatology.
  bool fReloadCaseStudyData = true;
};

class NFmiHelpDataInfoSystem
{
 public:
  NFmiHelpDataInfoSystem(void)
      : itsDynamicHelpDataInfos(),
        itsStaticHelpDataInfos(),
        itsCacheDirectory(),
        itsCacheTmpDirectory(),
        itsCachePartialDataDirectory(),
        itsCacheTmpFileNameFix(),
        fUseQueryDataCache(false),
        fDoCleanCache(false),
        itsCacheFileKeepMaxDays(-1),
        itsCacheMaxFilesPerPattern(-1),
        itsCacheMediumFileSizeMB(40),
        itsCacheLargeFileSizeMB(200),
        itsCacheMaximumFileSizeMB(10000),
        itsBaseNameSpace()
  {
  }

  void Clear(void) {}
  void InitFromSettings(const std::string &theInitNameSpace,
                        const std::string &absoluteControlBasePath,
                        std::string theHelpEditorFileNameFilter = "",
                        std::string theHelpDataName = "");
  void StoreToSettings(void);
  void InitSettings(const NFmiHelpDataInfoSystem &theOther, bool fDoHelpDataInfo);

  NFmiHelpDataInfo &DynamicHelpDataInfo(int theIndex);
  NFmiHelpDataInfo &StaticHelpDataInfo(int theIndex);
  int DynamicCount(void) const { return static_cast<int>(itsDynamicHelpDataInfos.size()); }
  int StaticCount(void) const { return static_cast<int>(itsStaticHelpDataInfos.size()); }
  NFmiDataIdent GetNextSatelChannel(const NFmiDataIdent &theDataIdent, FmiDirection theDir);
  void AddDynamic(const NFmiHelpDataInfo &theInfo);
  void AddStatic(const NFmiHelpDataInfo &theInfo);
  void ResetAllDynamicDataTimeStamps();
  NFmiHelpDataInfo *FindHelpDataInfo(const std::string &theFileNameFilter);
  std::vector<std::string> GetUniqueCustomMenuList(void);
  std::vector<NFmiHelpDataInfo> GetCustomMenuHelpDataList(const std::string &theCustomFolder);
  const std::vector<NFmiHelpDataInfo> &DynamicHelpDataInfos(void) const
  {
    return itsDynamicHelpDataInfos;
  }
  const std::vector<NFmiHelpDataInfo> &StaticHelpDataInfos(void) const
  {
    return itsStaticHelpDataInfos;
  }

  const std::string &CacheDirectory(void) const { return itsCacheDirectory; }
  void CacheDirectory(const std::string &newValue) { itsCacheDirectory = newValue; }
  const std::string &CacheTmpDirectory(void) const { return itsCacheTmpDirectory; }
  void CacheTmpDirectory(const std::string &newValue) { itsCacheTmpDirectory = newValue; }
  const std::string &CacheTmpFileNameFix(void) const { return itsCacheTmpFileNameFix; }
  void CacheTmpFileNameFix(const std::string &newValue) { itsCacheTmpFileNameFix = newValue; }
  bool UseQueryDataCache(void) const { return fUseQueryDataCache; }
  void UseQueryDataCache(bool newvalue) { fUseQueryDataCache = newvalue; }
  bool DoCleanCache(void) const { return fDoCleanCache; }
  void DoCleanCache(bool newValue) { fDoCleanCache = newValue; }
  float CacheFileKeepMaxDays(void) const { return itsCacheFileKeepMaxDays; }
  void CacheFileKeepMaxDays(float newValue) { itsCacheFileKeepMaxDays = newValue; }
  int CacheMaxFilesPerPattern(void) const { return itsCacheMaxFilesPerPattern; }
  void CacheMaxFilesPerPattern(int newValue) { itsCacheMaxFilesPerPattern = newValue; }
  double CacheMediumFileSizeMB(void) const { return itsCacheMediumFileSizeMB; }
  void CacheMediumFileSizeMB(double newValue) { itsCacheMediumFileSizeMB = newValue; }
  double CacheLargeFileSizeMB(void) const { return itsCacheLargeFileSizeMB; }
  void CacheLargeFileSizeMB(double newValue) { itsCacheLargeFileSizeMB = newValue; }
  double CacheMaximumFileSizeMB(void) const { return itsCacheMaximumFileSizeMB; }
  void CacheMaximumFileSizeMB(double newValue) { itsCacheMaximumFileSizeMB = newValue; }
  const std::string &CachePartialDataDirectory(void) const { return itsCachePartialDataDirectory; }
  void CachePartialDataDirectory(const std::string &newValue)
  {
    itsCachePartialDataDirectory = newValue;
  }

 private:
  void InitDataType(const std::string &theBaseKey,
                    std::vector<NFmiHelpDataInfo> &theHelpDataInfos,
                    bool fStaticData);
  void FixCombinedDataPaths(const std::string &absoluteControlBasePath);

  std::vector<NFmiHelpDataInfo> itsDynamicHelpDataInfos;  // tähän tulee jatkuvasti päivitettävät
                                                          // datat kuten havainnot, tutka ja
                                                          // analyysi datat
  std::vector<NFmiHelpDataInfo> itsStaticHelpDataInfos;  // tähän tulee kerran ladattavat jutut
                                                         // kuten maa/meri maskit ja
                                                         // klimatologiset jutut

  // SmartMet voidaan laittaa käyttämään queryData cachetusta, jolloin verkkopalvelimelta
  // luetaan data määrättyyn cache-hakemistoon omalle kovalevylle. Näin voidaan
  // välttää mahdolliset verkko-ongelmat memory-mapattujen dataojen kanssa.
  std::string itsCacheDirectory;     // qdatat kopioidaan haluttaessa tähän hakemistoon
  std::string itsCacheTmpDirectory;  // qdatat kopioidaan ensin tähän hakemistoon tmp-nimellä ja
                                     // lopuksi renametaan
                                     // oiekaan hakemistoon oikealla nimellä
  // HUOM!! että tmp-file rename toimisi ns 'atomisesti', on hakemistojen oltava samalla
  // levypartitiolla!!!
  std::string itsCachePartialDataDirectory;  // combineData-threadin käyttämät datat kopioidaan
                                             // tänne cacheen
  std::string
      itsCacheTmpFileNameFix;  // tämä name fix lisätään tmp tiedosto nimen alkuun ja loppuun
  bool fUseQueryDataCache;     // onko cachetus systeemi päällä vai ei?
  bool fDoCleanCache;          // siivotaanko cachea vai ei
  float itsCacheFileKeepMaxDays;  // kuinka vanhat tiedostot ainakin siivotaan pois (esim. 1.5 on
  // 1.5 päivää eli 36 tuntia) jos luku on <= 0 ei tätä käytetä
  int itsCacheMaxFilesPerPattern;  // kuinka monta tiedostoa maksimissaan pidetään kutakin tiedosto
                                   // patternia kohden, jos luku <= 0, ei tätä käytetä

  // medium ja large koot jakavat cachetettavat qdatat kolmeen osaan:
  // 1. pienet tiedostot 0 <= size < medium
  // 2. keskikokoiset tiedostot medium <= size < large
  // 3. isot tiedostot large <= size < ääretön
  // Jokaiselle kokoluokalle tehdään SmartMetissa oma datan kopiointi threadi, näin isot tiedostot
  // eivät jää blokkaamaan pinempien tiedostojen kopiointia.
  double itsCacheMediumFileSizeMB;
  double itsCacheLargeFileSizeMB;
  double itsCacheMaximumFileSizeMB;  // tätä isompia tiedostoja ei cache suostu siirtämään

  std::string itsBaseNameSpace;
};
