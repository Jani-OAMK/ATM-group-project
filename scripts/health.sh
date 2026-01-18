#!/usr/bin/env bash
set -euo pipefail

PORT="${1:-8100}"
URL="http://localhost:${PORT}/health"

echo "Health-check: $URL"
if curl -fsS "$URL" >/dev/null; then
  echo "\o/ OK \o/ ($PORT)"
  exit 0
else
  echo ":( FAILED :( ($PORT)"
  exit 1
fi
