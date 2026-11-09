include("tests/movebench_common.js");
include("tests/movebench_repair_common.js");

// An idle truck between two damaged structures: the nearer is walled in on every tile a truck could
// repair it from, the farther is open. The truck must repair the open one, and must not fix on the
// walled one once that is done (each pick shows in the scorecard's autoWorkPicks).

var truck = null;
var walled = null;
var open = null;

function check()
{
	removeTimer("rpSample");
	var w = rpHealth(walled);
	var o = rpHealth(open);
	rpReport("reach", o > RP_DAMAGED && w === RP_DAMAGED,
	         "open structure at " + o + "%, walled one at " + w + "%, started at " + RP_DAMAGED + "%");
}

function eventStartLevel()
{
	hackNetOff();
	rpEnable();
	walled = rpDamaged(RP_X + 3, RP_Y);
	rpWallRing(RP_X + 3, RP_Y, 1, 1);
	open = rpDamaged(RP_X - 4, RP_Y);
	truck = rpTruck(RP_X, RP_Y);
	hackNetOn();

	rpWatch(truck);
	setTimer("rpSample", 500);
	queue("check", 25000);
	debug("movebench: repair_reach");
}
