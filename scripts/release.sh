#!/usr/bin/env bash
# Uso: scripts/release.sh X.Y.Z[-pre]
# Actualiza la versión en CMakeLists.txt, crea el commit y el tag vX.Y.Z.
# El push del tag dispara el workflow de release.
set -euo pipefail
cd "$(dirname "$0")/.."

ver="${1:?Uso: $0 X.Y.Z}"
[[ "$ver" =~ ^[0-9]+\.[0-9]+\.[0-9]+(-[0-9A-Za-z.-]+)?$ ]] || { echo "Versión no válida (semver): $ver" >&2; exit 1; }
[ -z "$(git status --porcelain)" ] || { echo "Hay cambios sin commitear" >&2; exit 1; }

sed -i "s/^project(kpomodoro VERSION [0-9.]*/project(kpomodoro VERSION ${ver%%-*}/" CMakeLists.txt
git commit -am "chore(release): v$ver"
git tag -a "v$ver" -m "v$ver"
echo "Listo. Publica con: git push origin main v$ver"
