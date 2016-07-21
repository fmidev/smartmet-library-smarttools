
#include "NFmiInfoAreaMaskOccurrance.h"
#include "NFmiFastQueryInfo.h"
#include "NFmiDrawParam.h"
#include "NFmiProducerName.h"

// **********************************************************
// *****    NFmiInfoAreaMaskOccurrance  *********************
// **********************************************************

std::function<void(checkedVector<boost::shared_ptr<NFmiFastQueryInfo> > &, boost::shared_ptr<NFmiDrawParam> &, const boost::shared_ptr<NFmiArea> &)> NFmiInfoAreaMaskOccurrance::itsMultiSourceDataGetter; // Alustetaan tyhj�ksi ensin

NFmiInfoAreaMaskOccurrance::~NFmiInfoAreaMaskOccurrance(void)
{}

NFmiInfoAreaMaskOccurrance::NFmiInfoAreaMaskOccurrance(const NFmiCalculationCondition & theOperation,
    Type theMaskType,
    NFmiInfoData::Type theDataType,
    const boost::shared_ptr<NFmiFastQueryInfo> &theInfo,
    NFmiAreaMask::FunctionType thePrimaryFunc,
    NFmiAreaMask::FunctionType theSecondaryFunc,
    int theArgumentCount)
    :NFmiInfoAreaMaskProbFunc(theOperation, theMaskType, theDataType, theInfo, thePrimaryFunc, theSecondaryFunc, theArgumentCount)
    , fStationLocationsStoredInData(false)
    , fUseMultiSourceData(false)
{
    fStationLocationsStoredInData = theInfo->HasLatlonInfoInData();
    fUseMultiSourceData = IsKnownMultiSourceData();
}

NFmiInfoAreaMaskOccurrance::NFmiInfoAreaMaskOccurrance(const NFmiInfoAreaMaskOccurrance &theOther)
    : NFmiInfoAreaMaskProbFunc(theOther)
    , fStationLocationsStoredInData(theOther.fStationLocationsStoredInData)
    , fUseMultiSourceData(theOther.fUseMultiSourceData)
{
}

NFmiAreaMask* NFmiInfoAreaMaskOccurrance::Clone(void) const
{
    return new NFmiInfoAreaMaskOccurrance(*this);
}

void NFmiInfoAreaMaskOccurrance::SetMultiSourceDataGetterCallback(const std::function<void(checkedVector<boost::shared_ptr<NFmiFastQueryInfo> > &, boost::shared_ptr<NFmiDrawParam> &, const boost::shared_ptr<NFmiArea> &)> &theCallbackFunction)
{
    NFmiInfoAreaMaskOccurrance::itsMultiSourceDataGetter = theCallbackFunction;
}

// Nyt synop ja salama datat ovat t�ll�isi�. T�m� on yritys tehd� v�h�n optimointia muutenkin jo pirun raskaaseen koodiin.
// HUOM! T�m� on riippuvainen NFmiEditMapGeneralDataDoc::MakeDrawedInfoVectorForMapView -metodin erikoistapauksista.
bool NFmiInfoAreaMaskOccurrance::IsKnownMultiSourceData()
{
    if(itsInfo)
    {
        if(itsInfo->DataType() == NFmiInfoData::kFlashData)
            return true;
        // HUOM! kaikkien synop datojen k�ytt� on aivan liian hidasta, k�ytet��n vain prim��ri synop dataa laskuissa.
        //if(itsInfo->Producer()->GetIdent() == kFmiSYNOP)
        //    return true;
    }
    return false;
}

// t�t� kaytetaan smarttool-modifierin yhteydess�
double NFmiInfoAreaMaskOccurrance::Value(const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways)
{
    // N�m� occurance tetit tehd��n erilailla riippuen onko kyse hila vai asema datasta
    if(IsGridData())
    {
        NFmiInfoAreaMaskProbFunc::Value(theCalculationParams, fUseTimeInterpolationAlways);
        return itsConditionFullfilledGridPointCount; // T�h�n on laskettu hiladatan tapauksessa emoluokan Value -metodissa halutun ehdon t�yttymiset (tai puuttuva arvo)
    }
    else
    {
        // Asemadatalle tehd��n omat laskut. T�t� ei ole toteutettu emoluokan laskuissa ollenkaan, koska en oikein n�hnyt hy�dylliseksi/oikeelliseksi laskea asemadatalle todenn�k�isyyksi�.
        // Mutta esiintymislukum��r� on ihan ok laskea (mm. salamadatasta salamoiden esiintymiset jne.)

        InitializeFromArguments();
        NFmiLocation location(theCalculationParams.itsLatlon);
        int occurranceCount = 0;

        if(fUseMultiSourceData && itsMultiSourceDataGetter)
        {
            boost::shared_ptr<NFmiArea> dummyAreaPtr; // t�ll� ei tehd� mit��n haussa
            boost::shared_ptr<NFmiDrawParam> drawParam(new NFmiDrawParam(itsInfo->Param(), *itsInfo->Level(), 0, itsInfo->DataType()));
            checkedVector<boost::shared_ptr<NFmiFastQueryInfo>> infos;
            itsMultiSourceDataGetter(infos, drawParam, dummyAreaPtr);
            for(size_t i = 0; i < infos.size(); i++)
                DoCalculations(infos[i], theCalculationParams, location, occurranceCount);
        }
        else
        {
            DoCalculations(itsInfo, theCalculationParams, location, occurranceCount);
        }

        return occurranceCount;
    }
}

void NFmiInfoAreaMaskOccurrance::DoCalculations(boost::shared_ptr<NFmiFastQueryInfo> &theInfo, const NFmiCalculationParams &theCalculationParams, const NFmiLocation &theLocation, int &theOccurranceCountInOut)
{
    fStationLocationsStoredInData = theInfo->HasLatlonInfoInData(); // Eri multi source datoille voi olla erilaiset lokaatio jutut (tosessa synop datassa vakio maaasema, toisessa liikkuva laiva)
    // Lasketaan aikaloopitus rajat
    unsigned long origTimeIndex = theInfo->TimeIndex(); // Otetaan aikaindeksi talteen, jotta se voidaan lopuksi palauttaa takaisin
    unsigned long startTimeIndex = origTimeIndex;
    unsigned long endTimeIndex = origTimeIndex;
    bool doSpecialCalculationDummy = false;

    NFmiMetTime interpolationTimeDummy = NFmiInfoAreaMaskProbFunc::CalcTimeLoopLimits(theInfo, theCalculationParams, itsStartTimeOffsetInHours, itsEndTimeOffsetInHours, &startTimeIndex, &endTimeIndex, &doSpecialCalculationDummy, false);

    for(unsigned long timeIndex = startTimeIndex; timeIndex <= endTimeIndex; timeIndex++)
    {
        theInfo->TimeIndex(timeIndex); // K�yd��n l�pi aikajakso datan originaali aikaresoluutiossa

        for(theInfo->ResetLocation(); theInfo->NextLocation(); ) // K�yd��n l�pi kaikki asemat (miten k�yd��n l�pi eri synop datat????)
        {
            // Tarkastetaan jokainen piste erikseen, onko se halutun s�teisen ympyr�n sis�ll�
            double distanceInKM = theLocation.Distance(fStationLocationsStoredInData ? theInfo->GetLatlonFromData() : theInfo->LatLon()) * 0.001;
            if(distanceInKM > itsSearchRangeInKM)
                continue; // kyseinen piste oli ympyr�n ulkopuolella

            float value = theInfo->FloatValue();
            if(value != kFloatMissing)
            {
                if(CheckProbabilityCondition(value))
                    theOccurranceCountInOut++;
            }
        }
        if(startTimeIndex == gMissingIndex || endTimeIndex == gMissingIndex || startTimeIndex > endTimeIndex)
            break; // time-looppi voi menn� gMissingIndex => gMissingIndex, jolloin ++-operaatio veisi luvun takaisin 0:aan, siksi t�m� ehto ja loopin breikki
    }
    theInfo->TimeIndex(origTimeIndex);
}

bool NFmiInfoAreaMaskOccurrance::IsGridData() const
{
    if(itsInfo && itsInfo->Grid())
        return true;
    else
        return false;
}

// **********************************************************
// *****    NFmiInfoAreaMaskOccurrance  *********************
// **********************************************************
