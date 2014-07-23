/*
 * This file is part of the PulseView project.
 *
 * Copyright (C) 2012 Joel Holdsworth <joel@airwebreathe.org.uk>
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

#include <cassert>

#include "view.h"
#include "viewport.h"

#include "signal.h"
#include "../sigsession.h"

#include <QMouseEvent>
#include <QApplication>

using std::max;
using std::min;
using std::shared_ptr;
using std::vector;

namespace pv {
namespace view {

Viewport::Viewport(View &parent) :
	QWidget(&parent),
        _view(parent),
        _on_selection(FALSE)
{
	setMouseTracking(true);
	setAutoFillBackground(true);
	setBackgroundRole(QPalette::Base);
	_selection_color = QApplication::palette().highlight().color();
	_selection_color.setAlphaF(0.5);

	connect(&_view.session(), SIGNAL(signals_changed()),
		this, SLOT(on_signals_changed()));

	connect(&_view, SIGNAL(signals_moved()),
		this, SLOT(on_signals_moved()));

	// Trigger the initial event manually. The default device has signals
	// which were created before this object came into being
	on_signals_changed();
}

QPoint Viewport::get_selection_from() const
{
	return _selected_area.from;
}

QPoint Viewport::get_selection_to() const
{
	return _selected_area.to;
}

int Viewport::get_total_height() const
{
	int h = 0;
	const vector< shared_ptr<Trace> > traces(_view.get_traces());
	for (const shared_ptr<Trace> t : traces) {
		assert(t);
		h = max(t->get_v_offset() + View::SignalHeight, h);
	}

	return h;
}

void Viewport::paintEvent(QPaintEvent*)
{
	const vector< shared_ptr<Trace> > traces(_view.get_traces());

	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);

	if (_view.cursors_shown())
		_view.cursors().draw_viewport_background(p, rect());

	// Plot the signal
	for (const shared_ptr<Trace> t : traces)
	{
		assert(t);
		t->paint_back(p, 0, width());
	}

	for (const shared_ptr<Trace> t : traces)
		t->paint_mid(p, 0, width());

	for (const shared_ptr<Trace> t : traces)
		t->paint_fore(p, 0, width());

	if (_view.cursors_shown())
		_view.cursors().draw_viewport_foreground(p, rect());

	if (_on_selection) {
		p.fillRect(QRect(_selected_area.from, _selected_area.to).normalized(),
				_selection_color);
	}
	p.end();
}

void Viewport::mousePressEvent(QMouseEvent *event)
{
	assert(event);

	_mouse_down_point = event->pos();
	_mouse_down_offset = _view.offset();
	if (event->buttons() & Qt::RightButton) {
		_on_selection = TRUE;
		_selected_area.from = event->pos();
	}
}

void Viewport::mouseReleaseEvent(QMouseEvent *event)
{
	assert(event);

	_mouse_down_point = event->pos();
	_mouse_down_offset = _view.offset();
	if (_on_selection) {
		_on_selection = FALSE;
		// normalize that 'from' is on the left side of the selection
		// and 'to' on the right side
		if (_selected_area.from.x() <= event->pos().x()) {
			_selected_area.to = event->pos();
		} else {
			_selected_area.to = _selected_area.from;
			_selected_area.from = event->pos();
		}
		traces_selected();
	}
}

void Viewport::mouseMoveEvent(QMouseEvent *event)
{
	assert(event);

	if (event->buttons() & Qt::LeftButton)
	{
		_view.set_scale_offset(_view.scale(),
			_mouse_down_offset +
			(_mouse_down_point - event->pos()).x() *
			_view.scale());
	}
	if (_on_selection) {
		_selected_area.to = event->pos();
		update();
	}
}

void Viewport::mouseDoubleClickEvent(QMouseEvent *event)
{
	assert(event);

	if (event->buttons() & Qt::LeftButton)
		_view.zoom(2.0, event->x());
	else if (event->buttons() & Qt::RightButton)
		_view.zoom(-2.0, event->x());
}

void Viewport::wheelEvent(QWheelEvent *event)
{
	assert(event);

	if (event->orientation() == Qt::Vertical) {
		// Vertical scrolling is interpreted as zooming in/out
		_view.zoom(event->delta() / 120, event->x());
	} else if (event->orientation() == Qt::Horizontal) {
		// Horizontal scrolling is interpreted as moving left/right
		_view.set_scale_offset(_view.scale(),
				       event->delta() * _view.scale()
				       + _view.offset());
	}
}

void Viewport::on_signals_changed()
{
	const vector< shared_ptr<Trace> > traces(_view.get_traces());
	for (shared_ptr<Trace> t : traces) {
		assert(t);
		connect(t.get(), SIGNAL(visibility_changed()),
			this, SLOT(update()));
	}
}

void Viewport::on_signals_moved()
{
	update();
}

} // namespace view
} // namespace pv
