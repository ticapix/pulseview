/*
 * This file is part of the PulseView project.
 *
 * Copyright (C) 2013 Joel Holdsworth <joel@airwebreathe.org.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef PULSEVIEW_PV_PROP_BINDING_DECODEROPTIONS_H
#define PULSEVIEW_PV_PROP_BINDING_DECODEROPTIONS_H

#include <glib.h>

#include "binding.h"

namespace pv {

namespace data {
class DecoderStack;
}

namespace prop {
namespace binding {

class DecoderOptions : public Binding
{
public:
	DecoderOptions(boost::shared_ptr<pv::data::DecoderStack> decoder);

private:
	GVariant* getter(const char *id);

	void setter(const char *id, GVariant *value);

private:
	boost::shared_ptr<pv::data::DecoderStack> _decoder;
};

} // binding
} // prop
} // pv

#endif // PULSEVIEW_PV_PROP_BINDING_DECODEROPTIONS_H
