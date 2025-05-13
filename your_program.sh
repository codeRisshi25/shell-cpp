#!/bin/sh
set -e

# Function to ensure vcpkg is available
ensure_vcpkg() {
  # If VCPKG_ROOT is not set or doesn't exist
  if [ -z "$VCPKG_ROOT" ] || [ ! -d "$VCPKG_ROOT" ]; then
    echo "Setting up vcpkg..."
    # Use existing vcpkg directory if it exists
    if [ -d "./vcpkg" ]; then
      export VCPKG_ROOT="$(pwd)/vcpkg"
    else
      git clone https://github.com/Microsoft/vcpkg.git
      ./vcpkg/bootstrap-vcpkg.sh
      export VCPKG_ROOT="$(pwd)/vcpkg"
    fi
  fi
  echo "Using vcpkg at: $VCPKG_ROOT"
}

# Main build process
(
  cd "$(dirname "$0")" # Ensure compile steps are run within the repository directory
  
  # Ensure vcpkg is available
  ensure_vcpkg
  
  # Build the project
  cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
  cmake --build ./build
)

exec ./build/shell "$@"