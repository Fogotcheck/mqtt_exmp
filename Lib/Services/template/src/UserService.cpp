#include <stdexcept>
#include <iostream>
#include <cstring>
#include <queue>

#include "UserService.h"

void UserService::cmd(std::span<uint8_t> pl,
		      std::queue<std::span<uint8_t> > &queue)
{
	if (pl.size() < sizeof(head_t))
		throw std::invalid_argument("Invalid command length");

	head_t *headInput = reinterpret_cast<head_t *>(pl.data());
	head_t *headOutput = reinterpret_cast<head_t *>(buffer.data());

	*headOutput = *headInput;

	switch (headInput->comm) {
	case CMD_GET_VERSION: {
		headOutput->len = sizeof(regMap.version) + sizeof(head_t);
		if (buffer.size() < headOutput->len)
			throw std::invalid_argument("Invalid answer length");

		std::memcpy(buffer.data() + sizeof(head_t),
			    regMap.version.data(), sizeof(regMap.version));
		break;
	}
	default:
		break;
	}
	queue.push(std::span<uint8_t>(buffer.data(), headOutput->len));
}

UserService::UserService(void)
	: Services(name, std::span<uint8_t>(extBuffer.data(), extBuffer.size()))
{
}

UserService::~UserService(void)
{
}
