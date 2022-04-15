//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "CLI11.h"

#include <utility>

#include "logging.h"
#include "strings.h"

namespace vox {
Cli11CommandContext::Cli11CommandContext(CLI::App *cli, CLI11CommandContextState state) :
	CommandParserContext(), cli_11_(cli), state_(std::move(state)) {}

bool Cli11CommandContext::has_group_name() const {
    return !state_.group_name.empty();
}

const std::string &Cli11CommandContext::get_group_name() const {
    return state_.group_name;
}

CLI11CommandContextState Cli11CommandContext::get_state() const {
    return state_;
}

Cli11CommandParser::Cli11CommandParser(const std::string &name, const std::string &description,
									   const std::vector<std::string> &args) :
	cli_11_{std::make_unique<CLI::App>(description, name)}, formatter_{std::make_shared<HelpFormatter>()} {
    cli_11_->formatter(formatter_);
    
    args_.resize(args.size());
    std::transform(args.begin(), args.end(), args_.begin(),
				   [](const std::string &string) -> char * { return const_cast<char *>(string.c_str()); });
}

std::vector<std::string> Cli11CommandParser::help() const {
    return split(cli_11_->help(), "\n");
}

// Helper to reduce duplication - throw should not occur as there should always be a valid context passed
#define CAST(type)                                                                                 \
void Cli11CommandParser::parse(CommandParserContext *context, type *command)                   \
{                                                                                              \
parse(context == nullptr ? throw : dynamic_cast<Cli11CommandContext *>(context), command); \
}

CAST(CommandGroup)

CAST(SubCommand)

CAST(PositionalCommand)

CAST(FlagCommand)

#undef CAST

void Cli11CommandParser::parse(Cli11CommandContext *context, CommandGroup *command) {
    auto state = context->get_state();
    state.group_name = command->get_name();
    Cli11CommandContext group_context(context->cli_11_, state);
    CommandParser::parse(&group_context, command->get_commands());
}

void Cli11CommandParser::parse(Cli11CommandContext *context, SubCommand *command) {
    auto *subcommand = context->cli_11_->add_subcommand(command->get_name(), command->get_help_line());
    sub_commands_[command] = subcommand;
    subcommand->formatter(formatter_);
    Cli11CommandContext subcommand_context(subcommand, context->get_state());
    CommandParser::parse(&subcommand_context, command->get_commands());
}

void Cli11CommandParser::parse(Cli11CommandContext *context, PositionalCommand *command) {
    auto *option = context->cli_11_->add_option(command->get_name(), command->get_help_line());
    
    options_.emplace(command, option);
    
    if (context->has_group_name()) {
        option->group(context->get_group_name());
    }
}

void Cli11CommandParser::parse(Cli11CommandContext *context, FlagCommand *command) {
    CLI::Option *flag;
    
    switch (command->get_flag_type()) {
        case FlagType::FLAG_ONLY:
            flag = context->cli_11_->add_flag(command->get_name(), command->get_help_line());
            break;
        case FlagType::ONE_VALUE:
        case FlagType::MANY_VALUES:
            flag = context->cli_11_->add_option(command->get_name(), command->get_help_line());
            break;
    }
    
    options_.emplace(command, flag);
    
    if (context->has_group_name()) {
        flag->group(context->get_group_name());
    }
}

bool Cli11CommandParser::contains(Command *command) const {
    {
        auto it = options_.find(command);
        
        if (it != options_.end()) {
            return it->second->count() > 0;
        }
    }
    
    {
        auto it = sub_commands_.find(command);
        
        if (it != sub_commands_.end()) {
            return it->second->count() > 0;
        }
    }
    
    return false;
}

std::vector<std::string> Cli11CommandParser::get_command_value(Command *command) const {
    auto it = options_.find(command);
    
    if (it == options_.end()) {
        return {};
    }
    
    return it->second->results();
}

/*
 
 To create a CLI composed of multiple interoperable plugins using CLI11, we must create a CLI11 app from each plugin.
 This acts as a group for the commands used in said plugin. Once we have groups for each plugin we can then nest
 them inside each other using the CLI11::App::add_subcommand() method.
 
 This is required as CLI11 does not allow the redefinition of the same flag. Within the same app context.
 
 */
bool Cli11CommandParser::parse(const std::vector<Plugin *> &plugins) {
    // Generate all command groups
    for (auto plugin: plugins) {
        auto group = std::make_unique<CLI::App>();
        
        formatter_->register_meta(group.get(), {plugin->get_name(), plugin->get_description()});
        
        Cli11CommandContext context(group.get());
        CommandParser::parse(&context, plugin->get_cli_commands());
        
        option_groups_[plugin] = std::move(group);
    }
    
    // Associate correct command groups
    for (auto plugin: plugins) {
        auto plugin_cli = option_groups_[plugin];
        auto included_plugins = plugin->get_inclusions();
        auto commands = plugin->get_cli_commands();
        
        for (auto command: commands) {
            // Share flags and options with sub commands
            if (command->is<SubCommand>()) {
                auto cli11_sub_command = sub_commands_[command];
                
                for (auto included_plugin: included_plugins) {
                    cli11_sub_command->add_subcommand(option_groups_[included_plugin]);
                }
            }
        }
        
        cli_11_->add_subcommand(plugin_cli);
    }
    
    return cli_11_parse(cli_11_.get());
}

bool Cli11CommandParser::parse(const std::vector<Command *> &commands) {
    Cli11CommandContext context(cli_11_.get());
    if (!CommandParser::parse(&context, commands)) {
        return false;
    }
    
    return cli_11_parse(cli_11_.get());
}

bool Cli11CommandParser::cli_11_parse(CLI::App *app) {
    try {
        args_.insert(args_.begin(), "vulkan_samples");
        app->parse(static_cast<int>(args_.size()), args_.data());
    }
    catch (const CLI::CallForHelp &e) {
        return false;
    }
    catch (const CLI::ParseError &e) {
        bool success = e.get_exit_code() == static_cast<int>(CLI::ExitCodes::Success);
        
        if (!success) {
            LOGE("CLI11 Parse Error: [{}] {}", e.get_name(), e.what())
            return false;
        }
    }
    
    return true;
}

}        // namespace vox
