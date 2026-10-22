#!/usr/bin/env bash
# End-to-end network test: a headless server, raw ENet probes and headless clients on localhost.
# Usage: run.sh <directory with net_server, net_client and net_probe>
set -u

BIN="$1"
PORT="${MG_TEST_PORT:-18987}"
LOGS="$(mktemp -d)"
FAILED=0

expect() { # <file> <pattern> <expected count> <description>
    local count
    count=$(grep -c -- "$2" "$1")
    if [[ "$count" -eq "$3" ]]; then
        echo "ok:   $4"
    else
        echo "FAIL: $4 (found $count, expected $3)"
        FAILED=1
    fi
}

passes() { # <description> <command...>
    local description="$1"
    shift
    if "$@" > "$LOGS/last.log" 2>&1; then
        echo "ok:   $description"
    else
        echo "FAIL: $description"
        cat "$LOGS/last.log"
        FAILED=1
    fi
}

# No server on this port: the connection attempt times out (runs alongside the rest)
MG_TEST_PORT=$((PORT + 1)) "$BIN/net_client" timeout > "$LOGS/timeout.log" 2>&1 &
TIMEOUT_PID=$!

export MG_TEST_PORT="$PORT"

passes "invalid IP addresses are refused" "$BIN/net_client" invalid

"$BIN/net_server" 11 > "$LOGS/server.log" 2>&1 &
SERVER_PID=$!
sleep 0.5

# 5 connections: 3 accepted (P1-P3), 2 denied because the server is full
"$BIN/net_probe" 5 2500 > "$LOGS/full.log"
expect "$LOGS/full.log" "CONNECT_OK P[123] on channel 0 reliable" 3 "3 clients accepted on the reliable channel"
expect "$LOGS/full.log" "CONNECT_DENIED reason 0" 2 "2 clients denied: server full"
expect "$LOGS/full.log" "GAME_STATE 54 bytes on channel 1 unreliable" 3 "game state serialized (54 bytes) on the unreliable channel"
expect "$LOGS/full.log" "INVALID" 0 "no invalid messages"

# Clients of other protocol versions are denied
"$BIN/net_probe" 1 800 0 > "$LOGS/version.log"
"$BIN/net_probe" 1 800 2 >> "$LOGS/version.log"
expect "$LOGS/version.log" "CONNECT_DENIED reason 1" 2 "other protocol versions denied"

# A real client, twice: connects, sends its position, leaves with Esc
passes "client connects twice and receives the game state" "$BIN/net_client" connect

# Full server again, then a real client is denied
"$BIN/net_probe" 3 2500 > /dev/null &
PROBE_PID=$!
sleep 0.5
passes "client is denied by a full server" "$BIN/net_client" full
wait "$PROBE_PID"

if wait "$SERVER_PID"; then
    echo "ok:   server exited cleanly"
else
    echo "FAIL: server exited with an error"
    FAILED=1
fi
expect "$LOGS/server.log" "P1(x=123)" 2 "server received the client position"
expect "$LOGS/server.log" "server: final players P0(x=50)$" 1 "only the server player is left at the end"
expect "$LOGS/server.log" "runtime error\|AddressSanitizer" 0 "no sanitizer errors on the server"

if wait "$TIMEOUT_PID"; then
    echo "ok:   client times out without a server"
else
    echo "FAIL: client times out without a server"
    cat "$LOGS/timeout.log"
    FAILED=1
fi

if [[ "$FAILED" -ne 0 ]]; then
    echo "--- server log"
    cat "$LOGS/server.log"
fi
rm -rf "$LOGS"
exit "$FAILED"
