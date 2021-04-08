cmake_minimum_required(VERSION 3.19)
project(dhyara-docs)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(Doxygen)
find_package(Sphinx)

if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    set(DOXYGEN_XML_DIR ${CMAKE_CURRENT_BINARY_DIR}/xml)
    set(DOXYGEN_XML_INDEX ${DOXYGEN_XML_DIR}/index.xml)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_command(
        OUTPUT ${DOXYGEN_XML_INDEX} 
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
    
    add_custom_target(docs ALL DEPENDS ${DOXYGEN_XML_INDEX})
        
    if (SPHINX_FOUND)
        set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
        set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
        set(SPHINX_INDEX ${SPHINX_BUILD}/index.html)
        
        add_custom_command(
            OUTPUT ${SPHINX_INDEX}
            COMMAND ${SPHINX_EXECUTABLE} -b html -Dbreathe_projects.dhyara=${DOXYGEN_XML_DIR} ${SPHINX_SOURCE} ${SPHINX_BUILD}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            DEPENDS ${SPHINX_SOURCE}/index.rst ${DOXYGEN_XML_INDEX} 
            MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
            COMMENT "Generating documentation with Sphinx"
        )
        add_custom_target(sphinx ALL DEPENDS ${SPHINX_INDEX})
    else (SPHINX_FOUND)
        message("Sphinx need to be installed to generate the sphinx documentation")
    endif (SPHINX_FOUND)
        
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
