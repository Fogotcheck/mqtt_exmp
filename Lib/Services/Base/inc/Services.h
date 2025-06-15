#pragma once

#include <functional>
#include <span>
#include <map>
#include <atomic>

#include "lwjson/lwjson.h"

#define SERVICES_BASIS_HASH 0x811C9DC5U

class Services {
    private:
	const char *name;
	static void pvr_cb_static(lwjson_stream_parser_t *jsp,
				  lwjson_stream_type_t type);
	uint32_t hash;

    protected:
	static lwjson_stream_parser_t stream;
	static std::map<uint32_t, Services *> map;

	struct commands_t {
		uint32_t event;
		std::span<uint8_t> ctx;
		std::function<void(std::span<uint8_t>)> cb;
	};
	std::map<uint32_t, commands_t> commands;

    public:
	struct ServicesCB {
		std::function<void(Services *it, const char *subName,
				   std::span<uint8_t> data, void *ctx)>
			func = nullptr;
		void *ctx = nullptr;
	} cb;

	struct ServicesStat {
		std::atomic<uint32_t> msgERR{ 0 };
		std::atomic<uint32_t> msgOK{ 0 };
	} stat;

	const char *getName(void);
	void init(void);
	uint32_t getHash(const char *str);
	uint32_t getHash(const char *str, uint32_t tbasisHash);
	void setCommand(const char *cmdName, commands_t handle,
			uint32_t tBasisHash);
	int setBasisHash(uint32_t newHash);

	void eventHandle(uint32_t event);
	virtual void getHWParam(uint16_t nParam, void **param) = 0;
	lwjsonr_t parcer(const uint8_t *c, uint16_t len);
	virtual void pvr_cb(lwjson_stream_parser_t *jsp,
			    lwjson_stream_type_t type) = 0;

	Services(const char *name);
	virtual ~Services();
};
