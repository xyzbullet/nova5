#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
out_dir=${1:-"$root/build/compat"}

sh "$root/tests/compatibility/build-linux-fixture.sh" "$out_dir"
sh "$root/tests/compatibility/build-windows-fixture.sh" "$out_dir"
