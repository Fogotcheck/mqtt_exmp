#include <stdexcept>

#include "Services.h"

static uint32_t basisHash = SERVICES_BASIS_HASH;
std::map<uint32_t, Services *> Services::map;
lwjson_stream_parser_t Services::stream;

Services::Services(const char *name)
	: name(name)
{
	hash = getHash(name);
}

Services::~Services()
{
	map.erase(hash);
}

/**
 * @brief Calculates a 32-bit hash of a string using the FNV-1a algorithm.
 *
 * The FNV-1a (Fowler–Noll–Vo) algorithm is used for fast and uniform
 * string hashing. It performs a bitwise XOR operation and multiplies
 * by a fixed prime number for each character in the string.
 *
 * @return A 32-bit hash value of the string.
 *
 * @note The hash is case-sensitive and depends on the order of characters
 *       in the string. For example, the strings "abc" and "ABC" will produce
 *       different hashes.
 * @warning If `str` is nullptr, the result will be undefined.
 *
 * @details
 * The probability of a collision depends on the number of strings
 * being hashed.
 * For a 32-bit hash, there are \( 2^{32} = 4,294,967,296 \)
 * unique possible values.
 *
 * Using the birthday paradox approximation, the probability of
 * at least one collision
 *
 * for \( n \) strings can be estimated as:
 * \f[
 * P(\text{collision}) \approx 1 - e^{-\frac{n^2}{2 \cdot 2^{32}}}
 * \f]
 * Examples:
 * - For 1,000 strings, the probability of a collision is ~0.0116%.
 * - For 10,000 strings, the probability of a collision is ~1.15%.
 * - For 100,000 strings, the probability of a collision is ~69.2%.
 */
uint32_t Services::getHash(const char *str)
{
	const uint32_t fnv_prime = 16777619u;
	uint32_t hash = basisHash;

	if (!str)
		return hash;

	while (*str) {
		hash ^= static_cast<uint8_t>(*str);
		hash *= fnv_prime;
		++str;
	}
	return hash;
}

uint32_t Services::getHash(const char *str, uint32_t tbasisHash)
{
	const uint32_t fnv_prime = 16777619u;
	uint32_t hash = tbasisHash;

	if (!str)
		return hash;

	while (*str) {
		hash ^= static_cast<uint8_t>(*str);
		hash *= fnv_prime;
		++str;
	}
	return hash;
}

lwjsonr_t Services::parcer(const uint8_t *c, uint16_t len)
{
	lwjsonr_t res;
	while (len) {
		res = lwjson_stream_parse(&stream, (char)*c);
		switch (res) {
		case lwjsonSTREAMINPROG:
			break;
		case lwjsonSTREAMWAITFIRSTCHAR:
			break;
		case lwjsonSTREAMDONE:
			break;
		default:
			return res;
		}
		c++;
		len--;
	}
	return res;
}

void Services::pvr_cb_static(lwjson_stream_parser_t *jsp,
			     lwjson_stream_type_t type)
{
	Services *obj = static_cast<Services *>(jsp->user_data);

	uint32_t tmp_hash = obj->getHash(jsp->stack[1].meta.name);
	auto it = map.find(tmp_hash);
	if (it != map.end() && it->second) {
		it->second->pvr_cb(jsp, type);
	}
}

void Services::init(void)
{
	if (map.find(hash) != map.end()) {
		throw std::runtime_error(
			"Service with the same hash already exists");
	}
	map[hash] = this;
	if (lwjson_stream_init(&stream, &Services::pvr_cb_static))
		throw std::runtime_error(
			"Failed to initialize lwjson stream parser");
	stream.user_data = this;
}

const char *Services::getName(void)
{
	return name;
}

int Services::setBasisHash(uint32_t newHash)
{
	int res = -1;
	uint32_t prevBasisHash = basisHash;
	basisHash = newHash;
	try {
		std::map<uint32_t, Services *> newMap;

		for (auto &[old_hash, service] : map) {
			uint32_t new_service_hash =
				service->getHash(service->name);
			service->hash = new_service_hash;
			newMap[new_service_hash] = service;
		}

		map = std::move(newMap);
		res = 0;
	} catch (const std::bad_alloc &) {
		basisHash = prevBasisHash;

		for (auto &[_, service] : map) {
			service->hash = service->getHash(service->name);
		}
	}
	return res;
}

void Services::setCommand(const char *cmdName, commands_t handle,
			  uint32_t tBasisHash)
{
	if (cmdName == nullptr)
		throw std::invalid_argument("Failed cmdName::nullptr");

	uint32_t hashComm = getHash(cmdName, tBasisHash);
	if (commands.find(hashComm) != commands.end())
		throw std::runtime_error(
			"Commands with the same hash already exists");

	commands[hashComm] = handle;
}

void Services::eventHandle(uint32_t event)
{
	for (auto &[hash, cmd] : commands) {
		if (cmd.event == event && cmd.cb) {
			std::invoke(cmd.cb, cmd.ctx);
			cmd.ctx = std::span<uint8_t>();
		}
	}
}
