#!/usr/bin/env bash
set -euo pipefail
DEV="${1:-}"
if [[ -z "$DEV" ]]; then
  echo "Käyttö: $0 <dev-nimi>   esim: $0 jani"
  exit 1
fi
docker compose --project-name "$DEV" \
  -f docker-compose.yml \
  -f "dev-envs/$DEV/docker-compose.override.yml" \
  config
