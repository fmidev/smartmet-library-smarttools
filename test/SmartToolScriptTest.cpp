#define BOOST_TEST_MODULE "SmartToolScriptTest"

#include "NFmiSmartToolUtil.h"
#include <boost/test/included/unit_test.hpp>
#include <newbase/NFmiFastQueryInfo.h>
#include <newbase/NFmiQueryData.h>
#include <cmath>
#include <memory>
#include <string>

// Runs smarttool scripts on a small grid (30x70 points, 73 hourly times) with Temperature and
// DewPoint and checks the resulting values against the original data.

namespace
{
const char* datafile = "/usr/share/smartmet/test/data/qdtools/input/griddata.sqd.xz";

std::unique_ptr<NFmiQueryData> original()
{
  return std::make_unique<NFmiQueryData>(datafile, false);
}

std::unique_ptr<NFmiQueryData> run(const std::string& script)
{
  return std::unique_ptr<NFmiQueryData>(
      NFmiSmartToolUtil::ModifyData(script, new NFmiQueryData(datafile, false), false, false));
}

// Apply a check to every grid point and time of the given parameter

template <typename Check>
void compare(NFmiQueryData& theOriginal,
             NFmiQueryData& theResult,
             FmiParameterName theParam,
             Check check)
{
  NFmiFastQueryInfo q1(&theOriginal);
  NFmiFastQueryInfo q2(&theResult);
  BOOST_REQUIRE(q1.Param(theParam));
  BOOST_REQUIRE(q2.Param(theParam));
  BOOST_REQUIRE_EQUAL(q1.SizeLocations(), q2.SizeLocations());
  BOOST_REQUIRE_EQUAL(q1.SizeTimes(), q2.SizeTimes());

  std::size_t n = 0;
  for (q1.ResetTime(), q2.ResetTime(); q1.NextTime() && q2.NextTime();)
    for (q1.ResetLocation(), q2.ResetLocation(); q1.NextLocation() && q2.NextLocation();)
    {
      check(q1.FloatValue(), q2.FloatValue());
      ++n;
    }
  BOOST_CHECK_EQUAL(n, 30U * 70U * 73U);
}

bool close(float a, float b)
{
  return std::abs(a - b) < 1e-3;
}

}  // namespace

BOOST_AUTO_TEST_CASE(addition)
{
  auto orig = original();
  auto result = run("T = T + 1");
  BOOST_REQUIRE(result);
  int bad = 0;
  compare(*orig, *result, kFmiTemperature, [&](float a, float b) {
    if (a == kFloatMissing ? b != kFloatMissing : !close(b, a + 1))
      ++bad;
  });
  BOOST_CHECK_EQUAL(bad, 0);
}

BOOST_AUTO_TEST_CASE(other_parameters_are_unchanged)
{
  auto orig = original();
  auto result = run("T = T + 1");
  BOOST_REQUIRE(result);
  int bad = 0;
  compare(*orig, *result, kFmiDewPoint, [&](float a, float b) {
    if (a != b)
      ++bad;
  });
  BOOST_CHECK_EQUAL(bad, 0);
}

BOOST_AUTO_TEST_CASE(constant_and_other_parameter)
{
  auto orig = original();
  auto result = run("T = 5\nTD = T - 10");
  BOOST_REQUIRE(result);
  int bad = 0;
  compare(*orig, *result, kFmiTemperature, [&](float, float b) {
    if (!close(b, 5))
      ++bad;
  });
  // The second line sees the value assigned on the first line
  compare(*orig, *result, kFmiDewPoint, [&](float, float b) {
    if (!close(b, -5))
      ++bad;
  });
  BOOST_CHECK_EQUAL(bad, 0);
}

BOOST_AUTO_TEST_CASE(if_else)
{
  auto orig = original();
  auto result = run("IF(T > 0)\n{\nT = 1\n}\nELSE\n{\nT = -1\n}");
  BOOST_REQUIRE(result);
  int bad = 0;
  int positive = 0;
  compare(*orig, *result, kFmiTemperature, [&](float a, float b) {
    if (a == kFloatMissing)
      return;
    const float expected = (a > 0 ? 1 : -1);
    if (b != expected)
      ++bad;
    if (b > 0)
      ++positive;
  });
  BOOST_CHECK_EQUAL(bad, 0);
  // The data has both positive and negative temperatures
  BOOST_CHECK_GT(positive, 0);
  BOOST_CHECK_LT(positive, 30 * 70 * 73);
}

BOOST_AUTO_TEST_CASE(variables)
{
  auto orig = original();
  auto result = run("var x = 2\nT = T * x");
  BOOST_REQUIRE(result);
  int bad = 0;
  compare(*orig, *result, kFmiTemperature, [&](float a, float b) {
    if (a == kFloatMissing ? b != kFloatMissing : !close(b, 2 * a))
      ++bad;
  });
  BOOST_CHECK_EQUAL(bad, 0);
}

BOOST_AUTO_TEST_CASE(math_function)
{
  auto orig = original();
  auto result = run("T = abs(T)");
  BOOST_REQUIRE(result);
  int bad = 0;
  compare(*orig, *result, kFmiTemperature, [&](float a, float b) {
    if (a == kFloatMissing)
      return;
    if (!close(b, std::abs(a)))
      ++bad;
  });
  BOOST_CHECK_EQUAL(bad, 0);
}

BOOST_AUTO_TEST_CASE(syntax_error)
{
  // Errors are reported to stderr and by returning no data
  BOOST_CHECK(!run("T = = 1"));
  BOOST_CHECK(!run("NOSUCHPARAM = 1"));
  BOOST_CHECK(!run("IF(T > 0)\nT = 1\nELSE\nT = -1\nENDIF"));
}
