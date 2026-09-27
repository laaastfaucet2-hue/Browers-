#include "../../include/browser_core/ContainerManager.hpp"
#include <sstream>
#include <algorithm>

namespace BrowserCore {

ContainerManager::ContainerManager() {
    initDefaultContainers();
}

void ContainerManager::initDefaultContainers() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_containers.clear();

    // Default container (ID 0)
    ContainerIdentity def;
    def.id = 0;
    def.name = "العادي (Default)";
    def.color = "#94a3b8";
    def.icon = "globe";
    m_containers.push_back(def);

    // Firefox Standard Containers
    ContainerIdentity personal;
    personal.id = 1;
    personal.name = "شخصي (Personal)";
    personal.color = "#38bdf8"; // Blue
    personal.icon = "user";
    m_containers.push_back(personal);

    ContainerIdentity work;
    work.id = 2;
    work.name = "العمل (Work)";
    work.color = "#fb923c"; // Orange
    work.icon = "briefcase";
    m_containers.push_back(work);

    ContainerIdentity banking;
    banking.id = 3;
    banking.name = "البنوك والمعاملات (Banking)";
    banking.color = "#4ade80"; // Green
    banking.icon = "dollar";
    m_containers.push_back(banking);

    ContainerIdentity shopping;
    shopping.id = 4;
    shopping.name = "التسوق (Shopping)";
    shopping.color = "#f472b6"; // Pink
    shopping.icon = "cart";
    m_containers.push_back(shopping);

    m_nextId = 5;
}

uint32_t ContainerManager::createContainer(const std::string& name, const std::string& color, const std::string& icon) {
    std::lock_guard<std::mutex> lock(m_mutex);
    ContainerIdentity c;
    c.id = m_nextId++;
    c.name = name;
    c.color = color;
    c.icon = icon;
    m_containers.push_back(c);
    return c.id;
}

bool ContainerManager::removeContainer(uint32_t id) {
    if (id == 0) return false; // Cannot remove default container
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::remove_if(m_containers.begin(), m_containers.end(), [id](const ContainerIdentity& c) {
        return c.id == id;
    });
    if (it != m_containers.end()) {
        m_containers.erase(it, m_containers.end());
        return true;
    }
    return false;
}

const ContainerIdentity* ContainerManager::getContainer(uint32_t id) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& c : m_containers) {
        if (c.id == id) return &c;
    }
    return nullptr;
}

std::vector<ContainerIdentity> ContainerManager::getAllContainers() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_containers;
}

void ContainerManager::setCookie(uint32_t containerId, const std::string& domain, const std::string& name, const std::string& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& c : m_containers) {
        if (c.id == containerId) {
            c.cookieJar[domain][name] = value;
            return;
        }
    }
}

std::string ContainerManager::getCookie(uint32_t containerId, const std::string& domain, const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& c : m_containers) {
        if (c.id == containerId) {
            auto dIt = c.cookieJar.find(domain);
            if (dIt != c.cookieJar.end()) {
                auto cIt = dIt->second.find(name);
                if (cIt != dIt->second.end()) return cIt->second;
            }
        }
    }
    return "";
}

std::map<std::string, std::string> ContainerManager::getAllCookies(uint32_t containerId, const std::string& domain) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& c : m_containers) {
        if (c.id == containerId) {
            auto dIt = c.cookieJar.find(domain);
            if (dIt != c.cookieJar.end()) return dIt->second;
        }
    }
    return {};
}

void ContainerManager::clearContainerData(uint32_t containerId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& c : m_containers) {
        if (c.id == containerId) {
            c.cookieJar.clear();
            return;
        }
    }
}

std::string ContainerManager::exportContainersJson() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < m_containers.size(); ++i) {
        if (i > 0) ss << ",\n";
        ss << "  {\"id\": " << m_containers[i].id
           << ", \"name\": \"" << m_containers[i].name << "\""
           << ", \"color\": \"" << m_containers[i].color << "\""
           << ", \"icon\": \"" << m_containers[i].icon << "\"}";
    }
    ss << "\n]\n";
    return ss.str();
}

} // namespace BrowserCore
