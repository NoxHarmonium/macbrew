#!/usr/bin/env bash

# Starts the BasiliskII emulator with the project default preferences
# Without that switch, it will create a new pref file in the home drive
# It doesn't seem possible to specify the pram file at this time
# --nogui refers to the config gui, the actual emulator GUI still appears
BasiliskII --config basilisk_ii_prefs --nogui true
