CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

SET ( COMMUNICATIONINCLUDES
    GBaseClientT.hpp
    GAsioConsumerT.hpp
    GCourtierHelperFunctions.hpp
    GBaseConsumerT.hpp
    GCommandContainerT.hpp
    GExecutorT.hpp
    GBrokerT.hpp
    GBufferPortT.hpp
    GCourtierEnums.hpp
    GProcessingContainerT.hpp
    GSerialConsumerT.hpp
    GStdThreadConsumerT.hpp
    GWebsocketConsumerT.hpp
    GWorkerT.hpp
)

set_source_files_properties(
    ${COMMUNICATIONINCLUDES}
    PROPERTIES
    LANGUAGE CXX
    CXX_STANDARD 14
)

# This is a workaround for a CLion-problem -- see CPP270 in the JetBrains issue tracker
ADD_CUSTOM_TARGET(CLION_ALL_COURTIER_HEADERS SOURCES ${COMMUNICATIONINCLUDES})

SET_TARGET_PROPERTIES(
    CLION_ALL_COURTIER_HEADERS
    PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED 14
)

INSTALL ( FILES ${COMMUNICATIONINCLUDES} DESTINATION ${INSTALL_PREFIX_INCLUDES}/courtier )
