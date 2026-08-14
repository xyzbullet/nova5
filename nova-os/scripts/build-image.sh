#!/bin/sh
set -eu

if [ "$#" -ne 1 ]; then
  printf 'usage: %s BUILD_DIR\n' "$0" >&2
  exit 2
fi

build_dir=$1

case "$(uname -s)" in
  FreeBSD)
    if ! command -v makefs >/dev/null 2>&1 || ! command -v mkimg >/dev/null 2>&1; then
      printf '%s\n' 'FreeBSD image tools makefs and mkimg are required to build an image.' >&2
      exit 1
    fi
    ;;
  *)
    printf '%s\n' \
      'NovaOS image builds require a FreeBSD build host or a prepared cross-build environment.' \
      'The source skeleton is ready; no placeholder ISO is generated on this host.' >&2
    exit 1
    ;;
esac

mkdir -p "$build_dir/image"
printf '%s\n' 'Image assembly is reserved for the FreeBSD build pipeline.' >"$build_dir/image/README.txt"
printf 'Image staging directory: %s/image\n' "$build_dir"