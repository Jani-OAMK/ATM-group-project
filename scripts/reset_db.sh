#!/usr/bin/env bash
set -euo pipefail

DEV="${1:-}"
if [[ -z "$DEV" ]]; then
  echo "Käyttö: $0 <dev-nimi>   esim: $0 jani"
  exit 1
fi

cd "$(dirname "$0")/.."

# Down + volumes -> MySQL volyymi nollautuu, init.sql ajetaan
echo "Resetoi DB ja stack projektille: $DEV"
docker compose --project-name "$DEV" \
  -f docker-compose.yml \
  -f "dev-envs/$DEV/docker-compose.override.yml" \
  down --volumes

docker compose --project-name "$DEV" \
  -f docker-compose.yml \
  -f "dev-envs/$DEV/docker-compose.override.yml" \
  up -d --build

echo "Odotetaan kannan tervehtymistä..."
# Pollaa healthiä max  ~60s
for i in {1..30}; do
  if docker inspect -f '{{.State.Health.Status}}' "${DEV}-mysql-1" 2>/dev/null | grep -q healthy; then
    echo "Kanta taas terve! Up & running"
    break
  fi
  sleep 2
done

echo "Avataan MySQL-shell (atm/atmpw, atmdb). Poistu \\q tai Ctrl+D."
docker exec -it "${DEV}-mysql-1" mysql -u atm -patmpw atmdb
