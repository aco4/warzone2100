include("tests/movebench_common.js");
include("tests/movebench_repair_common.js");

// A factory left half built, then walled in on every tile of the ring a truck normally builds from.
// A truck may still join a build already under way from one tile outside that ring (PF_BUILD_FROM_HERE,
// on by default), so an idle truck three tiles away must still pick it and help build it from outside the wall.
// Run with the default feature set: with build from here off, the factory is out of reach.

const HW_STRUCT = "A0LightFactory";   // 3x3, footprint RP_X - 1 .. RP_X + 1
const HW_ABANDON_MS = 4000;          // still being built then (as in the dead_grid scenario)
const HW_CHECK_MS = 20000;
const DACTION_BUILD_VALUE = 2;

var builder = null;
var helper = null;
var helperBuilt = false;

function abandon()
{
	var b = getObject(DROID, ME, builder);
	if (b !== null)
	{
		removeObject(b);
	}
	var s = enumStruct(ME, HW_STRUCT);
	if (s.length === 0 || s[0].status !== BEING_BUILT)
	{
		rpReport("helpbuild-walled", false, "no factory left half built");
		return;
	}
	hackNetOff();
	rpWallRing(RP_X - 1, RP_Y - 1, 3, 1);
	helper = rpTruck(RP_X - 4, RP_Y).id;  // three tiles from the footprint, one past the zone it may build from
	hackNetOn();
	setTimer("watchHelper", 100);
	queue("check", HW_CHECK_MS);
}

function watchHelper()
{
	var h = getObject(DROID, ME, helper);
	if (h !== null && h.action === DACTION_BUILD_VALUE)
	{
		helperBuilt = true;
	}
}

function check()
{
	removeTimer("watchHelper");
	var s = enumStruct(ME, HW_STRUCT);
	var status = s.length === 0 ? "gone" : s[0].status === BUILT ? "built" : "being built";
	rpReport("helpbuild-walled", helperBuilt, "the helper " + (helperBuilt ? "helped build" : "never built") + ", the factory is " + status);
}

function eventStartLevel()
{
	hackNetOff();
	rpEnable();
	enableStructure(HW_STRUCT, ME);
	var b = rpTruck(RP_X, RP_Y + 3);
	builder = b.id;
	hackNetOn();
	orderDroidBuild(b, DORDER_BUILD, HW_STRUCT, RP_X, RP_Y);
	queue("abandon", HW_ABANDON_MS);
	debug("movebench: helpbuild_walled");
}
