#!/bin/sh

set -eu

prefix="$1"
pkgdir="$2"
exedir="${3:-}"

case "$pkgdir" in
  "$prefix"* )
    pkg_path="\${${exedir}prefix}${pkgdir#$prefix}"
  ;;
  * )
    pkg_path="$pkgdir"
  ;;
esac

printf %s\\n "$pkg_path"

exit 0
