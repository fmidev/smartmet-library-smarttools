%define DIRNAME smarttools
%define LIBNAME smartmet-%{DIRNAME}
%define SPECNAME smartmet-library-%{DIRNAME}
%define DEVELNAME %{SPECNAME}-devel
Summary: smarttools library
Name: %{SPECNAME}
Version: 25.2.18
Release: 1%{?dist}.fmi
License: MIT
Group: Development/Libraries
URL: https://github.com/fmidev/smartmet-library-smarttools
Source: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot-%(%{__id_u} -n)

%if 0%{?rhel} && 0%{rhel} < 9
%define smartmet_boost boost169
%else
%define smartmet_boost boost
%endif

%define smartmet_fmt_min 11.0.0
%define smartmet_fmt_max 12.0.0

BuildRequires: rpm-build
BuildRequires: gcc-c++
BuildRequires: make
BuildRequires: smartmet-library-macgyver-devel >= 25.2.18
BuildRequires: smartmet-library-newbase-devel >= 25.2.18
BuildRequires: smartmet-library-gis-devel >= 25.2.18
BuildRequires: %{smartmet_boost}-devel
BuildRequires: fmt-devel >= %{smartmet_fmt_min}, fmt-devel < %{smartmet_fmt_max}
Requires: smartmet-library-newbase >= 25.2.18
Requires: %{smartmet_boost}-filesystem
Requires: %{smartmet_boost}-thread
Requires: fmt-libs >= %{smartmet_fmt_min}, fmt-libs < %{smartmet_fmt_max}
Provides: %{LIBNAME}
Obsoletes: libsmartmet-smarttools < 17.1.4
Obsoletes: libsmartmet-smarttools-debuginfo < 17.1.4
#TestRequires: gcc-c++
#TestRequires: smartmet-library-newbase-devel >= 25.2.18
#TestRequires: %{smartmet_boost}-devel
#TestRequires: smartmet-library-regression

%description
FMI smarttools library

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q -n %{SPECNAME}
 
%build
make %{_smp_mflags}

%install
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(0775,root,root,0775)
%{_libdir}/lib%{LIBNAME}.so

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%package -n %{DEVELNAME}
Summary: FMI smarttools development files
Provides: %{DEVELNAME}
Requires: %{SPECNAME}
Obsoletes: libsmartmet-smarttools-devel < 17.1.4

%description -n %{DEVELNAME}
FMI smarttools development files

%files -n %{DEVELNAME}
%defattr(0664,root,root,0775)
%{_includedir}/smartmet/%{DIRNAME}/*.h


%changelog
* Tue Feb 18 2025 Andris Pavēnis <andris.pavenis@fmi.fi> 25.2.18-1.fmi
- Update to gdal-3.10, geos-3.13 and proj-9.5

* Fri Nov  1 2024 Mika Heiskanen <mika.heiskanen@fmi.fi> - 24.11.1-1.fmi
- Changed NFmiEnumConverter to be static to avoid multiple initializations

* Wed Aug  7 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.8.7-1.fmi
- Update to gdal-3.8, geos-3.12, proj-94 and fmt-11

* Wed Jul 17 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.7.17-1.fmi
- Do not link with libboost_filesystem

* Fri Jul 12 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.7.12-1.fmi
- Replace many boost library types with C++ standard library ones

* Thu May 16 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.5.16-1.fmi
- Clean up boost date-time uses

* Fri Feb 23 2024 Mika Heiskanen <mika.heiskanen@fmi.fi> 24.2.23-1.fmi
- Full repackaging

* Tue Jan 30 2024 Mika Heiskanen <mika.heiskanen@fmi.fi> - 24.1.30-1.fmi
- Fixed NFmiInfoOrganizer::CreateNewMacroParamData_checkedInput method to work with similar code with both newbase branches mater and WGS84.
- Numerous changes over years into smarttools language's intepreter class and modifier class (which executes the actual calculations).
- Major code arrangements: moved multi-data related codes to NFmiInfoAreaMask class.
- Changed observationRadiusInKm handling also to parent class.
- Added new NFmiInfoAreaMaskTimeRangeSecondParValue class to handle 'get max WS and give me from that place+time it's WD value' types smarttools functions.
- changed to use NFmiCalculationParams::UsedLatlon method here.
- Fixed in LookForAdditionalLocalExtremes function couple static_cast to correct data type (long -> int).
- Added time-serial calculation support for smarttools calculation system. It's special case like cross-section calculation, had to make code so that they are calculated primary with same code, and normal grid-level based calculations are calculated original way.
- Added support for secondary parameter in smarttools language for functions that seek max/min value from primary parameter, and they return value from secondary parameter from that max/min time/place/level.
- Changed couple View type enum names to more descriptive (SymbolView -> ArrowView cause it was used with directional arrow view and IndexTextView -> WindVectorView because it was used with wind-vector view).
- A lot of functions to deal with Windows clunky path systems that are used with Smartmet-workstation using files. Clunky means that from coding point of view (comparing file paths to each other) Windows has too many non strict rules with paths: 1) Windows paths are case-insensitive (Linux is not), 2) directory separators can be both ways '\' and '/' and there can be multiple of each marking as separator (in linux only single '/' is accepted), 3) Windows uses drive-letter and things can be reside on different drives on different computers (Linux has universal root /), 4) Windows can have network drives that have special \networkname\path type of path (Linux hasn't).
- Total do over in NFmiExtraMacroParamData class and related helper classes. These classes are used to retrieve macro-param calculation 'extra' parameters and also interpretin parameter-producer-level info in different systems that needs a wanted data to be defined (e.g. symbol color by secondary parameter values, beta-production data triggers, etc.)
- Many changes relating to: case-study data handling, file-path handling, comments in header file positioning, constructor/destructor defining, changed logics in local cache directory/file handlings, etc.
- Added observation data support into simple-condition system used by smarttools language.
- Changes about dealing data in case-study mode of Smartmet-workstation.
- A lot of changes when dealing different kind of macro-param calculation data with Smartmet-workstation (normal, cross-section, time-serial macro-param types).
- On Smartmet-workstation side many new options have been added into visualiszation system over years: itsSimpleIsoLineColorShadeHigh3ValueColor, itsColorContouringColorShadeHigh3ValueColor, fSimpleColorContourTransparentColor1, fSimpleColorContourTransparentColor2, fSimpleColorContourTransparentColor3, fSimpleColorContourTransparentColor4, fSimpleColorContourTransparentColor5, fDoIsoLineColorBlend, fTreatWmsLayerAsObservation, itsFixedTextSymbolDrawLength, itsSymbolDrawDensityX, itsSymbolDrawDensityY, itsPossibleColorValueParameter, fFlipArrowSymbol. These new data members are normally default valued and they stored into settings files so that they are backward and forward compatible with different workstation versions.
- Fixed reading older model run data from local cache directory into memory functionalities.
- Added GetLatestDataProducer functionality.
- Added a lot of possible trace-level-logging due problems with Smartmet-workstation older model-run usage.
- Changed comments in header so that they appear before each data member and not after (on the same line).
- Added new IcaoStations and WmoStations getter methods.
- Fixed NFmiAviationStationInfoSystem::InitFromMasterTableCsv method to use more robust std::string based getline file reading instead of previous of trying to use fixed size buffer with extra code resizing buffer over and over again.
- NFmiSoundingIndexCalculator::FillSoundingData method added theGroundLevelValue parameter (used to cut pressure-level-data based soundings against the actual ground in mountain areas).
- NFmiSoundingIndexCalculator::CalculateWholeSoundingData method now uses more sophisticated used working thread counter.
- Improvments in filling the sounding data logics.
- Added ability to cut pressure-level-data soundings with new NFmiGroundLevelValue system (not using it though after feedback from forecasters).
- Changed NFmiDrawParamList::Add method's behaviour so that satellite data are no longer treated differently. They were used to add in front of the drawing list so that they wouldn't cover everything else under them, but this caused some complications and now they are added to the end of drawing list like every other kind of data layer.
- NFmiOwnerInfo class added itsDataLoadedTimer which tells how long time ago this data has been loaded into use. Used in Smartmet-workstation to highlight to users which data are new (max 5 minutes since loaded) and which data are 'old'.
- Changed numeric values from double to float to remove casting warnings.
- Added ColorChannelLimitCheck function to simplify code where the same check are done repeatedly.
- Added some transparent color functionalities.

* Fri Jul 28 2023 Andris Pavēnis <andris.pavenis@fmi.fi> 23.7.28-1.fmi
- Repackage due to bulk ABI changes in macgyver/newbase/spine

* Tue Mar 28 2023 Mika Heiskanen <mika.heiskanen@fmi.fi> - 23.3.28-1.fmi
- Updated aviation station info reader to handle the standard edition CSV tables (QDTOOLS-174)

* Wed Aug 24 2022 Mika Heiskanen <mika.heiskanen@fmi.fi> - 22.8.24-1.fmi
- Fixed resolution calculations for extra macro parameters

* Fri Jun 17 2022 Andris Pavēnis <andris.pavenis@fmi.fi> 22.6.17-1.fmi
- Add support for RHEL9. Update libpqxx to 7.7.0 (rhel8+) and fmt to 8.1.1

* Tue May 24 2022 Mika Heiskanen <mika.heiskanen@fmi.fi> - 22.5.24-1.fmi
- Repackaged due to NFmiArea ABI changes

* Fri May 20 2022 Mika Heiskanen <mika.heiskanen@fmi.fi> - 22.5.20-1.fmi
- Removed some obsolete #ifdef WGS84 code

* Mon Sep 20 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.9.20-1.fmi
- Fixed to use NFmiAreaMaskHelperStructures

* Thu May  6 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.5.6-1.fmi
- Repackaged due to NFmiAzimuthalArea ABI changes

* Thu Feb 18 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.2.18-1.fmi
- Repackaged due to NFmiArea ABI changes

* Tue Feb 16 2021 Andris Pavēnis <andris.pavenis@fmi.fi> - 21.2.16-1.fmi
- Repackaged due to newbase ABI changes

* Mon Feb 15 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.2.15-1.fmi
- Ported to use new newbase interpolation API

* Wed Jan 20 2021 Andris Pavenis <andris.pavenis@fmi.fi> - 21.1.20-1.fmi
- Build update: use makefile.inc

* Thu Jan 14 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.1.14-1.fmi
- Repackaged smartmet to resolve debuginfo issues

* Tue Jan  5 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.1.5-1.fmi
- Upgrade to fmt 7.1.3

* Tue Dec 15 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.12.15-1.fmi
- Upgrade to pgdg12

* Fri Sep 11 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.9.11-1.fmi
- Removed obsolete FMI_DLL export declarations

* Fri Aug 21 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.8.21-1.fmi
- Upgrade to fmt 6.2

* Sat Apr 18 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.4.18-1.fmi
- Upgrade to Boost 1.69

* Wed Dec  4 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.12.4-1.fmi
- Use -fno-omit-frame-pointer for a better profiling and debugging experience                                                                                              
* Wed Nov 20 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.11.20-1.fmi
- checkedVector is replaced by std::vector
- Refactored NFmiSmartToolInterpreter variable handling code

* Thu Oct 31 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.10.31-1.fmi
- Merged SmartMet Editor changes

* Thu Sep 26 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.9.26-1.fmi
- Removed boost::regex dependency

* Mon Oct  1 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.10.1-1.fmi
- Added option -g to get a proper debuginfo package

* Wed May  2 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.5.2-1.fmi
- Repackaged since newbase NFmiEnumConverted ABI changed

* Sat Apr  7 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.4.7-1.fmi
- Upgrade to boost 1.66

* Mon Aug 28 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.8.28-1.fmi
- Upgrade to boost 1.65

* Tue Jun 13 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.6.13-1.fmi
- Run clang-format after the revert to default include order in clang 4.0.0

* Tue Apr  4 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.4.4-1.fmi
- Recompiled to use the latest newbase API

* Mon Apr  3 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.4.3-1.fmi
- Improved handling of sounding data
- Bug fix to sounding interpolations

* Tue Mar 28 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.3.28-1.fmi
- Refactored some code into newbase

* Thu Mar  9 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.3.9-1.fmi
- Final merge of the SmartMet Editor code branch, numerous changes

* Mon Mar  6 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.3.6-1.fmi
- Include newbase library headers with correct notation

* Fri Feb 10 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.2.10-1.fmi
- Merged SmartMet Editor changes

* Sat Feb  4 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.2.4-1.fmi
- Memory map helper files to enable larger input data for qdscript

* Fri Jan 27 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.1.27-1.fmi
- Recompiled since NFmiQueryData size changed

* Wed Jan  4 2017 Mika Heiskanen <mika.heiskanen@fmi.fi> - 17.1.4-1.fmi
- Switched to FMI open source naming conventions

* Thu Oct 27 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.10.29-1.fmi
- Enable scripting with multiple files with the same producer but different parameters

* Tue Jun 14 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.6.14-1.fmi
- Recompiled due to newbase API changes

* Sun Jan 17 2016 Mika Heiskanen <mika.heiskanen@fmi.fi> - 16.1.17-1.fmi
- "td" is now an alias for dew point, meteorologists prefer it over dp
- Handle sounding querydata
- Bug fix to ThetaE calculations
- Bug fix to parameter min/max limit handling
- Bug fix to progress bar updates
- Bug fix to moving parameters in lists
- Bug fix to level handling with respect to height as in T_EC_z500
- Bug fix to time indexing of SmartMet editor displays

* Wed Apr 15 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.4.15-1.fmi
- newbase API changed for LatLonCache queries

* Thu Apr  9 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.4.9-1.fmi
- newbase API changed

* Mon Mar 30 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.3.30-1.fmi
- Switched to dynamic linkage

* Mon Mar 16 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.3.16-1.fmi
- Recompiled due to newbase API changes

* Mon Feb 16 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.2.16-1.fmi
- Fixes to data validation
- Memory leak fixes
- Added capability to detect changes to configuration file settings
- Added smarttool parameter names qnh and icing

* Fri Feb  6 2015 Mika Heiskanen <mika.heiskanen@fmi.fi> - 15.2.6-1.fmi
- Recompiled with the latest newbase

* Thu Oct 30 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.10.30-1.fmi
- New vertical interpolation methods in newbase

* Mon Oct 13 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.10.13-1.fmi
- Fixes to decoding Columbian TEMP headers

* Fri Aug  1 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.8.1-1.fmi
- Fixes to sounding height calculations
- New code for rendering weather observations

* Wed May 28 2014 Santeri Oksman <santeri.oksman@fmi.fi> - 14.5.28-1.fmi
- Recompiled to get WeatherSymbol parameter

* Thu Apr 10 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.4.10-1.fmi
- Recompiled to gain access to pollen parameters

* Mon Apr  7 2014 Mika Heiskanen <mika.heiskanen@fmi.fi> - 14.4.7-1.fmi
- Sounding index calculations are now multi threaded
- API change to sounding index functions to enable using Himan

* Mon Dec  2 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.12.2-1.fmi
- Station IDs can now be negative

* Wed Nov 27 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.11.27-1.fmi
- Recompiled due to gnomonic area changes in newbase

* Mon Nov 25 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.11.25-1.fmi
- Fixes to SYNOP message parsing
- Improvements to sounding data handling

* Thu Oct 17 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.10.17-1.fmi
- Speed optimizations to qdsoundingindex
- Recompiled due to newbase NFmiQueryInfo changes

* Thu Sep 26 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.9.26-1.fmi
- Recompiled with the latest newbase

* Mon Sep 23 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.9.23-1.fmi
- Recompiled due to newbase API changes
- Moved colour tools from newbase to smarttools

* Thu Sep  5 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.9.5-1.fmi
- Compiled with latest newbase with API changes in NFmiQueryData
- Added checks for empty querydata to avoid segmentation faults

* Wed Aug 21 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.8.21-1.fmi
- Station data now uses comma as a field delimiter

* Fri Aug  2 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.8.2-1.fmi
- Compiled with support for 0...360 views of the world

* Mon Jul 22 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.7.22-1.fmi
- Improved thread safety in random number generation

* Wed Jul  3 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.7.3-1.fmi
- Updated to boost 1.54

* Mon Jun 10 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.6.10-1.fmi
- Added support for tracetory history data

* Thu May 23 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.5.23-1.fmi
- Recompiled due to changes in newbase headers

* Thu Mar 21 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.3.21-1.fmi
- Replaced auto_ptr with shared_ptr

* Wed Mar  6 2013 Mika Heiskanen <mika.heiskanen@fmi.fi> - 13.3.6-1.fmi
- Compiled with the latest newbase

* Tue Nov 27 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.11.27-2.el6.fmi
- Merged branch smartmet_5_8_final to get bug fixes by Marko Pietarinen

* Tue Nov 27 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.11.27-1.el6.fmi
- Recompiled since newbase headers changed

* Wed Nov  7 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.11.7-1.el6.fmi
- Upgrade to boost 1.52 

* Mon Oct 15 2012 Roope Tervo <roope.tervo@fmi.fi> - 12.10.2-1.el6.fmi
- Changes made by Mikko Partio

* Tue Oct  2 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.10.2-1.el6.fmi
- Newbase API changes force a recompile

* Fri Jul  6 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.7.6-1.el6.fmi
- Fixed all issues reported by cppcheck

* Thu Jul  5 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.7.5-1.el6.fmi
- Migration to boost 1.50

* Sat Mar 31 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.3.31-1.el5.fmi
- Upgraded to boost 12.3.31 in el5

* Wed Mar 28 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.3.28-1.el6.fmi
- Upgraded to boost 1.49

* Fri Mar 16 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.3.16-1.el6.fmi
- newbase headers have changed

* Fri Feb 24 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.2.24-1.el6.fmi
- Single threading removed due to extra maintenance required
- A shitload of changes, fixes and improvements by Marko Pietarinen

* Tue Feb  7 2012 Mika Heiskanen <mika.heiskanen@fmi.fi> - 12.2.7-1.el5.fmi
- Newbase headers changed

* Thu Nov 24 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.11.24-1.el5.fmi
- Newbase headers changed

* Thu Nov 17 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.11.17-1.el5.fmi
- Recompiled with -fPIC for q2engine

* Wed Nov 16 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.11.16-1.el5.fmi
- Recompiled due to newbase API changes

* Tue Nov  8 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.11.8-1.el5.fmi
- Recompiled due to newbase API changes

* Fri Oct 28 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.10.28-1.el6.fmi
- Bug fix to handling station numbers

* Thu Oct 27 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.10.27-1.el6.fmi
- New code to handle observations from stations

* Mon Oct 17 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.10.17-3.el5.fmi
- newbase headers changed again

* Mon Oct 17 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.10.17-2.el5.fmi
- Numerous improvements and bug fixes from Marko

* Mon Oct 17 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.10.17-1.el5.fmi
- Upgrade to newbase 11.10.17-1

* Wed Jul 20 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.7.20-1.el5.fmi
- Upgrade to newbase 11.7.20-1

* Mon Jun 20 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.6.20-1.el5.fmi
- Improvements needed for new grib tools

* Mon Jun  6 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.6.6-1.el5.fmi
- Newbase headers changed, rebuild forced

* Tue May 31 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.5.31-1.el6.fmi
- Major merge by Marko Pietarinen

* Fri May 20 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.5.20-1.el6.fmi
- RHEL6 release

* Tue Apr 19 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.4.19-1.el5.fmi
- Fixed all error messages to be in English

* Thu Mar 24 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.3.24-1.el5.fmi
- Several updates plus upgraded newbase

* Mon Jan  3 2011 Mika Heiskanen <mika.heiskanen@fmi.fi> - 11.2.3-1.el5.fmi
- Numerous updates by Marko plus upgrade to latest newbase

* Tue Sep 14 2010 Mika Heiskanen <mika.heiskanen@fmi.fi> - 10.9.14-1.el5.fmi
- Upgrade to boost 1.44 and newbase 10.9.14-1

* Mon Jul  5 2010 Mika Heiskanen <mika.heiskanen@fmi.fi> - 10.7.5-1.el5.fmi
- Compiled with newbase 10.7.5-1

* Thu May  6 2010 Mika Heiskanen <mika.heiskanen@fmi.fi> - 10.5.6-1.el5.fmi
- A large number of changes by Marko

* Fri Jan 15 2010 Mika Heiskanen <mika.heiskanen@fmi.fi> - 10.1.15-1.el5.fmi
- Recompiled with latest newbase

* Mon Jan 11 2010 Mika Heiskanen <mika.heiskanen@fmi.fi> - 10.1.11-1.el5.fmi
- Numerous minor updates

* Tue Jul 14 2009 Mika Heiskanen <mika.heiskanen@fmi.fi> - 9.7.14-1.el5.fmi
- Upgrade to boost 1.39 
- Minor changes by Marko Pietarinen

* Wed Apr 22 2009 mheiskan <mika.heiskanen@fmi.fi> - 9.4.22-1.el5.fmi
- Compile single- and multithread versions

* Fri Mar 27 2009 mheiskan <mika.heiskanen@fmi.fi> - 9.3.27-1.el5.fmi
- Updates from Marko Pietarinen

* Mon Jan 19 2009 Marko Pietarinen <marko.pietarinen@fmi.fi> - 9.1.20-1.el5.fmi
- Some bugfixes to newbase

* Mon Jan 19 2009 Marko Pietarinen <marko.pietarinen@fmi.fi> - 9.1.19-1.el5.fmi
- Smallish upgrades by Marko

* Tue Sep 30 2008 westerba <antti.westerberg@fmi.fi> - 8.9.30-1.el5.fmi
- Fixed typecast problems in NFmiSmartToolIntepreter.cpp

* Mon Sep 29 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.9.29-1.el5.fmi
- Newbase header change forced rebuild

* Mon Sep 22 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.9.22-1.el5.fmi
- A few updates by Marko

* Thu Sep 11 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.9.11-1.el5.fmi
- A few API updates

* Tue Jul 15 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.7.15-1.el5.fmi
- Compiled with newbase 8.7.15-1

* Tue Mar 11 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.3.11-1.el5.fmi
- Linked against 8.3.11-1

* Mon Feb 18 2008 pkeranen <pekka.keranen@fmi.fi> - 8.2.18-1.el5.fmi
- Linked against newbase 8.2.18-1 with new WeatherAndCloudinessParam

* Wed Jan 30 2008 mheiskan <mika.heiskanen@fmi.fi> - 8.1.30-1.el5.fmi
- Linked against newbase 8.1.25-3 with new parameter names

* Thu Dec 27 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.9-1.el5.fmi
- Linked against newbase-1.0.7-1 with fixed wind interpolation

* Wed Dec 19 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.8-1.el5.fmi
- Linked against newbase-1.0.6-1
- Some changes by Marko related to Q2 server

* Fri Nov 30 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.7-1.el5.fmi
- Marko added new features

* Mon Nov 19 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.7-1.el5.fmi
- Linked with newbase 1.0.4-1

* Thu Nov 15 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.6-1.el5.fmi
- Linked with newbase 1.0.3-1

* Thu Oct 18 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.5-1.el5.fmi
- New improvements from Marko Pietarinen

* Mon Sep 24 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.4-1.el5.fmi
- Fixed "make depend".

* Fri Sep 14 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.3-1.el5.fmi
- Added "make tag" feature

* Thu Sep 13 2007 mheiskan <mika.heiskanen@fmi.fi> - 1.0.2-1.el5.fmi
- Improved make system plus new code from Pietarinen

* Thu Jun  7 2007 tervo <tervo@xodin.weatherproof.fi> - 
- Initial build.
