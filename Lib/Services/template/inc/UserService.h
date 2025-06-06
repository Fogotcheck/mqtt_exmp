#pragma once

#include "Services.h"

#ifndef USER_SERVICES_VERSION_MAJOR
#define USER_SERVICES_VERSION_MAJOR 0
#endif

#ifndef USER_SERVICES_VERSION_MINOR
#define USER_SERVICES_VERSION_MINOR 0
#endif

#ifndef USER_SERVICES_VERSION_PATCH
#define USER_SERVICES_VERSION_PATCH 0
#endif

class UserService : public Services {
    private:
	const std::array<uint8_t, 16> name = { 'U', 's', 'e', 'r' };
	std::array<uint8_t, 256> extBuffer = {};

	struct CoreRegMap {
		const std::array<uint16_t, 3> version = {
			(uint16_t)USER_SERVICES_VERSION_MAJOR,
			(uint16_t)USER_SERVICES_VERSION_MINOR,
			(uint16_t)USER_SERVICES_VERSION_PATCH
		};
	} regMap;

    public:
	enum SERVICE_CORE_CMD {
		CMD_GET_VERSION = 0,
	};

	void cmd(std::span<uint8_t> pl,
		 std::queue<std::span<uint8_t> > &queue) override;

	UserService(void);
	~UserService(void) override;
};
