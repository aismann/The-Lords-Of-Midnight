#include "panel_splashscreen.h"
#include "panel_dedication.h"
#include "panel_mainmenu.h"

#include "../system/resolutionmanager.h"
#include "../ui/uihelper.h"
#include "../system/moonring.h"
#include "../system/configmanager.h"
#include "../system/panelmanager.h"
#include "../system/progressmonitor.h"

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <future>

USING_NS_CC;

#define MAX_SPLASHSCREEN_TIME   3000
#define TRANSITION_TIME         2.0

bool panel_splashscreen::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    uihelper::Init();
    
    SetBackground("splash.png");
    
    loading_width = RES(512);
    loading_height = RES(16);
    
    loading_progress = DrawNode::create();
    uihelper::AddBottomLeft(this, loading_progress, RES(8), RES(8));
    
    progress = new progressmonitor([&](int value) {
        UpdateProgress( (f32)value / 10.0f );
    });
    progress->Start();
    
    StartTime = utils::getTimeInMilliseconds();
    
    // Initialise in a thread
    auto atp = AsyncTaskPool::getInstance();
    atp->enqueue(AsyncTaskPool::TaskType::TASK_IO, [&]() {
        mr->Initialise( progress );
        progress->Stop();
        SAFEDELETE(progress);
        Complete();
    });
    
    
    
    return true;
}

void panel_splashscreen::Complete()
{
    auto Duration = utils::getTimeInMilliseconds() - StartTime;
    
    // we want at least 3 seconds of splash screen
    f32 delay = MAX(0,(f32)(MAX_SPLASHSCREEN_TIME-Duration) / 1000.0f);
    
    if ( CONFIG(screentransitions) ) {
        loading_progress->runAction( FadeOut::create(1.0) );
    }else{
        loading_progress->setVisible(false);
        delay = 1;
    }
    
    this->scheduleOnce( [&](float) {
#if _SHOW_DEDICATION_
        mr->panels->SetPanelMode(MODE_DEDICATION, TRANSITION_FADEIN );
#else
        mr->panels->SetPanelMode(MODE_MAINMENU, TRANSITION_FADEIN );
#endif
    }, delay, "show_mainmenu" );
}

void panel_splashscreen::UpdateProgress(f32 percent)
{
    RUN_ON_UI_THREAD([=](){
        loading_progress->clear();
        loading_progress->drawSolidRect(Vec2(0,0), Vec2(loading_width,loading_height), Color4F(_clrBlue) );
        loading_progress->drawSolidRect(Vec2(0,0), Vec2(loading_width*MIN(1.0,percent),loading_height), Color4F(_clrRed) );
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}



