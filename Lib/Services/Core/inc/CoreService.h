#pragma once

#include "CoreServiceHW.h"
#include "Services.h"

class CoreService : public Services {
    private:
	static const uint32_t coreBasisHash = SERVICES_BASIS_HASH;

	class CoreServiceHW *hw;
	std::array<uint8_t, 256> buf;
	std::span<uint8_t> saveCTX(const char *data);

	void setCoreCommands(void);

    public:
	void init(void);
	void versionCB(std::span<uint8_t> ctx);
	void showCB(std::span<uint8_t> ctx);

	void getHWParam(uint16_t nParam, void **param) override;
	void pvr_cb(lwjson_stream_parser_t *jsp,
		    lwjson_stream_type_t type) override;
	CoreService(const Services::ServicesCB &pcb);
	~CoreService(void);
};
