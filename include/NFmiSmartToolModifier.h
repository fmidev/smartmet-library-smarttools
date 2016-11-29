#pragma once
// Luokan on tarkoitus kovata nykyinen NFmiSmartToolModifier-luokka.
// Vanha luokka on rönsyillyt pahasti ja nyt on aika siivota jäljet
// kirjoittamaal toiminnallisuus uusiksi. Tässä tulee mukaan myös
// uuden NFmiInfoOrganizer-luokan käyttö.
// TODO:
// 1. Keksi parempi nimi tai muuta lopuksi NFmiSmartToolModifier-nimiseksi ja tuhoa alkuperäinen
// luokka.
// 2. Siivoa rajapintaa jos pystyt.
// 3. Muuta luokka käyttäään NFmiOwnerInfo-luokkaa, tämä tosin piilossa, koska rajapinnoissa
// käytetään NFmiFastQueryInfo.
// 4. Lisää tuki edellisiin mallidatoihin eli T_HIR[-1] viittaa edelliseen hirlam-ajoon.
// 5. Lisää tuki havaintodatan käytölle (muuta asema-data hilaksi ja käytä laskuissa.)
// 6. Voisi tehdä parserin fiksummin (boost:in tms avulla)
//
// Tämä luokka hoitaa koko smarttool-toiminnan. Sillä on tietokanta,
// tulkki, ja erilaisia maski/operaatio generaattoreita, joiden avulla
// laskut suoritetaan.
//**********************************************************

#include <NFmiParamBag.h>
#include <NFmiDataMatrix.h>
#include <NFmiInfoData.h>
#include <NFmiLevelType.h>
#include <NFmiAreaMask.h>
#include <boost/shared_ptr.hpp>
#include <string>

class NFmiInfoOrganizer;
class NFmiSmartToolIntepreter;
class NFmiTimeDescriptor;
class NFmiAreaMaskInfo;
class NFmiFastQueryInfo;
class NFmiAreaMaskSectionInfo;
class NFmiAreaMask;
class NFmiDataModifier;
class NFmiDataIterator;
class NFmiDataIdent;
class NFmiMetTime;
class NFmiPoint;
class NFmiSmartToolCalculationBlock;
class NFmiMacroParamValue;
class NFmiLevel;
class NFmiSmartToolCalculationSection;
class NFmiSmartToolCalculationSectionInfo;
class NFmiSmartToolCalculation;
class NFmiSmartToolCalculationInfo;
class NFmiSmartToolCalculationBlockInfo;
class NFmiSmartToolCalculationBlockInfoVector;
class MyGrid;
class NFmiThreadCallBacks;
class NFmiExtraMacroParamData;

class NFmiSmartToolCalculationBlockVector
{
 public:
  typedef checkedVector<boost::shared_ptr<NFmiSmartToolCalculationBlock> >::iterator Iterator;

  NFmiSmartToolCalculationBlockVector(void);
  NFmiSmartToolCalculationBlockVector(const NFmiSmartToolCalculationBlockVector &theOther);
  ~NFmiSmartToolCalculationBlockVector(void);
  boost::shared_ptr<NFmiFastQueryInfo> FirstVariableInfo(void);
  void SetTime(const NFmiMetTime &theTime);
  void Calculate(const NFmiCalculationParams &theCalculationParams,
                 NFmiMacroParamValue &theMacroParamValue);
  void Calculate_ver2(const NFmiCalculationParams &theCalculationParams);
  void Add(const boost::shared_ptr<NFmiSmartToolCalculationBlock> &theBlock);
  Iterator Begin(void) { return itsCalculationBlocks.begin(); }
  Iterator End(void) { return itsCalculationBlocks.end(); }
 private:
  // luokka ei omista vektorissa olevia otuksia, Clear pitää kutsua erikseen!!!
  checkedVector<boost::shared_ptr<NFmiSmartToolCalculationBlock> > itsCalculationBlocks;
};

class NFmiSmartToolCalculationBlock
{
 public:
  NFmiSmartToolCalculationBlock(void);
  NFmiSmartToolCalculationBlock(const NFmiSmartToolCalculationBlock &theOther);
  ~NFmiSmartToolCalculationBlock(void);
  boost::shared_ptr<NFmiFastQueryInfo> FirstVariableInfo(void);
  void Time(const NFmiMetTime &theTime);
  void Calculate(const NFmiCalculationParams &theCalculationParams,
                 NFmiMacroParamValue &theMacroParamValue);
  void Calculate_ver2(const NFmiCalculationParams &theCalculationParams,
                      bool fDoMiddlePartOnly = false);

  boost::shared_ptr<NFmiSmartToolCalculationSection> itsFirstCalculationSection;
  boost::shared_ptr<NFmiSmartToolCalculation> itsIfAreaMaskSection;
  boost::shared_ptr<NFmiSmartToolCalculationBlockVector> itsIfCalculationBlocks;
  boost::shared_ptr<NFmiSmartToolCalculation> itsElseIfAreaMaskSection;
  boost::shared_ptr<NFmiSmartToolCalculationBlockVector> itsElseIfCalculationBlocks;
  boost::shared_ptr<NFmiSmartToolCalculationBlockVector> itsElseCalculationBlocks;
  boost::shared_ptr<NFmiSmartToolCalculationSection> itsLastCalculationSection;
};

#ifdef FMI_SUPPORT_STATION_DATA_SMARTTOOL
class NFmiEditMapGeneralDataDoc;
#endif  // FMI_SUPPORT_STATION_DATA_SMARTTOOL

class NFmiSmartToolModifier
{
 public:
  void InitSmartTool(const std::string &theSmartToolText, bool fThisIsMacroParamSkript = false);
  void ModifyData(NFmiTimeDescriptor *theModifiedTimes,
                  bool fSelectedLocationsOnly,
                  bool isMacroParamCalculation,
                  NFmiThreadCallBacks *theThreadCallBacks);
  void ModifyData_ver2(NFmiTimeDescriptor *theModifiedTimes,
                       bool fSelectedLocationsOnly,
                       bool isMacroParamCalculation,
                       NFmiThreadCallBacks *theThreadCallBacks);
  float CalcSmartToolValue(const NFmiMetTime &theTime, const NFmiPoint &theLatlon);
  void CalcCrossSectionSmartToolValues(NFmiDataMatrix<float> &theValues,
                                       checkedVector<float> &thePressures,
                                       checkedVector<NFmiPoint> &theLatlonPoints,
                                       const checkedVector<NFmiMetTime> &thePointTimes);

  NFmiSmartToolModifier(NFmiInfoOrganizer *theInfoOrganizer);
  ~NFmiSmartToolModifier(void);

  bool IsMacroRunnable(void) const { return fMacroRunnable; }
  const std::string &GetErrorText(void) const { return itsErrorText; }
  const std::string &IncludeDirectory(void) const { return itsIncludeDirectory; }
  void IncludeDirectory(const std::string &newValue) { itsIncludeDirectory = newValue; }
  NFmiParamBag ModifiedParams(void);
  const std::string &GetStrippedMacroText(void) const;
  bool IsInterpretedSkriptMacroParam(void);  // kun intepreter on tulkinnut smarttool-tekstin,
                                             // voidaan kysyä, onko kyseinen makro ns.
  boost::shared_ptr<NFmiFastQueryInfo> UsedMacroParamData(void);
  // macroParam-skripti eli sisältääkö se RESULT = ???
  // tapaista tekstiä

  void ModifiedLevel(boost::shared_ptr<NFmiLevel> &theLevel);

 private:
  void ModifyData(NFmiTimeDescriptor *theModifiedTimes,
                  bool fSelectedLocationsOnly,
                  bool isMacroParamCalculation,
                  NFmiMacroParamValue &theMacroParamValue,
                  NFmiThreadCallBacks *theThreadCallBacks);
  float CalcSmartToolValue(NFmiMacroParamValue &theMacroParamValue);
  boost::shared_ptr<NFmiAreaMask> CreatePeekFunctionAreaMask(
      const NFmiAreaMaskInfo &theAreaMaskInfo, bool &fMustUsePressureInterpolation);
  boost::shared_ptr<NFmiAreaMask> CreateMetFunctionAreaMask(const NFmiAreaMaskInfo &theAreaMaskInfo,
                                                            bool &fMustUsePressureInterpolation);
  void SetInfosMaskType(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo);
  void ModifyConditionalData(
      const boost::shared_ptr<NFmiSmartToolCalculationBlock> &theCalculationBlock,
      NFmiMacroParamValue &theMacroParamValue,
      NFmiThreadCallBacks *theThreadCallBacks);
  void ModifyConditionalData_ver2(
      const boost::shared_ptr<NFmiSmartToolCalculationBlock> &theCalculationBlock,
      NFmiThreadCallBacks *theThreadCallBacks);
  void ModifyBlockData(const boost::shared_ptr<NFmiSmartToolCalculationBlock> &theCalculationBlock,
                       NFmiMacroParamValue &theMacroParamValue,
                       NFmiThreadCallBacks *theThreadCallBacks);
  void ModifyBlockData_ver2(
      const boost::shared_ptr<NFmiSmartToolCalculationBlock> &theCalculationBlock,
      NFmiThreadCallBacks *theThreadCallBacks);
  boost::shared_ptr<NFmiSmartToolCalculationBlockVector> CreateCalculationBlockVector(
      const boost::shared_ptr<NFmiSmartToolCalculationBlockInfoVector> &theBlockInfoVector);
  boost::shared_ptr<NFmiSmartToolCalculationBlock> CreateCalculationBlock(
      NFmiSmartToolCalculationBlockInfo &theBlockInfo);
  boost::shared_ptr<NFmiFastQueryInfo> CreateRealScriptVariableInfo(
      const NFmiDataIdent &theDataIdent);
  boost::shared_ptr<NFmiFastQueryInfo> GetScriptVariableInfo(const NFmiDataIdent &theDataIdent);
  void ClearScriptVariableInfos(void);
  boost::shared_ptr<NFmiFastQueryInfo> CreateScriptVariableInfo(const NFmiDataIdent &theDataIdent);
  boost::shared_ptr<NFmiAreaMask> CreateCalculatedAreaMask(const NFmiAreaMaskInfo &theAreaMaskInfo);
  void GetParamValueLimits(const NFmiAreaMaskInfo &theAreaMaskInfo,
                           float *theLowerLimit,
                           float *theUpperLimit,
                           bool *fCheckLimits);
  boost::shared_ptr<NFmiDataModifier> CreateIntegrationFuction(
      const NFmiAreaMaskInfo &theAreaMaskInfo);
  boost::shared_ptr<NFmiDataIterator> CreateIterator(
      const NFmiAreaMaskInfo &theAreaMaskInfo, const boost::shared_ptr<NFmiFastQueryInfo> &theInfo);
  void ModifyData2(boost::shared_ptr<NFmiSmartToolCalculationSection> &theCalculationSection,
                   NFmiMacroParamValue &theMacroParamValue,
                   NFmiThreadCallBacks *theThreadCallBacks);
  void ModifyData2_ver2(boost::shared_ptr<NFmiSmartToolCalculationSection> &theCalculationSection,
                        NFmiThreadCallBacks *theThreadCallBacks);
  boost::shared_ptr<NFmiAreaMask> CreateAreaMask(const NFmiAreaMaskInfo &theInfo);
  boost::shared_ptr<NFmiAreaMask> CreateEndingAreaMask(void);
  boost::shared_ptr<NFmiFastQueryInfo> CreateInfo(const NFmiAreaMaskInfo &theAreaMaskInfo,
                                                  bool &mustUsePressureInterpolation);
  boost::shared_ptr<NFmiFastQueryInfo> CreateInfo(const NFmiAreaMaskInfo &theAreaMaskInfo,
                                                  bool &mustUsePressureInterpolation,
                                                  unsigned long theWantedParamId);
  boost::shared_ptr<NFmiFastQueryInfo> GetPossibleLevelInterpolatedInfo(
      const NFmiAreaMaskInfo &theAreaMaskInfo, bool &mustUsePressureInterpolation);
  void CreateCalculationModifiers(void);
  void CreateFirstCalculationSection(void);
  boost::shared_ptr<NFmiSmartToolCalculationSection> CreateCalculationSection(
      const boost::shared_ptr<NFmiSmartToolCalculationSectionInfo> &theCalcSectionInfo);
  boost::shared_ptr<NFmiSmartToolCalculation> CreateCalculation(
      const boost::shared_ptr<NFmiSmartToolCalculationInfo> &theCalcInfo);
  boost::shared_ptr<NFmiSmartToolCalculation> CreateConditionalSection(
      const boost::shared_ptr<NFmiAreaMaskSectionInfo> &theAreaMaskSectionInfo);
  boost::shared_ptr<NFmiAreaMask> CreateSoundingIndexFunctionAreaMask(
      const NFmiAreaMaskInfo &theAreaMaskInfo);
  boost::shared_ptr<NFmiFastQueryInfo> CreateCopyOfAnalyzeInfo(const NFmiDataIdent &theDataIdent,
                                                               const NFmiLevel *theLevel);
  boost::shared_ptr<NFmiFastQueryInfo> GetWantedAreaMaskData(
      const NFmiAreaMaskInfo &theAreaMaskInfo,
      bool fUseParIdOnly,
      NFmiInfoData::Type theOverRideDataType = NFmiInfoData::kNoDataType,
      FmiLevelType theOverRideLevelType = kFmiNoLevelType);
  boost::shared_ptr<NFmiFastQueryInfo> GetInfoFromOrganizer(const NFmiDataIdent &theIdent,
                                                            const NFmiLevel *theLevel,
                                                            NFmiInfoData::Type theType,
                                                            bool fUseParIdOnly = false,
                                                            bool fLevelData = false,
                                                            int theModelRunIndex = 0);
  void MakeSoundingLevelFix(boost::shared_ptr<NFmiAreaMask> &theAreaMask,
                            const NFmiAreaMaskInfo &theAreaMaskInfo);
  boost::shared_ptr<NFmiAreaMask> CreateInfoVariableMask(const NFmiAreaMaskInfo &theAreaMaskInfo, bool &mustUsePressureInterpolation);
  boost::shared_ptr<NFmiAreaMask> CreateRampFunctionMask(const NFmiAreaMaskInfo &theAreaMaskInfo, bool &mustUsePressureInterpolation);
  boost::shared_ptr<NFmiAreaMask> CreateAreaIntegrationMask(const NFmiAreaMaskInfo &theAreaMaskInfo, NFmiAreaMask::CalculationOperationType maskType, bool &mustUsePressureInterpolation);
  boost::shared_ptr<NFmiAreaMask> CreateStartParenthesisMask(const NFmiAreaMaskInfo &theAreaMaskInfo);
  boost::shared_ptr<NFmiAreaMask> CreateEndParenthesisMask(const NFmiAreaMaskInfo &theAreaMaskInfo);
  boost::shared_ptr<NFmiAreaMask> CreateCommaOperatorMask(const NFmiAreaMaskInfo &theAreaMaskInfo);
  boost::shared_ptr<NFmiAreaMask> CreateMathFunctionStartMask(const NFmiAreaMaskInfo &theAreaMaskInfo);
  boost::shared_ptr<NFmiAreaMask> CreateThreeArgumentFunctionStartMask(const NFmiAreaMaskInfo &theAreaMaskInfo);
  boost::shared_ptr<NFmiAreaMask> CreateVertFunctionStartMask(const NFmiAreaMaskInfo &theAreaMaskInfo, bool &mustUsePressureInterpolation);
  void DoFinalAreaMaskInitializations(boost::shared_ptr<NFmiAreaMask> &areaMask, const NFmiAreaMaskInfo &theAreaMaskInfo, bool &mustUsePressureInterpolation);
  boost::shared_ptr<NFmiAreaMask> CreateOccurrenceMask(const NFmiAreaMaskInfo &theAreaMaskInfo, bool &mustUsePressureInterpolation);
  boost::shared_ptr<NFmiAreaMask> CreateTimeRangeMask(const NFmiAreaMaskInfo &theAreaMaskInfo, bool &mustUsePressureInterpolation);
  boost::shared_ptr<NFmiAreaMask> CreateProbabilityFunctionMask(const NFmiAreaMaskInfo &theAreaMaskInfo, bool &mustUsePressureInterpolation);
  boost::shared_ptr<NFmiAreaMask> CreateClosestObsValueMask(const NFmiAreaMaskInfo &theAreaMaskInfo, bool &mustUsePressureInterpolation);
  boost::shared_ptr<NFmiAreaMask> CreateNormalVertFuncMask(const NFmiAreaMaskInfo &theAreaMaskInfo, bool &mustUsePressureInterpolation);
  boost::shared_ptr<NFmiAreaMask> CreatePeekTimeMask(const NFmiAreaMaskInfo &theAreaMaskInfo, bool &mustUsePressureInterpolation);
  boost::shared_ptr<NFmiAreaMask> CreateVertConditionalMask(const NFmiAreaMaskInfo &theAreaMaskInfo, bool &mustUsePressureInterpolation);

  NFmiInfoOrganizer *itsInfoOrganizer;  // eli database, ei omista ei tuhoa
  boost::shared_ptr<NFmiSmartToolIntepreter> itsSmartToolIntepreter;
  bool fMacroRunnable;
  std::string itsErrorText;
  std::unique_ptr<NFmiExtraMacroParamData> itsExtraMacroParamData; // Tämä alustetaan smarttool-tulkissa (itsSmartToolIntepreter), ja otetaan omistukseen 'suorittajaan'

  bool fModifySelectedLocationsOnly;
  checkedVector<boost::shared_ptr<NFmiFastQueryInfo> >
      itsScriptVariableInfos;       // mahdolliset skripti-muuttujat talletetaan tänne
  std::string itsIncludeDirectory;  // mistä ladataan mahd. include filet

  NFmiTimeDescriptor *itsModifiedTimes;  // ei omista/tuhoa
  bool fMacroParamCalculation;  // tämä tieto tarvitaan scriptVariablejen kanssa, jos true,
  // käytetään pohjana macroParam-infoa, muuten editoitua dataa

  // Nämä muuttujat ovat sitä varten että SumZ ja MinH tyyppisissä funktoissa
  // käytetään parasta mahdollista level-dataa. Eli ensin hybridi ja sitten painepinta dataa.
  bool fHeightFunctionFlag;  // ollaanko tekemässä SumZ tai MinH tyyppisen funktion
                             // calculaatio-otusta
  bool fUseLevelData;  // kun tämä flagi on päällä, käytetään CreateInfo-metodissa hybridi-datoja
                       // jos mahd. ja sitten painepinta datoja.
  bool fDoCrossSectionCalculation;  // kun tämä flagi on päällä, ollaan laskemassa poikkileikkauksia
  // ja silloin level-infot yritetään tehdä ensin hybrididatasta
  // ja sitten painepintadatasta
  int itsCommaCounter;  // tarvitaan laskemaan pilkkuja, kun lasketaan milloin level-dataa pitää
                        // käyttää.
  int itsParethesisCounter;  // kun käytetään esim. Sumz-funktion 2. pilkun jälkeen level-dataa,
                             // pitää laskea sulkujen avulla, milloin funktio loppuu.
  // HUOM! sulkujen lisäksi pitää laskea myös erilaisten funktioiden alut.

  boost::shared_ptr<MyGrid> itsWorkingGrid;  // Tähän talletetaan ns. työskentely gidi, eli missä on
  // työskentely alueen area-määritys ja laskennallinen
  // hila koko.
  boost::shared_ptr<NFmiLevel> itsModifiedLevel;  // Jos ollaan editoimassa level-dataa, tähän on
                                                  // tarkoitus laittaa kulloinenkin muokattava level
                                                  // talteen.
// Tämä asetetaan nyt vain NFmiSmartToolUtil::ModifyData-funktiosta, jossa käydään levelit läpi.

#ifdef FMI_SUPPORT_STATION_DATA_SMARTTOOL
 public:
  void SetGeneralDoc(NFmiEditMapGeneralDataDoc *theDoc);

 private:
  NFmiEditMapGeneralDataDoc *itsDoc;
#endif  // FMI_SUPPORT_STATION_DATA_SMARTTOOL
};
