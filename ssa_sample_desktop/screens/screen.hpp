///////////////////////////////////////////////////////////////////////////////
//! \file			screen.hpp
//! \author			Edoardo "sparkon" Dominici
//! \copyright		Release under the MIT license 
//! \version		0.2
///////////////////////////////////////////////////////////////////////////////

#pragma once

//! Forward declarations
template <typename context_t>
class ScreenStack;

//! \brief Base class for every screen object
//!
//! It represents a state of an ideal state machine that supports transitioning in / out
template <typename context_t>
class Screen
{
public:
	//! \brief Enumeration of possible states of the Screen
	//!
	//! \note Screen is updated && renderered in all states except for Inactive
	enum class State
	{
		Inactive,		//!< Screen is inactive, it has been created but is not being updated / renderered
		TransitionIn,	//!< Screen is transitioning in, when the transition end it will become active
		Active,			//!< Screen is currently active 
		TransitionOut,	//!< Screen is being removed and will become inactive / destroyed at the end of the transition period
	};

public:
	//! \brief Constructs a Screen and sets all his values to default
	//! 
	//! Values are set to : 
	//! * state = inactive
	//! * hides = false
	//! * fade in period = 0.f
	//! * fade out period = 0.f
	//! \param [in] p_screen_machine Screen Machine that is responsible for the managing of this Screen
	Screen(ScreenStack<context_t>& p_screen_machine) : m_screen_machine{ p_screen_machine },
		m_state{ State::Inactive },
		m_hides{ false },
		m_delete_when_removed{ false },
		m_fade_in_period{ 0.f },
		m_fade_in_time{ 0.f },
		m_fade_out_period{ 0.f },
		m_fade_out_time{ 0.f } { }

	//! \brief Default destructor
	virtual ~Screen() = default;

	//! \brief Called when the screen is being activated ( before transitioning in )
	//! 
	//! \note The state should be Inactive for the whole function, the screen manager will set it to Active
	virtual void on_activation(context_t& p_context) { }

	//! \brief Called when the screen is being deactived ( after transitioning out )
	//!
	//! \note The state should be TransitioningOut for the whole function, the screen manager will set it to Inactive
	virtual void on_deactivation(context_t& p_context) { }

	//! \brief Updates the current screen 
	//! 
	//! Called while : 
	//! * TransitionIn
	//! * Active
	//! * TransitionOut
	//! \param [in] p_milliseconds Milliseconds elapsed since the previous update call
	//! \param [in] p_is_hidden True if the current screen is hidden, thus render() is not called
	virtual void update(context_t& p_context, unsigned int p_milliseconds, bool p_is_hidden) = 0;

	//! \brief Renders the current screen 
	//! 
	//! Called while : 
	//! * TransitionIn
	//! * Active
	//! * TransitionOut
	//! \param [in] p_milliseconds Milliseconds elapsed since the previous render call
	virtual void render(context_t& p_context, unsigned int p_milliseconds) = 0;

	//! \brief Retrieves the current state of the screen
	//!
	//! \return State of the screen
	inline State get_state()const { return m_state; }

	//! \brief Sets a new state for the screen
	//!
	//! \param [in] p_state New state to be set
	inline void set_state(State p_state) { m_state = p_state; }

	//! \brief True if the current screen should hide the underlying ones
	inline bool get_hides()const { return m_hides; }

	inline bool get_delete_when_removed()const { return m_delete_when_removed; }

	inline float get_fade_in_period()const { return m_fade_in_period; }
	inline void set_fade_in_period(float p_fade_in_period) { m_fade_in_period = p_fade_in_period; }

	inline float get_fade_in_time()const { return m_fade_in_time; }
	//	inline void set_fade_in_time(float p_fade_in_time) { m_fade_in_time = p_fade_in_time; }

	inline float get_fade_out_period()const { return m_fade_out_period; }
	inline void set_fade_out_period(float p_fade_out_period) { m_fade_out_period = p_fade_out_period; }

	inline float get_fade_out_time()const { return m_fade_out_time; }
	//	inline void set_fade_out_time(float p_fade_out_time) { m_fade_out_time = p_fade_out_time; }

protected:
	ScreenStack<context_t>& m_screen_machine;
	State	m_state;

	bool	m_hides;
	bool	m_delete_when_removed;

	float	m_fade_in_period;
	float	m_fade_in_time;

	float	m_fade_out_period;
	float	m_fade_out_time;
};