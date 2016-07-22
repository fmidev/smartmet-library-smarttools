#pragma once

#include "NFmiInfoAreaMask.h"

class NFmiDrawParam;

class _FMI_DLL NFmiInfoAreaMaskOccurrance : public NFmiInfoAreaMaskProbFunc
{
public:
    ~NFmiInfoAreaMaskOccurrance(void);
    NFmiInfoAreaMaskOccurrance(const NFmiCalculationCondition & theOperation,
        Type theMaskType,
        NFmiInfoData::Type theDataType,
        const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
        NFmiAreaMask::FunctionType thePrimaryFunc,
        NFmiAreaMask::FunctionType theSecondaryFunc,
        int theArgumentCount,
        const boost::shared_ptr<NFmiArea> &theCalculationArea,
        bool synopXCase);
    NFmiInfoAreaMaskOccurrance(const NFmiInfoAreaMaskOccurrance &theOther);
    void Initialize(void); // T�t� kutsutaan konstruktorin j�lkeen, t�ss� alustetaan tietyille datoille mm. k�ytetyt aikaindeksit ja k�ytetyt locaaion indeksit
    NFmiAreaMask* Clone(void) const;

    static void SetMultiSourceDataGetterCallback(const std::function<void(checkedVector<boost::shared_ptr<NFmiFastQueryInfo> > &, boost::shared_ptr<NFmiDrawParam> &, const boost::shared_ptr<NFmiArea> &)> &theCallbackFunction);

    // t�t� kaytetaan smarttool-modifierin yhteydess�
    double Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways);

protected:
    bool IsGridData() const;
    bool IsKnownMultiSourceData(); // nyt synop ja salama datat ovat t�ll�isi�
    void DoCalculations(boost::shared_ptr<NFmiFastQueryInfo> &theInfo, const NFmiCalculationParams &theCalculationParams, const NFmiLocation &theLocation, const std::vector<unsigned long> &theLocationIndexCache, int &theOccurranceCountInOut);
    void DoCalculateCurrentLocation(boost::shared_ptr<NFmiFastQueryInfo> &theInfo, const NFmiLocation &theLocation, bool theIsStationLocationsStoredInData, int &theOccurranceCountInOut);
    void InitializeLocationIndexCaches();
    std::vector<unsigned long> CalcLocationIndexCache(boost::shared_ptr<NFmiFastQueryInfo> &theInfo);

    bool fSynopXCase; // halutaanko vain normaali asemat (true), ei liikkuvia asemia (laivat, poijut)
    bool fUseMultiSourceData;
    boost::shared_ptr<NFmiArea> itsCalculationArea; // Joitain laskuja optimoidaan ja niill� l�hdedatasta laskut rajataan laskettavan kartta-alueen sis�lle
    checkedVector<boost::shared_ptr<NFmiFastQueryInfo>> itsInfoVector; // T�h�n laitetaan laskuissa k�ytett�v�t datat, joko se joko on jo emoluokassa oleva itsInfo, tai multisource tapauksissa joukko datoja

    // Jokaiselle k�yt�ss� olevalle datalle lasketaan locationIndex cache, eli kaikki ne pisteet kustakin datasta, 
    // joita k�ytet��n laskuissa. Jos jollekin datalle on tyhj� vektori, lasketaan siit� kaikki. Jos jostain datasta 
    // ei k�ytet� yht��n pistett�, on siihen kuuluvassa vektorissa vain yksi luku (gMissingIndex).
    // T�m� alustetaan Initialize -metodissa.
    std::vector<std::vector<unsigned long>> itsCalculatedLocationIndexies;

    static std::function<void(checkedVector<boost::shared_ptr<NFmiFastQueryInfo> > &, boost::shared_ptr<NFmiDrawParam> &, const boost::shared_ptr<NFmiArea> &)> itsMultiSourceDataGetter;

private:
    NFmiInfoAreaMaskProbFunc & operator=(const NFmiInfoAreaMaskProbFunc & theMask);
};
