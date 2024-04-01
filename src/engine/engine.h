#pragma once
#include "dungeon/dungeon.h"
#include "entity.h"
#include "heroes/arbalest.h"
#include "heroes/bounty_hunter.h"
#include "heroes/crusader.h"
#include "heroes/highwayman.h"
#include <memory>

namespace engine {
class Engine {
public:
    Engine() : m_dungeon(nullptr), m_party(nullptr) {
        m_party = std::make_shared<entities::Party>();
        m_camp.push_back(std::make_shared<entities::Highwayman>());
        m_camp.push_back(std::make_shared<entities::Crusader>());
        m_camp.push_back(std::make_shared<entities::BountyHunter>());
        m_camp.push_back(std::make_shared<entities::Arbalest>());
        m_party->addMember(m_camp[0]);
        m_party->addMember(m_camp[1]);
        m_party->addMember(m_camp[2]);
    }

    void bindDungeon(const std::shared_ptr<dungeon::Dungeon> &dungeon) {
        this->m_dungeon = dungeon;
    }

    std::shared_ptr<dungeon::Dungeon> getDungeon() const {
        return m_dungeon;
    }

    std::shared_ptr<entities::Party> getParty() const {
        return m_party;
    }

    const std::vector<std::shared_ptr<entities::Hero>> &getCampTeammates() const {
        return m_camp;
    }

private:
    std::shared_ptr<dungeon::Dungeon> m_dungeon;
    std::shared_ptr<entities::Party> m_party;
    std::vector<std::shared_ptr<entities::Hero>> m_camp;
};

class Battle {};
}   // namespace engine
