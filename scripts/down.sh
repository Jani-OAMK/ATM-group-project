#!/usr/bin/env bash
set -euo pipefail

DEV="${1:-}"
if [[ -z "$DEV" ]]; then
  echo "Käyttö: $0 <dev-nimi>   esim: $0 jani"
  exit 1
fi

cd "$(dirname "$0")/.."

echo "Sammutetaan ja poistetaan volyymit projektille: $DEV"
docker compose --project-name "$DEV" \
  -f docker-compose.yml \
  -f "dev-envs/$DEV/docker-compose.override.yml" \
  down --volumes

echo "Testi stack sammutettu ja volyymit poistettu ($DEV)."
