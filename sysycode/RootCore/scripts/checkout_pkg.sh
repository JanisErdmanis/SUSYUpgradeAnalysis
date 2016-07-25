#!/bin/bash

set -e
set -u

exec rc --internal manage_pkg checkout "$1"
