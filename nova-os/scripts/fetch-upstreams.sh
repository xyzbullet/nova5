#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
manifest=${1:-"$root/sources/phase3.1.toml"}
dest=${NOVA_UPSTREAM_DIR:-"$root/build/upstream"}
filter=${NOVA_UPSTREAM_FILTER:-}

if ! command -v git >/dev/null 2>&1; then
	printf '%s\n' 'git is required to fetch NovaOS upstream sources.' >&2
	exit 1
fi

mkdir -p "$dest"
awk '
	/^name = / { gsub(/"/, "", $3); name=$3 }
	/^url = / { gsub(/"/, "", $3); url=$3 }
	/^branch = / { gsub(/"/, "", $3); branch=$3; if (name != "" && url != "") print name "|" url "|" branch }
' "$manifest" |
while IFS='|' read -r name url branch; do
	case " $filter " in
		*" $name "*|"  ") ;;
		*) continue ;;
	esac
	target="$dest/$name"
	if [ -d "$target/.git" ]; then
		printf 'updating %s\n' "$name"
		git -C "$target" fetch --depth 1 origin "$branch"
		git -C "$target" checkout FETCH_HEAD
	else
		printf 'cloning %s from %s\n' "$name" "$url"
		git clone --depth 1 --branch "$branch" "$url" "$target"
	fi
	digest=$(git -C "$target" rev-parse --short=12 HEAD)
	printf '%s %s %s\n' "$name" "$branch" "$digest"
done
