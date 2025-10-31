add_google_test(SuffixTreeBasic basic.cpp)
add_google_test(SuffixTreeStress stress.cpp)

target_include_directories(SuffixTreeBasic PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(SuffixTreeStress PRIVATE ${CMAKE_SOURCE_DIR})
