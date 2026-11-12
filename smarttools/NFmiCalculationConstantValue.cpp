//**********************************************************
// C++ Class Name : NFmiCalculationConstantValue
// ---------------------------------------------------
//  Author         : pietarin
//  Creation Date  : 9.11. 2010
//
//**********************************************************

#include "NFmiCalculationConstantValue.h"
#include <newbase/NFmiAreaMaskHelperStructures.h>
#include <newbase/NFmiCalculationCondition.h>
#include <newbase/NFmiDataIterator.h>
#include <newbase/NFmiDataModifier.h>
#include <newbase/NFmiFastQueryInfo.h>
#include <newbase/NFmiGlobals.h>

// ****************************************************************************
// *************** NFmiCalculationConstantValue *******************************
// ****************************************************************************

//--------------------------------------------------------
// Constructor/Destructor
//--------------------------------------------------------
NFmiCalculationConstantValue::NFmiCalculationConstantValue(double value) : itsValue(value) {}
NFmiCalculationConstantValue::~NFmiCalculationConstantValue() = default;
NFmiCalculationConstantValue::NFmiCalculationConstantValue(
    const NFmiCalculationConstantValue& theOther) = default;

NFmiAreaMask *NFmiCalculationConstantValue::Clone() const
{
  return new NFmiCalculationConstantValue(*this);
}

//--------------------------------------------------------
// Value
//--------------------------------------------------------
double NFmiCalculationConstantValue::Value(const NFmiCalculationParams & /* theCalculationParams */,
                                           bool /* fUseTimeInterpolationAlways */)
{
  return itsValue;
}

// ****************************************************************************
// *************** NFmiCalculationConstantValue *******************************
// ****************************************************************************

// ****************************************************************************
// *************** NFmiCalculationSpecialCase *********************************
// ****************************************************************************

NFmiCalculationSpecialCase::NFmiCalculationSpecialCase(NFmiAreaMask::CalculationOperator theValue)
{
  SetCalculationOperator(theValue);
}

NFmiCalculationSpecialCase::NFmiCalculationSpecialCase(const NFmiCalculationSpecialCase& theOther) =
    default;

NFmiAreaMask *NFmiCalculationSpecialCase::Clone() const
{
  return new NFmiCalculationSpecialCase(*this);
}
// ****************************************************************************
// *************** NFmiCalculationSpecialCase *********************************
// ****************************************************************************

// ****************************************************************************
// *************** NFmiCalculationRampFunction *********************************
// ****************************************************************************

NFmiCalculationRampFunction::NFmiCalculationRampFunction(
    const NFmiCalculationCondition &theOperation,
    Type theMaskType,
    NFmiInfoData::Type theDataType,
    const std::shared_ptr<NFmiFastQueryInfo>& theInfo,
    unsigned long thePossibleMetaParamId,
    BinaryOperator thePostBinaryOperator)
    : NFmiInfoAreaMask(theOperation,
                       theMaskType,
                       theDataType,
                       theInfo,
                       thePossibleMetaParamId,
                       thePostBinaryOperator)
{
}

NFmiCalculationRampFunction::~NFmiCalculationRampFunction() = default;
NFmiCalculationRampFunction::NFmiCalculationRampFunction(const NFmiCalculationRampFunction& theOther) =
    default;

NFmiAreaMask *NFmiCalculationRampFunction::Clone() const
{
  return new NFmiCalculationRampFunction(*this);
}

double NFmiCalculationRampFunction::Value(const NFmiCalculationParams &theCalculationParams,
                                         bool fUseTimeInterpolationAlways)
{
  double value = NFmiInfoAreaMask::Value(theCalculationParams, fUseTimeInterpolationAlways);
  return itsMaskCondition.MaskValue(value);
}
// ****************************************************************************
// *************** NFmiCalculationRampFunction *********************************
// ****************************************************************************

// ****************************************************************************
// ************ NFmiCalculationIntegrationFunction *****************************
// ****************************************************************************

NFmiCalculationIntegrationFunction::NFmiCalculationIntegrationFunction(
    const std::shared_ptr<NFmiDataIterator>& theDataIterator,
    const std::shared_ptr<NFmiDataModifier>& theDataModifier,
    Type theMaskType,
    NFmiInfoData::Type theDataType,
    const std::shared_ptr<NFmiFastQueryInfo>& theInfo,
    unsigned long thePossibleMetaParamId)
    : NFmiInfoAreaMask(NFmiCalculationCondition(),
                       theMaskType,
                       theDataType,
                       theInfo,
                       thePossibleMetaParamId,
                       NFmiAreaMask::kNoValue),
      itsDataModifier(theDataModifier),
      itsDataIterator(theDataIterator)
{
}

NFmiCalculationIntegrationFunction::~NFmiCalculationIntegrationFunction() = default;
double NFmiCalculationIntegrationFunction::Value(const NFmiCalculationParams &theCalculationParams,
                                                bool /* fUseTimeInterpolationAlways */)
{
  // HUOM!!! Tähän tuli pikaviritys:
  // asetan vain lähimmän pisteen ja ajan kohdalleen.
  if (itsInfo->NearestPoint(theCalculationParams.UsedLatlon()) &&
      itsInfo->TimeToNearestStep(theCalculationParams.itsTime, kForward))
  {
    itsDataIterator->DoForEach(itsDataModifier.get());
    return itsDataModifier->CalculationResult();
  }
  return kFloatMissing;
}
// ****************************************************************************
// ************ NFmiCalculationIntegrationFunction *****************************
// ****************************************************************************

// ****************************************************************************
// ********** NFmiCalculationRampFunctionWithAreaMask **************************
// ****************************************************************************

// Ramppifunktioiden laskut AreaMask:ien avulla (mm. lat, lon ja elevationangle tapaukset).
double NFmiCalculationRampFunctionWithAreaMask::Value(
    const NFmiCalculationParams &theCalculationParams, bool fUseTimeInterpolationAlways)
{
  itsAreaMask->Time(theCalculationParams.itsTime);
  double value = itsAreaMask->Value(theCalculationParams, fUseTimeInterpolationAlways);
  return itsMaskCondition.MaskValue(value);
}

NFmiCalculationRampFunctionWithAreaMask::NFmiCalculationRampFunctionWithAreaMask(
    const NFmiCalculationCondition& theOperation,
    Type theMaskType,
    NFmiInfoData::Type theDataType,
    const std::shared_ptr<NFmiAreaMask>& theAreaMask,
    BinaryOperator thePostBinaryOperator)
    : NFmiAreaMaskImpl(theOperation, theMaskType, theDataType, thePostBinaryOperator),
      itsAreaMask(theAreaMask)
{
}

NFmiCalculationRampFunctionWithAreaMask::~NFmiCalculationRampFunctionWithAreaMask() = default;
NFmiCalculationRampFunctionWithAreaMask::NFmiCalculationRampFunctionWithAreaMask(
    const NFmiCalculationRampFunctionWithAreaMask& theOther)
    : NFmiAreaMaskImpl(theOther),
      itsAreaMask(theOther.itsAreaMask ? theOther.itsAreaMask->Clone() : nullptr),
      fIsTimeInterpolationNeededInValue(theOther.fIsTimeInterpolationNeededInValue)
{
}

NFmiAreaMask *NFmiCalculationRampFunctionWithAreaMask::Clone() const
{
  return new NFmiCalculationRampFunctionWithAreaMask(*this);
}
// ****************************************************************************
// ********** NFmiCalculationRampFunctionWithAreaMask **************************
// ****************************************************************************

// ****************************************************************************
// **************** NFmiCalculationDeltaZValue ********************************
// ****************************************************************************
double NFmiCalculationDeltaZValue::itsHeightValue;

NFmiCalculationDeltaZValue::NFmiCalculationDeltaZValue() = default;
NFmiCalculationDeltaZValue::NFmiCalculationDeltaZValue(const NFmiCalculationDeltaZValue& theOther) =
    default;

NFmiAreaMask *NFmiCalculationDeltaZValue::Clone() const
{
  return new NFmiCalculationDeltaZValue(*this);
}
// ****************************************************************************
// **************** NFmiCalculationDeltaZValue ********************************
// ****************************************************************************
