#!/usr/bin/env bash

# Run this when you have changed code inside the emulator and you want
# to pull the code out so you can commit the changes to git

# http://redsymbol.net/articles/unofficial-bash-strict-mode/
set -euo pipefail
IFS=$'\n\t'


TARGET_IMAGE="./system710-macbrew-dev-with-code.image"

hmount "$TARGET_IMAGE"
hcd :Development:Projects:macbrew-ui
hcopy ":*" "../macbrew-ui/"
humount

echo "Files extracted from '$TARGET_IMAGE' and written to '../macbrew-ui/'"
