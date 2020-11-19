#pragma once

#include <boost/shared_ptr.hpp>
#include <newbase/NFmiDataMatrix.h>
#include <newbase/NFmiLevelType.h>
#include <newbase/NFmiPoint.h>
#include <newbase/NFmiProducer.h>

class NFmiFastQueryInfo;
class NFmiInfoOrganizer;
class NFmiArea;

enum class MacroParamCalculationType
{
  Normal = 0,  // Normaalit reaaliluvut
  Index =
      1  // indeksi tyyppinen luku (usein kokonaisluku), jota ei saa interpoloida esim. tooltipiss�
};

// Kun smarttool:ia tulkitaan, siin� saattaa olla osia, joita voi k�ytt�� vain macroParamien
// yhteydess�.
// T�h�n luokkaan talletetaan kaikkea, mit� extra tietoa voi l�yty� annetusta skriptist�.
class NFmiExtraMacroParamData
{
 public:
  NFmiExtraMacroParamData();

  void FinalizeData(NFmiInfoOrganizer &theInfoOrganizer);
  bool UseSpecialResolution() const;
  static void SetUsedAreaForData(boost::shared_ptr<NFmiFastQueryInfo> &theData,
                                 const NFmiArea *theUsedArea);

  bool UseEditedDataForResolution() const { return fUseEditedDataForResolution; }
  void UseEditedDataForResolution(bool newValue) { fUseEditedDataForResolution = newValue; }
  float GivenResolutionInKm() const { return itsGivenResolutionInKm; }
  void GivenResolutionInKm(float newValue) { itsGivenResolutionInKm = newValue; }
  const NFmiProducer &Producer() const { return itsProducer; }
  void Producer(const NFmiProducer &newValue) { itsProducer = newValue; }
  FmiLevelType LevelType() const { return itsLevelType; }
  void LevelType(FmiLevelType newValue) { itsLevelType = newValue; }
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
  const std::string &MacroParamErrorMessage() const { return itsMacroParamErrorMessage; }
  void MacroParamErrorMessage(const std::string &message) { itsMacroParamErrorMessage = message; }

 private:
  void InitializeResolutionWithEditedData(NFmiInfoOrganizer &theInfoOrganizer);
  void InitializeResolutionData(NFmiInfoOrganizer &theInfoOrganizer, const NFmiPoint &usedResolutionInKm);
  void InitializeDataBasedResolutionData(NFmiInfoOrganizer &theInfoOrganizer,
                                         const NFmiProducer &theProducer,
                                         FmiLevelType theLevelType);
  void UseDataForResolutionCalculations(NFmiInfoOrganizer &theInfoOrganizer,
                                        boost::shared_ptr<NFmiFastQueryInfo> &theInfo);
  void InitializeRelativeObservationRange(NFmiInfoOrganizer &theInfoOrganizer, float usedRangeInKm);
  void AddCalculationPointsFromData(NFmiInfoOrganizer &theInfoOrganizer,
                                    const std::vector<NFmiProducer> &theProducers);

  // Jos skriptiss� on resolution = edited, k�ytet��n editoitua dataa resoluutio laskuissa.
  bool fUseEditedDataForResolution;
  // Jos skriptiss� on annettu haluttu laskenta resoluutio tyyliin "resolution = 12.5", talletetaan
  // kyseinen luku t�h�n.
  // Jos ei ole asetettu, on arvo missing.
  float itsGivenResolutionInKm;
  // Jos skriptiss� on annettu resoluutio data muodossa tyyliin "resolution = hir_pressure",
  // talletetaan lausekkeen tuottaja ja level tyyppi n�ihin kahteen arvoon.
  NFmiProducer itsProducer;   // puuttuva arvo on kun id = 0
  FmiLevelType itsLevelType;  // puuttuva arvo on kFmiNoLevelType eli 0

  // t�h�n lasketaan datasta haluttu resoluutio  makro-parametrien laskuja varten pit�� pit�� yll�
  // yhden
  // hilan kokoista dataa  (yksi aika,param ja level, editoitavan datan hplaceDesc). T�h�n
  // dataan on laskettu haluttu resoluutio t�m�n macroParamin laskujen ajaksi.
  NFmiPoint itsDataBasedResolutionInKm;
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
  float itsObservationRadiusInKm;
  // T�h�n lasketaan k�ytetyn kartta-alueen mukainen relatiivinen et�isyys (jota k�ytet��n itse
  // laskuissa)
  float itsObservationRadiusRelative;
  // Joillekin macroParameilla lasketuille symboleille halutaan antaa arvoon perustuvia selitt�vi�
  // tekstej� tooltipiss�
  std::string itsSymbolTooltipFile;
  // T�h�n voidaan sijoittaa macroParamiin liittyv� yleisselite, joka tulee tooltippiin
  std::string itsMacroParamDescription;
  MacroParamCalculationType itsCalculationType = MacroParamCalculationType::Normal;

  // T�m� ei kuuluvarsinaisesti n�ihin macroParam extra datoihin, mutta tarvitsen kuljettaa
  // mahdolliset smarttool kielen k��nt�j�/ajoaika virheilmoitukset takaisin k�ytt�j�lle,
  // mm. tooltip tekstiin.
  std::string itsMacroParamErrorMessage;
};
