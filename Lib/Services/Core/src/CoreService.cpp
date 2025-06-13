#include <stdexcept>
#include <cstring>
#include <format>

#include "CoreServiceHW.h"
#include "CoreService.h"

static const char *comm[] = { "version", "show" };

enum SERV_CORE_COMMANDS {
	COMMANDS_VERSION = (1 << 0),
	COMMANDS_SHOW = (1 << 1),
	COMMANDS_COUNT = 2
};

CoreService::CoreService(const Services::ServicesCB &pcb)
	: Services("Core")
	, hw(new CoreServiceHW(
		  [this](uint32_t event) { return this->eventHandle(event); }))
{
	cb.func = pcb.func;
	cb.ctx = pcb.ctx;
}

CoreService::~CoreService()
{
	delete hw;
}

void CoreService::init(void)
{
	Services::init();
	setCoreCommands();
	hw->init();
}

void CoreService::setCoreCommands(void)
{
	uint32_t eventMask = 1;
	for (uint8_t i = 0; i < COMMANDS_COUNT; i++) {
		std::function<void(std::span<uint8_t>)> cb = nullptr;

		switch (eventMask) {
		case COMMANDS_VERSION:
			cb = [this](std::span<uint8_t> ctx) {
				this->versionCB(ctx);
			};
			break;
		case COMMANDS_SHOW:
			cb = [this](std::span<uint8_t> ctx) {
				this->showCB(ctx);
			};
			break;
		default:
			throw std::runtime_error("Commands without event");
			break;
		}
		setCommand(comm[i],
			   Services::commands_t{ eventMask,
						 std::span<uint8_t>(), cb },
			   coreBasisHash);

		eventMask <<= 1;
	}
}

void CoreService::pvr_cb(lwjson_stream_parser_t *jsp, lwjson_stream_type_t type)
{
	if (type == LWJSON_STREAM_TYPE_OBJECT)
		return;
	uint32_t cmdHash = 0;

	if ((jsp->stack_pos >= 4) &&
	    (lwjson_stack_seq_4(jsp, 0, OBJECT, KEY, OBJECT, KEY))) {
		if (std::strcmp(jsp->stack[3].meta.name, "cmd") == 0) {
			cmdHash = getHash(jsp->data.str.buff, coreBasisHash);
			auto it = commands.find(cmdHash);

			if (it != commands.end() && it->second.event &&
			    it->second.cb) {
				it->second.ctx = std::span<uint8_t>();
				hw->callEvents(it->second.event);
			}
			return;
		}
		cmdHash = getHash(jsp->stack[3].meta.name, coreBasisHash);
		auto it = commands.find(cmdHash);

		if (it != commands.end() && it->second.event && it->second.cb) {
			it->second.ctx = saveCTX(jsp->data.str.buff);
			hw->callEvents(it->second.event);
		}
	}
}

std::span<uint8_t> CoreService::saveCTX(const char *data)
{
	static size_t cur = 0;
	uint16_t len = std::strlen(data);

	if (cur + len > buf.size())
		cur = 0;

	if (cur + len > buf.size())
		return std::span<uint8_t>(buf.data(), 0);

	std::memcpy(buf.data() + cur, data, len);
	auto span = std::span<uint8_t>(buf.data() + cur, len);

	cur += len;
	return span;
}

void CoreService::versionCB(std::span<uint8_t> ctx)
{
	const char *subName = "version";
	std::array<char, 32> version = { 0 };

	std::snprintf(version.data(), version.size(), "%u.%u.%u",
		      SERVICES_VERSION_MAJOR, SERVICES_VERSION_MINOR,
		      SERVICES_VERSION_PATCH);

	uint8_t len = std::strlen(version.data());

	if (cb.func)
		std::invoke(cb.func, this, subName,
			    std::span<uint8_t>((uint8_t *)version.data(), len),
			    cb.ctx);
}

void CoreService::showCB(std::span<uint8_t> ctx)
{
	for (auto &[itHash, itServ] : map) {
		std::array<char, 32> strHash = { 0 };
		std::snprintf(strHash.data(), strHash.size(), "0x%lx", itHash);
		if (cb.func)
			std::invoke(
				cb.func, this, itServ->getName(),
				std::span<uint8_t>((uint8_t *)strHash.data(),
						   std::strlen(strHash.data())),
				cb.ctx);
	}
}

void CoreService::getHWParam(uint16_t nParam, void **param)
{
	hw->getParam(nParam, param);
}
