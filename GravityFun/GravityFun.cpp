#include "GravityFun.h"

#include <iostream>
#include <memory>
#include <thread>
#include <vector>

int main()
{
    std::cout << "Powered by:\n";
    std::cout << GravityFun::Info::DEPENDENCIES << '\n';
    std::cout << GravityFun::Info::NAME << " v" << GravityFun::Info::VERSION << "\n";
    std::cout << GravityFun::Info::LICENSE << '\n';

    auto hardware_concurrency = std::thread::hardware_concurrency();

    // Modules Initialization

    std::shared_ptr<GravityFun::Window> window(new GravityFun::Window());
    std::shared_ptr<GravityFun::GameManager> game_manager(new GravityFun::GameManager(window));
    std::vector<std::shared_ptr<GravityFun::Physics>> physics_pass1;
    std::vector<std::shared_ptr<GravityFun::Physics>> physics_pass2; // hybrid pass
    for (int i = 0; i < hardware_concurrency; i++)
        physics_pass1.push_back(
            std::shared_ptr<GravityFun::Physics>(
                new GravityFun::Physics(game_manager, i, hardware_concurrency, false)
            )
        );
    for (int i = 0; i < hardware_concurrency; i++)
        physics_pass2.push_back(
            std::shared_ptr<GravityFun::Physics>(
                new GravityFun::Physics(game_manager, i, hardware_concurrency, true)
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
            physics_pass2_group,
            game_manager->GetPhysicsPassNotifier()
        })
    ));
    std::shared_ptr<LoopScheduler::ParallelGroup> physics_and_render_group(new LoopScheduler::ParallelGroup(
        {
            LoopScheduler::ParallelGroupMember(physics_passes_group, 1),
            LoopScheduler::ParallelGroupMember(renderer, 0)
        }
    ));
    std::shared_ptr<LoopScheduler::SequentialGroup> root_group(new LoopScheduler::SequentialGroup(
        std::vector<LoopScheduler::SequentialGroupMember>({
            game_manager,
            physics_and_render_group
        })
    ));

    LoopScheduler::Loop loop(root_group);

    loop.Run(hardware_concurrency + 1);

    return 0;
}
