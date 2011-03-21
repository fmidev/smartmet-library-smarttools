// ======================================================================
/*!
 * \file NFmiSoundingIndexCalculator.cpp
 *
 * T�m� luokka laskee erilaisia luotausi ndeksej� annettujen querinfojen avulla.
 * Mm. CAPE, CIN, LCL, BulkShear StormRelatedHellicity jne.
 */
// ======================================================================

#include "NFmiSoundingIndexCalculator.h"
#include "NFmiFastQueryInfo.h"
#include "NFmiDrawParam.h"
#include "NFmiGrid.h"
#include "NFmiInfoOrganizer.h"
#include "NFmiSoundingData.h"
#include "NFmiSoundingDataOpt1.h"
#include "NFmiSoundingFunctions.h"
#include "NFmiValueString.h"
#include "NFmiQueryData.h"
#include "NFmiQueryDataUtil.h"

#ifndef BOOST_DISABLE_THREADS

#ifdef _MSC_VER
#pragma warning (disable : 4244) // boost:in thread kirjastosta tulee ik�v�sti 4244 varoituksia
#endif
#include <boost/thread.hpp>
#ifdef _MSC_VER
#pragma warning (default : 4244) // laitetaan 4244 takaisin p��lle, koska se on t�rke� (esim. double -> int auto castaus varoitus)
#endif

#endif // BOOST_DISABLE_THREADS

using namespace NFmiSoundingFunctions;

bool NFmiSoundingIndexCalculator::FillSoundingData(boost::shared_ptr<NFmiFastQueryInfo> &theInfo, NFmiSoundingData &theSoundingData, const NFmiMetTime &theTime, const NFmiLocation &theLocation, boost::shared_ptr<NFmiFastQueryInfo> &theGroundDataInfo)
{
	if(theInfo)
	{
		if(theInfo->IsGrid())
			return theSoundingData.FillSoundingData(theInfo, theTime, theInfo->OriginTime(), theLocation.GetLocation(), theLocation.GetName(), theGroundDataInfo);
		else
			return theSoundingData.FillSoundingData(theInfo, theTime, theInfo->OriginTime(), theLocation);
	}
	return false;
}

bool NFmiSoundingIndexCalculator::FillSoundingDataOpt1(boost::shared_ptr<NFmiFastQueryInfo> &theInfo, NFmiSoundingDataOpt1 &theSoundingData, const NFmiMetTime &theTime, const NFmiLocation &theLocation, boost::shared_ptr<NFmiFastQueryInfo> &theGroundDataInfo)
{
	if(theInfo)
	{
		if(theInfo->IsGrid())
			return theSoundingData.FillSoundingData(theInfo, theTime, theInfo->OriginTime(), theLocation.GetLocation(), theLocation.GetName(), theGroundDataInfo);
		else
			return theSoundingData.FillSoundingData(theInfo, theTime, theInfo->OriginTime(), theLocation);
	}
	return false;
}

static bool FillSoundingData(boost::shared_ptr<NFmiFastQueryInfo> &theInfo, NFmiSoundingData &theSoundingData, const NFmiMetTime &theTime, const NFmiPoint &theLatlon)
{
	static NFmiString bsName("bsname");
	if(theInfo)
	{
		if(theInfo->IsGrid())
			return theSoundingData.FillSoundingData(theInfo, theTime, theInfo->OriginTime(), theLatlon, bsName, boost::shared_ptr<NFmiFastQueryInfo>());
	}
	return false;
}

static bool FillSoundingDataOpt1(boost::shared_ptr<NFmiFastQueryInfo> &theInfo, NFmiSoundingDataOpt1 &theSoundingDataOpt1, const NFmiMetTime &theTime, const NFmiPoint &theLatlon, bool useFastFill)
{
	static NFmiString bsName("bsname");
	if(theInfo)
	{
		if(theInfo->IsGrid())
			return theSoundingDataOpt1.FillSoundingData(theInfo, theTime, theInfo->OriginTime(), theLatlon, bsName, boost::shared_ptr<NFmiFastQueryInfo>(), useFastFill);
	}
	return false;
}

bool NFmiSoundingIndexCalculator::IsSurfaceBasedSoundingIndex(FmiSoundingParameters theSoundingParameter)
{
	if(theSoundingParameter >= kSoundingParLCLSurBas && theSoundingParameter <= kSoundingParCAPE_TT_SurBas)
		return true;
	else
		return false;
}

#if 0 // NEVER USED
static bool FillSurfaceValuesFromInfo(NFmiSmartInfo *theInfo, NFmiSoundingData &theSoundingData, const NFmiPoint &theLatLon, float &theT, float &theTd)
{
	theInfo->Param(kFmiTemperature);
	float T = theInfo->InterpolatedValue(theLatLon);
	theT = T;
	theInfo->Param(kFmiDewPoint);
	float Td = theInfo->InterpolatedValue(theLatLon);
	theTd = Td;
	if(T != kFloatMissing && Td != kFloatMissing)
	{
		theSoundingData.SetTandTdSurfaceValues(T, Td);
		return true;
	}
	else
		return false;
}
#endif

static void CheckIfStopped(NFmiStopFunctor *theStopFunctor)
{
	if(theStopFunctor && theStopFunctor->Stop())
		throw NFmiStopThreadException();
}

static void CalcAllSoundingIndexParamFields(boost::shared_ptr<NFmiFastQueryInfo> &theSourceInfo, boost::shared_ptr<NFmiFastQueryInfo> &theResultInfo, bool useFastFill, NFmiStopFunctor *theStopFunctor)
{
	bool fObsDataFound = false; // toistaiseksi ei k�yt�ss�
	bool useAnalyzeData = false; // toistaiseksi ei k�yt�ss�

	NFmiSoundingDataOpt1 soundingDataOpt1;
	for(theResultInfo->ResetLocation(); theResultInfo->NextLocation(); )
	{
		bool surfaceBaseStatus = false;
		if(useFastFill)
			theSourceInfo->LocationIndex(theResultInfo->LocationIndex());
		::FillSoundingDataOpt1(theSourceInfo, soundingDataOpt1, theResultInfo->Time(), theResultInfo->LatLon(), useFastFill);

		unsigned long counter = 0;
		for(theResultInfo->ResetParam(); theResultInfo->NextParam(); counter++)
		{
			if(counter%20 == 0)
				::CheckIfStopped(theStopFunctor); // joka 20 hilapisteell� katsotaan, pit��k� lopettaa

			FmiSoundingParameters soundingParameter = static_cast<FmiSoundingParameters>(theResultInfo->Param().GetParamIdent());
			bool surfaceBasedCalculation = NFmiSoundingIndexCalculator::IsSurfaceBasedSoundingIndex(soundingParameter); // onko surfacebased???

			// HUOM!!!! muista muuttaa luotaus-parametri pelk�ksi surface arvoksi, koska loppu menee itsest��n sitten
			float valueOpt1 = NFmiSoundingIndexCalculator::CalcOpt1(soundingDataOpt1, soundingParameter);
			theResultInfo->FloatValue(valueOpt1);
		}
	}
}

static void CalculatePartOfSoundingData(boost::shared_ptr<NFmiFastQueryInfo> &theSourceInfo, boost::shared_ptr<NFmiFastQueryInfo> &theResultInfo, unsigned long startTimeIndex, unsigned long endTimeIndex, bool useFastFill, NFmiStopFunctor *theStopFunctor, int index, bool fDoCerrReporting)
{
	try
	{
		if(theSourceInfo->IsGrid() == false || theResultInfo->IsGrid() == false)
			throw std::runtime_error("Error in CalculatePartOfSoundingData, source or result data was non grid-data.");

		for(unsigned long i = startTimeIndex; i <= endTimeIndex; i++)
		{
			if(theResultInfo->TimeIndex(i))
			{
				if(useFastFill == false || theSourceInfo->TimeIndex(theResultInfo->TimeIndex())) // optimointia, molemmissa samat ajat!!!
				{
					if(fDoCerrReporting)
						std::cerr << "thread nro: " << index << " starting time step nro: " << i << std::endl;
					::CheckIfStopped(theStopFunctor);
					::CalcAllSoundingIndexParamFields(theSourceInfo, theResultInfo, useFastFill, theStopFunctor);
				}
			}
		}
	}
	catch(NFmiStopThreadException & /* stopException */ )
	{
		// lopetetaan sitten kun on niin haluttu
		if(fDoCerrReporting)
			std::cerr << "thread nro: " << index << " stops because it was told to do so." << std::endl;
	}
	catch(std::exception &e)
	{
		// lopetetaan muutenkin, jos tulee poikkeus
		if(fDoCerrReporting)
			std::cerr << "thread nro: " << index << " stops because exception was thrown:\n" << e.what() << std::endl;
	}
	catch(...)
	{
		// lopetetaan muutenkin, jos tulee poikkeus
		if(fDoCerrReporting)
			std::cerr << "thread nro: " << index << " stops because unknown error." << std::endl;
	}
	if(fDoCerrReporting)
		std::cerr << "thread nro: " << index << " end here."<< std::endl;
}

#ifndef BOOST_DISABLE_THREADS

static void CalcTimeIndexStartsForThreeThreads(unsigned long timeSize, unsigned long &theTimeStart1, unsigned long &theTimeStart2, unsigned long &theTimeStart3)
{
	unsigned long partSize = timeSize/3;
	if(timeSize % 3 == 0)
	{
		theTimeStart1 = 0;
		theTimeStart2 = partSize;
		theTimeStart3 = partSize*2;
	}
	else if(timeSize % 3 == 1)
	{
		theTimeStart1 = 0;
		theTimeStart2 = partSize;
		theTimeStart3 = partSize*2; // viimeinen threadi saa tehd� yhden ylim��r�isen aika-askeleen
	}
	else if(timeSize % 3 == 2)
	{
		theTimeStart1 = 0;
		theTimeStart2 = partSize; // toinen threadi saa tehd� yhden ylim��r�isen aika-askeleen
		theTimeStart3 = partSize*2+1; // viimeinen threadi saa tehd� yhden ylim��r�isen aika-askeleen
	}
}
#endif

// Jos useFastFill on true, on datoilla sama hila ja aika descriptor rakenne
void NFmiSoundingIndexCalculator::CalculateWholeSoundingData(NFmiQueryData &theSourceData, NFmiQueryData &theResultData, bool useFastFill, bool fDoCerrReporting, NFmiStopFunctor *theStopFunctor)
{
	NFmiSoundingFunctions::CalcDP(1, 56); // t�m� funktio pit�� varmistaa ett� se on alustettu, koska siell� on pari staattista muuttujaa, jotka 
											// alustetaan ensimm�isell� kerralla ja multi-threaddaavassa jutussa se voisi olla ongelma.

	// Jaetaan laskut kolmeen osaan ja laitetaan ne laskettavaksi kolmeen eri threadiin.
	// Jako menee aika indeksien mukaisella jaolla.
	// Jokaiselle threadille annetaan omat kopiot fastInfo iteraattoreista.
	// Paitsi jos aikoja datoissa on alle 3 kpl
	// Lis�ksi threadit k�ynnistet��n alle normaalin prioriteetin, ett� kone ei tukahdu kun n�it� k�ynnistet��n.

	unsigned long timeSize = theResultData.Info()->SizeTimes();
#ifndef BOOST_DISABLE_THREADS
	if(timeSize < 3)
	{ // jos aikoja oli alle kolme, lasketaan data yhdess� funktiossa
#endif // BOOST_DISABLE_THREADS

		if(fDoCerrReporting)
			std::cerr << "making data in single thread" << std::endl;
		boost::shared_ptr<NFmiFastQueryInfo> sourceInfo(new NFmiFastQueryInfo(&theSourceData));
		boost::shared_ptr<NFmiFastQueryInfo> resultInfo(new NFmiFastQueryInfo(&theResultData));
		::CalculatePartOfSoundingData(sourceInfo, resultInfo, 0, timeSize-1, useFastFill, theStopFunctor, 1, fDoCerrReporting);
#ifndef BOOST_DISABLE_THREADS
	}
	else
	{
		if(fDoCerrReporting)
			std::cerr << "making data in three threads" << std::endl;
		unsigned long timeStart1 = 0;
		unsigned long timeStart2 = 0;
		unsigned long timeStart3 = 0;
		// jaetaan aika indeksi kolmeen osaa ja k�ynnistet��n kolme worker threadia, jotka laskevat eri osia datasta
		::CalcTimeIndexStartsForThreeThreads(timeSize, timeStart1, timeStart2, timeStart3);
		unsigned long timeEnd1 = timeStart2 - 1;
		unsigned long timeEnd2 = timeStart3 - 1;
		unsigned long timeEnd3 = timeSize - 1;

		boost::shared_ptr<NFmiFastQueryInfo> sourceInfo1(new NFmiFastQueryInfo(&theSourceData));
		boost::shared_ptr<NFmiFastQueryInfo> resultInfo1(new NFmiFastQueryInfo(&theResultData));
		boost::shared_ptr<NFmiFastQueryInfo> sourceInfo2(new NFmiFastQueryInfo(&theSourceData));
		boost::shared_ptr<NFmiFastQueryInfo> resultInfo2(new NFmiFastQueryInfo(&theResultData));
		boost::shared_ptr<NFmiFastQueryInfo> sourceInfo3(new NFmiFastQueryInfo(&theSourceData));
		boost::shared_ptr<NFmiFastQueryInfo> resultInfo3(new NFmiFastQueryInfo(&theResultData));

		// HUOM! multi threaddauksessa otettava huomioon!!!!
		// NFmiQueryData-luokassa on itsLatLonCache, joka on samalla k�yt�ss� kaikilla
		// erillisill� Info-iteraattoreilla. T�m� on ns. lazy-init dataa, joka initialisoidaan kerran
		// tarvittaessa. Nyt samoihin datoihin tehd��n kolme info-iteraattoria ja t�ss� kutsumalla yhdelle niist�
		// LatLon-metodia, varmistetaan ett� itsLatLonCache on initialisoitu, ennen kuin menn��n
		// suorittamaan kolmea threadia yht�aikaa. T�m� oli kaato bugi, kun kolme thtreadia kutsui Latlon:ia
		// ja cache-data ei oltu viel� initialisoitu, jolloin sit� initialisoitiin kolme kertaa p��llekk�in. 
		// Ongelma oli varsinkin isommille datoille (esim. mbe), joille kyseisen cachen rakentaminen kest�� kauemmin.
		NFmiPoint dummyPoint = resultInfo3->LatLon(); // Varmistetaan ett� NFmiQueryDatan itsLatLonCache on alustettu!!
		dummyPoint = sourceInfo3->LatLon(); // Varmistetaan ett� NFmiQueryDatan itsLatLonCache on alustettu!!

		boost::thread_group calcParts;
		calcParts.add_thread(new boost::thread(::CalculatePartOfSoundingData, sourceInfo3, resultInfo3, timeStart3, timeEnd3, useFastFill, theStopFunctor, 3, fDoCerrReporting));
		calcParts.add_thread(new boost::thread(::CalculatePartOfSoundingData, sourceInfo2, resultInfo2, timeStart2, timeEnd2, useFastFill, theStopFunctor, 2, fDoCerrReporting));
		calcParts.add_thread(new boost::thread(::CalculatePartOfSoundingData, sourceInfo1, resultInfo1, timeStart1, timeEnd1, useFastFill, theStopFunctor, 1, fDoCerrReporting));
		calcParts.join_all(); // odotetaan ett� threadit lopettavat
		if(fDoCerrReporting)
			std::cerr << "all threads ended" << std::endl;
	}
#endif // BOOST_DISABLE_THREADS

}

float NFmiSoundingIndexCalculator::CalcOpt1(NFmiSoundingDataOpt1 &theSoundingDataOpt1, FmiSoundingParameters theParam)
{
	double value = kFloatMissing;
	double xxxxValue = kFloatMissing; // t�m� on ns. hukka parametri, koska jotkut parametrit syntyv�t sivutuotteena ja t�h�n sijoitetaan aina se ei haluttu parametri
	switch(theParam)
	{
	// **** 1. yksinkertaiset indeksit, tarvitaan vain soundingdata ***
	case kSoundingParSHOW:
		value = theSoundingDataOpt1.CalcSHOWIndex();
		break;
	case kSoundingParLIFT:
		value = theSoundingDataOpt1.CalcLIFTIndex();
		break;
	case kSoundingParKINX:
		value = theSoundingDataOpt1.CalcKINXIndex();
		break;
	case kSoundingParCTOT:
		value = theSoundingDataOpt1.CalcCTOTIndex();
		break;
	case kSoundingParVTOT:
		value = theSoundingDataOpt1.CalcVTOTIndex();
		break;
	case kSoundingParTOTL:
		value = theSoundingDataOpt1.CalcTOTLIndex();
		break;

	// **** 2. indeksit joissa tarvitaan my�s pintakerros lasku tyyppi soundingdatan lis�ksi ja mahd. korkeus parametri ***
	// **** surface ****
	case kSoundingParLCLSur:
	case kSoundingParLCLSurBas:
		value = theSoundingDataOpt1.CalcLCLIndex(kLCLCalcSurface);
		break;
	case kSoundingParCAPESur:
	case kSoundingParCAPESurBas:
		value = theSoundingDataOpt1.CalcCAPE500Index(kLCLCalcSurface);
		break;
	case kSoundingParCAPE0_3kmSur:
	case kSoundingParCAPE0_3kmSurBas:
		value = theSoundingDataOpt1.CalcCAPE500Index(kLCLCalcSurface, 3000);
		break;
	case kSoundingParCAPE_TT_Sur:
	case kSoundingParCAPE_TT_SurBas:
		value = theSoundingDataOpt1.CalcCAPE_TT_Index(kLCLCalcSurface, -10, -40);
		break;
	case kSoundingParCINSur:
	case kSoundingParCINSurBas:
		value = theSoundingDataOpt1.CalcCINIndex(kLCLCalcSurface);
		break;
	case kSoundingParLCLHeightSur:
	case kSoundingParLCLHeightSurBas:
		value = theSoundingDataOpt1.CalcLCLHeightIndex(kLCLCalcSurface);
		break;

	// **** 500 m mixing ****
	case kSoundingParLCL500m:
		value = theSoundingDataOpt1.CalcLCLIndex(kLCLCalc500m2);
		break;
	case kSoundingParCAPE500m:
		value = theSoundingDataOpt1.CalcCAPE500Index(kLCLCalc500m2);
		break;
	case kSoundingParCAPE0_3km500m:
		value = theSoundingDataOpt1.CalcCAPE500Index(kLCLCalc500m2, 3000);
		break;
	case kSoundingParCAPE_TT_500m:
		value = theSoundingDataOpt1.CalcCAPE_TT_Index(kLCLCalc500m2, -10, -40);
		break;
	case kSoundingParCIN500m:
		value = theSoundingDataOpt1.CalcCINIndex(kLCLCalc500m2);
		break;
	case kSoundingParLCLHeight500m:
		value = theSoundingDataOpt1.CalcLCLHeightIndex(kLCLCalc500m2);
		break;

	// **** most unstable case ****
	case kSoundingParLCLMostUn:
		value = theSoundingDataOpt1.CalcLCLIndex(kLCLCalcMostUnstable);
		break;
	case kSoundingParCAPEMostUn:
		value = theSoundingDataOpt1.CalcCAPE500Index(kLCLCalcMostUnstable);
		break;
	case kSoundingParCAPE0_3kmMostUn:
		value = theSoundingDataOpt1.CalcCAPE500Index(kLCLCalcMostUnstable, 3000);
		break;
	case kSoundingParCAPE_TT_MostUn:
		value = theSoundingDataOpt1.CalcCAPE_TT_Index(kLCLCalcMostUnstable, -10, -40);
		break;
	case kSoundingParCINMostUn:
		value = theSoundingDataOpt1.CalcCINIndex(kLCLCalcMostUnstable);
		break;
	case kSoundingParLCLHeightMostUn:
		value = theSoundingDataOpt1.CalcLCLHeightIndex(kLCLCalcMostUnstable);
		break;

	// **** 3. indeksit jotka lasketaan jonkun muun indeksin yhteydess�, tarvitaan my�s mahdollisesti pintakerros lasku tyyppi ja soundingdata ***
	case kSoundingParLFCSur:
	case kSoundingParLFCSurBas:
		value = theSoundingDataOpt1.CalcLFCIndex(kLCLCalcSurface, xxxxValue);
		break;
	case kSoundingParELSur:
	case kSoundingParELSurBas:
		xxxxValue = theSoundingDataOpt1.CalcLFCIndex(kLCLCalcSurface, value);
		break;
	case kSoundingParLFCHeightSur:
	case kSoundingParLFCHeightSurBas:
		value = theSoundingDataOpt1.CalcLFCHeightIndex(kLCLCalcSurface, xxxxValue);
		break;
	case kSoundingParELHeightSur:
	case kSoundingParELHeightSurBas:
		xxxxValue = theSoundingDataOpt1.CalcLFCHeightIndex(kLCLCalcSurface, value);
		break;

	case kSoundingParLFC500m:
		value = theSoundingDataOpt1.CalcLFCIndex(kLCLCalc500m2, xxxxValue);
		break;
	case kSoundingParEL500m:
		xxxxValue = theSoundingDataOpt1.CalcLFCIndex(kLCLCalc500m2, value);
		break;
	case kSoundingParLFCHeight500m:
		value = theSoundingDataOpt1.CalcLFCHeightIndex(kLCLCalc500m2, xxxxValue);
		break;
	case kSoundingParELHeight500m:
		xxxxValue = theSoundingDataOpt1.CalcLFCHeightIndex(kLCLCalc500m2, value);
		break;

	case kSoundingParLFCMostUn:
		value = theSoundingDataOpt1.CalcLFCIndex(kLCLCalcMostUnstable, xxxxValue);
		break;
	case kSoundingParELMostUn:
		xxxxValue = theSoundingDataOpt1.CalcLFCIndex(kLCLCalcMostUnstable, value);
		break;
	case kSoundingParLFCHeightMostUn:
		value = theSoundingDataOpt1.CalcLFCHeightIndex(kLCLCalcMostUnstable, xxxxValue);
		break;
	case kSoundingParELHeightMostUn:
		xxxxValue = theSoundingDataOpt1.CalcLFCHeightIndex(kLCLCalcMostUnstable, value);
		break;

	// **** 4. indeksit joiden laskuissa tarvitaan korkeus parametreja ja soundingdata ***
	case kSoundingParBS0_6km:
		value = theSoundingDataOpt1.CalcBulkShearIndex(0, 6);
		break;
	case kSoundingParBS0_1km:
		value = theSoundingDataOpt1.CalcBulkShearIndex(0, 1);
		break;
	case kSoundingParSRH0_3km:
		value = theSoundingDataOpt1.CalcSRHIndex(0, 3);
		break;
	case kSoundingParSRH0_1km:
		value = theSoundingDataOpt1.CalcSRHIndex(0, 1);
		break;
	case kSoundingParWS1500m:
		value = theSoundingDataOpt1.CalcWSatHeightIndex(1500);
		break;
	case kSoundingParThetaE0_3km:
		value = theSoundingDataOpt1.CalcThetaEDiffIndex(0, 3);
		break;
	case kSoundingParNone:
		value = kFloatMissing;
		break;
	}

	return static_cast<float>(value);
}

float NFmiSoundingIndexCalculator::Calc(NFmiSoundingData &theSoundingData, FmiSoundingParameters theParam)
{
	double value = kFloatMissing;
	double xxxxValue = kFloatMissing; // t�m� on ns. hukka parametri, koska jotkut parametrit syntyv�t sivutuotteena ja t�h�n sijoitetaan aina se ei haluttu parametri
	switch(theParam)
	{
	// **** 1. yksinkertaiset indeksit, tarvitaan vain soundingdata ***
	case kSoundingParSHOW:
		value = theSoundingData.CalcSHOWIndex();
		break;
	case kSoundingParLIFT:
		value = theSoundingData.CalcLIFTIndex();
		break;
	case kSoundingParKINX:
		value = theSoundingData.CalcKINXIndex();
		break;
	case kSoundingParCTOT:
		value = theSoundingData.CalcCTOTIndex();
		break;
	case kSoundingParVTOT:
		value = theSoundingData.CalcVTOTIndex();
		break;
	case kSoundingParTOTL:
		value = theSoundingData.CalcTOTLIndex();
		break;

	// **** 2. indeksit joissa tarvitaan my�s pintakerros lasku tyyppi soundingdatan lis�ksi ja mahd. korkeus parametri ***
	// **** surface ****
	case kSoundingParLCLSur:
	case kSoundingParLCLSurBas:
		value = theSoundingData.CalcLCLIndex(kLCLCalcSurface);
		break;
	case kSoundingParCAPESur:
	case kSoundingParCAPESurBas:
		value = theSoundingData.CalcCAPE500Index(kLCLCalcSurface);
		break;
	case kSoundingParCAPE0_3kmSur:
	case kSoundingParCAPE0_3kmSurBas:
		value = theSoundingData.CalcCAPE500Index(kLCLCalcSurface, 3000);
		break;
	case kSoundingParCAPE_TT_Sur:
	case kSoundingParCAPE_TT_SurBas:
		value = theSoundingData.CalcCAPE_TT_Index(kLCLCalcSurface, -10, -40);
		break;
	case kSoundingParCINSur:
	case kSoundingParCINSurBas:
		value = theSoundingData.CalcCINIndex(kLCLCalcSurface);
		break;
	case kSoundingParLCLHeightSur:
	case kSoundingParLCLHeightSurBas:
		value = theSoundingData.CalcLCLHeightIndex(kLCLCalcSurface);
		break;

	// **** 500 m mixing ****
	case kSoundingParLCL500m:
		value = theSoundingData.CalcLCLIndex(kLCLCalc500m2);
		break;
	case kSoundingParCAPE500m:
		value = theSoundingData.CalcCAPE500Index(kLCLCalc500m2);
		break;
	case kSoundingParCAPE0_3km500m:
		value = theSoundingData.CalcCAPE500Index(kLCLCalc500m2, 3000);
		break;
	case kSoundingParCAPE_TT_500m:
		value = theSoundingData.CalcCAPE_TT_Index(kLCLCalc500m2, -10, -40);
		break;
	case kSoundingParCIN500m:
		value = theSoundingData.CalcCINIndex(kLCLCalc500m2);
		break;
	case kSoundingParLCLHeight500m:
		value = theSoundingData.CalcLCLHeightIndex(kLCLCalc500m2);
		break;

	// **** most unstable case ****
	case kSoundingParLCLMostUn:
		value = theSoundingData.CalcLCLIndex(kLCLCalcMostUnstable);
		break;
	case kSoundingParCAPEMostUn:
		value = theSoundingData.CalcCAPE500Index(kLCLCalcMostUnstable);
		break;
	case kSoundingParCAPE0_3kmMostUn:
		value = theSoundingData.CalcCAPE500Index(kLCLCalcMostUnstable, 3000);
		break;
	case kSoundingParCAPE_TT_MostUn:
		value = theSoundingData.CalcCAPE_TT_Index(kLCLCalcMostUnstable, -10, -40);
		break;
	case kSoundingParCINMostUn:
		value = theSoundingData.CalcCINIndex(kLCLCalcMostUnstable);
		break;
	case kSoundingParLCLHeightMostUn:
		value = theSoundingData.CalcLCLHeightIndex(kLCLCalcMostUnstable);
		break;

	// **** 3. indeksit jotka lasketaan jonkun muun indeksin yhteydess�, tarvitaan my�s mahdollisesti pintakerros lasku tyyppi ja soundingdata ***
	case kSoundingParLFCSur:
	case kSoundingParLFCSurBas:
		value = theSoundingData.CalcLFCIndex(kLCLCalcSurface, xxxxValue);
		break;
	case kSoundingParELSur:
	case kSoundingParELSurBas:
		xxxxValue = theSoundingData.CalcLFCIndex(kLCLCalcSurface, value);
		break;
	case kSoundingParLFCHeightSur:
	case kSoundingParLFCHeightSurBas:
		value = theSoundingData.CalcLFCHeightIndex(kLCLCalcSurface, xxxxValue);
		break;
	case kSoundingParELHeightSur:
	case kSoundingParELHeightSurBas:
		xxxxValue = theSoundingData.CalcLFCHeightIndex(kLCLCalcSurface, value);
		break;

	case kSoundingParLFC500m:
		value = theSoundingData.CalcLFCIndex(kLCLCalc500m2, xxxxValue);
		break;
	case kSoundingParEL500m:
		xxxxValue = theSoundingData.CalcLFCIndex(kLCLCalc500m2, value);
		break;
	case kSoundingParLFCHeight500m:
		value = theSoundingData.CalcLFCHeightIndex(kLCLCalc500m2, xxxxValue);
		break;
	case kSoundingParELHeight500m:
		xxxxValue = theSoundingData.CalcLFCHeightIndex(kLCLCalc500m2, value);
		break;

	case kSoundingParLFCMostUn:
		value = theSoundingData.CalcLFCIndex(kLCLCalcMostUnstable, xxxxValue);
		break;
	case kSoundingParELMostUn:
		xxxxValue = theSoundingData.CalcLFCIndex(kLCLCalcMostUnstable, value);
		break;
	case kSoundingParLFCHeightMostUn:
		value = theSoundingData.CalcLFCHeightIndex(kLCLCalcMostUnstable, xxxxValue);
		break;
	case kSoundingParELHeightMostUn:
		xxxxValue = theSoundingData.CalcLFCHeightIndex(kLCLCalcMostUnstable, value);
		break;

	// **** 4. indeksit joiden laskuissa tarvitaan korkeus parametreja ja soundingdata ***
	case kSoundingParBS0_6km:
		value = theSoundingData.CalcBulkShearIndex(0, 6);
		break;
	case kSoundingParBS0_1km:
		value = theSoundingData.CalcBulkShearIndex(0, 1);
		break;
	case kSoundingParSRH0_3km:
		value = theSoundingData.CalcSRHIndex(0, 3);
		break;
	case kSoundingParSRH0_1km:
		value = theSoundingData.CalcSRHIndex(0, 1);
		break;
	case kSoundingParWS1500m:
		value = theSoundingData.CalcWSatHeightIndex(1500);
		break;
	case kSoundingParThetaE0_3km:
		value = theSoundingData.CalcThetaEDiffIndex(0, 3);
		break;
	case kSoundingParNone:
		value = kFloatMissing;
		break;
	}

	return static_cast<float>(value);
}

float NFmiSoundingIndexCalculator::Calc(boost::shared_ptr<NFmiFastQueryInfo> &theInfo, const NFmiPoint &theLatlon, const NFmiMetTime &theTime, FmiSoundingParameters theParam)
{
	NFmiSoundingData soundingData;
	if(::FillSoundingData(theInfo, soundingData, theTime, theLatlon))
		return Calc(soundingData, theParam);
	return kFloatMissing;
}

static const NFmiParamDescriptor& GetSoundingIndexParams(void)
{
	static bool firstTime = true;
	static NFmiParamDescriptor soundingParams;
	if(firstTime)
	{
		firstTime = false;
		NFmiParamBag parBag;

		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParSHOW, "SHOW", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLIFT, "LIFT", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParKINX, "KINX", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCTOT, "CTOT", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParVTOT, "VTOT", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParTOTL, "TOTL", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));

		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLCLSur, "LCL (sur)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLFCSur, "LFC (sur)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParELSur, "EL (sur)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLCLHeightSur, "LCL height (sur)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLFCHeightSur, "LFC height (sur)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParELHeightSur, "EL height (sur)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCAPESur, "CAPE (sur)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCAPE0_3kmSur, "CAPE 0-3km (sur)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCAPE_TT_Sur, "CAPE -10-40 (sur)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCINSur, "CIN (sur)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));

		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLCL500m, "LCL (500m)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLFC500m, "LFC (500m)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParEL500m, "EL (500m)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLCLHeight500m, "LCL height (500m)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLFCHeight500m, "LFC height (500m)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParELHeight500m, "EL height (500m)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCAPE500m, "CAPE (500m)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCAPE0_3km500m, "CAPE 0-3km (500m)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCAPE_TT_500m, "CAPE -10-40 (500m)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCIN500m, "CIN (500m)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));

		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLCLMostUn, "LCL (mu)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLFCMostUn, "LFC (mu)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParELMostUn, "EL (mu)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLCLHeightMostUn, "LCL height (mu)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLFCHeightMostUn, "LFC height (mu)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParELHeightMostUn, "EL height (mu)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));

		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCAPEMostUn, "CAPE (mu)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));

		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCAPE0_3kmMostUn, "CAPE 0-3km (mu)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCAPE_TT_MostUn, "CAPE -10-40 (mu)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCINMostUn, "CIN (mu)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));

/* xxxx j�� kommenttiin
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLCLSurBas, "LCL (obs-bas)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLFCSurBas, "LFC (obs-bas)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParELSurBas, "EL (obs-bas)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLCLHeightSurBas, "LCL height (obs-bas)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParLFCHeightSurBas, "LFC height (obs-bas)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParELHeightSurBas, "EL height (obs-bas)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCAPESurBas, "CAPE (obs-bas)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCAPE0_3kmSurBas, "CAPE 0-3km (obs-bas)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCAPE_TT_SurBas, "CAPE -10-40 (obs-bas)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParCINSurBas, "CIN (obs-bas)", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
*/

		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParBS0_6km, "BS 0-6km", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParBS0_1km, "BS 0-1km", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParSRH0_3km, "SRH 0-3km", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParSRH0_1km, "SRH 0-1km", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParWS1500m, "WS 1500m", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));
		parBag.Add(NFmiDataIdent(NFmiParam(kSoundingParThetaE0_3km, "ThetaE 0-3km", kFloatMissing, kFloatMissing, kFloatMissing, kFloatMissing, "%.1f", kLinearly)));

		soundingParams = NFmiParamDescriptor(parBag);
	}

	return soundingParams;
}

static NFmiQueryInfo MakeSoundingIndexInfo(NFmiQueryData &theSourceData, const std::string &theProducerName)
{
	NFmiFastQueryInfo fInfo(&theSourceData);

	NFmiParamDescriptor params = ::GetSoundingIndexParams();
	NFmiProducer usedProducer = *fInfo.Producer();
	if(theProducerName.empty() == false)
		usedProducer.SetName(theProducerName);
	else
	{
		NFmiString prodName = usedProducer.GetName();
		prodName += " (sounding index)";
		usedProducer.SetName(prodName);
	}
	params.SetProducer(usedProducer); // tuottaja pit�� asettaa oikeaksi

	NFmiQueryInfo info(params, fInfo.TimeDescriptor(), fInfo.HPlaceDescriptor()); // default vplaceDesc riitt�� kun dataa lasketaan vain yhteen tasoon
	return info;
}

boost::shared_ptr<NFmiQueryData> NFmiSoundingIndexCalculator::CreateNewSoundingIndexData(const std::string &theSourceFileFilter, const std::string &theProducerName, bool fDoCerrReporting, NFmiStopFunctor *theStopFunctor)
{
	// 1. lue uusin pohjadata k�ytt��n
	boost::shared_ptr<NFmiQueryData> sourceData(NFmiQueryDataUtil::ReadNewestData(theSourceFileFilter));
	if(sourceData == 0)
		throw std::runtime_error("Error in CreateNewSoundingIndexData, cannot read source data.");
	else
	{
		if(fDoCerrReporting)
			std::cerr << "read qd-file: " << theSourceFileFilter << std::endl;
	}

	// 2. luo sen avulla uusi qinfo pohjaksi
	NFmiQueryInfo soundingIndexInfo = ::MakeSoundingIndexInfo(*sourceData.get(), theProducerName);
	// 3. luo uusi qdata
	boost::shared_ptr<NFmiQueryData> data(NFmiQueryDataUtil::CreateEmptyData(soundingIndexInfo));
	if(data == 0)
		throw std::runtime_error("Error in CreateNewSoundingIndexData, could not create result data.");
	// 4. t�yt� qdata
	NFmiSoundingIndexCalculator::CalculateWholeSoundingData(*sourceData.get(), *data.get(), true, fDoCerrReporting, theStopFunctor);

	return data;
}
