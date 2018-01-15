# Create a library called "Hello" which includes the source file "hello.cxx".
# The extension is already found. Any number of sources could be listed here.
# add_library (Hello hello.cxx)
add_library ( deinterleave deinterleave.c deinterleave.h )
add_executable (main-${ARCH} main.c)

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
# target_include_directories (src PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (main-${ARCH} LINK_PUBLIC pulse-simple pulse deinterleave fftw3)
