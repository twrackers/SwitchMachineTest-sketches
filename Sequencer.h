#ifndef _SEQUENCER__H_
#define _SEQUENCER__H_

#include <StateMachine.h>

class Sequencer : public StateMachine
{
  private:
    const unsigned int m_nsteps;
    int m_step;
    bool m_counting;

  public:
    Sequencer(const unsigned int interval, const unsigned int nsteps) :
    StateMachine(interval, true),
    m_nsteps(nsteps), m_step(-1), m_counting(false)
    {
    }

    virtual bool update()
    {
      if (StateMachine::update()) {
        if (m_counting) {
          if (++m_step >= m_nsteps) {
            m_step = -1;
            m_counting = false;
          }
        }
        return true;
      }
      return false;
    }

    void start(void)
    {
      m_step = 0;
      m_counting = true;
    }

    int read() const
    {
      return m_step;
    }
};

#endif
