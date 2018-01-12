# Add executable called "helloDemo" that is built from the source files
# "demo.cxx" and "demo_b.cxx". The extensions are automatically found.

add_executable (alsaMinCapture-${ARCH} alsaMinCapture.c)
add_executable (pulseMinCapture-${ARCH} pulseMinCapture.c)

# Link the executable to the Hello library. Since the Hello library has
# public include directories we will use those link directories when building
# helloDemo
target_link_libraries (alsaMinCapture-${ARCH} LINK_PUBLIC asound)
target_link_libraries (pulseMinCapture-${ARCH} LINK_PUBLIC pulse-simple pulse)
