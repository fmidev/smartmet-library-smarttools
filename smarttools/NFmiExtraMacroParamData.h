#pragma once

#include <boost/shared_ptr.hpp>
#include <newbase/NFmiDataMatrix.h>
#include <newbase/NFmiLevelType.h>
#include <newbase/NFmiPoint.h>
#include <newbase/NFmiProducer.h>
#include <newbase/NFmiParam.h>
#include <newbase/NFmiLevel.h>
#include <newbase/NFmiInfoData.h>
#include <set>

class NFmiFastQueryInfo;
class NFmiInfoOrganizer;
class NFmiArea;

enum class MacroParamCalculationType
{
  Normal = 0,  // Normaalit reaaliluvut
  Index =
      1  // indeksi tyyppinen luku (usein kokonaisluku), jota ei saa interpoloida esim. tooltipiss�
};

enum class ReasonForDataRejection
{
  NoGridData,
  NoParameter,
  NoLevel,
  WrongLevelType,
  WrongLevelStructure
};

struct FindWantedInfoData
{
  // foundInfo_ on nullptr, jos mit��n dataa ei l�ytynyt
  boost::shared_ptr<NFmiFastQueryInfo> foundInfo_;
  // Miten haluttu data alunperin kuvattiin smarttools kaavassa
  std::string originalDataDescription_;
  // Mit� erilaisia syit� oli hyl�t� ehdokasdata(t)
  std::set<ReasonForDataRejection> rejectionReasons_;

  FindWantedInfoData(boost::shared_ptr<NFmiFastQueryInfo> &foundInfo,
                     const std::string &originalDataDescription,
                     const std::set<ReasonForDataRejection> &rejectionReasons);
};

// Jos smarttools kieless� halutaan m��ritell� k�ytetty data (resoluution 
// tai fiksatun pohjahilan m��ritykseen), on siihen monia erilaisia tapoja:
// 1. resolution = edited // editoitu data
// 2. resolution = ec_pressure // ec:n painepintadata (my�s hybrid/height kelpaavat)
// 3. resolution = T_ec // Ec:n pintadata, jossa mukana l�mp�tila (my�s par4_prod240 variantit k�yv�t)
// 4. resolution = T_ec_850 // Ec:n painepintadata (leveleiss� my�s lev88 = hybrid, z1000 = height, fl75 = flight-level kelpaavat)
class NFmiDefineWantedData
{
 public:
  // Jos datatyyppi on NFmiInfoData::kEditable, se ohittaa kaiken muun
  NFmiInfoData::Type dataType_ = NFmiInfoData::kNoDataType;
  // Jos prod-id = 0, ei asetettu
  NFmiProducer producer_;
  // Jos par-id = 0, ei asetettu
  NFmiParam param_;
  // Jos level-id = 0, ei asetettu
  std::unique_ptr<NFmiLevel> levelPtr_;
  // Level tyyppi tulee mukaan, jos k�ytetty ec_surface/pressure/hybrid/height m��rityst�
  FmiLevelType levelType_ = kFmiNoLevelType;
  // Talletetaan alkuper�inen data stringi t�h�n
  std::string originalDataString_;

  NFmiDefineWantedData();
  NFmiDefineWantedData(NFmiInfoData::Type dataType, const std::string &originalDataString);
  NFmiDefineWantedData(const NFmiProducer &producer,
                       FmiLevelType levelType,
                       const std::string &originalDataString);
  NFmiDefineWantedData(const NFmiProducer &producer,
                       const NFmiParam &param,
                       const NFmiLevel *level,
                       const std::string &originalDataString);
  NFmiDefineWantedData(const NFmiDefineWantedData &other);
  NFmiDefineWantedData& operator=(const NFmiDefineWantedData &other);

  bool IsEditedData() const;
  bool IsProducerLevelType() const;
  bool IsParamProducerLevel() const;
  const NFmiLevel *UsedLevel() const;
  bool IsInUse() const;
};

// Kun smarttool:ia tulkitaan, siin� saattaa olla osia, joita voi k�ytt�� vain macroParamien
// yhteydess�.
// T�h�n luokkaan talletetaan kaikkea, mit� extra tietoa voi l�yty� annetusta skriptist�.
class NFmiExtraMacroParamData
{
 public:
  NFmiExtraMacroParamData();

  void Clear();
  void FinalizeData(NFmiInfoOrganizer &theInfoOrganizer);
  bool UseSpecialResolution() const;
  static void SetUsedAreaForData(boost::shared_ptr<NFmiFastQueryInfo> &theData,
                                 const NFmiArea *theUsedArea);

  const NFmiDefineWantedData &WantedResolutionData() const { return itsWantedResolutionData; }
  void WantedResolutionData(const NFmiDefineWantedData &wantedData) { itsWantedResolutionData = wantedData; }
  float GivenResolutionInKm() const { return itsGivenResolutionInKm; }
  void GivenResolutionInKm(float newValue) { itsGivenResolutionInKm = newValue; }
  boost::shared_ptr<NFmiFastQueryInfo> ResolutionMacroParamData()
  {
    return itsResolutionMacroParamData;
  }
  static void AdjustValueMatrixToMissing(const boost::shared_ptr<NFmiFastQueryInfo> &theData,
                                         NFmiDataMatrix<float> &theValueMatrix);

  void AddCalculationPoint(const NFmiPoint &latlon) { itsCalculationPoints.push_back(latlon); }
  const std::vector<NFmiPoint> &CalculationPoints() const { return itsCalculationPoints; }
  bool UseCalculationPoints() const { return !itsCalculationPoints.empty(); }
  const std::vector<NFmiProducer> &CalculationPointProducers() const { return itsCalculationPointProducers; }
  bool AddCalculationPointProducer(const NFmiProducer &theProducer);

  float ObservationRadiusInKm() const { return itsObservationRadiusInKm; }
  void ObservationRadiusInKm(float newValue) { itsObservationRadiusInKm = newValue; }
  float ObservationRadiusRelative() const { return itsObservationRadiusRelative; }
  void ObservationRadiusRelative(float newValue) { itsObservationRadiusRelative = newValue; }
  const std::string &SymbolTooltipFile() const { return itsSymbolTooltipFile; }
  void SymbolTooltipFile(const std::string &filePath) { itsSymbolTooltipFile = filePath; }
  const std::string &MacroParamDescription() const { return itsMacroParamDescription; }
  void MacroParamDescription(const std::string &newValue) { itsMacroParamDescription = newValue; }
  MacroParamCalculationType CalculationType() const { return itsCalculationType; }
  void CalculationType(MacroParamCalculationType newValue) { itsCalculationType = newValue; }
  int WorkingThreadCount() const { return itsWorkingThreadCount; }
  void WorkingThreadCount(int newValue) { itsWorkingThreadCount = newValue; }
  const std::string &MacroParamErrorMessage() const { return itsMacroParamErrorMessage; }
  void MacroParamErrorMessage(const std::string &message) { itsMacroParamErrorMessage = message; }
  const std::string &MacroParamWarningMessage() const { return itsMacroParamWarningMessage; }
  void MacroParamWarningMessage(const std::string &message) { itsMacroParamWarningMessage = message; }
  const NFmiDefineWantedData &WantedFixedBaseData() const { return itsWantedFixedBaseData; }
  void WantedFixedBaseData(const NFmiDefineWantedData &newData) { itsWantedFixedBaseData = newData; }
  boost::shared_ptr<NFmiFastQueryInfo> FixedBaseDataInfo() const { return itFixedBaseDataInfo; }
  void FixedBaseDataInfo(boost::shared_ptr<NFmiFastQueryInfo> &info) { itFixedBaseDataInfo = info; }
  void UseDataForResolutionCalculations(const NFmiArea *usedArea,
                                        boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
      const std::string &dataDescriptionForErrorMessage);
  bool IsFixedSpacedOutDataCase() const { return fIsFixedSpacedOutDataCase; }
  void IsFixedSpacedOutDataCase(bool newValue) { fIsFixedSpacedOutDataCase = newValue; }

 private:
  void InitializeResolutionData(const NFmiArea *usedArea,
                                const NFmiPoint &usedResolutionInKm);
  void InitializeDataBasedResolutionData(NFmiInfoOrganizer &theInfoOrganizer);
  FindWantedInfoData FindWantedInfo(
      NFmiInfoOrganizer &theInfoOrganizer, const NFmiDefineWantedData &wantedData);
  void InitializeRelativeObservationRange(NFmiInfoOrganizer &theInfoOrganizer, float usedRangeInKm);
  void AddCalculationPointsFromData(NFmiInfoOrganizer &theInfoOrganizer,
                                    const std::vector<NFmiProducer> &theProducers);
  void InitializeFixedBaseDataInfo(NFmiInfoOrganizer &theInfoOrganizer);

  // T�h�n tulee resolution = xxx m��rityksest� saatava datan tiedot
  NFmiDefineWantedData itsWantedResolutionData;
  // Jos skriptiss� on annettu haluttu laskenta resoluutio tyyliin "resolution = 12.5", talletetaan
  // kyseinen luku t�h�n.
  // Jos ei ole asetettu, on arvo missing.
  float itsGivenResolutionInKm = kFloatMissing;

  // t�h�n lasketaan datasta haluttu resoluutio  makro-parametrien laskuja varten pit�� pit�� yll�
  // yhden
  // hilan kokoista dataa  (yksi aika,param ja level, editoitavan datan hplaceDesc). T�h�n
  // dataan on laskettu haluttu resoluutio t�m�n macroParamin laskujen ajaksi.
  NFmiPoint itsDataBasedResolutionInKm = NFmiPoint::gMissingLatlon;
  boost::shared_ptr<NFmiFastQueryInfo> itsResolutionMacroParamData;

  // CalculationPoint listassa on pisteet jos niit� on annettu "CalculationPoint = lat,lon"
  // -lausekkeilla.
  // T�m� muuttaa macroParamin laskuja niin ett� lopullinen laskettu matriisi nollataan muiden
  // pisteiden
  // kohdalta paitsi n�iden pisteiden l�himmiss� hilapisteiss�.
  std::vector<NFmiPoint> itsCalculationPoints;
  // Jos skriptiss� on annettu CalculationPoint on muodossa "CalculationPoint = synop",
  // talletetaan lausekkeen tuottaja t�nne ja kyseisen datan asemat lis�t��n itsCalculationPoints
  // -listaan.
  std::vector<NFmiProducer> itsCalculationPointProducers;

  // Jos halutaan ett� havaintojen k�ytt�� laskuissa rajoitetaan laskentas�teell�, annetaan se
  // t�h�n kilometreiss�. Jos t�m� on kFloatMissing, k�ytet��n laskuissa havaintoja rajattomasti.
  float itsObservationRadiusInKm = kFloatMissing;
  // T�h�n lasketaan k�ytetyn kartta-alueen mukainen relatiivinen et�isyys (jota k�ytet��n itse
  // laskuissa)
  float itsObservationRadiusRelative = kFloatMissing;
  // Joillekin macroParameilla lasketuille symboleille halutaan antaa arvoon perustuvia selitt�vi�
  // tekstej� tooltipiss�
  std::string itsSymbolTooltipFile;
  // T�h�n voidaan sijoittaa macroParamiin liittyv� yleisselite, joka tulee tooltippiin
  std::string itsMacroParamDescription;
  MacroParamCalculationType itsCalculationType = MacroParamCalculationType::Normal;
  // Kuinka monella s�ikeell� halutaan ett� t�it� tehd��n. Oletusarvo 0 -> oletusm��r�ll�.
  int itsWorkingThreadCount = 0;

  // T�m� ei kuuluvarsinaisesti n�ihin macroParam extra datoihin, mutta tarvitsen kuljettaa
  // mahdolliset smarttool kielen k��nt�j�/ajoaika virheilmoitukset takaisin k�ytt�j�lle,
  // mm. tooltip tekstiin.
  std::string itsMacroParamErrorMessage;
  // Joskus pit�� saada varoittaa k�ytt�j��, ja sellainen varoitus
  // viesti laitetaan macroParamin arvojen yhteyteen tooltippiin.
  std::string itsMacroParamWarningMessage;
  // T�h�n tulee FixedBaseData = xxx m��rityksest� saatava datan tiedot
  NFmiDefineWantedData itsWantedFixedBaseData;
  // Jos k�ytt�j� haluaa kiinnitt�� k�ytetyn laskentahilan johonkin dataan,
  // otetaan kyseinen data t�h�n v�liaikaisesti talteen.
  boost::shared_ptr<NFmiFastQueryInfo> itFixedBaseDataInfo;
  bool fIsFixedSpacedOutDataCase = false;
};
