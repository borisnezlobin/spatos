#!/bin/bash

# URL of the tarball
TARBALL_URL="https://gitlab.redox-os.org/redox-os/cookbook/-/archive/master/cookbook-master.tar.gz"

# Temporary directory to extract the tarball
TEMP_DIR=$(mktemp -d)

# Download and extract the tarball
echo "Downloading and extracting tarball..."
curl -L $TARBALL_URL | tar -xz -C $TEMP_DIR --strip-components=1

# Move recipe.toml files to the current directory
echo "Moving recipe.toml files..."
find $TEMP_DIR -type f -name 'recipe.toml' | while read -r recipe_file; do
    relative_path=$(realpath --relative-to="$TEMP_DIR" "$recipe_file")
    target_path="$(pwd)/cookbook/$relative_path"
    mkdir -p "$(dirname "$target_path")"
    mv "$recipe_file" "$target_path"
done

echo "Done getting recipes."

echo "Moving .patch files..."
find $TEMP_DIR -type f -name '*.patch' | while read -r patch_file; do
    relative_path=$(realpath --relative-to="$TEMP_DIR" "$patch_file")
    target_path="$(pwd)/cookbook/$relative_path"
    mkdir -p "$(dirname "$target_path")"
    mv "$patch_file" "$target_path"
done

echo "Done moving .patch files"

echo "Moving recipe.sh files..."
find $TEMP_DIR -type f -name 'recipe.sh' | while read -r patch_file; do
    relative_path=$(realpath --relative-to="$TEMP_DIR" "$patch_file")
    target_path="$(pwd)/cookbook/$relative_path"
    mkdir -p "$(dirname "$target_path")"
    mv "$patch_file" "$target_path"
done

echo "Done moving recipe.sh files"

# Cleanup temporary directory
rm -rf $TEMP_DIR