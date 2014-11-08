///////////////////////////////////////////////////////////////////////////////
//! \file			screen_stack.hpp
//! \author			Edoardo "sparkon" Dominici
//! \copyright		Release under the MIT license 
//! \version		0.1
///////////////////////////////////////////////////////////////////////////////

#pragma once

// Includes - Extensions
#include "screen.hpp"

//! Includes - C++ STD
#include <vector>
#include <functional>


//! \brief Simple class whose purpose is to manage a stack of screens
//! 
//! \note Objects that are lower in the stack are updated / rendered first
template <typename context_t>
class ScreenStack
{
public :
	//! \brief Default constructor that initializes an empty stack
	ScreenStack();
	
	//! \brief Initialized-list constructor's version that creates a stack from the list of screens
	//! 
	//! \param [in] p_screens List of screens where the first screen is also the lower in the internal stack
	ScreenStack(std::initializer_list<Screen<context_t>&> p_screens);

	//! \brief Default destructor
	~ScreenStack();

	//! \brief Pushes a new state on top of the stack
	void push_state(Screen<context_t>& p_screen, context_t& p_context);

	//! \brief Pops the last state pushed from the top of the stack
	void pop_state(context_t& p_context);

	//! \brief Updates all the state from the lowest one in the stack top the highest one
	//!
	//! Even objects that are hidden from higher-standing ones are updated
	virtual void update(context_t& p_context, unsigned int p_milliseconds);

	//! \brief Renders all states up from the last one that hides 
	virtual void render(context_t& p_context, unsigned int p_milliseconds);

protected :
	std::vector<Screen<context_t>*> m_screen_stack;
};

template <typename context_t>
ScreenStack<context_t>::ScreenStack()
{

}

template <typename context_t>
ScreenStack<context_t>::~ScreenStack()
{

}

template <typename context_t>
void ScreenStack<context_t>::push_state(Screen<context_t>& _state, context_t& p_context)
{
	m_screen_stack.push_back(&_state);

	m_screen_stack.back()->on_activation(p_context);
	m_screen_stack.back()->set_state(Screen<context_t>::State::TransitionIn);
}

template <typename context_t>
void ScreenStack<context_t>::pop_state(context_t& p_context)
{
	m_screen_stack.back()->on_deactivation(p_context);
	m_screen_stack.back()->set_state(Screen<context_t>::State::TransitionOut);
}

template <typename context_t>
void ScreenStack<context_t>::update(context_t& p_context, unsigned int p_milliseconds)
{
	if (m_screen_stack.empty())
		return;

	//! Finding the first screen that is actually hiding
	int last{ static_cast<int>(m_screen_stack.size()) - 1 };

	for (; last >= 0; --last)
	{
		if (m_screen_stack[last]->get_hides())
			break;
	}

	last = last < 0 ? 0 : last;

	for (size_t s = 0; s < m_screen_stack.size(); ++s)
	{
		m_screen_stack[s]->update(p_context, p_milliseconds, s < last ? true : false);

		//! Screen has terminated the fading in phase and is now active
		if (m_screen_stack[s]->get_state() == Screen<context_t>::State::TransitionIn &&
			m_screen_stack[s]->get_fade_in_time() >= m_screen_stack[s]->get_fade_in_period())
		{
			m_screen_stack[s]->set_state(Screen<context_t>::State::Active);
		}
		//! Screen has terminated the fading out phase and is ready to be removed
		else if (m_screen_stack[s]->get_state() == Screen<context_t>::State::TransitionOut &&
			m_screen_stack[s]->get_fade_out_time() >= m_screen_stack[s]->get_fade_out_period())
		{
			//! Removing element
			Screen<context_t>* to_delete = *(m_screen_stack.begin() + s);
			m_screen_stack.erase(m_screen_stack.begin() + s);
				
			if (to_delete->get_delete_when_removed())
				delete to_delete;
		}
	}
}

template <typename context_t>
void ScreenStack<context_t>::render(context_t& p_context, unsigned int p_milliseconds)
{
	if (m_screen_stack.empty())
		return;

	//! Finding the first screen that is actually hiding
	int last{ static_cast<int>(m_screen_stack.size()) - 1 };

	for (; last >= 0; --last)
	{
		if (m_screen_stack[last]->get_hides())
			break;
	}

	last = last < 0 ? 0 : last;

	for (; last < m_screen_stack.size(); ++last)
		m_screen_stack[last]->render(p_context, p_milliseconds);
}