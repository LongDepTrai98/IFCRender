#include "UuidGenerator.h"
#include "uuid_4.h"
#include <stdexcept>

std::string UuidGenerator::CreateUUID()
{
    thread_local static UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;

    UUIDv4::UUID uuid = uuidGenerator.getUUID();
    return uuid.str();
}