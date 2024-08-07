#pragma once
//**********************************************************
// C++ Class Name : NFmiCalculationConstantValue
// ---------------------------------------------------
//  Author         : pietarin
//  Creation Date  : 9.11. 2010
//
//**********************************************************

#include <memory>
#include <newbase/NFmiAreaMaskImpl.h>
#include <newbase/NFmiInfoAreaMask.h>

class NFmiDataModifier;
class NFmiDataIterator;
class NFmiFastQueryInfo;

// Tämä luokka antaa laskuihin mukaan ajan mukana muuttuvat kertoimet. Aikasarja-editorista huom!
class NFmiCalculationConstantValue : public NFmiAreaMaskImpl
{
 public:
  double Value(const NFmiCalculationParams &theCalculationParams,
               bool fUseTimeInterpolationAlways) override;

  NFmiCalculationConstantValue(double value = 0);
  ~NFmiCalculationConstantValue();
  NFmiCalculationConstantValue(const NFmiCalculationConstantValue &theOther);
  NFmiAreaMask *Clone() const override;

  void SetValue(double value) { itsValue = value; }
  double GetValue() const { return itsValue; }

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
               bool /* fUseTimeInterpolationAlways */) override
  {
    return NFmiCalculationDeltaZValue::itsHeightValue;
  }

  NFmiCalculationDeltaZValue();
  ~NFmiCalculationDeltaZValue(){};
  NFmiCalculationDeltaZValue(const NFmiCalculationDeltaZValue &theOther);
  NFmiAreaMask *Clone() const override;

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
  ~NFmiCalculationSpecialCase(){};
  NFmiCalculationSpecialCase(const NFmiCalculationSpecialCase &theOther);
  NFmiAreaMask *Clone() const override;

 private:
};

// Ramppifunktioiden laskut.
class NFmiCalculationRampFuction : public NFmiInfoAreaMask
{
 public:
  double Value(const NFmiCalculationParams &theCalculationParams,
               bool fUseTimeInterpolationAlways) override;

  NFmiCalculationRampFuction(const NFmiCalculationCondition &theOperation,
                             Type theMaskType,
                             NFmiInfoData::Type theDataType,
                             std::shared_ptr<NFmiFastQueryInfo> &theInfo,
                             unsigned long thePossibleMetaParamId,
                             BinaryOperator thePostBinaryOperator);
  ~NFmiCalculationRampFuction();
  NFmiCalculationRampFuction(const NFmiCalculationRampFuction &theOther);
  NFmiAreaMask *Clone() const override;

 private:
};

// Ramppifunktioiden laskut AreaMask:ien avulla (mm. lat, lon ja elevationangle tapaukset).
class NFmiCalculationRampFuctionWithAreaMask : public NFmiAreaMaskImpl
{
 public:
  double Value(const NFmiCalculationParams &theCalculationParams,
               bool fUseTimeInterpolationAlways) override;

  NFmiCalculationRampFuctionWithAreaMask(const NFmiCalculationCondition &theOperation,
                                         Type theMaskType,
                                         NFmiInfoData::Type theDataType,
                                         std::shared_ptr<NFmiAreaMask> &theAreaMask,
                                         BinaryOperator thePostBinaryOperator);
  ~NFmiCalculationRampFuctionWithAreaMask();
  NFmiCalculationRampFuctionWithAreaMask(const NFmiCalculationRampFuctionWithAreaMask &theOther);
  NFmiAreaMask *Clone() const override;

 private:
  std::shared_ptr<NFmiAreaMask> itsAreaMask;
  bool fIsTimeIntepolationNeededInValue;  // erikois optimointia Value-metodin ja Time-metodin
                                          // käytössä
};

// Avg, min ja max funktioiden laskut.
class NFmiCalculationIntegrationFuction : public NFmiInfoAreaMask
{
 public:
  double Value(const NFmiCalculationParams &theCalculationParams,
               bool fUseTimeInterpolationAlways) override;

  NFmiCalculationIntegrationFuction(std::shared_ptr<NFmiDataIterator> &theDataIterator,
                                    std::shared_ptr<NFmiDataModifier> &theDataModifier,
                                    Type theMaskType,
                                    NFmiInfoData::Type theDataType,
                                    std::shared_ptr<NFmiFastQueryInfo> &theInfo,
                                    unsigned long thePossibleMetaParamId);
  ~NFmiCalculationIntegrationFuction();

 private:
  NFmiCalculationIntegrationFuction(const NFmiCalculationIntegrationFuction &theOther);

  std::shared_ptr<NFmiDataModifier> itsDataModifier;
  std::shared_ptr<NFmiDataIterator> itsDataIterator;
};
