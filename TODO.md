# TODO
对齐TestEventListener和ProgressListener
对齐TestCase和TestSuite
对齐TestInfo和Test


  const char* const filter = GTEST_FLAG(filter).c_str();

  // Prints the filter if it's not *.  This reminds the user that some
  // tests may be skipped.
  if (!String::CStringEquals(filter, kUniversalFilter)) {
    ColoredPrintf(COLOR_YELLOW,
                  "Note: %s filter = %s\n", GTEST_NAME_, filter);
  }

--gtest_filter=IsPrimeTest.Negative
