#include "rom.hpp"
#include "mappers/mapper.hpp"

Mapper::Mapper(Cart& cart) : m_cart(cart) {}
Mapper::~Mapper() {}

void Mapper::reset() {}