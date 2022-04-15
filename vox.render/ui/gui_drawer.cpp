//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gui_drawer.h"

#include <utility>

#include "ui/widgets/texts/text_colored.h"
#include "ui/widgets/drags/drag_multiple_scalars.h"
#include "ui/widgets/input_fields/input_text.h"
#include "ui/widgets/selection/color_edit.h"
#include "ui/widgets/layout/group.h"
#include "ui/widgets/selection/check_box.h"
#include "ui/widgets/buttons/button_small.h"
#include "ui/plugins/drag_drop_target.h"

namespace vox::ui {
const Color GuiDrawer::title_color_ = {0.85f, 0.65f, 0.0f};
const Color GuiDrawer::clear_button_color_ = {0.5f, 0.0f, 0.0f};
const float GuiDrawer::min_float_ = -999999999.f;
const float GuiDrawer::max_float_ = +999999999.f;
//OvRendering::Resources::Texture* GuiDrawer::__EMPTY_TEXTURE = nullptr;

//void GuiDrawer::ProvideEmptyTexture(OvRendering::Resources::Texture& p_emptyTexture) {
//    __EMPTY_TEXTURE = &p_emptyTexture;
//}

void GuiDrawer::create_title(WidgetContainer &p_root, const std::string &p_name) {
    p_root.create_widget<TextColored>(p_name, title_color_);
}

void GuiDrawer::draw_boolean(WidgetContainer &p_root, const std::string &p_name, bool &p_data) {
    create_title(p_root, p_name);
    auto &widget = p_root.create_widget<CheckBox>();
    auto &dispatcher = widget.add_plugin<DataDispatcher<bool>>();
    dispatcher.register_reference(reinterpret_cast<bool &>(p_data));
}

void GuiDrawer::draw_vec_2(WidgetContainer &p_root, const std::string &p_name,
                           Vector2F &p_data, float p_step, float p_min, float p_max) {
    create_title(p_root, p_name);
    auto &widget =
    p_root.create_widget<DragMultipleScalars<float, 2>>(get_data_type<float>(), p_min, p_max, 0.f, p_step, "",
                                                        get_format<float>());
    auto &dispatcher = widget.add_plugin<DataDispatcher<std::array<float, 2>>>();
    dispatcher.register_reference(reinterpret_cast<std::array<float, 2> &>(p_data));
}

void GuiDrawer::draw_vec_3(WidgetContainer &p_root, const std::string &p_name,
                           Vector3F &p_data, float p_step, float p_min, float p_max) {
    create_title(p_root, p_name);
    auto &widget =
    p_root.create_widget<DragMultipleScalars<float, 3>>(get_data_type<float>(), p_min, p_max, 0.f, p_step, "",
                                                        get_format<float>());
    auto &dispatcher = widget.add_plugin<DataDispatcher<std::array<float, 3>>>();
    dispatcher.register_reference(reinterpret_cast<std::array<float, 3> &>(p_data));
}

void GuiDrawer::draw_vec_4(WidgetContainer &p_root, const std::string &p_name,
                           Vector4F &p_data, float p_step, float p_min, float p_max) {
    create_title(p_root, p_name);
    auto &widget =
    p_root.create_widget<DragMultipleScalars<float, 4>>(get_data_type<float>(), p_min, p_max, 0.f, p_step, "",
                                                        get_format<float>());
    auto &dispatcher = widget.add_plugin<DataDispatcher<std::array<float, 4>>>();
    dispatcher.register_reference(reinterpret_cast<std::array<float, 4> &>(p_data));
}

void GuiDrawer::draw_quat(WidgetContainer &p_root, const std::string &p_name,
                          QuaternionF &p_data, float p_step, float p_min, float p_max) {
    create_title(p_root, p_name);
    auto &widget =
    p_root.create_widget<DragMultipleScalars<float, 4>>(get_data_type<float>(), p_min, p_max, 0.f, p_step, "",
                                                        get_format<float>());
    auto &dispatcher = widget.add_plugin<DataDispatcher<std::array<float, 4>>>();
    dispatcher.register_reference(reinterpret_cast<std::array<float, 4> &>(p_data));
}

void GuiDrawer::draw_string(WidgetContainer &p_root, const std::string &p_name, std::string &p_data) {
    create_title(p_root, p_name);
    auto &widget = p_root.create_widget<InputText>("");
    auto &dispatcher = widget.add_plugin<DataDispatcher<std::string>>();
    dispatcher.register_reference(p_data);
}

void GuiDrawer::draw_color(WidgetContainer &p_root, const std::string &p_name, Color &p_color, bool p_has_alpha) {
    create_title(p_root, p_name);
    auto &widget = p_root.create_widget<ColorEdit>(p_has_alpha);
    auto &dispatcher = widget.add_plugin<DataDispatcher<Color>>();
    dispatcher.register_reference(p_color);
}

//Text& GuiDrawer::drawMesh(WidgetContainer & p_root, const std::string & p_name, OvRendering::Resources::Model *& p_data, Event<>* p_updateNotifier)
//{
//    create_title(p_root, p_name);
//
//    std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
//    auto& rightSide = p_root.create_widget<Group>();
//
//    auto& widget = rightSide.create_widget<Text>(displayedText);
//
//    widget.add_plugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//    {
//        if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::MODEL)
//        {
//            if (auto resource = OVSERVICE(OvCore::ResourceManagement::ModelManager).GetResource(p_receivedData.first); resource)
//            {
//                p_data = resource;
//                widget.content = p_receivedData.first;
//                if (p_updateNotifier)
//                    p_updateNotifier->Invoke();
//            }
//        }
//    };
//
//    widget.lineBreak = false;
//
//    auto& resetButton = rightSide.create_widget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
//    resetButton.idleBackgroundColor = clear_button_color_;
//    resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
//    {
//        p_data = nullptr;
//        widget.content = "Empty";
//        if (p_updateNotifier)
//            p_updateNotifier->Invoke();
//    };
//
//    return widget;
//}
//
//OvUI::Widgets::Visual::Image& GuiDrawer::drawTexture(WidgetContainer & p_root, const std::string & p_name, OvRendering::Resources::Texture *& p_data, Event<>* p_updateNotifier)
//{
//    create_title(p_root, p_name);
//
//    std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
//    auto& rightSide = p_root.create_widget<Group>();
//
//    auto& widget = rightSide.create_widget<OvUI::Widgets::Visual::Image>(p_data ? p_data->id : (__EMPTY_TEXTURE ? __EMPTY_TEXTURE->id : 0), Vector2F{ 75, 75 });
//
//    widget.add_plugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//    {
//        if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::TEXTURE)
//        {
//            if (auto resource = OVSERVICE(OvCore::ResourceManagement::TextureManager).GetResource(p_receivedData.first); resource)
//            {
//                p_data = resource;
//                widget.textureID.id = resource->id;
//                if (p_updateNotifier)
//                    p_updateNotifier->Invoke();
//            }
//        }
//    };
//
//    widget.lineBreak = false;
//
//    auto& resetButton = rightSide.create_widget<OvUI::Widgets::Buttons::Button>("Clear");
//    resetButton.idleBackgroundColor = clear_button_color_;
//    resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
//    {
//        p_data = nullptr;
//        widget.textureID.id = (__EMPTY_TEXTURE ? __EMPTY_TEXTURE->id : 0);
//        if (p_updateNotifier)
//            p_updateNotifier->Invoke();
//    };
//
//    return widget;
//}
//
//Text& GuiDrawer::drawShader(WidgetContainer & p_root, const std::string & p_name, OvRendering::Resources::Shader *& p_data, Event<>* p_updateNotifier)
//{
//    create_title(p_root, p_name);
//
//    std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
//    auto& rightSide = p_root.create_widget<Group>();
//
//    auto& widget = rightSide.create_widget<Text>(displayedText);
//
//    widget.add_plugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//    {
//        if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::SHADER)
//        {
//            if (auto resource = OVSERVICE(OvCore::ResourceManagement::ShaderManager).GetResource(p_receivedData.first); resource)
//            {
//                p_data = resource;
//                widget.content = p_receivedData.first;
//                if (p_updateNotifier)
//                    p_updateNotifier->Invoke();
//            }
//        }
//    };
//
//    widget.lineBreak = false;
//
//    auto& resetButton = rightSide.create_widget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
//    resetButton.idleBackgroundColor = clear_button_color_;
//    resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
//    {
//        p_data = nullptr;
//        widget.content = "Empty";
//        if (p_updateNotifier)
//            p_updateNotifier->Invoke();
//    };
//
//    return widget;
//}
//
//Text& GuiDrawer::drawMaterial(WidgetContainer & p_root, const std::string & p_name, OvCore::Resources::Material *& p_data, Event<>* p_updateNotifier)
//{
//    create_title(p_root, p_name);
//
//    std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
//    auto& rightSide = p_root.create_widget<Group>();
//
//    auto& widget = rightSide.create_widget<Text>(displayedText);
//
//    widget.add_plugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//    {
//        if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::MATERIAL)
//        {
//            if (auto resource = OVSERVICE(OvCore::ResourceManagement::MaterialManager).GetResource(p_receivedData.first); resource)
//            {
//                p_data = resource;
//                widget.content = p_receivedData.first;
//                if (p_updateNotifier)
//                    p_updateNotifier->Invoke();
//            }
//        }
//    };
//
//    widget.lineBreak = false;
//
//    auto& resetButton = rightSide.create_widget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
//    resetButton.idleBackgroundColor = clear_button_color_;
//    resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
//    {
//        p_data = nullptr;
//        widget.content = "Empty";
//        if (p_updateNotifier)
//            p_updateNotifier->Invoke();
//    };
//
//    return widget;
//}
//
//Text& GuiDrawer::drawSound(WidgetContainer& p_root, const std::string& p_name, OvAudio::Resources::Sound*& p_data, Event<>* p_updateNotifier)
//{
//    create_title(p_root, p_name);
//
//    std::string displayedText = (p_data ? p_data->path : std::string("Empty"));
//    auto & rightSide = p_root.create_widget<Group>();
//
//    auto & widget = rightSide.create_widget<Text>(displayedText);
//
//    widget.add_plugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//    {
//        if (OvTools::Utils::PathParser::GetFileType(p_receivedData.first) == OvTools::Utils::PathParser::EFileType::SOUND)
//        {
//            if (auto resource = OVSERVICE(OvCore::ResourceManagement::SoundManager).GetResource(p_receivedData.first); resource)
//            {
//                p_data = resource;
//                widget.content = p_receivedData.first;
//                if (p_updateNotifier)
//                    p_updateNotifier->Invoke();
//            }
//        }
//    };
//
//    widget.lineBreak = false;
//
//    auto & resetButton = rightSide.create_widget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
//    resetButton.idleBackgroundColor = clear_button_color_;
//    resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
//    {
//        p_data = nullptr;
//        widget.content = "Empty";
//        if (p_updateNotifier)
//            p_updateNotifier->Invoke();
//    };
//
//    return widget;
//}
//
//Text& GuiDrawer::draw_asset(WidgetContainer& p_root, const std::string& p_name, std::string& p_data, Event<>* p_updateNotifier)
//{
//    create_title(p_root, p_name);
//
//    const std::string displayedText = (p_data.empty() ? std::string("Empty") : p_data);
//    auto& rightSide = p_root.create_widget<Group>();
//
//    auto& widget = rightSide.create_widget<Text>(displayedText);
//
//    widget.add_plugin<DDTarget<std::pair<std::string, Group*>>>("File").DataReceivedEvent += [&widget, &p_data, p_updateNotifier](auto p_receivedData)
//    {
//        p_data = p_receivedData.first;
//        widget.content = p_receivedData.first;
//        if (p_updateNotifier)
//            p_updateNotifier->Invoke();
//    };
//
//    widget.lineBreak = false;
//
//    auto& resetButton = rightSide.create_widget<OvUI::Widgets::Buttons::ButtonSmall>("Clear");
//    resetButton.idleBackgroundColor = clear_button_color_;
//    resetButton.ClickedEvent += [&widget, &p_data, p_updateNotifier]
//    {
//        p_data = "";
//        widget.content = "Empty";
//        if (p_updateNotifier)
//            p_updateNotifier->Invoke();
//    };
//
//    return widget;
//}

void GuiDrawer::draw_boolean(WidgetContainer &p_root, const std::string &p_name,
                             const std::function<bool(void)> &p_gatherer,
                             const std::function<void(bool)> &p_provider) {
    create_title(p_root, p_name);
    auto &widget = p_root.create_widget<CheckBox>();
    auto &dispatcher = widget.add_plugin<DataDispatcher<bool>>();
    
    dispatcher.register_gatherer([p_gatherer]() {
        bool value = p_gatherer();
        return reinterpret_cast<bool &>(value);
    });
    
    dispatcher.register_provider([p_provider](bool p_value) {
        p_provider(reinterpret_cast<bool &>(p_value));
    });
}

void GuiDrawer::draw_vec_2(WidgetContainer &p_root, const std::string &p_name,
                           const std::function<Vector2F(void)> &p_gatherer,
                           const std::function<void(Vector2F)> &p_provider,
                           float p_step, float p_min, float p_max) {
    create_title(p_root, p_name);
    auto &widget =
    p_root.create_widget<DragMultipleScalars<float, 2>>(get_data_type<float>(), p_min, p_max, 0.f, p_step, "",
                                                        get_format<float>());
    auto &dispatcher = widget.add_plugin<DataDispatcher<std::array<float, 2>>>();
    
    dispatcher.register_gatherer([p_gatherer]() {
        Vector2F value = p_gatherer();
        return reinterpret_cast<const std::array<float, 2> &>(value);
    });
    
    dispatcher.register_provider([p_provider](std::array<float, 2> p_value) {
        p_provider(reinterpret_cast<Vector2F &>(p_value));
    });
}

void GuiDrawer::draw_vec_3(WidgetContainer &p_root, const std::string &p_name,
                           const std::function<Vector3F(void)> &p_gatherer,
                           const std::function<void(Vector3F)> &p_provider,
                           float p_step, float p_min, float p_max) {
    create_title(p_root, p_name);
    auto &widget =
    p_root.create_widget<DragMultipleScalars<float, 3>>(get_data_type<float>(), p_min, p_max, 0.f, p_step, "",
                                                        get_format<float>());
    auto &dispatcher = widget.add_plugin<DataDispatcher<std::array<float, 3>>>();
    
    dispatcher.register_gatherer([p_gatherer]() {
        Vector3F value = p_gatherer();
        return reinterpret_cast<const std::array<float, 3> &>(value);
    });
    
    dispatcher.register_provider([p_provider](std::array<float, 3> p_value) {
        p_provider(reinterpret_cast<Vector3F &>(p_value));
    });
}

void GuiDrawer::draw_vec_4(WidgetContainer &p_root, const std::string &p_name,
                           const std::function<Vector4F(void)> &p_gatherer,
                           const std::function<void(Vector4F)> &p_provider,
                           float p_step, float p_min, float p_max) {
    create_title(p_root, p_name);
    auto &widget =
    p_root.create_widget<DragMultipleScalars<float, 4>>(get_data_type<float>(), p_min, p_max, 0.f, p_step, "",
                                                        get_format<float>());
    auto &dispatcher = widget.add_plugin<DataDispatcher<std::array<float, 4>>>();
    
    dispatcher.register_gatherer([p_gatherer]() {
        Vector4F value = p_gatherer();
        return reinterpret_cast<const std::array<float, 4> &>(value);
    });
    
    dispatcher.register_provider([p_provider](std::array<float, 4> p_value) {
        p_provider(reinterpret_cast<Vector4F &>(p_value));
    });
}

void GuiDrawer::draw_quat(WidgetContainer &p_root, const std::string &p_name,
                          const std::function<QuaternionF(void)> &p_gatherer,
                          const std::function<void(QuaternionF)> &p_provider,
                          float p_step, float p_min, float p_max) {
    create_title(p_root, p_name);
    auto &widget =
    p_root.create_widget<DragMultipleScalars<float, 4>>(get_data_type<float>(), p_min, p_max, 0.f, p_step, "",
                                                        get_format<float>());
    auto &dispatcher = widget.add_plugin<DataDispatcher<std::array<float, 4>>>();
    
    dispatcher.register_gatherer([p_gatherer]() {
        QuaternionF value = p_gatherer();
        return reinterpret_cast<const std::array<float, 4> &>(value);
    });
    
    dispatcher.register_provider([p_provider](std::array<float, 4> p_value) {
        p_provider(reinterpret_cast<QuaternionF &>(p_value).normalized());
    });
}

void GuiDrawer::draw_ddstring(WidgetContainer &p_root, const std::string &p_name,
                              std::function<std::string()> p_gatherer, std::function<void(std::string)> p_provider,
                              const std::string &p_identifier) {
    create_title(p_root, p_name);
    auto &widget = p_root.create_widget<InputText>("");
    auto &dispatcher = widget.add_plugin<DataDispatcher<std::string>>();
    dispatcher.register_gatherer(std::move(p_gatherer));
    dispatcher.register_provider(std::move(p_provider));
    
    auto &dd_target = widget.add_plugin<DDTarget<std::pair<std::string, Group *>>>(p_identifier);
    dd_target.data_received_event_ += [&widget, &dispatcher](const std::pair<std::string, Group *> &p_data) {
        widget.content_ = p_data.first;
        dispatcher.notify_change();
    };
}

void GuiDrawer::draw_string(WidgetContainer &p_root, const std::string &p_name,
                            std::function<std::string(void)> p_gatherer, std::function<void(std::string)> p_provider) {
    create_title(p_root, p_name);
    auto &widget = p_root.create_widget<InputText>("");
    auto &dispatcher = widget.add_plugin<DataDispatcher<std::string>>();
    dispatcher.register_gatherer(std::move(p_gatherer));
    dispatcher.register_provider(std::move(p_provider));
}

void GuiDrawer::draw_color(WidgetContainer &p_root,
                           const std::string &p_name,
                           std::function<Color(void)> p_gatherer,
                           std::function<void(Color)> p_provider,
                           bool p_has_alpha) {
    create_title(p_root, p_name);
    auto &widget = p_root.create_widget<ColorEdit>(p_has_alpha);
    auto &dispatcher = widget.add_plugin<DataDispatcher<Color>>();
    dispatcher.register_gatherer(std::move(p_gatherer));
    dispatcher.register_provider(std::move(p_provider));
}

}
