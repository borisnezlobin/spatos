#!/usr/bin/env bash

set -e

source config.sh

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
        echo "$recipe_path: using target/release/cook"
        target/release/cook --fetch-only "$recipe_path"
    else
        echo "$recipe_path": using ./cook.sh\"
        ./cook.sh "$recipe_path" fetch
    fi
done
