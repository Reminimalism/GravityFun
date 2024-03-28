#include "GravityFun.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

int main()
{
    std::cout << "Powered by:\n";
    std::cout << GravityFun::Info::DEPENDENCIES << '\n';
    std::cout << GravityFun::Info::CREATOR << " - " << GravityFun::Info::NAME << " v" << GravityFun::Info::VERSION << "\n";
    std::cout << GravityFun::Info::LICENSE << '\n';
    std::cout << GravityFun::Info::DESCRIPTION << '\n';

    auto concurrency = std::thread::hardware_concurrency();

    // Override concurrency if the conf exists with a concurrency value
    constexpr auto conf_filename = "GravityFun.conf";
    if (std::filesystem::exists(conf_filename) && std::filesystem::is_regular_file(conf_filename))
    {
        std::ifstream conf(conf_filename);
        std::string line;
        while (std::getline(conf, line))
        {
            auto i = line.find("concurrency");
            if (i == std::string::npos)
                continue;
            i += std::string("concurrency").length();
            for (; i < line.length(); i++)
                if (line[i] != ' ')
                    break;
            if (line[i] != '=')
                continue;
            i++;
            for (; i < line.length(); i++)
                if (line[i] != ' ')
                    break;
            int j = i;
            for (; j < line.length(); j++)
                if (!std::isdigit(line[i]))
                    break;
            if (i != j)
            {
                concurrency = std::stoi(line.substr(i, j - i));
                if (concurrency < 1)
                    concurrency = 1;
                if (concurrency > 1024)
                    concurrency = 1024;
                std::cout << "Config found, concurrency set to " << concurrency << ".\n";
            }
        }
    }

    // Modules Initialization

    std::shared_ptr<GravityFun::Window> window(new GravityFun::Window(std::string(GravityFun::Info::NAME) + " v" + GravityFun::Info::VERSION));
    std::shared_ptr<GravityFun::EnergySaver> energy_saver(new GravityFun::EnergySaver());
    std::shared_ptr<GravityFun::GameManager> game_manager(new GravityFun::GameManager(window, energy_saver));
    std::vector<std::shared_ptr<GravityFun::Physics>> physics_pass1;
    std::vector<std::shared_ptr<GravityFun::Physics>> physics_pass2; // hybrid pass
    auto physics_modules_count = concurrency;
    for (int i = 0; i < physics_modules_count; i++)
        physics_pass1.push_back(
            std::shared_ptr<GravityFun::Physics>(
                new GravityFun::Physics(game_manager, i, physics_modules_count, nullptr)
            )
        );
    for (int i = 0; i < physics_modules_count; i++)
        physics_pass2.push_back(
            std::shared_ptr<GravityFun::Physics>(
                new GravityFun::Physics(game_manager, i, physics_modules_count, physics_pass1[i].get())
            )
        );
    std::shared_ptr<GravityFun::Renderer> renderer(new GravityFun::Renderer(window, game_manager));

    // Construct Loop

    std::vector<LoopScheduler::ParallelGroupMember> physics_pass1_members;
    std::vector<LoopScheduler::ParallelGroupMember> physics_pass2_members;
    for (auto& item : physics_pass1)
        physics_pass1_members.push_back(
            LoopScheduler::ParallelGroupMember(item, 0)
        );
    for (auto& item : physics_pass2)
        physics_pass2_members.push_back(
            LoopScheduler::ParallelGroupMember(item, 0)
        );

    std::shared_ptr<LoopScheduler::ParallelGroup> physics_pass1_group(new LoopScheduler::ParallelGroup(physics_pass1_members));
    std::shared_ptr<LoopScheduler::ParallelGroup> physics_pass2_group(new LoopScheduler::ParallelGroup(physics_pass2_members));
    std::shared_ptr<LoopScheduler::SequentialGroup> physics_passes_group(new LoopScheduler::SequentialGroup(
        std::vector<LoopScheduler::SequentialGroupMember>({
            physics_pass1_group,
            game_manager->GetPhysicsPass1Notifier(),
            physics_pass2_group,
            game_manager->GetPhysicsPass2Notifier(),
            energy_saver
        })
    ));
    std::shared_ptr<LoopScheduler::ParallelGroup> physics_and_render_group(new LoopScheduler::ParallelGroup(
        {
            LoopScheduler::ParallelGroupMember(renderer, 0),
            LoopScheduler::ParallelGroupMember(physics_passes_group, 1)
        },
        true // To support multiple SequentialGroup runs in ParallelGroup that is in another SequentialGroup
    ));
    std::shared_ptr<LoopScheduler::SequentialGroup> root_group(new LoopScheduler::SequentialGroup(
        std::vector<LoopScheduler::SequentialGroupMember>({
            game_manager,
            physics_and_render_group
        })
    ));

    game_manager->SetGroups(root_group.get(), physics_pass1_group.get(), physics_pass2_group.get());

    LoopScheduler::Loop loop(root_group);

    loop.Run(concurrency);

    return 0;
}
