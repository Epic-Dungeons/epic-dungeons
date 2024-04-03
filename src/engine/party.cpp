#include "entity.h"
#include "items/consumables/heal.h"
#include "items/golden_coin.h"
#include "logging/logger.h"

namespace engine {
namespace entities {

Party::Party() : GameObject() {
    m_members.reserve(4);   // 4 is the max party size
    m_inventory = std::make_shared<items::Storage>();
    m_inventory->addItem(std::make_shared<items::GoldenCoin>(), 10);
    m_inventory->addItem(std::make_shared<items::Heal>(), 2);
}

void Party::addMember(const std::shared_ptr<Entity> &member) {
    if (m_members.size() == 4) {
        throw std::runtime_error("Party is full");
        return;
    }
    if (member->m_party.lock() != nullptr) {
        throw std::runtime_error("Entity is already in a party");
        return;
    }
    member->m_party = shared_from_this();
    m_members.push_back(member);
}

void Party::removeMember(const std::shared_ptr<Entity> &member) {
    auto it = std::find(m_members.begin(), m_members.end(), member);
    if (it != m_members.end()) {
        m_members.erase(it);
        member->m_party.reset();
    }
}

std::shared_ptr<Entity> Party::getMember(const uint8_t &index) const {
    if (index < m_members.size()) {
        return m_members[index];
    }
    return nullptr;
}

std::vector<std::shared_ptr<Entity>> Party::getMembers() const {
    return m_members;
}

uint8_t Party::getMembersCount() const {
    return m_members.size();
}

const uint8_t Party::getAliveMembersCount() const {
    uint8_t count = 0;
    for (auto &member : m_members) {
        if (member->isAlive()) {
            count++;
        }
    }
    return count;
}

uint8_t Party::getMemberPosition(const std::shared_ptr<const Entity> &member) const {
    for (uint8_t i = 0; i < m_members.size(); i++) {
        if (m_members[i] == member) {
            return i;
        }
    }
    return 0;
}

void Party::swapMembers(const uint8_t &index1, const uint8_t &index2) {
    if (index1 < m_members.size() && index2 < m_members.size()) {
        std::swap(m_members[index1], m_members[index2]);
    } else {
        throw std::runtime_error("Index out of range");
    }
}

void Party::swapMembers(const std::shared_ptr<Entity> &member1, const std::shared_ptr<Entity> &member2) {
    if (member1 == member2)
        return;
    else if (hasMember(member1) && hasMember(member2)) {
        uint8_t index1 = getMemberPosition(member1);
        uint8_t index2 = getMemberPosition(member2);
        swapMembers(index1, index2);
    } else if (!hasMember(member1) && hasMember(member2)) {
        uint8_t old_idx = getMemberPosition(member2);
        removeMember(member2);
        addMember(member1);
        uint8_t new_idx = getMemberPosition(member1);
        swapMembers(old_idx, new_idx);
    } else if (hasMember(member1) && !hasMember(member2)) {
        uint8_t old_idx = getMemberPosition(member1);
        removeMember(member1);
        addMember(member2);
        uint8_t new_idx = getMemberPosition(member2);
        swapMembers(old_idx, new_idx);
    } else {
        throw std::runtime_error("Members not found in the party");
    }
    logging::debug("Swapped members " + member1->getName() + " and " + member2->getName());
}

void Party::arrangeMembers() {
    // move dead members to the end
    // bubble sort
    for (uint8_t i = 0; i < m_members.size(); i++) {
        while (i + 1 < m_members.size() && !m_members[i]->isAlive() && m_members[i + 1]->isAlive())
            swapMembers(i, i + 1);
    }
}

void Party::clear() {
    for (auto &member : m_members) {
        member->m_party.reset();
    }
    m_members.clear();
}

void Party::memberDied(const std::shared_ptr<Entity> &member) {
    logging::debug("Member " + member->getName() + " died");
    // arrangeMembers();
    // removeMember(member);
}

bool Party::hasMember(const std::shared_ptr<const Entity> &member) const {
    return std::find(m_members.begin(), m_members.end(), member) != m_members.end();
}

std::shared_ptr<items::Storage> Party::getInventory() const {
    return m_inventory;
}

std::vector<std::shared_ptr<skills::Skill>> Party::getSkills() const {
    std::vector<std::shared_ptr<skills::Skill>> skills;
}

}   // namespace entities
}   // namespace engine