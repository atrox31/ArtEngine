#pragma once
// Short type names for often using types

#include "plf/plf_colony-master/plf_colony.h"
#include <vector>
#include <string>

template<typename T>
using vec_it = typename  std::vector<T>::iterator;

template<typename T>
using plf_it = typename  plf::colony<T>::iterator;


using str_vec = std::vector<std::string>;
