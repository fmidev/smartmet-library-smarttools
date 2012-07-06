// ======================================================================
/*!
 * \file NFmiInfoAreaMaskSoundingIndex.h
 * \brief Interface of class NFmiInfoAreaMaskSoundingIndex
 */
// ======================================================================

#ifndef NFMIINFOAREAMASKSOUNDINGINDEX_H
#define NFMIINFOAREAMASKSOUNDINGINDEX_H

#include <NFmiInfoAreaMask.h>
#include "NFmiSoundingIndexCalculator.h"

class NFmiFastQueryInfo;

//! T�m� luokka toimii kuten NFmiInfoAreaMask mutta kurkkaa halutun x-y hila pisteen yli arvoa
class NFmiInfoAreaMaskSoundingIndex : public NFmiInfoAreaMask
{

public:

	virtual ~NFmiInfoAreaMaskSoundingIndex(void);
	NFmiInfoAreaMaskSoundingIndex(boost::shared_ptr<NFmiFastQueryInfo> &theInfo, FmiSoundingParameters theSoundingParam);
	NFmiInfoAreaMaskSoundingIndex(const NFmiInfoAreaMaskSoundingIndex &theOther);
	NFmiAreaMask* Clone(void) const;

	FmiSoundingParameters SoundingParam(void) const {return itsSoundingParam;}
	void SoundingParam(FmiSoundingParameters newValue) {itsSoundingParam = newValue;}

	// t�t� kaytetaan smarttool-modifierin yhteydess�
    using NFmiInfoAreaMask::Value;
	double Value(const NFmiPoint & theLatlon, const NFmiMetTime & theTime, int theTimeIndex, bool fUseTimeInterpolationAlways);

private:
	FmiSoundingParameters itsSoundingParam;

	NFmiInfoAreaMaskSoundingIndex & operator=(const NFmiInfoAreaMaskPeekXY & theMask);

}; // class NFmiInfoAreaMaskSoundingIndex

#endif // NFMIINFOAREAMASKSOUNDINGINDEX_H
