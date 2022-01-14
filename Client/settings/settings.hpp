#pragma once
#include "configManager.hpp"
#include "platform_folders.h"
#include "graphics.hpp"
#include "events.hpp"

enum class SettingType {CHOICE_SETTING, BOOLEAN_SETTING};

class SettingChangeEvent {};

class Setting {
public:
    SettingType type;
    Setting(const std::string& indent) : indent(indent) {}
    const std::string indent;
    virtual std::string exportToStr() = 0;
    virtual void loadFromStr(std::string value) = 0;
    EventSender<SettingChangeEvent> setting_change_event;
};

class ChoiceSetting : public Setting {
    int selected_choice;
public:
    ChoiceSetting(const std::string& indent, const std::vector<std::string>& choices, int default_value) : Setting(indent), choices(choices), selected_choice(default_value) { type = SettingType::CHOICE_SETTING; };
    const std::vector<std::string> choices;
    int getSelectedChoice();
    void setSelectedChoice(int choice);
    
    std::string exportToStr() override;
    void loadFromStr(std::string value) override;
};

class BooleanSetting : public Setting {
    bool value;
public:
    BooleanSetting(const std::string& indent, bool default_value) : Setting(indent), value(default_value) { type = SettingType::BOOLEAN_SETTING; };
    bool getValue();
    void setValue(bool new_value);
    
    std::string exportToStr() override;
    void loadFromStr(std::string value) override;
};

class Settings : public gfx::GlobalUpdateFunction, EventListener<SettingChangeEvent> {
    std::vector<Setting*> settings;
    ConfigFile config_file;
    int count = 0;
    void update() override;
    void onEvent(SettingChangeEvent& event) override;
    bool is_loading = false;
public:
    Settings() : config_file(sago::getDataHome() + "/Terralistic/settings.txt") {}
    void addSetting(Setting* setting);
    void removeSetting(Setting* setting);
    const std::vector<Setting*>& getSettings();
    void reloadSettings();
    ~Settings();
};
