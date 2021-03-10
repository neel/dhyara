#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMPONENT_SRCS 
    "application.cpp" 
    "mainx.cpp"
    "main.c" 
)
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()
