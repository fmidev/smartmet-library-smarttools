// ======================================================================
/*!
 * \file NFmiInfoAreaMaskSoundingIndex.cpp
 * \brief Implementation of class NFmiInfoAreaMaskSoundingIndex
 */
// ======================================================================

#include "NFmiInfoAreaMaskSoundingIndex.h"
#include "NFmiFastQueryInfo.h"

NFmiInfoAreaMaskSoundingIndex::~NFmiInfoAreaMaskSoundingIndex(void)
{
}

NFmiInfoAreaMaskSoundingIndex::NFmiInfoAreaMaskSoundingIndex(boost::shared_ptr<NFmiFastQueryInfo> &theInfo, FmiSoundingParameters theSoundingParam)
:NFmiInfoAreaMask(theInfo, kNoValue)
,itsSoundingParam(theSoundingParam)
{
}

NFmiInfoAreaMaskSoundingIndex::NFmiInfoAreaMaskSoundingIndex(const NFmiInfoAreaMaskSoundingIndex &theOther)
:NFmiInfoAreaMask(theOther)
,itsSoundingParam(theOther.itsSoundingParam)
{
}

NFmiAreaMask* NFmiInfoAreaMaskSoundingIndex::Clone(void) const
{
	return new NFmiInfoAreaMaskSoundingIndex(*this);
}

// t�t� kaytetaan smarttool-modifierin yhteydess�
double NFmiInfoAreaMaskSoundingIndex::Value(const NFmiPoint & theLatlon, const NFmiMetTime & theTime, int /* theTimeIndex */ , bool /* fUseTimeInterpolationAlways */ )
{
	// RUMAA CAST-koodia!!!!!
	return NFmiSoundingIndexCalculator::Calc(itsInfo, theLatlon, theTime, itsSoundingParam);
}
