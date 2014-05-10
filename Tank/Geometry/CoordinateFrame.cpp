// Copyright (©) Jamie Bayne, David Truby, David Watson 2013-2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "CoordinateFrame.hpp"
#include <SFML/Graphics/Transformable.hpp>

namespace tank
{

InertialFrame const* RootFrame::getRootFrame() const
{
    return this;
}

InertialFrame const* RootFrame::getParentFrame() const
{
    return this;
}

Transform RootFrame::getTransformFromParent() const
{
    return Transform();
}

////////////////////////////////////////////////////////////////////////////////

CoordinateFrame::CoordinateFrame(Vectorf const& position)
    : pos_(position)
{}

Vectorf CoordinateFrame::getPos() const
{
    return pos_;
}

float CoordinateFrame::getRotation() const
{
    return rotation_;
}

Vectorf CoordinateFrame::getOrigin() const
{
    return origin_;
}

float CoordinateFrame::getZoom() const
{
    return zoom_;
}

void CoordinateFrame::setPos(Vectorf const& pos)
{
    pos_ = pos;
}

void CoordinateFrame::setRotation(float rotation)
{
    rotation_ = rotation;
}

void CoordinateFrame::setOrigin(Vectorf const& o)
{
    origin_ = o;
}

void CoordinateFrame::setZoom(float zoom)
{
    zoom_ = zoom;
}

void CoordinateFrame::setParentFrame(observing_ptr<InertialFrame> frame)
{
    parentFrame_ = frame;
}

void CoordinateFrame::setParentFrameIfNotSameRoot(observing_ptr<InertialFrame> frame)
{
    InertialFrame const* frameRoot = frame->getRootFrame();
    InertialFrame const* thisRoot = getRootFrame();
    if (frameRoot != thisRoot or thisRoot == nullptr)
    {
        parentFrame_ = frame;
    }
}

InertialFrame const* CoordinateFrame::getRootFrame() const
{
    InertialFrame const* parentFrame_ = getParentFrame();
    if (parentFrame_)
    {
        return getParentFrame()->getRootFrame();
    }
    else
    {
        return nullptr;
    }
}

InertialFrame const* CoordinateFrame::getParentFrame() const
{
    return parentFrame_.get();
}

Transform CoordinateFrame::getTransformFromParent() const
{
    return Transform(getRotation(), getPos() - getOrigin().rotate(getRotation()), getZoom());
}

////////////////////////////////////////////////////////////////////////////////

Vectorf GraphicalCoordinateFrame::getScale() const
{
    return scale_;
}

void GraphicalCoordinateFrame::setScale(Vectorf const& scale)
{
    scale_ = scale;
}

Transform GraphicalCoordinateFrame::getTransformFromParent() const
{
    return Transform(getRotation(),
                     getPos() - getOrigin().rotate(getRotation()),
                     getZoom(), getScale());
}

} // tank
