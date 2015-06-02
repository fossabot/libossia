cmake_minimum_required(VERSION 3.0)
project(APITests)

include_directories("${API_IMPL_FOLDER}/Jamoma/Sources")

find_package(Qt5 5.4 REQUIRED COMPONENTS Core Test)
find_package(Jamoma 0.6)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

function(addAPIQtTest TESTNAME TESTSRCS)
    if(${Jamoma_FOUND})
        add_executable(Jamoma_${TESTNAME} ${TESTSRCS})
        target_link_libraries(Jamoma_${TESTNAME} Qt5::Core Qt5::Test Jamoma::Modular Jamoma::Foundation APIJamoma Jamoma::Score)
        add_test(NAME Jamoma_target_${TESTNAME} COMMAND Jamoma_${TESTNAME})
    endif()
#		add_executable(Mock_${TESTNAME} ${TESTSRCS})
#		target_link_libraries(Mock_${TESTNAME} Qt5::Core Qt5::Test Jamoma::Modular Jamoma::Foundation APIMock)
#		add_test(Mock_target_${TESTNAME} ${TESTNAME})

endFunction()

#addAPIQtTest(APIScenarioTest	    "${CMAKE_CURRENT_SOURCE_DIR}/ScenarioTest.cpp")
#addAPIQtTest(APITimeEventTest	    "${CMAKE_CURRENT_SOURCE_DIR}/TimeEventTest.cpp")

addAPIQtTest(APIDeviceTest	    "${CMAKE_CURRENT_SOURCE_DIR}/Network/DeviceTest.cpp")
addAPIQtTest(APINodeTest	    "${CMAKE_CURRENT_SOURCE_DIR}/Network/NodeTest.cpp")
addAPIQtTest(APIAddressTest	    "${CMAKE_CURRENT_SOURCE_DIR}/Network/AddressTest.cpp")

include(CTest)