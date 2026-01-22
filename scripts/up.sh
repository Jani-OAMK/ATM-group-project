#!/usr/bin/env bash
set -euo pipefail

DEV="${1:-}"
if [[ -z "$DEV" ]]; then
  echo "Käyttö: $0 <dev-nimi>   esim: $0 jani"
  exit 1
fi

cd "$(dirname "$0")/.."

# konfiguraation tarkistus ennen ajoa
docker compose --project-name "$DEV" \
  -f docker-compose.yml \
  -f "dev-envs/$DEV/docker-compose.override.yml" \
  config >/dev/null

echo "Compose-konfiguraatio OK (project: $DEV). Käynnistetään..."
docker compose --project-name "$DEV" \
  -f docker-compose.yml \
  -f "dev-envs/$DEV/docker-compose.override.yml" \
  up -d --build

echo "Testi stack käynnissä projektille: $DEV"
