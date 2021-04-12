set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMPONENT_SRCS 
    "mainx.cpp"
    "main.c" 
)
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()
