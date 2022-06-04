#include <kernel.h>
#include <paf.h>

#include "pages/page.hpp"
#include "utils.hpp"
#include "common.hpp"
#include "main.hpp"

generic::Page *generic::Page::currPage = SCE_NULL;
paf::ui::Plane *generic::Page::templateRoot = SCE_NULL;

paf::ui::CornerButton *g_backButton;
paf::ui::CornerButton *g_forwardButton;
paf::ui::BusyIndicator *g_busyIndicator;

generic::Page::BackButtonEventCallback generic::Page::backCallback;
void *generic::Page::backData;

generic::Page::Page(const char *pageName)
{
    this->prev = currPage;
    currPage = this;

    paf::Plugin::TemplateInitParam tInit;
    paf::Resource::Element e;

    e.hash = Utils::GetHashById(pageName);
    
    SceBool isMainThread = paf::thread::IsMainThread();
    if(!isMainThread)
        paf::thread::s_mainThreadMutex.Lock();

    mainPlugin->TemplateOpen(templateRoot, &e, &tInit);
    root = (paf::ui::Plane *)templateRoot->GetChildByNum(templateRoot->childNum - 1);

	if (currPage->prev != NULL)
	{
		currPage->prev->root->PlayAnimation(0, paf::ui::Widget::Animation_3D_SlideToBack1);
		if (prev->root->animationStatus & 0x80)
			prev->root->animationStatus &= ~0x80;

		g_backButton->PlayAnimation(0, paf::ui::Widget::Animation_Reset);
        Page::SetBackButtonEvent(NULL, NULL);

		if (currPage->prev->prev != SCE_NULL)
		{
			currPage->prev->prev->root->PlayAnimationReverse(0, paf::ui::Widget::Animation_Reset);
			if (prev->prev->root->animationStatus & 0x80)
				prev->prev->root->animationStatus &= ~0x80;
		}
	}
	else g_backButton->PlayAnimationReverse(0, paf::ui::Widget::Animation_Reset);
	
    g_forwardButton->PlayAnimationReverse(0, paf::ui::Widget::Animation_Reset);

    root->PlayAnimation(-50000, paf::ui::Widget::Animation_3D_SlideFromFront);
	if (root->animationStatus & 0x80)
		root->animationStatus &= ~0x80;

    if(!isMainThread)
        paf::thread::s_mainThreadMutex.Lock();
}

SceVoid generic::Page::OnRedisplay()
{

}

SceVoid generic::Page::OnDelete()
{

}

generic::Page::~Page()
{
	paf::common::Utils::WidgetStateTransition(-100, this->root, paf::ui::Widget::Animation_3D_SlideFromFront, SCE_TRUE, SCE_TRUE);
	if (prev != SCE_NULL)
	{
		prev->root->PlayAnimationReverse(0.0f, paf::ui::Widget::Animation_3D_SlideToBack1);
		prev->root->PlayAnimation(0.0f, paf::ui::Widget::Animation_Reset);
		if (prev->root->animationStatus & 0x80)
			prev->root->animationStatus &= ~0x80;

		if (prev->prev != SCE_NULL) {
			prev->prev->root->PlayAnimation(0.0f, paf::ui::Widget::Animation_Reset);
			if (prev->prev->root->animationStatus & 0x80)
				prev->prev->root->animationStatus &= ~0x80;
		}
	}
	currPage = this->prev;

    if (currPage != NULL && currPage->prev != SCE_NULL)
        g_backButton->PlayAnimation(0, paf::ui::Widget::Animation_Reset);
    else g_backButton->PlayAnimationReverse(0, paf::ui::Widget::Animation_Reset);

    if(currPage != SCE_NULL) currPage->OnRedisplay(); 
}

void generic::Page::Setup()
{
    if(templateRoot) return; //Already Initialised

    paf::Resource::Element e;
    paf::Plugin::PageInitParam pInit;
    
    e.hash = Utils::GetHashById("page_main");
    paf::ui::Widget *page =  mainPlugin->PageOpen(&e, &pInit);
    
    e.hash = Utils::GetHashById("template_plane");
    templateRoot = (paf::ui::Plane *)page->GetChildByHash(&e, 0);

    currPage = SCE_NULL;

    e.hash = Utils::GetHashById("back_button");
    g_backButton = (paf::ui::CornerButton *)page->GetChildByHash(&e, 0);

    backCallback = NULL;
    backData = NULL;

    g_backButton->PlayAnimationReverse(0, paf::ui::Widget::Animation::Animation_Reset);

    paf::ui::Widget::EventCallback *backButtonEventCallback = new paf::ui::Widget::EventCallback();
    backButtonEventCallback->eventHandler = generic::Page::BackButtonEventHandler;
    g_backButton->RegisterEventCallback(paf::ui::Widget::EventMain_Decide, backButtonEventCallback, 0);

    e.hash = Utils::GetHashById("main_busy");
    g_busyIndicator = (paf::ui::BusyIndicator *)page->GetChildByHash(&e, 0);
    g_busyIndicator->Stop();

    e.hash = Utils::GetHashById("forward_button");
    g_forwardButton = (paf::ui::CornerButton *)page->GetChildByHash(&e, 0);
    g_forwardButton->PlayAnimationReverse(0, paf::ui::Widget::Animation_Reset);
}

void generic::Page::SetBackButtonEvent(BackButtonEventCallback callback, void *data)
{
    backCallback = callback;
    backData = data;
}

void generic::Page::BackButtonEventHandler(SceInt32, paf::ui::Widget *, SceInt32, ScePVoid)
{
    if(backCallback)
        backCallback(backData);
    else    
        generic::Page::DeleteCurrentPage();
}

void generic::Page::DeleteCurrentPage()
{
	delete currPage;
}