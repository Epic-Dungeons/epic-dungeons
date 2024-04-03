#pragma once

#include <memory>
#include <utility>
#include <vector>

namespace dungeon {
typedef std::pair<int, int> Position;
class Room;

enum class CellType {
    // Both corridor and room types
    NOTHING,
    FIGHT,
    TREASURE,
    // Room types
    // SHOP,
    BOSS,
    EXIT,
    ENTRANCE,
    // Corridor types
    TRAP,
    DOOR,
    // Storyline types
    FIRST_FIGHT,
    FIRST_TREASURE,
    FIRST_SHOP,
};

enum class Direction {
    NONE = -1,
    UP = 0,
    RIGHT,
    DOWN,
    LEFT,
};

class Cell : public std::enable_shared_from_this<Cell> {
    friend class Dungeon;

public:
    explicit Cell(Position position, bool is_room = false);
    virtual ~Cell() = default;

    constexpr Position getPosition() const {
        return m_position;
    }

    constexpr bool isRoom() const {
        return m_is_room;
    }

    bool isVisited() const {
        return m_is_visited;
    }

    bool isDiscovered() const {
        return m_is_discovered;
    }

    std::vector<std::weak_ptr<Cell>> getNeighbours() {
        return m_neighbours;
    }

    CellType getType() const {
        return m_type;
    }

    void setType(const CellType &type) {
        m_type = type;
    }

    void visit() {
        m_is_visited = true;
    }

    void discover() {
        m_is_discovered = true;
    }

    std::weak_ptr<Cell> getNeighbour(const Direction &direction);

    friend void connectCells(std::weak_ptr<Cell> cell1, std::weak_ptr<Cell> cell2);
    friend void setCellType(std::weak_ptr<Cell> cell, const CellType &type);
    friend Direction getDirection(std::weak_ptr<Cell> from, std::weak_ptr<Cell> to);

private:
    const Position m_position;
    const bool m_is_room;

    bool m_is_visited = false;
    bool m_is_discovered = false;

    CellType m_type = CellType::NOTHING;
    std::vector<std::weak_ptr<Cell>> m_neighbours;
};

class Room : public Cell {
public:
    explicit Room(Position position);
};

void connectCells(std::weak_ptr<Cell> cell1, std::weak_ptr<Cell> cell2);
}   // namespace dungeon
