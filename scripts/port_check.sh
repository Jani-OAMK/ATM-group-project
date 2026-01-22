#!/usr/bin/env bash
set -euo pipefail

# Käyttö: ./scripts/port_check.sh 8100 3310 [3100]
if [[ $# -lt 1 ]]; then
  echo "Käyttö: $0 <portti1> [portti2] [portti3] ..."
  exit 1
fi

status=0
for P in "$@"; do
  if ss -lnt | awk '{print $4}' | grep -qE "[:.]${P}$"; then
    echo "Portti $P on varattu."
    status=1
  else
    echo "Portti $P on vapaa."
  fi
done
exit $status
