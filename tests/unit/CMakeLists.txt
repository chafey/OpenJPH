
# Tests need to be added as executables first
add_executable(unittest codestreamRoundTrip.cpp codeblockRoundTrip.cpp )

# I'm using C++17 in the test
target_compile_features(unittest PRIVATE cxx_std_17)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(unittest PRIVATE openjph Catch2::Catch2WithMain)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
add_test(NAME unittest COMMAND unittest) # Command can be a target

include(Catch)

#catch_discover_tests(unittest)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose DEPENDS unittest)