CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

SET ( COMMONOPTTESTINCLUDES
    GCommon_tests.hpp
    GBoundedBufferT_tests.hpp
)

# This is a workaround for a CLion-problem -- see CPP270 in the JetBrains issue tracker
ADD_CUSTOM_TARGET(CLION_ALL_COMMON_TEST_HEADERS SOURCES ${COMMONOPTTESTINCLUDES})

INSTALL ( FILES ${COMMONOPTTESTINCLUDES} DESTINATION ${INSTALL_PREFIX_INCLUDES}/common/tests )
