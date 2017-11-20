#pragma once
#include "miscellaneous/common.hxx"
#include <ostream>


namespace ast {


struct Node;


std::ostream& operator<<(std::ostream& stream, Node& node);


}
