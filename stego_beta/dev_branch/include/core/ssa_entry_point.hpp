//! \copyright Mozilla Public License Version 2.0
//! \note [License]		 license/license.txt
//! \note [Contributors] license/contributors.txt

#pragma once

// ssa
#include "ssa_platform.hpp"

// C++ STD
#include <vector>

typedef std::vector<std::string> cmd_args_t;
extern int entry_point(cmd_args_t& p_args, ssa::Platform p_platform);

#if defined(ssa_os_windows)

#define ssa_entry_point(p_main)

// Defining both
ssa_export int main(int argc, char* argv[]);
ssa_export int WINAPI WinMain(HINSTANCE p_instance, HINSTANCE p_prev_instance, LPSTR p_cmd_line, int p_cmd_show);

#elif defined(ssa_os_windows_phone)

//
// PLEASE NEVER WRITE CODE FOR WINDOWS 8, I BEG YOU
// 

#include <agile.h>

ref class AppListener sealed : public Windows::ApplicationModel::Core::IFrameworkView
{
public :
	AppListener();

	void update();

	virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
	virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
	virtual void Load(Platform::String^ entryPoint);
	virtual void Run();
	virtual void Uninitialize();

protected :
	void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);
	void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
	void OnResuming(Platform::Object^ sender, Platform::Object^ args);

	// @TODO : Should i use Platform::Agile i guess it's like a smart pointer
	Windows::UI::Core::CoreWindow^ get_handle() { return m_window.Get(); }

private :
	Platform::Agile<Windows::UI::Core::CoreWindow> m_window;
};

using namespace Windows;

AppListener::AppListener()
{

}

void AppListener::Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView)
{
	// I guess i registered some events.. still no clue how

	applicationView->Activated += ref new Foundation::TypedEventHandler<ApplicationModel::Core::CoreApplicationView^, ApplicationModel::Activation::IActivatedEventArgs^>(this, &AppListener::OnActivated);

	ApplicationModel::Core::CoreApplication::Suspending += ref new Foundation::EventHandler<ApplicationModel::SuspendingEventArgs^>(this, &AppListener::OnSuspending);
	ApplicationModel::Core::CoreApplication::Resuming += ref new Foundation::EventHandler<Platform::Object^>(this, &AppListener::OnResuming);
}

void AppListener::SetWindow(Windows::UI::Core::CoreWindow^ window)
{
	m_window = window;

}

void AppListener::Load(Platform::String^ entryPoint)
{

}

void AppListener::Run()
{

}

void AppListener::Uninitialize()
{

}

void AppListener::OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args)
{

}

void AppListener::OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args)
{

}

void AppListener::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{

}

ref class AppListenerSource sealed : public Windows::ApplicationModel::Core::IFrameworkViewSource
{
public :
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView()
	{
		return ref new AppListener();
	}
};

#define ssa_entry_point(p_main) \
	[Platform::MTAThread] \
	int main(Platform::Array<Platform::String^>^ p_args) \
	{ \
		cmd_args_t args { }; \
		auto app_listener_source = ref new AppListenerSource(); \
		Windows::ApplicationModel::Core::CoreApplication::Run(app_listener_source); \
		p_main(args, ssa::Platform::WinRT);\
	} \

#elif defined(ssa_os_linux)

#elif defined(ssa_os_macos)

#endif