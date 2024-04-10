#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace engine {
namespace parsing {

class DataParameter {
public:
    DataParameter(const std::string &rawData);

    std::string name;

    uint32_t size() const;

    template<typename T = std::string>
    T getValue(uint32_t index = 0) const {
        return T(values[index]);
    }

    template<>
    std::string getValue<std::string>(uint32_t index) const {
        if (index >= values.size())
            return "";
        return values[index];
    }

    template<>
    int32_t getValue<int32_t>(uint32_t index) const {
        if (index >= values.size())
            return 0;
        std::string value = values[index];
        if (value.empty())
            return 0;
        for (auto &c : value)
            c = std::tolower(c);
        if (value == "true")
            return 1;
        if (value == "false")
            return 0;
        if (value[value.size() - 1] == '%')
            return std::stoi(value.substr(0, value.size() - 1));
        if (value.find('.') != std::string::npos)
            return std::stof(value);
        return std::stoi(values[index]);
    }

    template<>
    float getValue<float>(uint32_t index) const {
        if (index >= values.size())
            return 0;
        std::string value = values[index];
        // lowercase
        for (auto &c : value)
            c = std::tolower(c);
        if (value.empty())
            return 0;
        if (value == "true")
            return 1;
        if (value == "false")
            return 0;
        if (value[value.size() - 1] == '%')
            return std::stof(value.substr(0, value.size() - 1)) / 100.0f;
        return std::stof(value);
    }

    template<>
    bool getValue<bool>(uint32_t index) const {
        return getValue<int32_t>(index) != 0;
    }

    template<typename T = std::string>
    std::vector<T> getValues() const;

    template<>
    std::vector<std::string> getValues<std::string>() const {
        return values;
    }

    template<>
    std::vector<int32_t> getValues<int32_t>() const {
        std::vector<int32_t> result;
        for (size_t i = 0; i < values.size(); i++)
            result.push_back(getValue<int32_t>(i));
        return result;
    }

    template<>
    std::vector<float> getValues<float>() const {
        std::vector<float> result;
        for (size_t i = 0; i < values.size(); i++)
            result.push_back(getValue<float>(i));
        return result;
    }

private:
    std::vector<std::string> values;
};

class DataRow {
public:
    DataRow(const std::string &rawData);
    std::string name;

    std::shared_ptr<DataParameter> getParameter(const std::string &name) const;

    std::vector<std::string> getParameterNames() const;

    bool hasParameter(const std::string &name) const;
    std::string getString(const std::string &name, uint32_t index = 0, const std::string &defaultValue = "") const;
    float getFloat(const std::string &name, uint32_t index = 0, const float &defaultValue = 0.0f) const;
    int32_t getInt(const std::string &name, uint32_t index = 0, const int32_t &defaultValue = 0) const;
    bool getBool(const std::string &name, uint32_t index = 0, const bool &defaultValue = false) const;

    uint32_t size() const;

private:
    std::map<std::string, std::shared_ptr<DataParameter>> parameters;
};

class FileData {
public:
    FileData(const std::string &rawData);

    const std::shared_ptr<DataRow> &getRow(uint32_t index) const;
    const std::shared_ptr<DataRow> &getRow(const std::string &name) const;

    /// @brief  Find rows with the given name and matching arguments
    /// @tparam ...Args - pair<const char *, T>
    /// @param name - row name
    /// @param ...args - pairs <const char *, T> where first is the parameter name and second is the value
    /// @return - vector of shared pointers to DataRow
    template<typename... Args>
    std::vector<std::shared_ptr<DataRow>> findRows(const std::string &name, Args... args) const {
        std::vector<std::shared_ptr<DataRow>> result = getRows(name);
        (processRow(result, args), ...);
        return result;
    }

    const std::vector<std::shared_ptr<DataRow>> &getRows() const;
    const std::vector<std::shared_ptr<DataRow>> getRows(const std::string &name) const;

protected:
    template<typename T>
    void processRow(std::vector<std::shared_ptr<DataRow>> &rows, const std::pair<const char *, T> &arg) const {
        std::vector<std::shared_ptr<DataRow>> result;
        for (const std::shared_ptr<DataRow> &row : rows) {
            if (row->getParameter(arg.first)->template getValue<T>() == arg.second)
                result.push_back(row);
        }
        rows = result;
    }

private:
    std::vector<std::shared_ptr<DataRow>> rows;
};

class FileReader {
public:
    FileReader() = delete;
    [[nodiscard]] static std::shared_ptr<FileData> read(const std::string &path);
};

typedef std::shared_ptr<DataParameter> DataParameterPtr;
typedef std::shared_ptr<DataRow> DataRowPtr;
typedef std::shared_ptr<FileData> FileDataPtr;
typedef std::pair<const char *, int32_t> IntParam;
typedef std::pair<const char *, float> FloatParam;
typedef std::pair<const char *, std::string> StringParam;

}   // namespace parsing
}   // namespace engine