include("tests/movebench_common.js");
include("tests/movebench_repair_common.js");

// Two trucks told to hold position.
// - far: a damaged structure two tiles away (inside REPAIR_RANGE) but not next to it, so it can't repair it
//   without moving. It must leave it alone, and not pick it every tick only for the repair action to drop it
//   again (the scorecard's autoWorkPicks).
// - near: a damaged structure right next to it, which it must repair from where it stands.
// They stand far enough apart to stay out of each other's way.

var farTruck = null;
var nearTruck = null;
var farTarget = null;
var nearTarget = null;

function check()
{
	removeTimer("rpSample");
	var f = rpHealth(farTarget);
	var n = rpHealth(nearTarget);
	var stray = Math.max(rpStray(farTruck), rpStray(nearTruck));
	rpReport("hold", f === RP_DAMAGED && n > RP_DAMAGED && stray === 0,
	         "out of reach structure at " + f + "%, the one next to the near truck at " + n + "%, the trucks strayed up to " + stray + " tile(s)");
}

function eventStartLevel()
{
	hackNetOff();
	rpEnable();
	farTarget = rpDamaged(RP_X + 2, RP_Y);
	farTruck = rpTruck(RP_X, RP_Y);
	nearTarget = rpDamaged(RP_X + 13, RP_Y);
	nearTruck = rpTruck(RP_X + 12, RP_Y);
	hackNetOn();

	orderDroid(farTruck, DORDER_HOLD);
	orderDroid(nearTruck, DORDER_HOLD);
	rpWatch(farTruck);
	rpWatch(nearTruck);
	setTimer("rpSample", 500);
	queue("check", 25000);
	debug("movebench: repair_hold");
}
