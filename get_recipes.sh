#!/bin/bash
# written by chatgpt, not sketchy at all
# URL of the tarball
TARBALL_URL="https://gitlab.redox-os.org/redox-os/cookbook/-/archive/master/cookbook-master.tar.gz"

# Temporary directory to extract the tarball
TEMP_DIR=$(mktemp -d)

# Download and extract the tarball
echo "Downloading and extracting tarball..."
curl -L $TARBALL_URL | tar -xz -C $TEMP_DIR --strip-components=1

# Move recipe.toml files to the current directory
echo "Moving recipe.toml files..."
find $TEMP_DIR -type f -name 'recipe.toml' -exec mv -t . {} +

# Cleanup temporary directory
rm -rf $TEMP_DIR

echo "Done"
