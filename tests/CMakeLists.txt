# Creates the binary file (e.g., mms-test.exe) that will run all unit tests.
add_executable(mms-test MazeGen-Test.cpp)

# Connects the test executable to the necessary compiled code.
target_link_libraries(mms-test 
		PRIVATE 
				# Links to the GoogleTest main library, which provides the main() function 
				# to run the tests and the testing framework itself.
				GTest::gtest_main 
				
				# Links to your project's core logic library (defined in the root CMakeLists.txt).
				# This is essential for calling the functions that we want test.
				mms-core
)

# Includes GoogleTest-specific CMake macros. This is necessary for gtest_discover_tests.
include(GoogleTest)

# Searches the compiled executable (mms-test) for all defined tests (using TEST/TEST_F macros)
# and registers them with CTest. This allows you to run tests using the 'ctest' command.
gtest_discover_tests(mms-test)
