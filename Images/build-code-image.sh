#!/usr/bin/env bash

# http://redsymbol.net/articles/unofficial-bash-strict-mode/
set -euo pipefail
IFS=$'\n\t'


SOURCE_IMAGE="./system710-macbrew-dev.image"
TARGET_IMAGE="./system710-macbrew-dev-with-code.image"

usage() { echo "Usage: $0 [-f]" 1>&2; exit 1; }

FORCE=false

while getopts "f" o; do
    case "${o}" in
        f)
            FORCE=true
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))


if  [ -f "$TARGET_IMAGE" ] && [ "$FORCE" = false ]; then
    echo "'$TARGET_IMAGE' exists. Delete it or use '-f' switch to overwrite it."
    exit 1
fi

rm -f "$TARGET_IMAGE"
cp "$SOURCE_IMAGE" "$TARGET_IMAGE"
hmount "$TARGET_IMAGE"
hcd :Development:Projects:macbrew-ui
hcopy ../macbrew-ui/* :
humount

echo "Image with embedded code written to '$TARGET_IMAGE'"
