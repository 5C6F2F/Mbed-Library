#pragma once
#include "ISection.hpp"

template <int N>
class SectionController
{
public:
    SectionController(array<ISection *, N> sections)
        : sections(sections), is_running(false) {}

    void start()
    {
        if (sections.empty() || is_running)
        {
            return;
        }

        is_running = true;
        thread.start(callback(this, &SectionController::run));
    }

    void stop()
    {
        is_running = false;
        thread.join();
    }

    bool isRunning()
    {
        return is_running;
    }

private:
    void run()
    {
        for (ISection *section : sections)
        {
            if (!is_running)
            {
                break;
            }

            section->action();
        }

        // 全てのセクションが完了
        is_running = false;
    }

    array<ISection *, N> sections;
    Thread thread;
    bool is_running;
};
