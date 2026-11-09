#!/bin/sh
# Idle truck work-pick test: runs the movement bench scenarios in which idle trucks pick structures to
# repair or help build by themselves (checkForDamagedStruct), and checks that they pick only what they can work on.
# - repair_reach: a walled-in damaged structure and an open one, the truck must repair the open one.
# - repair_walled: only walled-in damaged structures, the truck must stay put and pick none.
# - repair_hold: a truck holding position must not pick a damaged structure it can't repair without moving,
#   and one standing next to a damaged structure must repair it.
# - helpbuild_walled: a half built factory walled in at the ring, a truck must still help build it from outside
#   the wall (needs the build-from-here feature, on by default).
# Each scenario prints "repairpick: <case>: ok" or "BUG". The scorecard's autoWorkPicks counts the picks, and a truck
# fixed on something it can't work on picks it again every tick, so it is held to a small number.
#
# usage: repairpick.sh <warzone2100 binary> [extra game arguments]
# Environment: OUT (default /tmp/gamestate-forkcrc), RUN_TIMEOUT (default 300 seconds).
# Exit status: 0 passed, 1 a case failed, 2 a run failed.
set -u
BIN=${1:?binary}
shift
OUT=${OUT:-/tmp/gamestate-forkcrc}
RUN_TIMEOUT=${RUN_TIMEOUT:-300}
CFG="$OUT/config"
mkdir -p "$CFG"

# scenario, then the most picks allowed over the run: one per structure a truck is meant to work on, plus slack
# for a repick after an interruption. A fixation costs one pick per tick, a couple of hundred over the run.
CASES="repair_reach 4
repair_walled 0
repair_hold 4
helpbuild_walled 4"

rc=0
echo "$CASES" | while read -r scenario maxpicks; do
	LOG="$OUT/repairpick-$scenario.log"
	# shellcheck disable=SC2068
	timeout "$RUN_TIMEOUT" "$BIN" --configdir="$CFG" --movementbench="$scenario" --gamestate-test-seed=1 $@ > "$LOG" 2>&1 \
		|| { echo "repairpick: $scenario: RUN FAILED (see $LOG)"; exit 2; }
	OK=$(grep -c 'repairpick: [a-z-]*: ok' "$LOG")
	BUGS=$(grep -c 'repairpick: [a-z-]*: BUG' "$LOG")
	PICKS=$(sed -n 's/^ *"autoWorkPicks": *\([0-9]*\).*/\1/p' "$LOG" | head -1)
	if [ -z "$PICKS" ]; then
		echo "repairpick: $scenario: no scorecard (see $LOG)"
		exit 2
	fi
	if [ "$OK" -eq 1 ] && [ "$BUGS" -eq 0 ] && [ "$PICKS" -le "$maxpicks" ]; then
		echo "repairpick: $scenario: ok, $PICKS pick(s)"
	else
		echo "repairpick: $scenario: FAILED, $OK ok, $BUGS BUG, $PICKS pick(s) of at most $maxpicks (log: $LOG)"
		grep 'repairpick: [a-z-]*: \(ok\|BUG\)' "$LOG" | sed 's/^.*repairpick: /  /'
		exit 1
	fi
done || rc=$?
exit $rc
