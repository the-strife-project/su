#include <cstdio>
#include <syscalls>
#include <users>
#include <switcher>
#include <registry>

static size_t pubSwitch(std::PID client, size_t uid) {
	// UID exists?
	auto name = std::uidToName(uid);
	if(!name.size())
		return std::switcher::NOT_FOUND;

	// Allowed?
	auto info = std::info(client);
	auto who = info.uid;
	if(!who)
		return std::switcher::CONNECTION_ERROR; // hmmm

	if(who != 1) {
		// Check registry
		std::string path = "/u/";
		path += std::uToStr(uid);
		path += "/SUPER/";
		path += std::uToStr(who);
		if(std::registry::exists(path) != std::registry::OK)
			return std::switcher::NOT_ALLOWED;
	}

	std::sysSwitchUser(client, uid);
	return std::switcher::OK;
}

extern "C" void _start() {
	std::exportProcedure((void*)pubSwitch, 1);
	std::enableRPC();
	if(!std::publish("switcher"))
		std::exit(1);
	std::halt();
}
