#!/usr/bin/env bash
set -euo pipefail

declare -A DEV_PORTS=(
  [jani]=8100
  [marianna]=8200
  [joona]=8300
  [emil]=8400
)

for dev in "${!DEV_PORTS[@]}"; do
  port="${DEV_PORTS[$dev]}"
  url="http://localhost:${port}/health"
  printf "käyttäjä  %s -> %s ... " "$dev" "$url"
  if curl -fsS "$url" >/dev/null; then
    echo "OK"
  else
    echo "FAILED"
  fi
done
