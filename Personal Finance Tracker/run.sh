#!/usr/bin/env bash
set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$DIR"

if ! command -v python3 &>/dev/null; then
  echo "ERROR: python3 not found"; exit 1
fi

if [ ! -d ".venv" ]; then
  echo "Creating virtual environment..."
  python3 -m venv .venv
fi

source .venv/bin/activate
pip install -q -r requirements.txt

echo ""
echo "  Personal Finance Tracker"
echo "  Open -> http://127.0.0.1:8000"
echo ""

python3 -m uvicorn main:app --host 127.0.0.1 --port 8000 --reload
