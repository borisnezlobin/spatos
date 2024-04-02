#!/usr/bin/env bash
echo "starting fetch.sh"
set -e
echo "source from config.sh..."
source config.sh
echo "sourced!"

if [ $# = 0 ]
then
    recipes="$(target/release/list_recipes)"
else
    recipes="$@"
fi

for recipe_path in $recipes
do
    if [ -e "$recipe_path/recipe.toml" ]
    then
        target/release/cook --fetch-only "$recipe_path"
    else
        ./cook.sh "$recipe_path" fetch
    fi
done
