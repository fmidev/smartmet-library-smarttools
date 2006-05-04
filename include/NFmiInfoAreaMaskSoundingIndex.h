// ======================================================================
/*!
 * \file NFmiInfoAreaMaskSoundingIndex.h
 * \brief Interface of class NFmiInfoAreaMaskSoundingIndex
 */
// ======================================================================

#ifndef NFMIINFOAREAMASKSOUNDINGINDEX_H
#define NFMIINFOAREAMASKSOUNDINGINDEX_H

#include "NFmiInfoAreaMask.h"
#include "NFmiSoundingIndexCalculator.h"

class NFmiQueryInfo;

//! T�m� luokka toimii kuten NFmiInfoAreaMask mutta kurkkaa halutun x-y hila pisteen yli arvoa
class NFmiInfoAreaMaskSoundingIndex : public NFmiInfoAreaMask
{

public:

	virtual ~NFmiInfoAreaMaskSoundingIndex(void);
	NFmiInfoAreaMaskSoundingIndex(NFmiQueryInfo * theInfo, FmiSoundingParameters theSoundingParam, bool ownsInfo, bool destroySmartInfoData);
	FmiSoundingParameters SoundingParam(void) const {return itsSoundingParam;}
	void SoundingParam(FmiSoundingParameters newValue) {itsSoundingParam = newValue;}

	// t�t� kaytetaan smarttool-modifierin yhteydess�
	double Value(const NFmiPoint & theLatlon, const NFmiMetTime & theTime, int theTimeIndex, bool fUseTimeInterpolationAlways);

private:
	FmiSoundingParameters itsSoundingParam;

	// ei toteutusta n�ille!!!
	NFmiInfoAreaMaskSoundingIndex(const NFmiInfoAreaMaskSoundingIndex & theMask);
	NFmiInfoAreaMaskSoundingIndex & operator=(const NFmiInfoAreaMaskPeekXY & theMask);

}; // class NFmiInfoAreaMaskSoundingIndex

#endif // NFMIINFOAREAMASKSOUNDINGINDEX_H
