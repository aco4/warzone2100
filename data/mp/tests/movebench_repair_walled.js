include("tests/movebench_common.js");
include("tests/movebench_repair_common.js");

// An idle truck with three damaged structures within reach of its search, each walled in on every tile a
// truck could repair it from. It must stay where it is rather than set off for one, or alternate between
// them, and pick none of them (the scorecard's autoWorkPicks).

var truck = null;
var targets = [];

function check()
{
	removeTimer("rpSample");
	var healths = targets.map(rpHealth);
	var untouched = healths.every(function (h) { return h === RP_DAMAGED; });
	var stray = rpStray(truck);
	rpReport("walled", untouched && stray <= 1,
	         "the truck strayed up to " + stray + " tile(s), structures at " + healths.join(", ") + "%");
}

function eventStartLevel()
{
	hackNetOff();
	rpEnable();
	var spots = [[RP_X + 3, RP_Y], [RP_X - 3, RP_Y], [RP_X, RP_Y + 4]];
	for (var i = 0; i < spots.length; i++)
	{
		targets.push(rpDamaged(spots[i][0], spots[i][1]));
		rpWallRing(spots[i][0], spots[i][1], 1, 1);
	}
	truck = rpTruck(RP_X, RP_Y);
	hackNetOn();

	rpWatch(truck);
	setTimer("rpSample", 500);
	queue("check", 25000);
	debug("movebench: repair_walled");
}
