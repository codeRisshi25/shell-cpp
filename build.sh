#!/bin/sh
set -e

(
  cd "$(dirname "$0")" # Ensure compile steps are run within the repository directory
  
  # Check if build directory already exists
  if [ ! -d "build" ]; then
    # First-time setup with vcpkg
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$(pwd)/vcpkg/scripts/buildsystems/vcpkg.cmake
  else
    # Incremental build - skip vcpkg checks if CMakeCache.txt exists
    if [ ! -f "build/CMakeCache.txt" ]; then
      cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$(pwd)/vcpkg/scripts/buildsystems/vcpkg.cmake
    fi
  fi
  
  # Build the project
  cmake --build ./build
)

exec ./build/shell "$@"