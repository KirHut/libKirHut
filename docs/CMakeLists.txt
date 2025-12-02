## The KirHut Software Development Library
## Documentation CMake subdirectory file
##
## Copyright © KirHut Software Company

option(KH_BUILD_DOCS "Build library documentation along with the default build (ALL)." OFF)
option(KH_DOCS_INTERNAL "Generate internal documentation rather than user documentation." OFF)
option(KH_DOCS_GENERATE_QCH "Generate a .qch file along with the HTML documentation." OFF)
option(KH_DOCS_GENERATE_DOCSET "Generate an Apple Xcode Docset along with the HTML documentation." OFF)
option(KH_DOCS_SUPPRESS_WARNINGS "Suppress warning generation from Doxygen." OFF)
option(KH_DOCS_WARNINGS_ARE_ERRORS "Treat Doxygen warnings as errors." OFF)
option(KH_DOCS_CLANG_PARSING "Use clang-assisted parsing in Doxygen for more accurate results." ON)

mark_as_advanced(
    KH_DOCS_INTERNAL
    KH_DOCS_SUPPRESS_WARNINGS
    KH_DOCS_WARNINGS_ARE_ERRORS
    KH_DOCS_CLANG_PARSING
)

if(NOT Doxygen_FOUND)
    # This should never happen since the docs folder isn't included unless Doxygen is found.
    message(FATAL_ERROR "Doxygen is required at this point, but it wasn't found. Please report this bug.")
endif()

if(KH_DOCS_INTERNAL)
    set(KH_INTERNAL_DOCS YES)
    set(KH_EXTRACT_PRIVATE YES)
    set(KH_HIDE_UNDOC NO)
    set(KH_QUIET_DOCS NO)
    set(KH_WARN_IF_UNDOCUMENTED YES)
    set(KH_USER_OR_INTERNAL_DOCS " (Internal)")
else()
    set(KH_INTERNAL_DOCS NO)
    set(KH_EXTRACT_PRIVATE NO)
    set(KH_HIDE_UNDOC YES)
    set(KH_QUIET_DOCS YES)
    set(KH_WARN_IF_UNDOCUMENTED NO)
    set(KH_USER_OR_INTERNAL_DOCS "")
endif()

if(KH_DOCS_GENERATE_QCH)
    set(KH_GENERATE_QCH YES)
else()
    set(KH_GENERATE_QCH NO)
endif()

if(KH_DOCS_GENERATE_DOCSET)
    set(KH_GENERATE_DOCSET YES)
else()
    set(KH_GENERATE_DOCSET NO)
endif()

if(KH_DOCS_SUPPRESS_WARNINGS)
    set(KH_EMIT_WARNINGS NO)
else()
    set(KH_EMIT_WARNINGS YES)
endif()

if(KH_DOCS_WARNINGS_ARE_ERRORS)
    set(KH_DOCS_ERROR_ON_WARN YES)
else()
    set(KH_DOCS_ERROR_ON_WARN NO)
endif()

if(KH_DOCS_CLANG_PARSING)
    set(KH_CLANG_ASSISTED_PARSING YES)
else()
    set(KH_CLANG_ASSISTED_PARSING NO)
endif()

configure_file(Doxyfile.in Doxyfile @ONLY)

if(KH_BUILD_DOCS)
    set(KH_DOCS_ALL_FLAG ALL)
else()
    set(KH_DOCS_ALL_FLAG)
endif()

doxygen_add_docs(docs
    ${PROJECT_SOURCE_DIR}/include
    ${KH_DOCS_ALL_FLAG}
    CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    COMMENT "Generating documentation with Doxygen."
)

set_target_properties(docs PROPERTIES FOLDER "Documentation" VERBATIM TRUE)
