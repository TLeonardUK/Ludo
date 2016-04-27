// Copyright 2016 TwinDrills
#pragma once

// These constants should be passed in by the compiler when built for release
// by a build server. Otherwise they will default to dummy values.

#if !defined(LD_BUILD_CHANGELIST)
#define LD_BUILD_CHANGELIST 0
#endif

#if !defined(LD_BUILD_BRANCH)
#define LD_BUILD_BRANCH "Local"
#endif

#if !defined(LD_BUILD_TIMESTAMP)
#define LD_BUILD_TIMESTAMP __DATE__ " at " __TIME__
#endif