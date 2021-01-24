#ifndef __INCLUDED_SRC_HCI_COMMANDER_INTERFACE_H__
#define __INCLUDED_SRC_HCI_COMMANDER_INTERFACE_H__

#include <vector>
#include "../objectdef.h"
#include "../hci.h"
#include "objects_stats.h"

class CommanderController: public BaseStatsController, public std::enable_shared_from_this<CommanderController>
{
public:
	STRUCTURE_STATS *getObjectStatsAt(size_t objectIndex) const override;

	size_t statsSize() const override
	{
		return 0;
	}

	size_t objectsSize() const override
	{
		return commanders.size();
	}

	DROID *getObjectAt(size_t index) const override
	{
		return commanders.at(index);
	}

	bool findObject(std::function<bool (BASE_OBJECT *)> iteration) const override
	{
		return BaseObjectsController::findObject(commanders, iteration);
	}

	void updateData();
	bool showInterface() override;
	void refresh() override;
	void displayOrderForm();

	DROID *getHighlightedObject() const override
	{
		return highlightedCommander;
	}

	void setHighlightedObject(BASE_OBJECT *object) override
	{
		auto commander = castDroid(object);
		ASSERT_NOT_NULLPTR_OR_RETURN(, commander);
		ASSERT_OR_RETURN(, commander->droidType == DROID_COMMAND, "Droid is not a commander");
		highlightedCommander = commander;
	}

private:
	void updateCommandersList();
	std::vector<DROID *> commanders;
	static DROID *highlightedCommander;
};

#endif // __INCLUDED_SRC_HCI_COMMANDER_INTERFACE_H__