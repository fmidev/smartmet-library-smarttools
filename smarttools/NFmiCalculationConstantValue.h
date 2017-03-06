#pragma once
//**********************************************************
// C++ Class Name : NFmiCalculationConstantValue
// ---------------------------------------------------
//  Author         : pietarin
//  Creation Date  : 9.11. 2010
//
//**********************************************************

#include <newbase/NFmiAreaMaskImpl.h>
#include <newbase/NFmiInfoAreaMask.h>
#include <boost/shared_ptr.hpp>

#ifdef FMI_SUPPORT_STATION_DATA_SMARTTOOL

#ifdef _MSC_VER
#pragma warning( \
    disable : 4244 4267 4512)  // boost:in thread kirjastosta tulee ikävästi 4244 varoituksia
#endif
#include <boost/thread.hpp>
#ifdef _MSC_VER
#pragma warning(default : 4244 4267 4512)  // laitetaan 4244 takaisin päälle, koska se on tärkeä
                                           // (esim. double -> int auto castaus varoitus)
#endif

#endif  // FMI_SUPPORT_STATION_DATA_SMARTTOOL

class NFmiDataModifier;
class NFmiDataIterator;
class NFmiFastQueryInfo;

// Tämä luokka antaa laskuihin mukaan ajan mukana muuttuvat kertoimet. Aikasarja-editorista huom!
class NFmiCalculationConstantValue : public NFmiAreaMaskImpl
{
 public:
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);

  NFmiCalculationConstantValue(double value = 0);
  ~NFmiCalculationConstantValue();
  NFmiCalculationConstantValue(const NFmiCalculationConstantValue &theOther);
  NFmiAreaMask *Clone(void) const;

  void SetValue(double value) { itsValue = value; }
  double GetValue(void) const { return itsValue; }
 private:
  double itsValue;
};

// Tämä luokka antaa laskuihin mukaan korkeus laskuissa (esim. SumZ tai MinH tyyliset funktiot)
// käytetyn kerroksen paksuuden. Korkeus laskut jakavat laskuissaan korkeus akselin vaihtuvan
// paksuisiin kerroksiin siten että alhaalla on tarkempi resoluutio kuin ylhäällä. Tätä voi käyttää
// vain ed. mainittujen korkeus laskujen yhteydessä.
class NFmiCalculationDeltaZValue : public NFmiAreaMaskImpl
{
 public:
  double Value(const NFmiCalculationParams & /* theCalculationParams */,
               bool /* fUseTimeInterpolationAlways */)
  {
    return NFmiCalculationDeltaZValue::itsHeightValue;
  }

  NFmiCalculationDeltaZValue(void);
  ~NFmiCalculationDeltaZValue(){};
  NFmiCalculationDeltaZValue(const NFmiCalculationDeltaZValue &theOther);
  NFmiAreaMask *Clone(void) const;

  // tätä funktiota käyttämällä asetetaan korkeus 'siivun' paksuus. HUOM! se on staattinen kuten on
  // itsHeightValue-dataosakin, joten se tulee kaikille 'DeltaZ':oille yhteiseksi arvoksi.
  static void SetDeltaZValue(double value) { itsHeightValue = value; }
 private:
  static double itsHeightValue;
};

// Tämä on taas pika suunnittelua/toteutusta, mutta laitoin tälläisen luokan
// jonka avulla hoidetaan smarttools laskuissa mm. sulut ja lasku- ja vertailu operaattorit.
// Kun laitoin uuden smarttool systeemin laskemaan rekursiivisesti laskut ja
// kaikki (muuttujat, operatorit, sulut jne.) pitää laittaa samaan putkeen.
class NFmiCalculationSpecialCase : public NFmiAreaMaskImpl
{
 public:
  NFmiCalculationSpecialCase(NFmiAreaMask::CalculationOperator theValue = NotOperation);
  ~NFmiCalculationSpecialCase(void){};
  NFmiCalculationSpecialCase(const NFmiCalculationSpecialCase &theOther);
  NFmiAreaMask *Clone(void) const;

 private:
};

// Ramppifunktioiden laskut.
class NFmiCalculationRampFuction : public NFmiInfoAreaMask
{
 public:
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);

  NFmiCalculationRampFuction(const NFmiCalculationCondition &theOperation,
                             Type theMaskType,
                             NFmiInfoData::Type theDataType,
                             boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                             BinaryOperator thePostBinaryOperator = kNoValue);
  ~NFmiCalculationRampFuction(void);
  NFmiCalculationRampFuction(const NFmiCalculationRampFuction &theOther);
  NFmiAreaMask *Clone(void) const;

 private:
};

// Ramppifunktioiden laskut AreaMask:ien avulla (mm. lat, lon ja elevationangle tapaukset).
class NFmiCalculationRampFuctionWithAreaMask : public NFmiAreaMaskImpl
{
 public:
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);

  NFmiCalculationRampFuctionWithAreaMask(const NFmiCalculationCondition &theOperation,
                                         Type theMaskType,
                                         NFmiInfoData::Type theDataType,
                                         boost::shared_ptr<NFmiAreaMask> &theAreaMask,
                                         BinaryOperator thePostBinaryOperator = kNoValue);
  ~NFmiCalculationRampFuctionWithAreaMask(void);
  NFmiCalculationRampFuctionWithAreaMask(const NFmiCalculationRampFuctionWithAreaMask &theOther);
  NFmiAreaMask *Clone(void) const;

 private:
  boost::shared_ptr<NFmiAreaMask> itsAreaMask;
  bool fIsTimeIntepolationNeededInValue;  // erikois optimointia Value-metodin ja Time-metodin
                                          // käytössä
};

// Avg, min ja max funktioiden laskut.
class NFmiCalculationIntegrationFuction : public NFmiInfoAreaMask
{
 public:
  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);

  NFmiCalculationIntegrationFuction(boost::shared_ptr<NFmiDataIterator> &theDataIterator,
                                    boost::shared_ptr<NFmiDataModifier> &theDataModifier,
                                    Type theMaskType,
                                    NFmiInfoData::Type theDataType,
                                    boost::shared_ptr<NFmiFastQueryInfo> &theInfo);
  ~NFmiCalculationIntegrationFuction(void);

 private:
  NFmiCalculationIntegrationFuction(const NFmiCalculationIntegrationFuction &theOther);

  boost::shared_ptr<NFmiDataModifier> itsDataModifier;
  boost::shared_ptr<NFmiDataIterator> itsDataIterator;
};

// tämä luokka on ikävästi riippuvainen
// 1. NFmiEditMapGeneralDataDoc -luokasta (smartmetbizcode\generaldoc -moduuli)
// 2. NFmiStationView -luokasta (smartmetbizcode\stationviews -moduuli)
// 3. AVS ToolMaster-kirjastosta (joka tekee varsinaisen griddauksen)
// Siksi se on mukana vain optionaalisesti, eli määritä FMI_SUPPORT_STATION_DATA_SMARTTOOL
// jos haluat tämän käyttöön.
#ifdef FMI_SUPPORT_STATION_DATA_SMARTTOOL

class NFmiEditMapGeneralDataDoc;

// tämä maski osaa laskea halutulle asemadatalle hilatut arvot halutulle alueelle
// Jos maskin itsInfo on station-dataa, sen laskut tehdään toisella tavalla kuin 'normaalin'
// hila-datan kanssa
// 1. Se pitää initilisoida kerran joka erillistä aikaa kohden eli lasketaan matriisiin valmiiksi
// kaikki arvot kerralla
// 2. Kun maskin arvoja pyydetään Value-metodissa, ne saadaan valmiiksi lasketusta taulukosta (aika
// initialisointi voi tapahtua myös siellä)
class NFmiStation2GridMask : public NFmiInfoAreaMask
{
  typedef std::map<NFmiMetTime, NFmiDataMatrix<float> > DataCache;

 public:
  NFmiStation2GridMask(Type theMaskType,
                       NFmiInfoData::Type theDataType,
                       boost::shared_ptr<NFmiFastQueryInfo> &theInfo);
  ~NFmiStation2GridMask(void);
  NFmiStation2GridMask(const NFmiStation2GridMask &theOther);
  NFmiAreaMask *Clone(void) const;

  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);
  void SetGriddingHelpers(NFmiArea *theArea,
                          NFmiEditMapGeneralDataDoc *theDoc,
                          const NFmiPoint &theStation2GridSize,
                          float theObservationRadiusRelative);

 private:
  void DoGriddingCheck(const NFmiCalculationParams &theCalculationParams);

  //  NFmiDataMatrix<float> itsGriddedStationData; // tässä on asemadatasta lasketut hilatut arvot
  boost::shared_ptr<DataCache> itsGriddedStationData;   // Tämä jaetaan kaikkien kopioiden kesken,
                                                        // jotta multi-thread -koodi saa jaettua
                                                        // työtä
  NFmiDataMatrix<float> *itsCurrentGriddedStationData;  // tähän on laitettu se matriisi, joka
                                                        // sisältää halutun ajan asemadatasta
                                                        // lasketut hilatut arvot
  NFmiMetTime itsLastCalculatedTime;  // tälle ajanhetkelle on station data laskettu (tai puuttuva
                                      // aika), mutta onko se sama kuin itsTime, jos ei ole, pitää
                                      // laskea juuri tälle ajalle

  // Näille muuttujille pitää asettaa arvot erillisellä SetGridHelpers-funktiolla
  boost::shared_ptr<NFmiArea> itsAreaPtr;  // omistaa ja tuhoaa!!
  NFmiEditMapGeneralDataDoc *itsDoc;
  NFmiPoint itsStation2GridSize;  // tämän kokoiseen hilaan asema data lasketaan
                                  // (itsGriddedStationData -koko)
  // Normaalisti havainto laskuissa ei rajoiteta käytettyjä havaintoja etäisyyden perusteellä.
  // Jos tähän annetaan jotain kFloatMissing:istä poikkeavaa, niin silloin rajoitetaan.
  float itsObservationRadiusRelative;

  // Kun itsCurrentGriddedStationData -muuttujaa lasketaan tai asetetaan, sen saa tehdä kullekin
  // ajalle vain kerran. Tämä lukko systeemi takaa sen.
  typedef boost::shared_mutex MutexType;
  typedef boost::shared_lock<MutexType>
      ReadLock;  // Read-lockia ei oikeasti tarvita, mutta laitan sen tähän, jos joskus tarvitaankin
  typedef boost::unique_lock<MutexType> WriteLock;
  boost::shared_ptr<MutexType> itsCacheMutex;  // TÄMÄ jaetaan kaikkien kopioiden kesken, jotta
                                               // multi-thread -koodi saa jaettua työtä
};

// NFmiNearestObsValue2GridMask -luokka laskee havainto datasta sellaisen
// hilan, mihin sijoitetaan kuhunkin hilapisteeseen vain sitä lähimmän aseman
// arvon (oli se puuttuvaa tai ei). Eli kaikkiin hilapisteisiin ei tule arvoa.
// Tämän avulla on tarkoitus voida tehdä 'asemapiste' -laskuja ja visualisoida
// niitä kartalla vain teksti muodossa. Tällöin teksti tulee näkyviin kartalla
// lähellä kunkin aseman omaa pistettä ja muut hilapisteet ovat puuttuvaa joten
// siihen ei tule näkyviin mitään.
class NFmiNearestObsValue2GridMask : public NFmiInfoAreaMask
{
  typedef std::map<NFmiMetTime, NFmiDataMatrix<float> > DataCache;

 public:
  NFmiNearestObsValue2GridMask(Type theMaskType,
                               NFmiInfoData::Type theDataType,
                               boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                               int theArgumentCount);
  ~NFmiNearestObsValue2GridMask(void);
  NFmiNearestObsValue2GridMask(const NFmiNearestObsValue2GridMask &theOther);
  NFmiAreaMask *Clone(void) const;

  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);
  void SetGriddingHelpers(NFmiArea *theArea,
                          NFmiEditMapGeneralDataDoc *theDoc,
                          const NFmiPoint &theResultGridSize);
  void SetArguments(std::vector<float> &theArgumentVector);

 private:
  void DoNearestValueGriddingCheck(const NFmiCalculationParams &theCalculationParams);

  boost::shared_ptr<DataCache> itsNearestObsValuesData;  // Tämä jaetaan kaikkien kopioiden kesken,
                                                         // jotta multi-thread -koodi saa jaettua
                                                         // työtä
  NFmiDataMatrix<float> *itsCurrentNearestObsValuesData;  // tähän on laitettu se matriisi, joka
                                                          // sisältää halutun ajan asemadatasta
                                                          // lasketut hilatut arvot
  NFmiMetTime itsLastCalculatedTime;  // tälle ajanhetkelle on station data laskettu (tai puuttuva
                                      // aika), mutta onko se sama kuin itsTime, jos ei ole, pitää
                                      // laskea juuri tälle ajalle

  // Näille muuttujille pitää asettaa arvot erillisellä SetGridHelpers-funktiolla
  boost::shared_ptr<NFmiArea> itsAreaPtr;  // omistaa ja tuhoaa!!
  NFmiEditMapGeneralDataDoc *itsDoc;
  NFmiPoint itsResultGridSize;  // tämän kokoiseen hilaan asema data lasketaan
                                // (itsNearestObsValuesData -hilakoko)

  std::vector<float> itsArgumentVector;  // tähän lasketaan lennossa laskuissa tarvittavat
                                         // argumentit (1. aikahyppy)

  // Kun itsCurrentNearestObsValuesData -muuttujaa lasketaan tai asetetaan, sen saa tehdä kullekin
  // ajalle vain kerran. Tämä lukko systeemi takaa sen.
  typedef boost::shared_mutex MutexType;
  typedef boost::shared_lock<MutexType>
      ReadLock;  // Read-lockia ei oikeasti tarvita, mutta laitan sen tähän, jos joskus tarvitaankin
  typedef boost::unique_lock<MutexType> WriteLock;
  boost::shared_ptr<MutexType> itsCacheMutex;  // TÄMÄ jaetaan kaikkien kopioiden kesken, jotta
                                               // multi-thread -koodi saa jaettua työtä
};

#endif  // FMI_SUPPORT_STATION_DATA_SMARTTOOL

// NFmiPeekTimeMask -luokka 'kurkkaa' datasta annetun tunti offsetin verran ajassa eteen/taaksepäin.
class NFmiPeekTimeMask : public NFmiInfoAreaMask
{
 public:
  NFmiPeekTimeMask(Type theMaskType,
                   NFmiInfoData::Type theDataType,
                   boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
                   int theArgumentCount);
  ~NFmiPeekTimeMask(void);
  NFmiPeekTimeMask(const NFmiPeekTimeMask &theOther);
  NFmiAreaMask *Clone(void) const;

  double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);
  void SetArguments(std::vector<float> &theArgumentVector);

 private:
  long itsTimeOffsetInMinutes;  // kuinka paljon kurkataan ajassa eteen/taakse
};
