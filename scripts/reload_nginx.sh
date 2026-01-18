#!/usr/bin/env bash
set -euo pipefail

DEV="${1:-}"
PORT="${2:-8100}"

if [[ -z "$DEV" ]]; then
  echo "Käyttö: $0 <dev-nimi> [nginx-portti]  esim: $0 erkki 8800"
  exit 1
fi

cd "$(dirname "$0")/.."

echo "Testataan Nginx-konfiguraatio (nginx -t)..."
docker compose --project-name "$DEV" \
  -f docker-compose.yml \
  -f "dev-envs/$DEV/docker-compose.override.yml" \
  run --rm nginx nginx -t

echo "uudelleenlataa  nginx prosessiin kontissa: ${DEV}-nginx-1"
docker exec "${DEV}-nginx-1" nginx -s reload || {
  echo "ℹ️  Kontti ei ehkä ole vielä käynnissä. Yritetään käynnistää se..."
  docker compose --project-name "$DEV" \
    -f docker-compose.yml \
    -f "dev-envs/$DEV/docker-compose.override.yml" \
    up -d nginx
}

echo "Health-check http://localhost:${PORT}/health"
if curl -fsS "http://localhost:${PORT}/health" >/dev/null; then
  echo "Nginx terve (${PORT})"
else
  echo "Nginx health ei onnistunu prkl... (${PORT})"
  exit 1
fi
