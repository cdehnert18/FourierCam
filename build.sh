#!/bin/bash

# Create build dir (wipe if exists)
meson setup build --wipe
if [ $? -ne 0 ]; then
  echo "Meson setup failed."
  exit 1
fi

# Compile
meson compile -C build
if [ $? -ne 0 ]; then
  echo "Build failed."
  exit 1
fi

# Run only if build succeeded
./build/src/fourierCam