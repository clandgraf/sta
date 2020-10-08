#include "rom.hpp"
#include "nes/mappers/mapper.hpp"

Mapper::Mapper(Cart& cart) : m_cart(cart) {}
Mapper::~Mapper() {}

void Mapper::reset() {}