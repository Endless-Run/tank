/* This file is part of Tank.
 *
 * Tank is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tank is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License and
 * the GNU Lesser General Public Licence along with Tank. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Copyright 2013 (©) Jamie Bayne, David Truby, David Watson.
 */

#include "State.hpp"

#include <algorithm>
#include <stdexcept>

#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>

#include "Entity.hpp"
#include "Game.hpp"

namespace tank
{

State::State() = default;
State::~State()
{
    connections_.clear();
}

void State::insertEntity(std::unique_ptr<Entity>&& entity)
{
    if (not entity)
    {
        Game::log << "Warning: You can't add a null entity." << std::endl;
        return;
    }

    // Stops an entity being added several times
    auto x = boost::range::find_if(entities_,
        [&entity](std::unique_ptr<Entity>& existing)
        {
            return entity.get() == existing.get();
        }
    );

    if (x != end(entities_))
    {
        throw std::invalid_argument("Entity already added");
    }

    entity->setState(this);
    entity->onAdded();
    entities_.push_back(std::move(entity));
}

void State::moveEntity(observing_ptr<State> state, observing_ptr<Entity> entity)
{
    if (not entity)
    {
        Game::log << "Warning: attempted to move null entity." << std::endl;
        return;
    }

    if (not state)
    {
        Game::log << "Warning: attempted to move entity to null state."
                  << std::endl;
        return;
    }

    if (entity->isRemoved())
    {
        // Don't let an entity escape deletion
        return;
    }

    toMove_.emplace_back(state, entity);

    if(not updating_)
    {
        moveEntities();
    }
}

std::unique_ptr<Entity> State::releaseEntity(observing_ptr<Entity> entity)
{
    auto it = boost::range::find_if(entities_,
        [&entity](std::unique_ptr<Entity>& ent)
        {
            return entity == ent.get();
        }
    );

    if (it == end(entities_))
    {
        return nullptr;
    }

    auto ent = std::move(*it);
    entities_.erase(it);
    ent->onRemoved();
    return ent;
}

void State::update()
{
    updating_ = true;
    for (auto& entity : entities_)
    {
        entity->update();
    }

    addEntities();
    moveEntities();
    deleteEntities();
    updating_ = false;
}

void State::draw()
{
    boost::range::stable_sort(entities_,
                     [](std::unique_ptr<Entity> const& e1,
                        std::unique_ptr<Entity> const& e2) {
        return e1->getLayer() < e2->getLayer();
    });


    for (auto& entity : entities_)
    {
        entity->draw(camera());
    }
}

void State::addEntities()
{
    std::move(newEntities_.begin(), newEntities_.end(),
              std::back_inserter(entities_));
    newEntities_.clear();
}

void State::moveEntities()
{
    while (not toMove_.empty())
    {
        observing_ptr<State> state = std::get<0>(toMove_.back());
        observing_ptr<Entity> entity = std::get<1>(toMove_.back());
        toMove_.pop_back();

        std::unique_ptr<Entity> entPtr = releaseEntity(entity);
        if (not entPtr.get())
        {
            Game::log << "Entity not found in move operation" << std::endl;
            continue;
        }

        state->insertEntity(std::move(entPtr));
    }
}

void State::deleteEntities()
{
    boost::range::remove_erase_if(entities_,
        [](const std::unique_ptr<Entity>& ent)
        {
            if (ent->isRemoved()) {
                ent->onRemoved();
                return true;
            }
            return false;
        }
    );
}

tank::observing_ptr<tank::EventHandler::Connection> State::connect(
                                       EventHandler::Condition condition,
                                       EventHandler::Effect effect)
{
    auto cond = this->eventHandler.connect(condition, effect);
    connections_.push_back(std::move(cond));
    return connections_.back();
}

}
