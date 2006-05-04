// ======================================================================
/*!
 * \file NFmiInfoAreaMaskSoundingIndex.cpp
 * \brief Implementation of class NFmiInfoAreaMaskSoundingIndex
 */
// ======================================================================

#include "NFmiInfoAreaMaskSoundingIndex.h"

NFmiInfoAreaMaskSoundingIndex::~NFmiInfoAreaMaskSoundingIndex(void)
{
}

NFmiInfoAreaMaskSoundingIndex::NFmiInfoAreaMaskSoundingIndex(NFmiQueryInfo * theInfo, FmiSoundingParameters theSoundingParam, bool ownsInfo, bool destroySmartInfoData)
:NFmiInfoAreaMask(theInfo, ownsInfo, kNoValue, destroySmartInfoData)
,itsSoundingParam(theSoundingParam)
{
}

// t�t� kaytetaan smarttool-modifierin yhteydess�
double NFmiInfoAreaMaskSoundingIndex::Value(const NFmiPoint & theLatlon, const NFmiMetTime & theTime, int /* theTimeIndex */ , bool /* fUseTimeInterpolationAlways */ )
{
	return NFmiSoundingIndexCalculator::Calc(itsInfo, theLatlon, theTime, itsSoundingParam);
}
