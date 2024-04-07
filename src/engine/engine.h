#pragma once
#include "dungeon/dungeon.h"
#include "entities.h"
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

    std::shared_ptr<entities::Hero> createHero(const std::string &id) {
        if (id == "highwayman") {
            return std::make_shared<entities::Highwayman>();
        } else if (id == "crusader") {
            return std::make_shared<entities::Crusader>();
        } else if (id == "bounty_hunter") {
            return std::make_shared<entities::BountyHunter>();
        } else if (id == "arbalest") {
            return std::make_shared<entities::Arbalest>();
        } else if (id == "grave_robber") {
            return std::make_shared<entities::GraveRobber>();
        } else if (id == "hellion") {
            return std::make_shared<entities::Hellion>();
        } else if (id == "antiquarian") {
            return std::make_shared<entities::Antiquarian>();
        } else if (id == "abomination") {
            return std::make_shared<entities::Abomination>();
        }
        return nullptr;
    }

    std::shared_ptr<entities::Hero> createNewTeammate() {
        std::vector<std::string> ids = {
            "highwayman",   "crusader", "bounty_hunter", "arbalest",
            "grave_robber", "hellion",  "antiquarian",   "abomination",
        };
        for (const auto &hero : m_camp) {
            ids.erase(std::find(ids.begin(), ids.end(), hero->getId()));
        }
        if (ids.empty()) {
            return nullptr;
        }
        return createHero(ids[rand() % ids.size()]);
    }

    bool isRecruited(const std::shared_ptr<entities::Hero> &hero) const {
        for (const auto &camp_hero : m_camp) {
            if (camp_hero->getId() == hero->getId()) {
                return true;
            }
        }
        return false;
    }

private:
    std::shared_ptr<dungeon::Dungeon> m_dungeon;
    std::shared_ptr<entities::Party> m_party;
    std::vector<std::shared_ptr<entities::Hero>> m_camp;
};

class Battle {};

}   // namespace engine
