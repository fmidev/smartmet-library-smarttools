// ======================================================================
/*!
 * \file NFmiSoundingIndexCalculator.h
 *
 * T�m� luokka laskee erilaisia luotausi ndeksej� annettujen querinfojen avulla.
 * Mm. CAPE, CIN, LCL, BulkShear StormRelatedHellicity jne.
 */
// ======================================================================

#ifndef NFMISOUNDINGINDEXCALCULATOR_H
#define NFMISOUNDINGINDEXCALCULATOR_H

#include <string>
#include <boost/shared_ptr.hpp>

class NFmiQueryData;
class NFmiFastQueryInfo;
class NFmiSoundingData;
class NFmiSoundingDataOpt1;
class NFmiDrawParam;
class NFmiMetTime;
class NFmiInfoOrganizer;
class NFmiLocation;
class NFmiPoint;
class NFmiStopFunctor;

// N�m� parametrit ovat niit� joita lasketaan luotausn�yt�ss�.
// Tarkoitus on nyt pysty� laskemaan niit� my�s hilamuodossa karttan�yt�lle.
// Sit� varten tein oman parametrilistan, joka kuuluisi NFmiParameterName-enumiin.
// Mutta en halua sotkea sit� n�ill�, joten tein oman listan, jota kuitenkin k�ytet��n
// kuin se olisi originaali NFmiParameterName-enumissa.
typedef enum
{
	kSoundingParNone = 4619, // aloitan luvut jostain numerosta mik� ei mene sekaisin originaali param-listan kanssa
	// aluksi surface (sur) arvojen avulla lasketut parametrit
	kSoundingParLCLSur = 4720,
	kSoundingParLFCSur,
	kSoundingParELSur,
	kSoundingParCAPESur,
	kSoundingParCAPE0_3kmSur,
	kSoundingParCINSur,
	kSoundingParLCLHeightSur,
	kSoundingParLFCHeightSur,
	kSoundingParELHeightSur,
	kSoundingParCAPE_TT_Sur, // cape -10 ja -40 asteen kerroksen l�pi
	// sitten 500m arvojen avulla lasketut parametrit
	kSoundingParLCL500m = 4730,
	kSoundingParLFC500m,
	kSoundingParEL500m,
	kSoundingParCAPE500m,
	kSoundingParCAPE0_3km500m,
	kSoundingParCIN500m,
	kSoundingParLCLHeight500m,
	kSoundingParLFCHeight500m,
	kSoundingParELHeight500m,
	kSoundingParCAPE_TT_500m, // cape -10 ja -40 asteen kerroksen l�pi
	// sitten mostunstable (MostUn) arvojen avulla lasketut parametrit
	kSoundingParLCLMostUn = 4740,
	kSoundingParLFCMostUn,
	kSoundingParELMostUn,
	kSoundingParCAPEMostUn,
	kSoundingParCAPE0_3kmMostUn,
	kSoundingParCINMostUn,
	kSoundingParLCLHeightMostUn,
	kSoundingParLFCHeightMostUn,
	kSoundingParELHeightMostUn,
	kSoundingParCAPE_TT_MostUn, // cape -10 ja -40 asteen kerroksen l�pi
	// sitten normaaleja yleis indeksej�
	kSoundingParSHOW = 4750,
	kSoundingParLIFT,
	kSoundingParKINX,
	kSoundingParCTOT,
	kSoundingParVTOT,
	kSoundingParTOTL,
	// sitten viel� 'speciaali' paramereja
	kSoundingParBS0_6km = 4770, // BS=bulkshear
	kSoundingParBS0_1km,
	kSoundingParSRH0_3km, // SRH=storm related helicity
	kSoundingParSRH0_1km,
	kSoundingParWS1500m,
	kSoundingParThetaE0_3km,
	// sitten obs-surface-based (SurBas) arvojen avulla lasketut parametrit
	kSoundingParLCLSurBas = 4780,
	kSoundingParLFCSurBas,
	kSoundingParELSurBas,
	kSoundingParCAPESurBas,
	kSoundingParCAPE0_3kmSurBas,
	kSoundingParCINSurBas,
	kSoundingParLCLHeightSurBas,
	kSoundingParLFCHeightSurBas,
	kSoundingParELHeightSurBas,
	kSoundingParCAPE_TT_SurBas // cape -10 ja -40 asteen kerroksen l�pi
} FmiSoundingParameters;

class NFmiSoundingIndexCalculator
{
public:

	static bool IsSurfaceBasedSoundingIndex(FmiSoundingParameters theSoundingParameter);
	static bool FillSoundingData(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo, NFmiSoundingData &theSoundingData, const NFmiMetTime &theTime, const NFmiLocation &theLocation, const boost::shared_ptr<NFmiFastQueryInfo> &theGroundDataInfo);
	static bool FillSoundingDataOpt1(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo, NFmiSoundingDataOpt1 &theSoundingData, const NFmiMetTime &theTime, const NFmiLocation &theLocation, const boost::shared_ptr<NFmiFastQueryInfo> &theGroundDataInfo);
	static float Calc(NFmiSoundingData &theSoundingData, FmiSoundingParameters theParam);
	static float CalcOpt1(NFmiSoundingDataOpt1 &theSoundingDataOpt1, FmiSoundingParameters theParam);
	static float Calc(const boost::shared_ptr<NFmiFastQueryInfo> &theInfo, const NFmiPoint &theLatlon, const NFmiMetTime &theTime, FmiSoundingParameters theParam);
	static void CalculateWholeSoundingData(NFmiQueryData &theSourceData, NFmiQueryData &theResultData, bool useFastFill, bool fDoCerrReporting, NFmiStopFunctor *theStopFunctor = 0, bool fUseOnlyOneThread = true);
	static boost::shared_ptr<NFmiQueryData> CreateNewSoundingIndexData(const std::string &theSourceFileFilter, const std::string &theProducerName, bool fDoCerrReporting, NFmiStopFunctor *theStopFunctor = 0, bool fUseOnlyOneThread = true);
};

#endif // NFMISOUNDINGINDEXCALCULATOR_H
