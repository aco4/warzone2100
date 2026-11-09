// Shared setup for the scenarios about idle trucks picking their own work (checkForDamagedStruct):
// repair_reach, repair_walled, repair_hold and helpbuild_walled.
//
// Each case prints "repairpick: <case>: ok" or "BUG", which tests/gamestate/repairpick.sh checks
// together with the scorecard's autoWorkPicks, the number of times an idle truck picked a structure
// to work on by itself. A truck fixed on something it cannot work on picks it again every tick.
//
// Not a benchmark. Include tests/movebench_common.js first.

const ME = 0;
const RP_X = X_GAP;
const RP_Y = Y_OPEN;
const RP_TARGET = "PillBox1";
const RP_WALL = "A0HardcreteMk1Wall";
const RP_DAMAGED = 50;  // percent

function rpReport(name, ok, detail)
{
	debug("repairpick: " + name + ": " + (ok ? "ok" : "BUG") + " (" + detail + ")");
}

function rpEnable()
{
	benchEnable(ME, "truck");
	enableStructure(RP_TARGET, ME);
	enableStructure(RP_WALL, ME);
}

// Walls on every tile in the ring `ring` tiles out from the footprint of a size x size structure whose
// footprint starts at (x0, y0). Every tile a truck could work on it from is then a wall, so it can't be reached.
function rpWallRing(x0, y0, size, ring)
{
	for (var y = y0 - ring; y <= y0 + size - 1 + ring; y++)
	{
		for (var x = x0 - ring; x <= x0 + size - 1 + ring; x++)
		{
			if (x === x0 - ring || x === x0 + size - 1 + ring || y === y0 - ring || y === y0 + size - 1 + ring)
			{
				addStructure(RP_WALL, ME, x * 128, y * 128);
			}
		}
	}
}

// A damaged one-tile structure at (x, y), returned by id.
function rpDamaged(x, y)
{
	var s = addStructure(RP_TARGET, ME, x * 128, y * 128);
	setHealth(s, RP_DAMAGED);
	return s.id;
}

function rpHealth(id)
{
	var s = getObject(STRUCTURE, ME, id);
	return s === null ? -1 : s.health;
}

function rpTruck(x, y)
{
	return benchSpawnFixed(ME, "truck", x, y, 1, +1, 1, 1)[0];
}

// Tracks how far a truck strays from where it started, sampled by rpWatch.
var rpWatched = [];

function rpWatch(droid)
{
	rpWatched.push({ id: droid.id, x: droid.x, y: droid.y, maxStray: 0 });
}

function rpSample()
{
	for (var i = 0; i < rpWatched.length; i++)
	{
		var w = rpWatched[i];
		var d = getObject(DROID, ME, w.id);
		if (d !== null)
		{
			w.maxStray = Math.max(w.maxStray, Math.abs(d.x - w.x), Math.abs(d.y - w.y));
		}
	}
}

function rpStray(droid)
{
	for (var i = 0; i < rpWatched.length; i++)
	{
		if (rpWatched[i].id === droid.id)
		{
			return rpWatched[i].maxStray;
		}
	}
	return -1;
}
