set(LIBRARY_DIR "${ZEROSERVER_PROJECT_DIR}/contrib/http-parser")

add_library(http_parser ${LIBRARY_DIR}/http_parser.h ${LIBRARY_DIR}/http_parser.c)
target_include_directories(http_parser PUBLIC ${LIBRARY_DIR})
