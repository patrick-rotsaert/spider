//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "spider/version.h"

#include <cassert>

namespace spider {

int version::number()
{
	return SPIDER_VERSION_NUMBER;
}

int version::major()
{
	return SPIDER_VERSION_MAJOR;
}

int version::minor()
{
	return SPIDER_VERSION_MINOR;
}

int version::patch()
{
	return SPIDER_VERSION_PATCH;
}

void version::check()
{
	assert(SPIDER_VERSION_NUMBER == number());
}

} // namespace spider
